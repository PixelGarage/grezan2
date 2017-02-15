<?php
/**
 * @file
 * View theme template to layout the proximity items and add the bootstrap modal dialog functionality to each item.
 */
?>

<div id="<?php print $pe_container_id; ?>" class="pe-container">
  <div class="pe-background-image"></div>
  <div class="grid-sizer"></div>
  <div class="gutter-sizer"></div>
  <div class="stamp stamp1"></div>

  <?php foreach ($rows as $id => $row): ?>

    <div class="pe-item pe-item-ajax <?php print 'pe-item-' . $ajax_load_params[$id]; ?> <?php if ($classes_array[$id]) print $classes_array[$id]; ?>" style="margin: 0 -2px">
      <div class="pe-item-inner">
        <!-- modal trigger -->
        <a class="button" role="button" href="/node/<?php print $ajax_load_params[$id]; ?>" <?php print drupal_attributes($toggle_attributes); ?>>
          <?php print $row; ?>
        </a>
      </div>
    </div>

  <?php endforeach; ?>


</div>
