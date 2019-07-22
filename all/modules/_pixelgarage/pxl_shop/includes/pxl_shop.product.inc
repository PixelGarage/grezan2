<?php
/**
 * Contains product related methods.
 * TODO: Implement plugin for these functions
 *
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 29.01.17
 * Time: 16:42
 */

/**
 * Return the product-ID's for the given calendar view context.
 * The view is filtered by these products.
 *
 * @param $product_filter  array
 *                          Array of product id which is used to filter calendar view
 * @param $context          array
 *                          Associative array with the following keys:
 *                          - block_id: the id of the availability block.
 *                          - view_name: name of view
 *                          - view_display_id:  the display id of view
 *                          - intl_url:  The current internal URL.
 *
 * @return void Returns the filter array filled with all product id's for the given context.
 */
function pxl_shop_pxl_availability_filter_by_product_alter(&$product_filter, $context) {
  $session_data = &pxl_shop_session_data();
  $url_split = explode('/', $context['intl_url']);

  if ($url_split[0] == 'node') {
    //
    // called when product page is loaded
    $node = node_load($url_split[1]);
    $sku_id = isset($node->tnid) ? $node->tnid : $node->nid;
    $session_data['sku']['current_sku_id'] = $sku_id;
    $product_filter = array($sku_id);

    // clear shopping cart on each product page change (only one product at the time)
    pxl_shop_cart_remove_item('all');
  }
  else if ($url_split[0] == 'ajax'){
    //
    // called when availability form is reloaded after emptying the shopping cart
    $product_filter = array($session_data['sku']['current_sku_id']);
  }
}

/**
 * Get the product with the given sku id.
 *
 * Throws an exception, if the product does not exist.
 *
 * @param $sku_id   string    The product id (master id of translation set)
 * @return \stdClass  The fully populated product.
 * @throws \Exception
 */
function pxl_shop_get_product($sku_id) {
  $node = node_load($sku_id);
  if (!$node) {
    $msg = t('No product node with id = @skuid found', array('@skuid' => $sku_id));
    throw new Exception($msg);
  }

  //
  // get translated title, if multi-language is enabled
  if (module_exists('pxl_availability')) {
    global $language;
    $lang = $language->language;
    $translations = translation_node_get_translations($sku_id);
    $title = isset($translations[$lang]) ? $translations[$lang]->title : $node->title;
  }
  else {
    $title = $node->title;
  }

  //
  // create SKU object
  $product = new stdClass();
  $product->id = isset($node->tnid) ? $node->tnid : $node->nid;
  $product->title = $title;
  $product->price_per_day = $node->field_price[LANGUAGE_NONE][0]['value'];

  return $product;
}

