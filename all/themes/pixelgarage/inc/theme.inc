<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 06.02.15
 * Time: 17:22
 */

/* =============================================================================
 *      Language block theme
 * ========================================================================== */

function pixelgarage_links__locale_block(&$vars) {
  foreach($vars['links'] as $language => $langInfo) {
    $abbr = $langInfo['language']->language;
    $vars['links'][$language]['title'] = substr($abbr, 0, 2);
  }
  $content = theme_links($vars);
  return $content;
}


/* =============================================================================
 *      Image field carousel
 * ========================================================================== */

/**
 * Bootstrap carousel image field theme override function.
 */
function pixelgarage_bootstrap_carousel_if($vars) {
  $files = array();
  $indicators_img = array();

  foreach ($vars['items'] as $file) {
    $file_path = ($vars['image_style']) ? image_style_url($vars['image_style'], $file['uri']) : file_create_url($file['uri']);
    $indicators_img[] = $file_path;

    if ($vars['background']) {
      $files[] = '<div style="width: ' . check_plain($vars['width']) . '; height: ' . $vars['height'] . '; background-position: ' . $vars['background_pos'] . '; background-size: cover; background-image: url(' . $file_path . ');"></div>';
    }
    else {
      $type = ($vars['image_style']) ? 'image_style' : 'image';
      $file['style_name'] = ($vars['image_style']) ? $vars['image_style'] : '';
      $file['path'] = $file['uri'];
      $files[] = theme($type, $file);
    }
  }

  $filecount = count($files);
  $img_width = 100 / $filecount;

  //
  // create carousel
  $output = '<div id="' . $vars['id'] . '" class="carousel carousel-fade slide" data-ride="carousel" data-interval="' . $vars['interval'] .
    '" data-pause="' . $vars['pause'] . '" data-wrap="' . $vars['wrap'] .'" data-keyboard="' . $vars['keyboard'] . '">';
  $output .= '<div class="carousel-inner" role="listbox">';
  // Files/slides
  for ($i = 0; $i < $filecount; $i++) {
    $output .= (!$i) ? '<div class="item active">' : '<div class="item">';
    $output .= $files[$i];
    $output .= '</div>';
  }
  $output .= '</div>';

  if ($vars['indicators'] && $filecount > 1) {
    $output .= '<!-- Indicators -->
        <ol class="carousel-indicators">';
    for ($i = 0; $i < $filecount; $i++) {
      if (!$i) {
        $output .= '<li data-target="#' . $vars['id'] . '" data-slide-to="0" class="active" style="overflow: hidden; width: ' . $img_width . '%; height: 0; padding-bottom: 9%"><img src="' . $indicators_img[$i] . '"></li>';
      } else {
        $output .= '<li data-target="#' . $vars['id'] . '" data-slide-to="' . $i . '" style="overflow: hidden; width: ' . $img_width . '%; height: 0; padding-bottom: 9%"><img src="' . $indicators_img[$i] . '"></li>';
      }
    }
    $output .= '</ol>';
  }

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

