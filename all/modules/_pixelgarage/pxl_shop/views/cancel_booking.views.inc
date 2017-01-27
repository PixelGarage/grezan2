<?php

/**
* Implements hook_views_data_alter()
*/
function pxl_shop_views_data_alter(&$views_data) {
  $views_data['node']['cancel_booking'] = array(
    'field' => array(
      'title' => t('Cancel booking'),
      'help' => t('Provide a simple link to cancel and refund a booking.'),
      'handler' => 'views_handler_field_node_link_cancel_booking',
    ),
  );
}
