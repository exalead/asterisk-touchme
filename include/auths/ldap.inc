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
 *                          Authentication: LDAP
 *
 *****************************************************************************/

/* Check username and password, using ldap */
function authUserPassLdap($args, &$errMsg) {
  $auth = false;

  if (!function_exists('ldap_connect')) {
    $errMsg = 'fatal: ldap_connect does not exist';
    return false;
  }
  
  if (!isset($args['user']) || !isset($args['pass'])) {
    $errMsg = 'no username/password cgi variables';
    return false;
  }
  $username = $args['user'];
  $password = $args['pass'];
  if (!eregi('^[A-Za-z0-9_]+$', $username)) {
    $errMsg = 'bad characters in login name';
    return false;
  }
  
  if (strlen($username) != 0 && strlen($password) != 0) {
    $server = $args['host'];
    $domain = $args['domain'];
    $ds = @ldap_connect($server, $args['port']);
    if ($ds) {
      ldap_set_option($ds, LDAP_OPT_PROTOCOL_VERSION, 3);
      ldap_set_option($ds, LDAP_OPT_TIMELIMIT, 1);
      $ldapbind = @ldap_bind($ds, $username.'@'.$domain, $password);
      if($ldapbind) {
        $auth = true;
      } else {
        $errMsg = 'ldap authentication error: ' . ldap_error($ds);
      }
      ldap_close($ds);
    } else {
      $errMsg = 'ldap connection failed';
    }
  } else if (strlen($username) == 0) {
    $errMsg = 'empty username';
  } else {
    $errMsg = 'empty password';
  }
  /* auth succeeded: pass the authenticated login name */
  if ($auth) {
    return $username;
  }
  return false;
}
?>
