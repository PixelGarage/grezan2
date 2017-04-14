<?php
/**
 * Created by PhpStorm.
 * User: ralph
 * Date: 15.03.17
 * Time: 13:07
 */

/* =============================================================================
 *   Define image styles for the imagemagick toolkit
 * ========================================================================== */
/**
 * Multiply image with a background image.
 * The background image is scaled and cropped to the image size before multiplied.
 * Transparent parts of the background image will remain transparent (composite).
 *
 * @param $image
 *   An image object. The $image->resource value will be modified by this call.
 * @param array $data
 *   The data object holding the form settings.
 *
 * @return
 *   TRUE or FALSE, based on success.
 */
function image_imagemagick_compose(stdClass $image, $compose_method, $background_path) {
  //
  // convert composite reverse order of images, so apply ops to image in brackets and switch images
  $img_path = pxl_hooks_find_file($image->source);
  $img_source = !empty($image->ops) ? '\( ' . $img_path . ' ' . implode(' ', $image->ops) . ' \)' : $img_path;
  $image->ops = array();
  $image->ops[] = $img_source;
  $image->ops[] = '-gravity center';
  $image->ops[] = '-channel RGBA';
  $image->ops[] = '-compose ' . $compose_method;
  $image->ops[] = '-composite';

  // switch image paths (order is reversed for convert composite)
  $image->source = $background_path;
  return TRUE;
}


/* =============================================================================
 *   Image styles definition
 * ========================================================================== */
/**
 * Implements hook_image_effect_info().
 */
function pxl_hooks_image_effect_info() {
  $effects['imagemagick_composition'] = array(
    'label' => t('Compose'),
    'help' => t('Compose the image with a background image.'),
    'effect callback' => 'pxl_hooks_compose_effect',
    'form callback' => 'pxl_hooks_compose_form',
  );
  return $effects;
}


/**
 * Image effect callback; Compose image with a background image.
 * The background image is scaled and cropped to the image size before composed.
 *
 * @param $image
 *   An image object returned by image_load().
 * @param array $data
 *   The data object holding the form settings.
 *
 * @return bool
 *   TRUE on success. FALSE on failure.
 *
 * @see image_imagemagick_multiply()
 */
function pxl_hooks_compose_effect($image, $data) {
  //
  // scale and crop background image and switch with source image (used for convert composite)
  $tmp_background_path = drupal_tempnam('temporary://', 'style_');
  $back = pxl_hooks_image_load($data['background_path']);
  image_scale_and_crop($back, $image->info['width'], $image->info['height']);
  if (!image_save($back, $tmp_background_path)) {
    watchdog('ImageMagick image style', 'Image multiply failed using the %toolkit toolkit on %path (%mimetype, %dimensions)', array(
      '%toolkit' => $back->toolkit,
      '%path' => $back->source,
      '%mimetype' => $back->info['mime_type'],
      '%dimensions' => $back->info['height'] . 'x' . $back->info['height'],
    ), WATCHDOG_ERROR);
    return false;
  }

  if (!image_toolkit_invoke('compose', $image, array($data['compose_method'], $tmp_background_path))) {
    watchdog('ImageMagick image style', 'Image multiply failed using the %toolkit toolkit on %path (%mimetype, %dimensions)', array(
      '%toolkit' => $image->toolkit,
      '%path' => $image->source,
      '%mimetype' => $image->info['mime_type'],
      '%dimensions' => $image->info['height'] . 'x' . $image->info['height'],
    ), WATCHDOG_ERROR);
    return FALSE;
  }
  return TRUE;
}

/**
 * Image effect form callback for the multiply effect.
 *
 * @param array $data
 *   The current configuration for this image effect.
 *
 * @return array
 *   The form definition for this effect.
 */
function pxl_hooks_compose_form($data) {
  $options = array(
    'Plus' => 'Plus',
    'Minus' => 'Minus',
    'Screen' => 'Screen',
    'Multiply' => 'Multiply',
    'Difference' => 'Difference',
    'Exclusion' => 'Exclusion',
    'Lighten' => 'Lighten',
    'Darken' => 'Darken',
    'Divide' => 'Divide',
    'LinearBurn' => 'LinearBurn',
    'ColorBurn' => 'ColorBurn',
    'LinearDodge' => 'LinearDodge',
    'ColorDodge' => 'ColorDodge',
    'Overlay' => 'Overlay',
    'SoftLight' => 'SoftLight',
  );

  $form = array(
    'help' => array(
      '#markup' => t('Compose the RGBA channels of the source image with the background image in various ways.'),
    ),
  );
  $form['compose_method'] = array(
    '#type' => 'select',
    '#title' => t('Compose method'),
    '#options' => $options,
    '#default_value' => isset($data['compose_method']) ? $data['compose_method'] : '',
    '#description' => t('Select one of the compose method.'),
  );
  $form['background_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Background image path'),
    '#default_value' => isset($data['background_path']) ? $data['background_path'] : '',
    '#description' => t('File path of the background image. Path is either defined with one of the valid schemes, an absolute path, or a relative path'),
    '#element_validate' => array('pxl_hooks_validate_file'),
  );
  return $form;
}



/* =============================================================================
 *   Helper functions
 * ========================================================================== */
/**
 * Looks up and returns the full path of the given file.
 *
 * @param string $file
 *   A file name. Accepted are:
 *   - stream wrapper notation like: private://, public://, temporary:// and
 *     module:// (the latter provided by the system stream wrapper module).
 *   - relative: relative to the current directory (probably DRUPAL_ROOT).
 *   - absolute: as is.
 *
 * @return string|false
 *   The full file path of the file, so the image toolkit knows exactly where it
 *   is. False if the file cannot be found or is not readable.
 */
function pxl_hooks_find_file($file) {
  $result = FALSE;
  if (is_readable($file)) {
    $result = drupal_realpath($file);
  }
  return $result;
}

/**
 * Loads the given file as an image object.
 *
 * @param string $file
 *   A file name, with a scheme, relative, or absolute.
 * @param bool $toolkit
 *
 * @return object|FALSE
 *   The image object.
 *
 * @see imagecache_actions_find_file()
 * @see image_load()
 */
function pxl_hooks_image_load($file, $toolkit = FALSE) {
  $full_path = pxl_hooks_find_file($file);
  if (!$full_path) {
    trigger_error("Failed to find file $file.", E_USER_ERROR);
    return FALSE;
  }

  $image = image_load($full_path, $toolkit);
  if (!$image) {
    trigger_error("Failed to open file '$file' as an image resource.", E_USER_ERROR);
  }

  return $image;
}

/**
 * Validates that the element is a positive number.
 *
 * This is a Form API #element_validate callback.
 *
 * @param array $element
 * param array $form_status
 */
function pxl_hooks_validate_number_positive(&$element/*, &$form_status*/) {
  $value = $element['#value'];
  if ($value != '') {
    if (!is_numeric($value) || (float) $value <= 0.0) {
      form_error($element, t('%name must be a positive number.', array('%name' => $element['#title'])));
    }
  }
}

/**
 * Validates that the file as specified in the element exists and is readable.
 *
 * This is a Form API #element_validate callback.
 *
 * @param array $element
 * param array $form_status
 */
function pxl_hooks_validate_file(&$element/*, &$form_status*/) {
  if (!pxl_hooks_find_file($element['#value'])) {
    form_error($element, t("Unable to find the file '%file'. Please check the path.", array('%file' => $element['#value'])));
  }
}

