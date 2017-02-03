<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 07.12.16
 * Time: 21:28
 */
?>

<div id="<?php print $block_id; ?>" class="block block-availability-form">
  <!-- The form section -->
  <div class="availability-form-container"><?php print drupal_render($availability_form); ?></div>

  <!-- Modal dialog containing the calendar -->
  <div id="calendar-modal-<?php print $id; ?>" class="modal" tabindex="-1" role="dialog" aria-labelledby="pe-modal-label" aria-hidden="true">
    <div class="modal-dialog" role="document">
      <div class="modal-content">
        <!-- Header -->
        <div class="modal-header">
          <span class="label label-danger"></span>
          <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
        </div>
        <!-- Body -->
        <div class="modal-body">
          <?php if ($calendar) print $calendar ; ?>
        </div>
      </div>
    </div>
  </div>

</div>

