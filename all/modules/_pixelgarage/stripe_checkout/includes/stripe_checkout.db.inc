<?php
/** --------------------------------------------------
 *  DB methods
 * --------------------------------------------------*/


/**
 * Adds a new record to the {pxl_user_stripe_subscription} db.
 *
 * @param $uid                int       User id.
 * @param $plan_id            string    Stripe subscription plan id
 * @param $user_mail          string    The user mail.
 * @param $app_fee_percentage float     The chosen fee percentage for the subscription. Default = 0.0.
 * @param $cust_id            string    Stripe customer id if any. Default: empty
 *
 * @return array
 *    Returns the added subscription as associative array.
 * @throws \Exception
 */
function pxl_user_stripe_subscription_add($uid, $plan_id, $user_mail, $app_fee_percentage = 0.0, $cust_id = '') {
  $fields = array(
    'user_id' => $uid,
    'stripe_cust_id' => $cust_id,
    'stripe_plan_id' => $plan_id,
    'app_fee_percentage' => $app_fee_percentage,
    'stripe_evt_id' => $user_mail,
  );
  db_insert('pxl_user_stripe_subscription')
    ->fields($fields)
    ->execute();

  return $fields;
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
function pxl_user_stripe_subscription_get_subscriptions($uid) {
  // get related stripe customer for the given user id
  $results = db_select('pxl_user_stripe_subscription', 'p')
    ->fields('p', array('stripe_cust_id', 'app_fee_percentage', 'stripe_plan_id', 'stripe_evt_id'))
    ->condition('p.user_id', $uid)
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);

  return $results;
}

/**
 * Gets the internal subscription of the given stripe customer.
 *
 * REMARK: THe combination of customer_id and user email is a unique key because
 * customer id is unique, user email is unique and one of them is always available in a subscription
 *
 * @param $cust_id
 * @return mixed  Associative array with subscription parameters, false otherwise.
 */
function pxl_user_stripe_subscription_get_subscription($cust_id, $cust_email) {
  // get the subscription for the given stripe customer
  $result = db_select('pxl_user_stripe_subscription', 'p')
    ->fields('p', array('stripe_cust_id', 'user_id', 'app_fee_percentage', 'stripe_plan_id', 'stripe_evt_id'))
    ->condition(db_or()
      ->condition('stripe_cust_id', $cust_id)
      ->condition('stripe_evt_id', $cust_email))
    ->execute()
    ->fetchAssoc();

  return $result;
}

/**
 * Updates the internal subscription with the Stripe customer id and evt_id.
 *
 * REMARK: THe combination of customer_id and user email is a unique key because
 * customer id is unique, user email is unique and one of them is always available in a subscription
 *
 * @param $cust_id   string   The Stripe customer id.
 * @param $cust_email string  The user email
 * @param $evt_id    string   The Stripe event id.
 */
function pxl_user_stripe_subscription_update_subscription($cust_id, $cust_email, $evt_id) {
  $fields = array(
    'stripe_cust_id' => $cust_id,
    'stripe_evt_id' => $evt_id,
  );
  db_update('pxl_user_stripe_subscription')
    ->fields($fields)
    ->condition(db_or()
      ->condition('stripe_cust_id', $cust_id)
      ->condition('stripe_evt_id', $cust_email))
    ->execute();
}

/**
 * Deletes the subscription with the given stripe_cust_id.
 *
 * @param $stripe_cust_id  string    The Stripe customer ID.
 *
 * @return int Number of rows that has been deleted
 */
function pxl_user_stripe_subscription_delete($stripe_cust_id) {
  // delete subscription with given stripe customer (unique)
  $deleted = db_delete('pxl_user_stripe_subscription')
    ->condition('stripe_cust_id', $stripe_cust_id)
    ->execute();
  return $deleted;
}

/**
 * Cleanup the internal subscriptions for the given user. All entries with
 * an empty stripe customer are deleted.
 *
 * @param $user_id int Internal user ID (uid)
 */
function pxl_user_stripe_subscription_cleanup($user_id) {
  // delete all subscriptions without a related stripe customer of given user.
  $deleted = db_delete('pxl_user_stripe_subscription')
    ->condition(db_and()
      ->condition('user_id', $user_id)
      ->condition('stripe_cust_id', ''))
    ->execute();
  return $deleted;
}



