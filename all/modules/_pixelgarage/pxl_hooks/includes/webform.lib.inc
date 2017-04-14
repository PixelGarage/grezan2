<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.04.17
 * Time: 19:45
 */
/* --------------------------------------------------
 * Webform hooks
 * --------------------------------------------------*/

/**
 * Called for each webform submission before it is saved.
 *
 * Creates an alter hook.
 */
function pxl_hooks_webform_submission_presave($node, &$submission) {
  //
  // alter submission before saving
  drupal_alter('webform_submission_presave', $node, $submission);

}


/* --------------------------------------------------
 * Helper function for webform
 * --------------------------------------------------*/

/**
 * Helper function to get submission value for given form key.
 */
function _get_webform_submission_value($master, $submission, $form_key) {
  foreach ($master->webform['components'] as $key => $data) {
    if ($data['form_key'] == $form_key) {
      return isset($submission->data[$key]) ? $submission->data[$key][0] : false;
    }
  }
}

/**
 * Helper function to set submission value for given form key.
 */
function _set_webform_submission_value($master, $submission, $form_key, $value) {
  foreach ($master->webform['components'] as $key => $data) {
    if ($data['form_key'] == $form_key) {
      $submission->data[$key][0] = $value;
      break;
    }
  }
}

