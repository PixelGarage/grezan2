<?php
/**
 * @file
 * template.php
 */

global $pixelgarage_path;
$pixelgarage_path = drupal_get_path('theme', 'pixelgarage');

include_once($pixelgarage_path . '/inc/login.inc');        // alters login forms
include_once($pixelgarage_path . '/inc/alter.inc');        // all alter hooks
include_once($pixelgarage_path . '/inc/preprocess.inc');   // all preprocess overrides
include_once($pixelgarage_path . '/inc/process.inc');      // all process overrides
include_once($pixelgarage_path . '/inc/theme.inc');        // all theme overrides
