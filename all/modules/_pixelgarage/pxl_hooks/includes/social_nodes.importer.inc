<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.04.17
 * Time: 19:41
 */

/* --------------------------------------------------
 * Social media node importer
 * (see Twitter and Instagram_social_feed modules)
 * --------------------------------------------------*/
/**
 * Saves all imported tweets to a Tweet node.
 *
 * @param array $statuses
 *   The statuses that were saved.
 * @param object $account
 *   User account object.
 *
 * @see twitter_fetch_user_timeline()
 */
function pxl_hooks_twitter_insert_statuses($statuses, $account) {
  //
  // import all new tweets (statuses) of an account into tweet nodes
  $full_name = $account->name;
  $screen_name = $account->screen_name;
  $filename = $screen_name . '_profile_image';
  $profile_image = _import_picture($filename, $account->profile_image_url);

  foreach ($statuses as $tweet) {
    //
    // check if tweet is already imported
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'tweet')
      ->fieldCondition('field_twitter_id', 'value', $tweet->id);
    $result = $query->execute();

    // import new tweets
    if (!isset($result['node'])) {
      $node = new stdClass();
      $node->type = 'tweet';
      $node->language = LANGUAGE_NONE;
      $node->uid = 1;
      $node->status = 1;
      node_object_prepare($node);

      // assign all fields
      $node->title = $full_name;
      $node->field_profile_image[LANGUAGE_NONE][0]['fid'] = $profile_image->fid;
      $node->field_twitter_id[LANGUAGE_NONE][0]['value'] = $tweet->id;
      $node->field_screen_name[LANGUAGE_NONE][0]['value'] = $screen_name;
      $node->field_created_time[LANGUAGE_NONE][0]['value'] = strtotime($tweet->created_at);
      $node->created = strtotime($tweet->created_at);
      $node->body[LANGUAGE_NONE][0]['value'] = $tweet->text;

      // save node
      $node = node_submit($node);
      node_save($node);
    }
  }

  // delete tweet nodes older than configured time
  if ($age = variable_get('twitter_expire', 0)) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'tweet')
      ->fieldCondition('field_screen_name', 'value', $screen_name)
      ->fieldCondition('field_created_time', 'value', REQUEST_TIME - $age, '<');
    $result = $query->execute();

    if (isset($result['node'])) {
      node_delete_multiple(array_keys($result['node']));
    }
  }

}


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
function pxl_hooks_instagram_media_save($type, $item) {
  //
  // save only image (no video)
  if ($type != 'image') return;

  $filename = $type . '-' . $item->instagram_id;
  $picture = _import_picture($filename, $item->standard_resolution);

  $node = new stdClass();
  $node->type = 'instagram';
  $node->language = LANGUAGE_NONE;
  $node->uid = 1;
  $node->status = 1;
  node_object_prepare($node);

  // assign all fields
  $node->title = $filename;
  $node->field_post_id[LANGUAGE_NONE][0]['value'] = $item->instagram_id;
  $node->field_picture[LANGUAGE_NONE][0]['fid'] = $picture->fid;
  $node->field_created_time[LANGUAGE_NONE][0]['value'] = $item->created_time;
  $node->created = $item->created_time;
  $node->field_link[LANGUAGE_NONE][0]['url'] = $item->instagram_link;
  $node->field_instagram_user[LANGUAGE_NONE][0]['value'] = $item->instagram_user;
  $node->field_instagram_tags[LANGUAGE_NONE][0]['value'] = $item->tags;
  $node->body[LANGUAGE_NONE][0]['value'] = $item->caption;

  // save node
  $node = node_submit($node);
  node_save($node);
}


function _import_picture($unique_name, $uri) {
  // download file and manage it
  $ext = '.' . pathinfo($uri, PATHINFO_EXTENSION);
  $filename = $unique_name . $ext;

  // get or create user picture object
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'file')
    ->propertyCondition('filename', $filename)
    ->range(0, 1);
  $result = $query->execute();

  if (isset($result['file'])) {
    // get existing
    $file = file_load(key($result['file']));

  } else {
    $file = system_retrieve_file($uri, 'public://images/' . $filename, TRUE, FILE_EXISTS_REPLACE);
  }
  return $file;
}
