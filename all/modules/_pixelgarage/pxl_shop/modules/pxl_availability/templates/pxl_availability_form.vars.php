<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.12.16
 * Time: 21:29
 */
function template_preprocess_pxl_availability_form(&$vars) {
  //
  // create the form for the availability check
  $session_data = &pxl_availability_session_data();
  $session_data['placeholder_start'] = $vars['start_placeholder'];
  $session_data['placeholder_end'] = $vars['end_placeholder'];
  $form = drupal_get_form('pxl_availability_check_form');
  $vars['availability_form'] = $form;

  //
  // render calendar view with customizable contextual filter
  $block_id = $vars['block_id'];
  $view_array = explode(':', $vars['calendar_view']);
  $context['block_id'] = $block_id;
  $context['view_name'] = $view_array[0];
  $context['view_display_id'] = $view_array[1];
  $sku_filters = array(null);

  // get filter for specific context and render view
  drupal_alter('pxl_availability_filter_by_sku', $sku_filters, $context);
  $vars['calendar'] = views_embed_view($context['view_name'], $context['view_display_id'], $sku_filters[0]);
  //$results = views_get_view_result($view_name, $view_display_id, $view_args[0]);
  if (is_null($sku_filters[0])) {
    // TODO: get all product SKUs and add them to the $sku_filters array (replace null)
  }

  //
  // add js files and settings
  $path = drupal_get_path('module', 'pxl_availability');
  drupal_add_js($path . '/js/pxl_availability.js');

  $js_settings['pxl_availability']['blocks'] = array(
    $block_id => array(
      'selectable_days' => $vars['selectable_days'],
      'SKUs' => $sku_filters,
    ),
  );
  $js_settings['pxl_availability']['error_no_availability'] = t('No availability for the selected time range.');
  $js_settings['pxl_availability']['error_in_past'] = t('The selected time range is in the past.');
  drupal_add_js($js_settings, 'setting');

}


/**
 * Creates the availability form presenting a start and end date.
 */
function pxl_availability_check_form($form, &$form_state) {
  // get session data
  $session_data = &pxl_availability_session_data();

  // add wrapper to entire form
  $form['pxl_availability_start_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Start date'),
    '#description' => t('Enter the start date of the period to be checked for availability.'),
    '#element_validation' => array('pxl_availability_validate_date'),
    '#attributes' => array('placeholder' => $session_data['placeholder_start']),
    '#required' => true,
    '#weight' => 0,
  );
  $form['pxl_availability_end_date'] = array(
    '#type' => 'textfield',
    '#title' => t('End date'),
    '#description' => t('Enter the end date of the period to be checked for availability.'),
    '#element_validation' => array('pxl_availability_validate_date'),
    '#attributes' => array('placeholder' => $session_data['placeholder_end']),
    '#required' => true,
    '#weight' => 1,
  );
  $form['pxl_availability_submit'] = array(
    '#type' => 'button',
    '#value' => t('Book'),
    '#weight' => 10,
  );

  return $form;
}

/**
 * Performs the availability check of the given period.
 *
 * @param $form
 * @param $form_state
 */
function pxl_availability_check_form_submit($form, &$form_state) {
  $startDate = $form_state['values']['pxl_availability_start_date'];

}
