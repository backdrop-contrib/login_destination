# ![Login Destination](https://github.com/backdrop-contrib/login_destination/blob/1.x-1.x/images/login_destination.png "Login Destination for BackdropCMS")

The Login Destination module allows you to customize the destination that a user
is redirected to after logging in, registering to the site, using a one-time
login link, or logging out. The destination can be an internal page or an
external URL. It is possible to specify certain conditions like referring pages
or user roles and make the destination depend upon them. You may use PHP snippets
to provide custom conditions and destinations. It is also possible to keep users
on the currently visited page after logging in or out.

## Installation

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)

- Specify rules at **Configuration > User Accounts > Login Destinations**
  (`admin/config/people/login-destination`), which also has a tab to adjust
  settings.

## Issues

 - Bugs and Feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/login_destination/issues).

## Current Maintainers

- [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org).

## Credits

This module was ported to Backdrop by:
  - [Jerome Danthinne](https://github.com/jdanthinne/)

This module was originally maintained for Drupal by:
  - [Moshe Weitzman](https://www.drupal.org/u/moshe-weitzman)
  - [ARDAS group](https://www.drupal.org/u/ardas)
  - [rsvelko](https://www.drupal.org/u/rsvelko)
  - [mithy](http://drupal.org/user/258911/)
  - [Others have also contributed](https://www.drupal.org/node/69051/committers) – thank you!

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
