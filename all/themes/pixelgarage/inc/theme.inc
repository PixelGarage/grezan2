<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/**
 * Bootstrap carousel image field theme override function.
 */
function pixelgarage_bootstrap_carousel_if($vars) {
  $files = array();
  foreach ($vars['items'] as $file) {
    if ($vars['background']) {
      $files[] = '<div style="width: ' . check_plain($vars['width']) . '; height: ' . $vars['height'] . '; background-position: ' . $vars['background_pos'] . '; background-size: cover; background-image: url(' . file_create_url($file['uri']) . ');"></div>';
    } else {
      $type = ($vars['image_style']) ? 'image_style' : 'image';
      $file['style_name'] = ($vars['image_style']) ? $vars['image_style'] : '';
      $file['path'] = $file['uri'];
      $files[] = theme($type, $file);
    }
  }
  $filecount = count($files);
  $output = '
      <div id="' . $vars['id'] . '" class="carousel carousel-fade slide" data-ride="carousel" data-interval="' . $vars['interval'] . '" data-pause="' . $vars['pause'] . '" data-wrap="' . $vars['wrap'] .'" data-keyboard="' . $vars['keyboard'] . '">';
  if ($vars['indicators'] && $filecount > 1) {
    $output .= '<!-- Indicators -->
        <ol class="carousel-indicators">';
    for ($i = 0; $i < $filecount; $i++) {
      if (!$i) {
        $output .= '<li data-target="#' . $vars['id'] . '" data-slide-to="0" class="active"></li>';
      } else {
        $output .= '<li data-target="#' . $vars['id'] . '" data-slide-to="' . $i . '"></li>';
      }
    }
    $output .= '</ol>';
  }

  $output .= '<div class="carousel-inner" role="listbox">';
  // Files/slides
  for ($i = 0; $i < $filecount; $i++) {
    $output .= (!$i) ? '<div class="item active">' : '<div class="item">';
    $output .= $files[$i];
    $output .= '</div>';
  }
  $output .= '</div>';
  if ($filecount > 1) {
    $output .= '<!-- Controls -->
          <a class="left carousel-control" href="#' . $vars['id'] . '" role="button" data-slide="prev">
            <span class="fa fa-angle-left" aria-hidden="true"></span>
            <span class="sr-only">Previous</span>
          </a>
          <a class="right carousel-control" href="#' . $vars['id'] . '" role="button" data-slide="next">
            <span class="fa fa-angle-right" aria-hidden="true"></span>
            <span class="sr-only">Next</span>
          </a>
    ';
  }
  $output .= '</div>';
  return $output;
}

