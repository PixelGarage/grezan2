<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 03.01.17
 * Time: 18:24
 */

/**
 * Returns true, if Stripe library is loaded and the API key is set. False otherwise.
 */
function stripe_button_library_loaded () {
  $library = libraries_load('stripe');
  if ($library['loaded']) {
    //
    // set the private API key of specific account
    \Stripe\Stripe::setApiKey(_stripe_api_secret_key());
    return true;
  }

  return false;
}

/**
 * Returns the stripe fee for the given amount.
 *
 * @param $amount float   The charged amount.
 * @return float Stripe fee charged with any transaction.
 */
function stripe_button_stripe_fee($amount) {
  return round($amount * 0.029) + 30;
}

/**
 * Charges the credit card (token) with the given amount in the given currency
 * and sends an confirmation email when successful.
 *
 * @param $amount
 * @param $currency
 * @param $token
 * @param $email
 * @param string $desc
 */
function stripe_button_charge_card_once($amount, $currency, $token, $email, $desc = '') {
  //
  // create the Stripe charge
  $charge = \Stripe\Charge::create(array(
    "amount" => $amount,
    "currency" => $currency,
    "source" => $token,
    "description" => $desc,
    "receipt_email" => $email
  ));

  // return charge id
  return $charge->id;
}


/**
 * Return the registered user object, if available. Otherwise an Exception is thrown.
 *
 * @param $email  string
 *    The email address entered into Stripe Checkout dialog.
 * @return mixed|null
 *
 * @throws \Exception
 */
function stripe_button_registered_user($email) {
  $registered_user = null;
  if (user_is_anonymous()) {
    $registered_user = user_load_by_mail($email);
    if (!$registered_user) {
      // no recurring billing possible for anonymous user
      throw new Exception('No registration found. Only registered user can subscribe to recurring payments.');
    }
  }
  else {
    $registered_user = user_load($GLOBALS['user']->uid);
  }
  return $registered_user;
}

/**
 * Returns the ID of the Stripe subscription plan fitting the given parameter or of the newly created one.
 *
 * @param $amount
 * @param $currency
 * @param $recurring_billing
 * @return object   Created or retrieved Stripe plan.
 */
function stripe_button_subscription_plan($amount, $currency, $recurring_billing) {
  //
  // get subscription plan via plan-id, if available
  try {
    $plan_id = 'bge_' . $recurring_billing . '_' . $amount . '_' . $currency;
    $subscription_plan = \Stripe\Plan::retrieve($plan_id);
    return $subscription_plan;
  }
  catch (Exception $e) {
    // no plan with given id found, do nothing here
  }

  //
  // create a new subscription plan
  switch ($amount) {
    case 300:
      $plan_name = t('Basic Income Silver');
      break;
    case 600:
      $plan_name = t('Basic Income Gold');
      break;
    case 1000:
      $plan_name = t('Basic Income Platinum');
      break;
    default:
      $plan_name = t('Basic Income Personal-@amount', array('@amount' => $amount));
      break;
  }

  $plan = \Stripe\Plan::create(array(
    "id" => $plan_id,
    "amount" => $amount,
    "currency" => $currency,
    "interval" => _recurring_billing_to_interval($recurring_billing),
    "name" => $plan_name,
  ));

  return $plan;
}

/**
 * Subscribes the registered user to the Stripe subscription plan. Returns the created Stripe customer id.
 *
 * REMARK: If the registered user is already subscribed to a plan, the old plan
 * will be deleted before the user is subscribed to the new plan.
 *
 * @param $registered_user  object
 *    The registered user object.
 * @param $plan   object
 *    The Stripe subscription plan id
 * @param $email
 * @param $source
 * @param float $app_fee_percentage
 * @return string Returns the created Stripe customer id.
 * Returns the created Stripe customer id.
 */
function stripe_button_subscribe_user_to_plan($registered_user, $plan, $email, $source, $app_fee_percentage = 0.0) {
  //
  // check, if user is already subscribed to a plan and delete it, if any
  stripe_button_delete_subscription_for_user($registered_user);

  //
  // creating a Stripe customer with a plan and a source (credit card) performs the following:
  // - Creates a customer (unique)
  // - Saves the customer’s payment information
  // - Signs the customer up for a subscription
  // - By default, Stripe bills the customer immediately for their first week of the subscription
  $customer = \Stripe\Customer::create(array(
    'email' => $email,
    'source'  => $source,
    'plan' => $plan->id
  ));

  //
  // new Stripe subscription successful, store relevant data in subscription db
  // e.g. user wants to cancel subscription
  $subscription = db_user_stripe_subscription_add($registered_user->uid, $customer->id, $plan->id, $app_fee_percentage);

  // successful subscription, inform other modules
  $params = _get_params_from_subscription($subscription, $plan);
  module_invoke_all('stripe_button_user_subscribed', $registered_user, $params);

  // return customer id (holding subscription)
  return $customer->id;
}

/**
 * Deletes the Stripe subscription for the given registered user.
 *
 * If the Stripe subscription could not be deleted due to a Stripe exception,
 * the local subscription is also not deleted. So the subscription can be deleted later.
 *
 * @param $registered_user  object
 *    The registered user object.
 */
function stripe_button_delete_subscription_for_user($registered_user) {
  $subscriptions = db_user_stripe_subscription_get_subscriptions($registered_user->uid);
  foreach ($subscriptions as $subscription) {
    $stripe_cust_id = $subscription['stripe_cust_id'];
    try {
      $customer = \Stripe\Customer::retrieve($stripe_cust_id);
      if (!$customer->deleted) {
        $customer->delete();
      }
      db_user_stripe_subscription_delete($stripe_cust_id);

      // inform other modules about the subscription deletion, if any
      $params = _get_params_from_subscription($subscription);
      module_invoke_all('stripe_button_user_unsubscribed', $registered_user, $params);
    }
    catch (Exception $e) {
      watchdog('stripe_button', 'Stripe customer @cust could not be deleted', array('@cust' => $stripe_cust_id), WATCHDOG_ERROR);
    }
  }
}

/**
 * Hook called before user is deleted.
 * Guarantees, that the Stripe subscriptions are deleted.
 *
 * @param $account
 */
function stripe_button_user_delete($account) {
  // TODO: what if Stripe subscription could not be deleted
  if (stripe_button_library_loaded ()) {
    stripe_button_delete_subscription_for_user($account);
  }
}


/* --------------------------------------------------
 *  DB methods
 * --------------------------------------------------*/
/**
 * Adds a new record to the {pxl_user_stripe_subscription} db.
 *
 * @param $uid      int       User id.
 * @param $cust_id  string    Stripe customer id (subscription).
 * @param $plan_id string
 * @param $app_fee_percentage float   The chosen fee percentage for the subscription. Default = 0.0.
 * @param $evt_id   string    Unique event id of charge.succeeded event.
 * @return array
 *    Returns the added subscription as associative array.
 */
function db_user_stripe_subscription_add($uid, $cust_id, $plan_id, $app_fee_percentage = 0.0, $evt_id = '') {
  $fields = array(
    'stripe_cust_id' => $cust_id,
    'user_id' => $uid,
    'stripe_plan_id' => $plan_id,
    'app_fee_percentage' => $app_fee_percentage,
    'stripe_evt_id' => $evt_id,
  );
  db_insert('pxl_user_stripe_subscription')
    ->fields($fields)
    ->execute();

  return $fields;
}

/**
 * Updates the subscription (Stripe customer) with the charge.succeeded event id (idempotency)
 * in the {pxl_user_stripe_subscription} db.
 *
 * @param $cust_id  string    Stripe customer id (subscription).
 * @param $evt_id   string    The unique event id to guarantee idempotency of charge events.
 */
function db_user_stripe_subscription_update_evt_id($cust_id, $evt_id = '') {
  $fields = array(
    'stripe_evt_id' => $evt_id,
  );
  db_update('pxl_user_stripe_subscription')
    ->fields($fields)
    ->condition('stripe_cust_id', $cust_id)
    ->execute();
}

/**
 * Gets all stored subscriptions for the given user id.
 *
 * @param $uid      int
 *    User id.
 * @return mixed    array
 *    Associative array of subscriptions with keys = {'stripe_cust_id', 'app_fee_percentage', 'stripe_plan_id', 'event_id'}
 *    for the given user or empty array.
 */
function db_user_stripe_subscription_get_subscriptions($uid) {
  // get related stripe customer for the given user id
  $results = db_select('pxl_user_stripe_subscription', 'p')
    ->fields('p', array('stripe_cust_id', 'app_fee_percentage', 'stripe_plan_id', 'stripe_evt_id'))
    ->condition('p.user_id', $uid)
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  return $results;
}

/**
 * Gets the subscription of the given stripe customer (only one subscription).
 * @param $cust_id
 * @return mixed  Associative array with subscription parameters, false otherwise.
 */
function db_user_stripe_subscription_get_subscription($cust_id) {
  // get related stripe customer for the given user id
  $result = db_select('pxl_user_stripe_subscription', 'p')
    ->fields('p', array('stripe_cust_id', 'user_id', 'app_fee_percentage', 'stripe_plan_id', 'stripe_evt_id'))
    ->condition('p.stripe_cust_id', $cust_id)
    ->execute()
    ->fetchAssoc();

  return $result;
}

/**
 * Deletes a given Stripe subscription (stripe_cust_id).
 *
 * @param $cust_id  string    Subscription to be deleted, e.g. the Stripe customer id.
 * @return bool   True, if deletion was successful, false otherwise.
 */
function db_user_stripe_subscription_delete($cust_id) {
  // delete stripe customer relation
  $deleted = db_delete('pxl_user_stripe_subscription')
    ->condition('stripe_cust_id', $cust_id)
    ->execute();
  return $deleted == 1;
}



/* --------------------------------------------------
 * User profile - extend profile with subscription information
 * --------------------------------------------------*/
/**
 * Alter the user profile form.
 *
 * Alter the user profile to make the subscription of a user manageable.
 */
function stripe_button_form_user_profile_form_alter (&$form, &$form_state, $form_id) {
  //
  // show subscription information in the user profile, if available
  $user = $form['#user'];
  $stripe_subscriptions = db_user_stripe_subscription_get_subscriptions($user->uid);
  if (!empty($stripe_subscriptions) && stripe_button_library_loaded()) {
    // get subscription info for user
    $header = array(
      'subscription' => array('data' => t('Subscription')),
      'created' => array('data' => t('Created')),
      'ops' => array('data' => t('Operations')),
    );
    $rows = array();
    $empty = t('No recurring payment subscription available.');
    $has_valid_subscription = false;

    try {
      foreach ($stripe_subscriptions as $stripe_subscription) {
        $stripe_cust_id = $stripe_subscription['stripe_cust_id'];
        $customer = \Stripe\Customer::retrieve(array(
            "id" => $stripe_cust_id,
            "expand" => array("subscriptions")
          )
        );
        if ($customer->deleted) {
          // cleanup db and continue
          db_user_stripe_subscription_delete($stripe_cust_id);
          continue;
        }

        // create a row per subscription (should be only one)
        foreach($customer->subscriptions->data as $index => $subscription) {
          $has_valid_subscription = true;
          $plan = $subscription->plan;
          $subscription_text = t('@plan: @amount @curr per @interval', array(
              '@plan' => $plan->name,
              '@amount' => $plan->amount/100,
              '@curr' => strtoupper($plan->currency),
              '@interval' => $plan->interval,
            )
          );

          // create delete link
          $uid = $user->uid;
          $del_subscription = 'stripe/user/' . $uid . '/delete_subscription';
          $del_link = l(t('Delete'), $del_subscription, array(
            'attributes' => array('class' => array('btn btn-default stripe-user-subscription-delete'), 'title' => t('Delete user subscription'))
          ));

          // create row
          $rows[] = array(
            'subscription' => $subscription_text,
            'created' => format_date($plan->created, 'short'),
            'ops' => $del_link,
          );
        }
      }

      // no valid stripe subscription exists, don't show subscription table
      if (!$has_valid_subscription) return;
    }
    catch (Exception $e) {
      $empty = t('Stripe subscription could not be retrieved. Try later again.');
    }

    // add subscription table
    $form['subscription'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => $empty,
    );
  }
}

/**
 * Confirmation form for the subscription deletion.
 */
function stripe_button_user_subscription_delete_confirm($form, &$form_state, $user) {
  $confirm = null;
  $form_state['subscribed_user'] = $user;

  $message = t('Are you sure you want to delete the subscription?');
  $caption = '<p>' . t('This action cannot be undone.') . '</p>';
  return confirm_form($form, $message, "user/{$user->uid}/edit", $caption, t('Delete'));
}

/**
 * Submit handler from user_subscription_delete_confirm form.
 */
function stripe_button_user_subscription_delete_confirm_submit($form, &$form_state) {
  $user = $form_state['subscribed_user'];

  // delete subscription
  if (stripe_button_library_loaded ()) {
    stripe_button_delete_subscription_for_user($user);
  }

  // redirect to pixel structure main config page
  $form_state['redirect'] = "user/{$user->uid}/edit";
}



/* --------------------------------------------------
 *  Stripe event - charge succeeded
 * --------------------------------------------------*/
/**
 * Guarantees, that each event is only processed once (idempotency) and that an
 * event triggered by a user action (pressing a stripe button to be charged) is
 * NOT performed, because in this case the charge is already completed.
 *
 * @param $stripe_evt_id    string  Stripe charge.succeeded event id.
 * @param $stripe_cust_id   string  Stripe customer id of charge.
 * @param $message  string  If event is not processed, the message contains the reason.
 * @return bool
 *    Returns TRUE, if event can be processed, FALSE otherwise (message returned with reason).
 */
function stripe_button_do_process_charge_event($stripe_evt_id, $stripe_cust_id, &$message) {
  //
  // customer id has to exist
  $message = '';
  if (empty($stripe_cust_id)) {
    $message = 'Stripe customer id is empty.';
    return false;
  }

  //
  // customer id needs valid subscription
  $subscription = db_user_stripe_subscription_get_subscription($stripe_cust_id);
  if (!$subscription) {
    $message = 'Stripe customer id has no local subscription.';
    return false;
  }

  //
  // if no event id is stored in subscription, update it and leave (event of ongoing subscription process)
  $evt_id = $subscription['stripe_evt_id'];
  if (empty($evt_id)) {
    db_user_stripe_subscription_update_evt_id($stripe_cust_id, $stripe_evt_id);
    $message = 'Local subscription updated with event id, no further processing needed.';
    return false;
  }

  //
  // check. if incoming event has been processed already (idempotency)
  if ($evt_id === $stripe_evt_id) {
    $message = 'Incoming event already processed.';
    return false;
  }

  //
  // update new event id and process event
  db_user_stripe_subscription_update_evt_id($stripe_cust_id, $stripe_evt_id);
  return true;
}

/**
 * Stripe event charge.succeeded webhook implementation.
 *
 * The implementation guarantees the following:
 * - each event is only processed once (idempotency).
 * - the event is only processed, if a customer is attached to the event and a subscription for
 *    this customer exists with a different event id stored.
 * - if no subscription or a subscription with no event id is found,
 *    the event is sent due to a user
 * @param $data
 * @param \Stripe\Event|NULL $event
 */
function stripe_button_stripe_api_webhook_charge_succeeded($data, Stripe\Event $event = NULL) {
  $time = date('d-m-Y H:i:s');
  watchdog('stripe_button', 'Webhook charge.succeeded event received: ' . $time, array(), WATCHDOG_DEBUG);

  //
  // Process charge.succeeded event for recurring payments initialized on Stripe
  // create charge params form charge.succeeded event and inform other modules about charge
  $charge = $data->object;
  $stripe_evt_id = $event ? $event->id : 'evt_' . drupal_hash_base64($time);  // create test event id in TEST mode, always perform event

  if (stripe_button_library_loaded () && $charge->object === 'charge') {
    //
    // check if event has to be processed
    $stripe_cust_id = $charge->customer;
    $message = '';
    if (!stripe_button_do_process_charge_event($stripe_evt_id, $stripe_cust_id, $message)) {
      watchdog('stripe_button', 'Webhook charge.succeeded event: @msg', array('@msg' => $message), WATCHDOG_DEBUG);
      return;
    }

    //
    // get charge parameters
    $subscription = db_user_stripe_subscription_get_subscription($stripe_cust_id);
    $params = _get_params_from_subscription($subscription);
    if (!$params) {
      watchdog('stripe_button', 'Webhook charge.succeeded event: Stripe plan could not be retrieved.', array(), WATCHDOG_DEBUG);

      // create incomplete params from charge object
      $amount = $charge->amount;
      $currency = $charge->currency;
      $stripe_fee = stripe_button_stripe_fee($amount);
      $app_fee_percentage = $subscription['app_fee_percentage'];
      $app_fee = $amount * $app_fee_percentage;
      $params = array (
        'stripe_api_mode' => strtolower(variable_get('stripe_api_mode', 'test')),
        'recurring_billing' => 'unknown',
        'currency' => $currency,
        'amount' => ($amount / 100),
        'stripe_fee' => ($stripe_fee / 100),
        'app_fee' => ($app_fee / 100),
        'stripe_cust_id' => $stripe_cust_id,
      );
    }
    module_invoke_all('stripe_button_charge_completed', $params);

  }
  else {
    watchdog('stripe_button', 'Webhook charge.succeeded event: Missing charge object: @data', array(
      '@data' => json_encode($data),
    ), WATCHDOG_ERROR);

  }
}


/* --------------------------------------------------
 *  Helper methods
 * --------------------------------------------------*/
/**
 * Transforms stripe interval string into recurring payment string.
 */
function _interval_to_recurring_billing($interval) {
  switch ($interval) {
    case 'day':
      return 'daily';
    case 'week':
      return 'weekly';
    case 'month':
    default:
      return'monthly';
    case 'year':
      return 'yearly';
  }
}

/**
 * Transforms recurring payment string into stripe interval string.
 */
function _recurring_billing_to_interval($recurring_billing) {
  switch ($recurring_billing) {
    case 'daily':
      return 'day';
    case 'weekly':
      return 'week';
    case 'monthly':
    default:
      return 'month';
    case 'yearly':
      return 'year';
  }
}

/**
 * Get the charge parameters from the subscription.
 *
 * @param $subscription   array
 *    Associative array of subscription parameter
 * @param $plan   object
 *    The subscription plan.
 * @return mixed Associative array of charge parameters or false, if Stripe plan could not be retrieved.
 * Associative array of charge parameters or false, if Stripe plan could not be retrieved.
 */
function _get_params_from_subscription($subscription, $plan = false) {
  //
  // get charge parameters from subscription
  if (!$subscription) return false;

  try {
    if (!$plan) {
      $plan_id = $subscription['stripe_plan_id'];
      $plan = \Stripe\Plan::retrieve($plan_id);
    }
    $amount = $plan->amount;
    $currency = $plan->currency;
    $stripe_fee = stripe_button_stripe_fee($amount);
    $app_fee = $amount * $subscription['app_fee_percentage'];
    $interval = $plan->interval;
    $recurring_billing = _interval_to_recurring_billing($interval);
    $stripe_cust_id = $subscription['stripe_cust_id'];

    return array(
      'stripe_api_mode' => strtolower(variable_get('stripe_api_mode', 'test')),
      'recurring_billing' => $recurring_billing,
      'currency' => $currency,
      'amount' => ($amount / 100),
      'stripe_fee' => ($stripe_fee / 100),
      'app_fee' => ($app_fee / 100),
      'stripe_cust_id' => $stripe_cust_id,
    );
  }
  catch (Exception $e) {
    return false;
  }
}
