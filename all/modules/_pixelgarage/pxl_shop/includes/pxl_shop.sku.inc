<?php
/**
 * Contains the public functions for the SKU object.
 * TODO: Implement plugin for these functions
 *
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 29.01.17
 * Time: 16:42
 */

/**
 * Returns the SKU object-ID for the given context.
 * Use the context to evaluate the correct SKU.
 *
 * @param $sku_filters  array
 *    Array of SKU id which is used to filter view
 * @param $context  array
 *    Associative array with the following keys:
 *      - block_id: the id of the availability block.
 *      - view_name: name of view
 *      - view_display_id:  the display id of view
 *      - intl_url:  The current internal URL.
 * @return string
 *    Returns the sku id for the given context (master id of translation set).
 */
function pxl_shop_pxl_availability_filter_by_sku_alter(&$sku_filters, $context) {
  $session_data = &pxl_shop_session_data();
  $url_split = explode('/', $context['intl_url']);

  if ($url_split[0] == 'node') {
    $node = node_load($url_split[1]);
    $sku_id = isset($node->tnid) ? $node->tnid : $node->nid;
    $session_data['sku']['current_sku_id'] = $sku_id;
    $sku_filters = array($sku_id);
  }
  else if ($url_split[0] == 'ajax'){
    $sku_filters = array($session_data['sku']['current_sku_id']);
  }
}

/**
 * Retrieves the SKU object with the given sku id.
 *
 * Throws an exception, if the SKU object does not exist.
 *
 * @param $sku_id   string    The SKU node id (master id of translation set)
 * @return \stdClass  The fully populated SKU object.
 * @throws \Exception
 */
function pxl_shop_retrieve_sku($sku_id) {
  $sku = node_load($sku_id);
  if (!$sku) {
    $msg = t('No SKU with id = @skuid found', array('@skuid' => $sku_id));
    throw new Exception($msg);
  }

  //
  // get translated title, if multi-language is enabled
  if (module_exists('pxl_availability')) {
    global $language;
    $lang = $language->language;
    $translations = translation_node_get_translations($sku_id);
    $title = isset($translations[$lang]) ? $translations[$lang]->title : $sku->title;
  }
  else {
    $title = $sku->title;
  }

  //
  // create SKU object
  $sku_obj = new stdClass();
  $sku_obj->id = isset($sku->tnid) ? $sku->tnid : $sku->nid;
  $sku_obj->title = $title;
  $sku_obj->price_per_day = $sku->field_price[LANGUAGE_NONE][0]['value'];

  return $sku_obj;
}

