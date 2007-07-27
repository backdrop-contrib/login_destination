Description
-----------------
This module controls where users go after logging in.
You can configure login destination on a settings page.
It will be either static URL or a PHP snippet evaluating your own site's logic.
You can also configure which pages redirection is applied.

To achieve this you can specify either a list of URLs or a PHP snippet to build this list dynamically.
So, you can redirect user to various pages depending on which pages they are logging from.

You can also use alias URLs instead of internal paths.
  

Configuration
-----------------
All configuration is performed on an Administer -> User management -> Login destination page
(admin/user/login_destination).


Examples
-----------------

PHP snippet for redirection URL should return a string. Here is an example:

  global $user;
  if ($user->uid == 1) {
    // Redirect the Administrator
    return 'admin';
  } elseif ($user->uid == 2) {
    // Redirect the Site Owner to the 'create content' page
    return 'node/add';
  } else {
    return 'node';
  }

PHP snippet for Redirection condition should return boolean value. An example is:

  return ($_GET['q'] == 'user/login');

Don't put PHP tags when creating snippets.


Authors
-----------------
Moshe Weitzman <weitzman AT tejasa DOT com>
ARDAS group <info AT ardas DOT dp DOT ua>