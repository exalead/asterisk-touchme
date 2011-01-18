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
 *                                Index
 *
 *****************************************************************************/

/* Includes */
require_once('touchme.inc');

/* Auth */
$auth = $xtmGlobal->getAuth();
$auth->ensureCredentials(false);

/* We are securely authenticated: back to http */
if (isset($_REQUEST['redirect_http']) && isset($_SERVER["HTTPS"])) {
  Header("Location: http://".$_SERVER["SERVER_NAME"]
         .$_SERVER["REQUEST_URI"]);
  include('include/exit.inc');
}
/* Redirect on POST using RFC-suggested method */
else if (isset($_SERVER["REQUEST_METHOD"])
    && $_SERVER["REQUEST_METHOD"] == 'POST') {
  Header("Location: index.php");
  include('include/exit.inc');
}

/* Notice */
$fp = fopen('LICENSE', 'rb');
$legalNotice = "\r\n" . "<!-- " . "\r\n" .fread($fp, 8192) . "-->" . "\r\n";
fclose($fp);

/* Index html data */
require($xtmGlobal->inc('index.inc'));
?>
