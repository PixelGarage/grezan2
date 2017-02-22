<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:23
 */

/**
 * Preprocess variables for the html template.
 */
function pixelgarage_preprocess_html(&$vars) {
  // make sure jQuery UI and effects is loaded for anonymous users
  drupal_add_library('system', 'ui');
  drupal_add_library('system', 'effects');

}

/**
 * Override or insert variables for the page templates.
 */
function pixelgarage_preprocess_page(&$vars) {
  // hide titles on login forms
  pg_login_preprocess_page($vars);

  //$vars['logo'] = str_replace(array('.jpg', '.png'), '.svg', $vars['logo']);
}


function pixelgarage_preprocess_node(&$vars) {
  //
  // set css class on node, if available
  $node = $vars['node'];
  if (isset($node->field_css_class) && !empty($node->field_css_class)) {
    $vars['classes_array'][] = drupal_clean_css_identifier($node->field_css_class[LANGUAGE_NONE][0]['value']);
  }
}


