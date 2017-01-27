<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/**
 * Alters the menu entries.
 * @param $items
 */
function pixelgarage_menu_alter(&$items) {
  // hides local task tabs
  pg_login_menu_alter($items);
}

/**
 * Implements hook_pxl_availability_view_args_alter().
 */
function pixelgarage_pxl_availability_filter_by_sku_alter(&$filter, $context) {
  //
  // filter the calendar events by the attached product SKU of the displayed node
  $filter = array(null);
}


