<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.12.16
 * Time: 21:29
 */

/**
 * Preprocessor for the pxl_availability_form theme.
 * @param $vars
 */
function template_preprocess_pxl_availability_form(&$vars) {
  //
  // set block variables
  $block_id = 'block-availability-form-' . $vars['id'];
  $vars['block_id'] = $block_id;

  //
  // create the form for the availability check
  $form = drupal_get_form('pxl_availability_check_form');
  $vars['availability_form'] = $form;

  //
  // render calendar view with customizable contextual filter
  $view_array = explode(':', $vars['calendar_view']);
  $context['block_id'] = $block_id;
  $context['view_name'] = $view_array[0];
  $context['view_display_id'] = $view_array[1];
  $context['intl_url'] = $_GET['q'];
  $sku_filters = array();
  drupal_alter('pxl_availability_filter_by_sku', $sku_filters, $context);
  $vars['calendar'] = views_embed_view($context['view_name'], $context['view_display_id'], !empty($sku_filters) ? $sku_filters[0] : null);

  //
  // define hidden calendar days 0=Sunday,...until 6=Saturday
  $vars['selectable_days'] = !empty($vars['selectable_days']) ? $vars['selectable_days'] : array(0,1,2,3,4,5,6);
  $hidden_days = array_diff(array(0,1,2,3,4,5,6), $vars['selectable_days']);
  // add js files and settings
  $path = drupal_get_path('module', 'pxl_availability');
  drupal_add_js($path . '/js/pxl_availability.js');

  $js_settings['pxl_availability']['blocks'] = array(
    $block_id => array(
      'check_in_time' => $vars['check_in_time'],
      'check_out_time' => $vars['check_out_time'],
      'min_days' => $vars['min_days'],
      'hidden_days' => $hidden_days,
      'SKUs' => $sku_filters,
    ),
  );
  $js_settings['pxl_availability']['errorNoAvailability'] = t('The selected time range is not available.');
  $js_settings['pxl_availability']['errorInPast'] = t('The selected time range cannot be in the past.');
  $js_settings['pxl_availability']['errorMinDays'] = t('The minimal selectable time range must include at least @days days.', array('@days'=>$vars['min_days']));
  drupal_add_js($js_settings, 'setting');

  // render shopping cart form to add all needed js settings and files
  $block = pxl_shop_block_view('pxl_shop_cart_form');
  drupal_render($block);

}


/**
 * Creates the availability form presenting a start and end date.
 */
function pxl_availability_check_form($form, &$form_state) {
  // add wrapper to entire form
  $form['pxl_availability_start_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Start date'),
    '#description' => t('Enter the start date.'),
    '#element_validation' => array('pxl_availability_validate_date'),
    '#attributes' => array('placeholder' => t('Check-in ...')),
    '#required' => true,
    '#weight' => 0,
  );
  $form['pxl_availability_end_date'] = array(
    '#type' => 'textfield',
    '#title' => t('End date'),
    '#description' => t('Enter the end date.'),
    '#element_validation' => array('pxl_availability_validate_date'),
    '#attributes' => array('placeholder' => t('Check-out ...')),
    '#required' => true,
    '#weight' => 1,
  );
  $form['pxl_availability_submit'] = array(
    '#type' => 'button',
    '#value' => t('Checkout'),
    '#weight' => 10,
  );

  return $form;
}

