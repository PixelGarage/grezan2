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

  <!-- The AGB checkbox -->
  <div class="shopping-cart-agb">
    <div class="form-item form-type-checkbox checkbox">
      <label class="control-label" for="edit-agb-checkbox">
        <span class="form-required" title="Required input">*</span>
        <input id="edit-agb-checkbox" name="agb_checkbox" value="1" class="form-checkbox required" type="checkbox"><?php print $agb_text; ?>
      </label>
    </div>
  </div>

  <!-- The actions section -->
  <div class="shopping-cart-actions">
    <button type="button" class="cart-items-reset" data-item-id="all"><?php print $button_label_reset ; ?></button>
    <?php print $stripe_button; ?>
  </div>
  <div class="label label-danger"></div>
</div>

