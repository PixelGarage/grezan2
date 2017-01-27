<?php

/**
* Field handler to present a clone node link.
*
* Closely modeled after views/modules/node/views_handler_field_node_link_edit.inc
*/
class views_handler_field_node_link_cancel_booking extends views_handler_field_node_link {
  /**
   * Renders the link.
   */
  function render_link($node, $values) {

    module_load_include('inc', 'stripe_booking', 'inc/stripe_booking.action');

    if (!cancel_booking_access($node)) {
      return;
    }

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "node/{$node->nid}/cancel_booking";
    $this->options['alter']['query'] = drupal_get_destination();

    $text = !empty($this->options['text']) ? $this->options['text'] : t('Cancel booking');
    return $text;
  }
}
