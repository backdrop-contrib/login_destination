<?php

/**
 * Categories/list tab.
 */
function login_destination_overview() {
  $header = array(
    t('Destination'),
    t('Pages'),
    t('Roles'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $rows = array();

  // Get all login redirect rules from the database.
  $result = db_select('login_redirect', 'l')
    //->addTag('translatable')
    ->fields('l', array('id', 'roles', 'pages_type', 'pages', 'destination'))
    ->orderBy('weight')
    ->execute()
    ->fetchAll();

  // Loop through the categories and add them to the table.
  foreach ($result as $rule) {

    $roles = array_map('check_plain', unserialize($rule->roles));
    if(empty($roles))
      $roles = array();

    $rows[] = array(
      nl2br(check_plain($rule->destination)),
      theme('login_redirect_pages', array('pages' => $rule->pages, 'pages_type' => $rule->pages_type)),
      //nl2br(check_plain($rule->pages)),
      theme('login_redirect_roles', array('roles' => $roles)),
      l(t('Edit'), 'admin/config/people/login-redirect/edit/' . $rule->id),
      l(t('Delete'), 'admin/config/people/login-redirect/delete/' . $rule->id),
    );
  }

  if (!$rows) {
    $rows[] = array(array(
      'data' => t('No redirect rules available.'),
      'colspan' => 5,
    ));
  }

  $build['login-redirect_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );
  return $build;
}

function theme_login_redirect_pages($variables) {

  $type = $variables['pages_type'];
  
  if($type == LOGIN_DESTINATION_REDIRECT_PHP) {
    return nl2br(check_plain($variables['pages']));
  }

  $pages = trim($variables['pages']);

  if(empty($pages)) {
    if($type == LOGIN_DESTINATION_REDIRECT_NOTLISTED) {
      return '<i>'.t('All pages').'</i>';
    } else {
      return '<i>'.t('No pages').'</i>';
    }
  }

  $pages = explode("\n", preg_replace('/\r/', '', check_plain($variables['pages'])));

  $output = '';
  foreach($pages as &$page) {
    if($type == LOGIN_DESTINATION_REDIRECT_NOTLISTED) {
      $output .= "~ ";
    }
    $output .= $page."<br/>";
  } 

  return $output;
}

function theme_login_redirect_roles($variables) {

  $role_options = array_map('check_plain', user_roles(TRUE));
  unset($role_options[DRUPAL_AUTHENTICATED_RID]);

  $roles = array_values(array_intersect_key($role_options, $variables['roles']));

  if(empty($roles)) {
    return '<i>'.t('All roles').'</i>';
  }

  return theme('item_list', array('items' => $roles));
}

/**
 * Category edit page.
 */
function login_destination_edit_form($form, &$form_state, array $login_redirect = array()) {

  // default values
  $login_redirect += array(
    'roles' => array(),
    'pages_type' => LOGIN_DESTINATION_REDIRECT_NOTLISTED,
    'pages' => '',
    'destination_type' => LOGIN_DESTINATION_STATIC,
    'destination' => '<front>',
    'id' => NULL,
    'weight' => 0,
  );

  $access = user_access('use PHP for settings');

  $type = $login_redirect['destination_type'];

  if ($type == LOGIN_DESTINATION_SNIPPET && !$access) {
    $form['destination_type'] = array(
      '#type' => 'value',
      '#value' => LOGIN_DESTINATION_SNIPPET,
    );
    $form['destination'] = array(
      '#type' => 'value',
      '#value' => $login_redirect['destination'],
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

    $form['destination_type'] = array(
      '#type' => 'radios',
      '#title' => 'Redirect to page',
      '#default_value' => $type,
      '#options' => $options,
    );
    $form['destination'] = array(
      '#type' => 'textarea',
      '#default_value' => $login_redirect['destination'],
      '#description' => $description,
    );
  }

  $type = $login_redirect['pages_type'];

  if ($type == LOGIN_DESTINATION_REDIRECT_PHP && !$access) {
    $form['pages_type'] = array(
      '#type' => 'value',
      '#value' => LOGIN_DESTINATION_REDIRECT_PHP,
    );
    $form['pages'] = array(
      '#type' => 'value',
      '#value' => $login_redirect['destination'],
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

    $form['pages_type'] = array(
      '#type' => 'radios',
      '#title' => t('Redirect from specific pages'),
      '#default_value' => $type,
      '#options' => $options,
    );
    $form['pages'] = array(
      '#type' => 'textarea',
      '#default_value' => $login_redirect['pages'],
      '#description' => $description,
    );
  }

  // Add user role selection, without anonymous and authentificated user roles.
  $role_options = array_map('check_plain', user_roles(TRUE));
  unset($role_options[DRUPAL_AUTHENTICATED_RID]);

  $default_role_options = array_map('check_plain', $login_redirect['roles']);
  if(empty($default_role_options)) {
    $default_role_options = array();
  }

  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Redirect users with roles'),
    '#options' => $role_options,
    '#default_value' => $default_role_options,
    '#description' => 'Redirect only the selected role(s). If you select no roles, all users will be redirected.',
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => $login_redirect['weight'],
    '#description' => t('When listing login redirects, those with lighter (smaller) weights get listed before categories with heavier (larger) weights.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if ($login_redirect['id']) {
    $form['id'] = array(
      '#type' => 'hidden',
      '#value' => $login_redirect['id'],
    );
  }

  return $form;
}

/**
 * Validate the contact category edit page form submission.
 */
function login_destination_edit_form_validate($form, &$form_state) {
}

/**
 * Process the contact category edit page form submission.
 */
function login_destination_edit_form_submit($form, &$form_state) {

  $roles = array_filter($form_state['values']['roles']);

  $form_state['values']['roles'] = serialize($roles);

  if (empty($form_state['values']['id'])) {
    drupal_write_record('login_redirect', $form_state['values']);
  }
  else {
    drupal_write_record('login_redirect', $form_state['values'], array('id'));
  }

  drupal_set_message(t('Login redirect to %destination has been saved.', array('%destination' => $form_state['values']['destination'])));
  
  $form_state['redirect'] = 'admin/config/people/login-redirect';
}

/**
 * Form builder for deleting a login redirect.
 */
function login_destination_delete_form($form, &$form_state, array $login_redirect) {

  $form['login_redirect'] = array(
    '#type' => 'value',
    '#value' => $login_redirect,
  );

  return confirm_form(
    $form,
    t('Are you sure you want to delete the login redirect to %destination ?', array('%destination' => $login_redirect['destination'])),
    'admin/config/people/login-redirect',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit handler for the confirm delete login redirect form.
 */
function login_destination_delete_form_submit($form, &$form_state) {
  $login_redirect = $form['login_redirect']['#value'];

  db_delete('login_redirect')
    ->condition('id', $login_redirect ['id'])
    ->execute();

  drupal_set_message(t('The login redirect to %destination has been deleted.', array('%destination' => $login_redirect['destination'])));

  $form_state['redirect'] = 'admin/config/people/login-redirect';
}

function login_destination_admin_settings() {
  $form = array();

  # on which pages we redirect
//  $form['settings'] = array(
//    '#type' => 'vertical_tabs',
//  );

//  $form['settings']['roles'] = array(
//    '#type' => 'fieldset',
//    '#title' => t('Roles'),
//  );

  // Add user role selection, without anonymous and authentificated user roles.
  $role_options = array_map('check_plain', user_roles(TRUE));
  unset($role_options[DRUPAL_AUTHENTICATED_RID]);
  
  $form['settings']['roles']['ld_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Redirect users with roles'),
    '#options' => $role_options,
    '#default_value' => variable_get('ld_roles', array()),
    '#description' => 'Redirect only the selected role(s). If you select no roles, all users will be redirected.',
  );

//  $form['settings']['conditions'] = array(
//    '#type' => 'fieldset',
//    '#title' => t('Conditions'),
//  );

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
//  $form['settings']['destination'] = array(
//    '#type' => 'fieldset',
//    '#title' => t('Destination'),
//  );

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
