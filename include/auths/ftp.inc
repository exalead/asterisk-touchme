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
 *                          Authentication: ftp
 *
 *****************************************************************************/

/* Check username and password, using ldap */
function authUserPassFTP($args, &$errMsg) {
  $auth = false;

  if (!function_exists('ftp_connect')) {
    $errMsg = 'fatal: ftp_connect does not exist';
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
  } else if ($username == 'anonymous') {  /* would not match anything in ldap anyway, but .. */
    $errMsg = 'anonymous login not accepted';
    return false;
  }
  
  if (strlen($username) != 0 && strlen($password) != 0) {
    $server = $args['host'];
    $conn = @ftp_connect($server, $port);
    if ($conn) {
      if (@ftp_login($conn, $username, $password)) {
        $auth = true;  /* success! */
      } else {
        $errMsg = 'authentication failed';
      }
      ftp_close($conn);
    } else {
      $errMsg = 'could not connect to '.$args['host'].':'.$args['port'];
    }
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
