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

/* =============================================================================
 *   Shopping cart management
 * ========================================================================== */
/**
 * Creates shopping cart item for the given product SKU.
 *
 * @param $sku_id  string
 *    The product SKU ID (master id of translation set).
 * @param $startDate  int
 *    Unix timestamp of the start date. Only used for bookable items in a time period.
 * @param $endDate  int
 *    Unix timestamp of the end date. Only used for bookable items in a time period.
 * @return object
 *    The created shopping cart item.
 */
function pxl_shop_create_cart_item($sku_id, $startDate = null, $endDate = null) {
  // calculate time range and format dates
  $time_diff = $endDate - $startDate;
  $days = round($time_diff / 86400);
  $checkin = date('d.m.y, H:i', $startDate);
  $checkout = date('d.m.y, H:i', $endDate);

  // get sku object
  $sku_obj = pxl_shop_retrieve_sku($sku_id);
  $sku_title = $sku_obj->title;

  // create cart item
  $item = new stdClass();
  $item->id = $sku_obj->id;
  $item->title = $sku_title;
  $item->description = t('Booking of "@name" for @days days.<br>Check-in: @startDate; Check-out: @endDate', array(
      '@name' => $sku_title,
      '@startDate' => $checkin,
      '@endDate' => $checkout,
      '@days' => $days,
    )
  );
  $item->start = $startDate;
  $item->end = $endDate;
  $item->price = $sku_obj->price_per_day*$days;

  return $item;
}

/**
 * Adds a shop item to the shopping cart.
 * @param $item object
 *    Shopping cart item.
 */
function pxl_shop_cart_add_item($item) {
  $shopping_cart_items = &pxl_shop_session_data()['cart']['items'];
  $shopping_cart_items[$item->id] = $item;
}

/**
 * Removes a shop item from the shopping cart.
 *
 * @param $key  string
 *    Unique item key or 'all' to empty shopping cart.
 * @return bool
 *    True, if shopping cart is empty, false otherwise.
 */
function pxl_shop_cart_remove_item($key) {
  $shopping_cart_items = &pxl_shop_session_data()['cart']['items'];
  if ($key == 'all') {
    $shopping_cart_items = array();
  }
  else {
    unset($shopping_cart_items[$key]);
  }

  // return flag indicating if cart is empty
  return empty($shopping_cart_items);
}

/**
 * Returns all items in the shopping cart or an empty array, if no cart items are available.
 */
function pxl_shop_cart_items() {
  $shopping_cart_items = &pxl_shop_session_data()['cart']['items'];
  return $shopping_cart_items ? $shopping_cart_items : array();
}


