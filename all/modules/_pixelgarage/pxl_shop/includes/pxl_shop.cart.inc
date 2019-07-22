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
 * Creates shopping cart item for the given product id.
 *
 * @param $sku_id     string  The product SKU ID (master id of translation set).
 * @param $startDate  int     Unix timestamp of the start date in seconds.
 * @param $endDate    int     Unix timestamp of the end date in seconds.
 *
 * @return object| null
 *    The created shopping cart item, if successful, otherwise null.
 */
function pxl_shop_create_cart_item($sku_id, $startDate = null, $endDate = null) {
  // calculate time range and format dates
  $time_diff = $endDate - $startDate;
  $days = round($time_diff / 86400);
  $checkin = date('d.m.y, H:i', $startDate);
  $checkout = date('d.m.y, H:i', $endDate);

  // get sku object
  $item = null;
  try {
    $product = pxl_shop_get_product($sku_id);
    $product_title = $product->title;

    // create cart item
    $item = new stdClass();
    $item->id = $product->id;
    $item->title = $product_title;
    $item->description = t('Booking of @name for @days days.<br>Check-in: @startDate; Check-out: @endDate', array(
        '@name' => $product_title,
        '@startDate' => $checkin,
        '@endDate' => $checkout,
        '@days' => $days,
      )
    );
    $item->start = $startDate;
    $item->end = $endDate;
    $item->price = $product->price_per_day*$days;

  }
  catch (Exception $e) {
    watchdog('pxl_shop', 'Error: ' . $e->getMessage(), array(), WATCHDOG_DEBUG);
  }
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
 * @param $key  string  Unique item key or 'all' to empty shopping cart.
 * @return bool  True, if shopping cart is empty, false otherwise.
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


