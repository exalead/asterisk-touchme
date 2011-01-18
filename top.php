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
 *                               Top frame
 *
 *****************************************************************************/

/* Includes */
require_once('touchme.inc');

/* Auth */
$auth = $xtmGlobal->getAuth();
if (isset($_REQUEST['logout'])) {
  $auth->logout();
  Header('Location: top.php');
  include('include/exit.inc');
}
$auth->ensureCredentials(true);

/* Args */
$caller = $auth->getCredentialsElement('phone');
$callerName = $auth->getCredentialsElement('cn');
$callee = '';
if ($caller == '') {
  $callee = '';
} else if (isset($_REQUEST['hangup']) && strlen($_REQUEST['hangup']) != 0) {
  $hangup = true;
} else if (isset($_REQUEST['found']) && strlen($_REQUEST['found']) != 0) {
  $found = $_REQUEST['found'];
  $callee = $found;
} else if (isset($_REQUEST['callee']) && strlen($_REQUEST['callee']) != 0) {
  $callee = $_REQUEST['callee'];
}
if (isset($callee) && strlen($callee) != 0 && isset($_REQUEST['info'])
    && intval($_REQUEST['info']) != 0) {
  $info = true;
}

/* Dial ? */
if (isset($info)) {
  $message = _T('Dialing').': ' . $callee;
  $noError = true;
  $redirect = true;
} else if ((isset($callee) && strlen($callee) != 0) || isset($hangup)) {
  if (isset($hangup) || ereg($xtmConfig['callmanager']['dialValidString'],
                             $callee)) {
    $touch = $xtmGlobal->getTouch();
    if ($xtmGlobal->touchLogin()) {
      if (isset($hangup)) {
        if (!$touch->hangup($caller)) {
          $message = _TERR('error: '.$touch->getErrorMessage());
        }
      } else {
        if (!$touch->contact($caller, $callee, $callerName)) {
          $te = $touch->getErrorMessage();
          if (eregi('Originate failed', $te)) {
            $message = _T('unable to dial').' '.$caller.' => '. $callee;
          } else {
            $message = _T('unexpected error') . ': ' . _TERR($te);
          }
        }
      }
    } else {
      $te = $touch->getErrorMessage();
      $message = _T('unexpected error') . ': ' . _TERR($te);
    }
    unset($touch);
  } else {
    $message = _T('please be more precise');
  }
}

/* Top html data */
require($xtmGlobal->inc('top.inc'));

/* Exit */
include('include/exit.inc');
?>
