<?php
/**
 * @file
 * Drush System Architecture Tools.
 */
/**
 * Implements hook_drush_command().
 */
function darch_drush_command() {
  $items = array();

  $items['new-module'] = array(
    'description' => 'Create a new module shell in your site\'s custom modules folder.',
    'callback' => 'drush_darch_new_module',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
    'options' => array(
      //'option-name' => 'Option description.',
    ),
    'examples' => array(
      //'example command' => 'Example description.',
    ),
    'aliases' => array('newmod'),
  );

  return $items;
}

/**
 * Create a new module folder and initial module files.
 */
function drush_darch_new_module($mn = '', $name = '') {
  drush_include(DRUSH_BASE_PATH . '/commands/pm', 'download.pm');

  // We need a name
  if (empty($mn)) {
    return drush_set_error("You forgot to name your module!");
  }

  // We need a valid name.
  if (is_numeric($mn[0]) || $mn[strlen($mn) - 1] == '/' || !preg_match('/^[a-z0-9\_\/]+$/i', $mn)) {
    return drush_set_error("Invalid module name.");
  }

  $mn = strtolower($mn);

  // @todo Check system table for conflicts?

  // Get the location of the modules directory.
  $dir = _pm_download_destination('module');

  // If we found the contrib directory, change it to custom
  if (substr($dir, -7) == 'contrib') {
    $dir = preg_replace('#contrib$#', 'custom', $dir) . "/$mn";
  }
  else {
    $dir .= "/custom/$mn";
  }

  // Check for an existing directory?
  if (is_dir($dir)) {
    return drush_set_error(dt('The module \'!module\' already exists.  Try again.', array('!module' => $mn)));
  }
  else {
    drush_mkdir($dir, TRUE);
    if (is_dir($dir)) {
      $root = dirname(__FILE__);

      // Fix name paths before creating any files in case this is a submodule.
      $path_parts = explode('/', $mn);
      $filename = array_pop($path_parts);

      if (empty($name)) {
        $name = $filename;
      }

      // Create substitution values
      $subs = array(
        '[drupal:core]' => preg_replace('/\.\d{1,2}$/', '.x', drush_drupal_version()),
        '[mod:machine-name]' => $filename,
        '[mod:name]' => $name,
      );

      // Create the info file
      $data = file("$root/templates/module_info.inc");
      $data = implode('', $data);
      $data = str_replace(array_keys($subs), array_values($subs), $data);
      drush_file_append_data("$dir/$filename.info", $data);

      // Create the module file
      $data = file("$root/templates/module_module.inc");
      $data = implode('', $data);
      $data = str_replace(array_keys($subs), array_values($subs), $data);
      drush_file_append_data("$dir/$filename.module", $data);

      drush_log(dt("Created module !name in !dir", array('!name' => $filename, '!dir' => $dir)), 'success');
    }
  }
}
