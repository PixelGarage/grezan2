<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 22.09.16
 * Time: 18:17
 */

/**
 * Notifies of a newly saved instagram media item.
 *
 * @param $type  string
 *    The type of the instagram media (image, video)
 * @param $item
 *    The instagram media item object
 *   stdClass containing the instagram media item.
 * @see https://www.instagram.com/developer/endpoints/media/ for details about the contents of $item.
 */
function hook_instagram_media_save($type, $item) {
  //
  // add a node for all new items
  $node = new stdClass();
  $node->type = 'instagram';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->status = 1;
  node_object_prepare($node);

  // assign all fields
  $node->body[LANGUAGE_NONE][0]['value'] = $item->caption;

  // save node
  $node = node_submit($node);
  node_save($node);

}

