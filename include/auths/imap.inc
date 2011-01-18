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
 *                            Authentication: IMAP
 *
 *****************************************************************************/

/* Check username and password, using imap */
function authUserPassImap($args, &$errMsg) {
  if (!function_exists('imap_open')) {
    $errMsg = 'fatal: imap_open does not exist';
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
  
  $fd = @imap_open('{' . $args['host']
                   . ':' . $args['port'].'}'.$args['mailbox'],
                   $username  ,$password, OP_READONLY | OP_HALFOPEN);
  if ($fd !== false) {
    imap_close($fd);
    return $username;
  }
  $errMsg = imap_last_error();
  return false;
}
?>
