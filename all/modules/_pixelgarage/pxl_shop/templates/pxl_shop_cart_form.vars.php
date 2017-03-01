<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 30.01.17
 * Time: 17:41
 */

/**
 * Preprocessor for the pxl_shop_cart_form theme.
 * @param $vars
 */
function template_preprocess_pxl_shop_cart_form(&$vars) {
  // define block variables
  $vars['block_id'] = 'block-shopping-cart-form-' . $vars['id'];
  $vars['button_label_reset'] = t('Reset');
  $currency = !empty($vars['currency']) ? strtoupper($vars['currency']) : 'EUR';

  //
  // define shopping cart table
  $header = array(
    'booking' => array('data' => t('Booking')),
    'price' => array('data' => t('@curr', array('@curr' => $currency))),
    'ops' => array('data' => ''),
  );
  $rows = array();
  $amount = 0.0;
  foreach (pxl_shop_cart_items() as $key => $item) {
    $rows[$key] = array(
      'booking' => $item->description,
      'price' => $item->price,
      'ops' => '<button type="button" class="cart-item-delete" data-item-id="' . $key . '"><span class="fa fa-times"></span></button>',
    );
    $amount += floatval($item->price);
  }
  $rows['vat'] = array('booking' => t('VAT incl.'), 'price' => '0', 'ops' => '');
  $rows['total'] = array('booking' => t('Total amount'), 'price' => $amount, 'ops' => '');
  $vars['shopping_cart'] = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('The shopping cart is empty'),
  ));

  //
  // define the AGB checkbox
  global $language;
  switch ($language->language) {
    case 'de':
      $node_path = 'node/87';
      break;
    case 'fr':
      $node_path = 'node/90';
      break;
    case 'en':
    default:
      $node_path = 'node/86';
      break;
  }
  $agb_link = l(t('terms of condition'), $node_path, array('attributes' => array('target' => '_blank')));
  $vars['agb_text'] = t('I hereby confirm to agree to the !url', array('!url' => $agb_link));

  //
  // define the stripe payment button
  $description = !empty($vars['pay_dialog_text']) ? t($vars['pay_dialog_text']) : t('Pay shopping cart items');
  $button_title = !empty($vars['pay_button_title']) ? $vars['pay_button_title'] : t('Pay');
  $button_text = !empty($vars['pay_button_text']) ? $vars['pay_button_text'] : t('with your credit card');
  $stripe_settings = array(
    'name' => $vars['pay_dialog_title'],
    'description' => $description,
    'currency' => $currency,
    'buttonPrefix' => '',
    'zipCode' => 0,
    'billingAddress' => isset($vars['billing_address']) ? $vars['billing_address'] : 0,
    'shippingAddress' => 0,
    'allowRememberMe' => 0,
    'recurringBilling' => 'one-time',
  );
  $vars['stripe_button'] = theme('stripe_button_fix_value', array(
    'field_id' => 'button-pay-shopping-cart-' . $vars['id'],
    'box_title' => $button_title,
    'box_text' => $button_text,
    'amount' => $amount,
    'currency' => $currency,
    'stripe_settings' => $stripe_settings,
  ));

  //
  // add js files and settings
  $path = drupal_get_path('module', 'pxl_shop');
  drupal_add_js($path . '/js/pxl_shop.js');
  $block_id = $vars['block_id'];

  $js_settings['pxl_shop']['blocks'] = array(
    $block_id => array(),
  );
  drupal_add_js($js_settings, 'setting');
}

