<?php
/**
 * Implements the shopping cart of the Pixelgarage online shop.
 * The shopping cart holds the added items only during the active user session.
 *
 * Created by PhpStorm.
 * User: ralph
 * Date: 26.01.17
 * Time: 23:54
 */

/**
 * Creates shopping cart item for the given product SKU.
 *
 * @param $sku  string
 *    The product SKU of the item.
 * @param $startDate  int
 *    Unix timestamp of the start date. Only used for bookable items in a time period.
 * @param $endDate  int
 *    Unix timestamp of the end date. Only used for bookable items in a time period.
 * @return \stdObject
 *    The created shopping cart item.
 */
function pxl_shop_create_cart_item($sku, $startDate = null, $endDate = null) {
  $item = new stdObject();

  return $item;
}

/**
 * Adds a shop item to the shopping cart.
 * @param $key  string
 *    Unique item key.
 * @param $item object
 *    Shopping cart item.
 */
function pxl_shop_cart_add_item($key, $item) {
  $shopping_cart = &pxl_shop_session_data()['shopping_cart'];
  $shopping_cart[$key] = $item;
}

/**
 * Removes a shop item from the shopping cart.
 *
 * @param $key  string
 *    Unique item key.
 */
function pxl_shop_cart_remove_item($key) {
  $shopping_cart = &pxl_shop_session_data()['shopping_cart'];
  unset($shopping_cart[$key]);
}
