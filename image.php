<?php
/******************************************************************************
 * TouchMe, an ldap dialer add-on for Asterisk-based PBX systems.
 * Copyright (C) 2008 Exalead SA.
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
/******************************************************************************
 *
 *                               Bottom frame
 *
 *****************************************************************************/

/* Enable caching */
require_once('include/cache.inc');

/* Includes */
require_once('touchme.inc');

/* Auth */
if (!isset($xtmConfig['ldap']['directory_public'])
    || !$xtmConfig['ldap']['directory_public']) {
  $auth = $xtmGlobal->getAuth();
  $auth->ensureCredentials(true);
}

/* image field */
$image_name = $xtmConfig['ldap']['image_name'];
$cn_name = $xtmConfig['ldap']['cn_name'];

/* Fetch image */

unset($q);
unset($busyWait);
if (isset($_REQUEST['cn']) && strlen($_REQUEST['cn']) != 0) {
  $q = $_REQUEST['cn'];

  /* No bad characters ? */
  if (eregi($xtmConfig['ldap']['valid_chars'], $q)) {
    /* Create a new ldap connection */
    $ldap = $xtmGlobal->getLdap();
  
    $what = $cn_name;
    $find = $ldap->findUserNumber($q, false, $what, 2);
    if (is_array($find)) {
      $entry = $find[0];
      header('Content-type: image/jpeg');
      echo $entry[$image_name];
      exit(0);
    }
  }
}
header('HTTP/1.0 404 Image not found');

?>
