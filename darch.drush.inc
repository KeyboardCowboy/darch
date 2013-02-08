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
function drush_darch_new_module($name = '') {
  drush_include(DRUSH_BASE_PATH . '/commands/pm', 'download.pm');

  // We need a name
  if (empty($name)) {
    return drush_set_error("You forgot to name your module!");
  }

  // We need a valid name.
  if (is_numeric($name[0]) || !preg_match('/^[a-z0-9\_]+$/i', $name)) {
    return drush_set_error("Invalid module name.");
  }

  $name = strtolower($name);

  // @todo Check system table for conflicts?

  // Get the location of the modules directory.
  $dir = _pm_download_destination('module');

  // If we found the contrib directory, change it to custom
  if (substr($dir, -7) == 'contrib') {
    $dir = preg_replace('#contrib$#', 'custom', $dir) . "/$name";
  }
  else {
    $dir .= "/custom/$name";
  }

  // Check for an existing directory?
  if (is_dir($dir)) {
    return drush_set_error(dt('The module \'!module\' already exists.  Try again.', array('!module' => $name)));
  }
  else {
    drush_mkdir($dir, TRUE);
    if (is_dir($dir)) {
      $root = dirname(__FILE__);

      // Create the info file
      $data = file("$root/templates/module_info.inc");
      $data = implode('', $data);
      $data = str_replace('[mod:name]', $name, $data);
      drush_file_append_data("$dir/$name.info", $data);

      // Create the module file
      $data = file("$root/templates/module_module.inc");
      drush_file_append_data("$dir/$name.module", implode('', $data));
    }
  }

  drush_print("Creating module $name in $dir");
}
