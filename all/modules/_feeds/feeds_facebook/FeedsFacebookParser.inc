<?php
/**
 * @file
 * Facebook feeds parser class.
 */

/**
 * Class definition for Facebook Parser.
 */
class FeedsFacebookParser extends FeedsParser {

  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    $facebook_feed = $fetcher_result->getRaw();
    $result = new FeedsParserResult();

    $json = json_decode($facebook_feed);

    $items = array();

    foreach ($json->data as $entry) {
      if (!isset($entry->to)) {
        //
        // support for facebook cached images (safe_image.php)
        $image_url = isset($entry->full_picture) ? $entry->full_picture : '';
        if ($image_url && $pos = strpos($image_url, 'safe_image.php')) {
          // get the direct image url
          $pos = strpos($entry->full_picture, 'url=') + 4;
          $image_url = rawurldecode(substr($entry->full_picture, $pos));
        }

        //
        // support for facebook videos
        // use link field as video source, source field contains link to cache in form https://scontent.xx.fbcdn.net/hads-xaf1/v/...
        // or https://video.xx.fbcdn.net/hvideo-xtl1/v/...
        $source = isset($entry->source) ? $entry->source : '';
        $link = isset($entry->link) ? $entry->link : '';
        if ($link && $source && strpos($source, 'xx.fbcdn.net') > 0) {
          $source = $link;
          $link = '';
        }

        //
        // add the corrected parsed item
        $items[] = array(
          'post_id' => $entry->id,
          'from_name' => $entry->from->name,
          'from_category' => (isset($entry->from->category) ? $entry->from->category : ''),
          'from_id' => $entry->from->id,
          'to_name' => (isset($entry->to) ? $entry->to->data[0]->name : ''),
          'to_category' => (isset($entry->to) ? $entry->to->data[0]->category : ''),
          'to_id' => (isset($entry->to) ? $entry->to->data[0]->id : ''),
          'message' => (isset($entry->message) ? $entry->message : ''),
          'picture' => (isset($entry->picture) ? $entry->picture : ''),
          'full_picture' => $image_url,
          'link' => $link,
          'source' => $source,
          'name' => (isset($entry->name) ? $entry->name : ''),
          'caption' => (isset($entry->caption) ? $entry->caption : ''),
          'description' => (isset($entry->description) ? $entry->description : ''),
          'icon' => (isset($entry->icon) ? $entry->icon : ''),
          'type' => (isset($entry->type) ? $entry->type : ''),
          'likes_count' => (isset($entry->likes) ? count($entry->likes->data) : 0),
          'likes' => (isset($entry->likes) ? json_encode($entry->likes) : ''),
          'comments_count' => (isset($entry->comments) ? $entry->comments->count : 0),
          'comments' => (isset($entry->comments) ? json_encode($entry->comments) : ''),
          'created_time' => (isset($entry->created_time) ? $entry->created_time : ''),
          'updated_time' => (isset($entry->updated_time) ? $entry->updated_time : ''),
        );
      }
    }
    $result->items = $items;
    return $result;
  }

  public function getMappingSources() {
    return parent::getMappingSources() + array(
      'post_id' => array(
        'name' => t('Post id'),
        'description' => t('Facebook post id'),
      ),
      'from_name' => array(
        'name' => t('From: name'),
        'description' => t(''),
      ),      
      'from_category' => array(
        'name' => t('From: category'),
        'description' => t(''),
      ),
      'from_id' => array(
        'name' => t('From: id'),
        'description' => t(''),
      ),
      'to_name' => array(
        'name' => t('To: name'),
        'description' => t(''),
      ),      
      'to_category' => array(
        'name' => t('To: category'),
        'description' => t(''),
      ),
      'to_id' => array(
        'name' => t('To: id'),
        'description' => t(''),
      ),  
      'message' => array(
        'name' => t('Message'),
        'description' => t(''),
      ),
      'picture' => array(
        'name' => t('Picture'),
        'description' => t('Picture or screenshot of the post'),
      ),
      'full_picture' => array(
        'name' => t('Full Picture'),
        'description' => t('Full picture or screenshot of the post'),
      ),
      'name' => array(
        'name' => t('Name'),
        'description' => t('Post name, could be a file name'),
      ),
      'link' => array(
        'name' => t('Link'),
        'description' => t(''),
      ),
      'source' => array(
        'name' => t('Source'),
        'description' => t('Music file source'),
      ),      
      'description' => array(
        'name' => t('Description'),
        'description' => t(''),
      ),
      'icon' => array(
        'name' => t('Icon'),
        'description' => t(''),
      ),
      'type' => array(
        'name' => t('Type'),
        'description' => t('Type of post'),
      ),
      'likes' => array(
        'name' => t('Likes'),
        'description' => t('Likes json encoded'),
      ),
      'likes_count' => array(
        'name' => t('Likes count'),
        'description' => t('Number of likes'),
      ),      
      'comments' => array(
        'name' => t('Comments'),
        'description' => t('Comments json encoded'),
      ),
      'comments_count' => array(
        'name' => t('Comments count'),
        'description' => t('Number of comments'),
      ),            
      'created_time' => array(
        'name' => t('Created time'),
        'description' => t('Date/time'),
      ),
      'updated_time' => array(
        'name' => t('Updated time'),
        'description' => t('Date/time'),
      ),
    );
  } 
}
