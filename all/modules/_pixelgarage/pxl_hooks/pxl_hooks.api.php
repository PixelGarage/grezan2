<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.04.17
 * Time: 19:53
 */

/**
 * Alter the webform submission, before it is saved.
 *
 * @param $node   object
 *   The webform node.
 * @param $submission   object
 *   The Webform submission that is about to be saved to the database.
 */
function hook_webform_submission_presave_alter($node, &$submission) {
  //
  // alter submission before saving
}
