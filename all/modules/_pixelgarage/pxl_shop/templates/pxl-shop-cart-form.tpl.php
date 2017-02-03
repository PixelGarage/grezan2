<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 30.01.17
 * Time: 17:42
 */
?>

<div id="<?php print $block_id; ?>" class="block block-shopping-cart-form">
  <!-- The form section -->
  <div class="shopping-cart-table"><?php print $shopping_cart; ?></div>

  <!-- The actions section -->
  <div class="shopping-cart-actions">
    <button type="button" class="cart-items-reset" data-item-id="all"><?php print $button_label_reset ; ?></button>
    <?php print $stripe_button; ?>
  </div>
  <div class="label label-danger"></div>

</div>


