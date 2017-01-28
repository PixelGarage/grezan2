<?php
/**
 * Contains the API function of the stripe button module.
 *
 * Created: ralph
 */

/**
 * This hook allows to define all relations between a stripe button field and a fee button field.
 *
 * Use this hook to relate a stripe button field with a fee button field, so
 * that the correct fee percentage is used for the pressed stripe button.
 *
 * REMARK: Use machine names of fields, but replace '_' with '-'.
 * Delta values of fields have not to be considered.
 *
 * @param $stripe_button_relations  array
 *    Empty array to be filled with all stripe button - fee button relations.
 */
function hook_stripe_button_fee_button_relation_alter(&$stripe_button_relations) {
  $stripe_button_relations = array(
    'stripe-button-field-id' => 'fee-button-field-id'
  );
}

/**
 * This hook alters the feedback associative array to provide a feedback for each selectable fee percentage.
 * Use it to give a positive feedback to the user and explain, what the selected fee is used for.
 *
 * REMARK: Keys are the defined fee percentages (string) of a fee button.
 *
 * @param $feedbacks  array
 *    An associative array to be altered with a feedback pro selectable fee percentage.
 * @param $fee_button_id   string
 *    The id of the Stripe fee button field.
 */
function hook_stripe_button_fee_select_feedback_alter(&$feedbacks, $fee_button_id) {
  if ($fee_button_id == 'xy') {
    $feedbacks += array (
      '0.0' => t('<strong>Too bad!</strong> We are entirely financed by voluntary commission. Your contribution would make a difference.'),
      '0.05' => t('<strong>Thank you!</strong> Your contribution shows us that you appreciate our work.'),
      '0.1' => t('<strong>Wow!</strong> Your contribution allows us to keep this platform up and running.'),
      '0.2' => t('<strong>Amazing!</strong> Your contribution enables us to enhance the functionality of this platform.'),
      '0.3' => t('<strong>Absolutely awesome!</strong> We are very grateful that you honor our work so generously.'),
    );
  }
}

/**
 * This hook is called when the recurring payment subscription (Stripe) of the user is
 * successfully finished.
 * @param $user object
 *    The subscribed user object.
 * @param $params  array
 *    An associative array with the following charge parameters as key-value pairs:
 *      stripe_api_mode:  The stripe API mode, e.g. test | live.
 *      currency:         The currency of the charged amount.
 *      amount:           The charged amount in currency.
 *      stripe_fee:       The stripe fee in currency.
 *      app_fee:          The application fees in currency.
 *      recurring_billing:The recurring payment interval, e.g. daily|weekly|monthly|yearly
 *      stripe_cust_id:   The Stripe customer id holding the subscription.
 *
 */
function hook_stripe_button_user_subscribed($user, $params) {
  // update user after subscription, if needed
}

/**
 * This hook is called after the recurring payment subscription (Stripe) of the user has been deleted.
 * @param $user object
 *    The subscribed user object.
 * @param $params  mixed
 *    An associative array with the charge parameters (see hook_stripe_button_user_subscribed)
 *    or false, if no parameters are available anymore.
 */
function hook_stripe_button_user_unsubscribed($user, $params) {
  // update user after subscription, if needed
}

/**
 * This hook is called right before the charging (subscribing) takes place.
 * All charging parameters are input so that a last check can be performed.
 * To prevent  the charging, throw an exception with a corresponding message.
 *
 * @param $charge_params
 *    The associative array with the following charge parameters as key-value pairs:
 *      stripe_api_mode:  The stripe API mode, e.g. test | live.
 *      currency:         The currency of the charged amount.
 *      amount:           The charged amount in currency.
 *      stripe_fee:       The stripe fee in currency.
 *      app_fee:          The application fees in currency.
 *      recurring_billing:The recurring payment interval, e.g. daily|weekly|monthly|yearly
 * @throws \Exception
 */
function hook_stripe_button_charge_completing($charge_params) {
  if (empty($charge_params['currency'])) {
    throw new Exception('No currency set. No charge can be performed.');
  }
}

/**
 * This hook is called, after a stripe transaction has been successfully performed.
 * It's called inside a try-catch clause catching all Stripe exceptions, so no
 * special exception handling has to be done in this hook.
 *
 * An associative array with all charge parameters is transferred to the hook as input parameter.
 *
 * @param $charge_params       array
 *    The associative array with the following charge parameters as key-value pairs:
 *      stripe_api_mode:  The stripe API mode, e.g. test | live.
 *      currency:         The currency of the charged amount.
 *      amount:           The charged amount in currency.
 *      stripe_fee:       The stripe fee in currency.
 *      app_fee:          The application fees in currency.
 *      recurring_billing:The recurring payment interval, e.g. daily|weekly|monthly|yearly
 *      stripe_cust_id:   The Stripe customer id holding the subscription, not existing for single payment.
 *      charge_id:        The Stripe charge id for a single payment, not existing for a subscription.
 */
function hook_stripe_button_charge_completed($charge_params) {
  watchdog(
    'stripe_button',
    '@amount @curr charged including stripe fee = @stripe_fee @curr and application fee = @app_fee @curr',
    array(
      '@amount' => $charge_params['amount'],
      '@stripe_fee' => $charge_params['stripe_fee'],
      '@app_fee' => $charge_params['app_fee'],
      '@curr' => $charge_params['currency'],
    ), WATCHDOG_DEBUG);
}
