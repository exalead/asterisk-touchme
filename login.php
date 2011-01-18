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
 *                                 Login
 *
 *****************************************************************************/

/* Includes */
require_once('touchme.inc');

/* Secure redirect */
if (isset($xtmConfig['auth']['enforce_ssl_login'])
    && $xtmConfig['auth']['enforce_ssl_login']
    && !isset($_SERVER["HTTPS"])
    && !isset($_REQUEST['redirect_http'])) {
  Header("Location: https://".$_SERVER["SERVER_NAME"]
         .$_SERVER["PHP_SELF"]."?"."redirect_http=1");
  include('include/exit.inc');
}

/* Index page */
$additional_login_vars = '';
if (isset($_REQUEST['redirect_http']) && isset($_SERVER["HTTPS"])) {
  $additional_login_vars
    = '<input type="hidden" name="redirect_http" value="1" />';
}

/* Login html data */
require($xtmGlobal->inc('login.inc'));
?>
