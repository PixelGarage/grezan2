<?php
/**
 * The preprocess function for the stripe button fee block.
 */

function template_preprocess_stripe_button_fee_percentage(&$vars) {
  //
  // define stripe button fee block
  $field_id = $vars['field_id'];
  $default_fee_percentage = $vars['fee_items'][$vars['default_button_index']];
  $session_data = &stripe_button_session_data();
  $session_data['fee_buttons'][$field_id] = $default_fee_percentage;

  $vars['top_text'] = !empty($vars['top_text']) ? t($vars['top_text']) : t('We are financed entirely by voluntary contributions. Please support our work with');
  $vars['bottom_text'] = !empty($vars['bottom_text']) ? t($vars['bottom_text']) : t('of your donation.');
  $vars['stripe_fee_text'] = !empty($vars['stripe_fee_text']) ? t($vars['stripe_fee_text']) : t('2.9% + 0.30 CHF payment transaction fees will always be deducted from your donation.');
  $vars['answer_text'] = '';

  //
  // add js
  $path = drupal_get_path('module', 'stripe_button');
  drupal_add_js($path . '/js/stripe_button_fee.js');
}
