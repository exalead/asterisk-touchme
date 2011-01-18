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
 *                        Authentication: KERBEROS
 *
 *****************************************************************************/

/* Check username and password, using imap */
function authUserPassKerberos($args, &$errMsg) {
  if (!isset($args['authkey'])) {
    $errMsg = 'fatal: authentication key does not exist (bad server configuration ?)';
    return false;
  }

  if (strlen($args['authkey']) > 0) {
    $arr = explode('@', $args['authkey']);
    if (count($arr) > 0) {
      $username = strtolower($arr[0]);
      return $username;
    }
    unset($ldap);
  }
  return false;
}

?>
