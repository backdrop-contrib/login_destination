<?php

function login_destination_admin_settings() {
  $form = array();

  # on which pages we redirect
  $form['settings'] = array(
    '#type' => 'vertical_tabs',
  );

  $form['settings']['conditions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Conditions'),
  );

  $type = variable_get('ld_condition_type', LOGIN_DESTINATION_REDIRECT_NOTLISTED);
  $pages = variable_get('ld_condition_pages', '');

  $access = user_access('use PHP for settings');

  if ($type == LOGIN_DESTINATION_REDIRECT_PHP && !$access) {
    $form['settings']['conditions']['ld_condition_type'] = array(
      '#type' => 'value',
      '#value' => LOGIN_DESTINATION_REDIRECT_PHP,
    );
    $form['settings']['conditions']['ld_condition_pages'] = array(
      '#type' => 'value',
      '#value' => $pages,
    );
  }
  else {
    $options = array(
        LOGIN_DESTINATION_REDIRECT_NOTLISTED => t('All pages except those listed'),
        LOGIN_DESTINATION_REDIRECT_LISTED => t('Only the listed pages'),
      );
    $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if (module_exists('php') && $access) {
      $options += array(LOGIN_DESTINATION_REDIRECT_PHP => t('Pages on which this PHP code returns <code>TRUE</code> (experts only)'));
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
    }

    $form['settings']['conditions']['ld_condition_type'] = array(
      '#type' => 'radios',
      '#title' => t('Redirect from specific pages'),
      '#default_value' => $type,
      '#options' => $options,
    );
    $form['settings']['conditions']['ld_condition_pages'] = array(
      '#type' => 'textarea',
      '#default_value' => $pages,
      '#description' => $description,
    );
  }
  
  # to where we redirect
  $form['settings']['destination'] = array(
    '#type' => 'fieldset',
    '#title' => t('Destination'),
  );

  $type = variable_get('ld_destination_type', LOGIN_DESTINATION_STATIC);
  // redirect to front by default
  $destination = variable_get('ld_destination_page', '<front>');

  if ($type == LOGIN_DESTINATION_SNIPPET && !$access) {
    $form['settings']['destination']['ld_destination_type'] = array(
      '#type' => 'value',
      '#value' => LOGIN_DESTINATION_SNIPPET,
    );
    $form['settings']['destination']['ld_destination_page'] = array(
      '#type' => 'value',
      '#value' => $destination,
    );
  }
  else {
    $options = array(
        LOGIN_DESTINATION_STATIC => t('Static URL'),
      );
    $description = t("Specify page by using its path. Example path is %blog for the blog page. %front is the front page. Precede with http:// for an external URL.", array('%blog' => 'blog', '%front' => '<front>'));

    if (module_exists('php') && $access) {
      $options += array(LOGIN_DESTINATION_SNIPPET => t('PHP snippet (experts only)'));
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. It should return either a string value or an array in the form of return array(\'path\' => \'node/add/video or alias\', \'query\' => \'param1=100&param2=200\');. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
    }

    $form['settings']['destination']['ld_destination_type'] = array(
      '#type' => 'radios',
      '#title' => 'Redirect to page',
      '#default_value' => $type,
      '#options' => $options,
    );
    $form['settings']['destination']['ld_destination_page'] = array(
      '#type' => 'textarea',
      '#default_value' => $destination,
      '#description' => $description,
    );
  }

  $form['settings']['destination']['ld_destination_preserve'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('ld_destination_preserve', FALSE),
    '#title' => t('Preserve the destination parameter'),
    '#description' => t("If checked, the 'destination=' parameter specified in URL will have priority over the settings of this module. Redirect from the user login block will not work."),
  );

  return system_settings_form($form);
}

?>
