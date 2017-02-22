/**
 * Created by ralph on 31.01.17.
 */

(function ($) {

  /**
   * Opens the calendar popup on form element click.
   */
  Drupal.behaviors.pxlShoppingCartEvents = {
    attach: function (context) {
      // Iterate through all availability block instances
      $.each(Drupal.settings.pxl_shop.blocks, function (block_id, settings) {
        var $block = $('#' + block_id),
          $errorLabel = $block.find('>.label'),
          $deleteButtons = $block.find('.shopping-cart-table .cart-item-delete').add('.cart-items-reset');

        //
        // submit button click
        $deleteButtons.once('button-click', function () {
          $(this).on('click', function (e) {
            var $blockContainer = $block.parent(),
              itemID = $(this).attr('data-item-id'),
              url = '/ajax/pxl-shop/cart-item/' + itemID + '/delete';

            //
            // load shopping cart with stripe payment (replace entire availability form)
            $blockContainer.load(url, function (response, status, xhr) {
              if (status == "error") {
                var msg = "Server error " + xhr.status + ": " + xhr.statusText;
                $errorLabel.html(msg);
              }
              else {
                // attach behaviours to new block content
                Drupal.attachBehaviors($blockContainer);
              }
            });

            e.preventDefault();
          });
        });
      });
    }
  }

})(jQuery);
