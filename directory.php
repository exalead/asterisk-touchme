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
require_once('include/tools.inc');

/* Auth */
if (!isset($xtmConfig['ldap']['directory_public'])
    || !$xtmConfig['ldap']['directory_public']) {
  $auth = $xtmGlobal->getAuth();
  $auth->ensureCredentials(true);
  /* caller phone */
  /*   $caller = $auth->getCredentialsElement('phone'); */
  /*   $callerName = $auth->getCredentialsElement('cn'); */
}

/* fwd declaration */
$scriptFoundValue = '';

/* Args */
unset($q);
unset($busyWait);
$q = '';  /* match all */
$compact = false;
if (isset($_REQUEST['format'])) {
  if ($_REQUEST['format'] == "compact") {
    $compact = true;
  }
}


/* Create a new ldap connection */
$touch = $xtmGlobal->getTouch();
$ldap = $xtmGlobal->getLdap();
$what = $cn_name;
$find = $ldap->findUserNumber($q, false, $what,
                              $xtmConfig['ldap']['directory_max']);
if (is_array($find)) {
  if (!$compact) {
    sortLdapStructure($find);
  } else {
    sortLdapBy($find, $xtmConfig['ldap']['dn_name'],
               $xtmConfig['ldap']['ou_name'],
               $xtmConfig['ldap']['skip_ou'],
               '');
  }
  
  $table = '<table '
    . ' cellpadding="0"'
    . ' cellspacing="0">';
  
  $trToggle = 0;
  $current_categ = '';
  $current_categ_n = 0;
  $current_ndx = '';
  $ndx_arr = array();
  $master_table = array();
  
  foreach($find as $entry) {
    if (!isset($entry[$cn_name])) {
      continue;
    }

    if (!$compact) {
      $categ = $entry['_category'];
    } else {
      $a = explode('/', $entry['_category']);
      $categ = $a[0];
    }
    if ($current_categ != $categ) {
      $current_categ = $categ;
      $current_categ_n++;

      $table .= getAltTr($trToggle);
      $table .=
        '<td colspan="2">';
      
      // index
      $table .= '<a name="ndx_'.$current_categ_n.'"></a>';
      $master_table[] = $current_categ;
      
      $table .=
        '<h2>'
        . htmlspecialchars($categ)
        . '</h2>'
        . '</td>'
        . '</tr>';
      getAltTr($trToggle);
      $current_ndx = '';

      if (!$compact) {
        $table .= '<tr><td colspan="2">';
        
        $table .= '<table width="100%"><tr>';
        for($key = ord('A') ; $key <= ord('Z') ; $key++) {
          $table .= '<td>'.'<a href="#ndx_'.$current_categ_n.'_'.chr($key).'">'
            . chr($key).'</a>'.'</td>';
        }
        $table .= '</tr></table>';
        $table .= '</tr>';
      }
    }
    
    if (isset($entry[$phone_name])) {
      $num = $touch->getReducedNumber($entry[$phone_name]);
    } else {
      $num = '';
    }

    $gn = isset($entry[$gn_name]) ? $entry[$gn_name] : '';
    $sn = isset($entry[$sn_name]) ? $entry[$sn_name] : '';
    $role = isset($entry[$role_name]) ? $entry[$role_name] : '';
    $desc = isset($entry[$description_name]) ? $entry[$description_name] : '';
    $ph = isset($entry[$phone_name])
      ? $touch->getReducedNumber($entry[$phone_name]) : '';
    $px = isset($entry[$phone_ext_name[0]])
      ? $touch->getReducedNumber($entry[$phone_ext_name[0]]) : '';
    $o = isset($entry[$o_name]) ? $entry[$o_name] : '';
    $l = isset($entry[$l_name]) ? $entry[$l_name] : '';
    $st = isset($entry[$st_name]) ? $entry[$st_name] : '';
    $mail = isset($entry[$mail_name]) ? $entry[$mail_name] : '';
    $street = isset($entry[$street_name]) ? $entry[$street_name] : '';
    $postalcode = isset($entry[$postal_name]) ? $entry[$postal_name] : '';
    $department = isset($entry[$department_name])
      ? $entry[$department_name] : '';
    
    $altTr = getAltTr($trToggle);
    $table .= $altTr;

    if ($compact) {


      
      $table .= '<td>';
      $table .= strtoupper($sn);
      $table .= '</td><td>';
      $table .= $gn;
      $table .= '</td><td>';
      $table .= prettyNumber($num);
      $table .= '</td><td>';
      $table .= prettyNumber($px);
      $table .= '</td><td>';
      $table .= htmlspecialchars($mail);
      $table .= '</td><td>';
      $table .= htmlspecialchars($department);
      $table .= '</td>';
      $table .= '</tr>';
    } else {
      $table .= '<td>';
      if ($sn[0] != $current_ndx) {
        $current_ndx = $sn[0];
        $table .= '<a name="ndx_'.$current_categ_n.'_'.$current_ndx.'"></a>';
        $ndx_arr[] = $current_ndx;
      }
      $table .= '<b>'.$gn.'</b></td>'
        . '</td><td><b>'.strtoupper($sn).'</b></td>';
      $table .= '</tr>';
      
      $table .= $altTr;
      if (isset($entry[$image_name])) {
        $w = $xtmConfig['ldap-image']['directory_height'];
        $h = $xtmConfig['ldap-image']['directory_height'];
        
      $im = @imagecreatefromstring($entry[$image_name]);
      if ($im) {
        $img_w = imagesx($im);
        $img_h = imagesy($im);
        imagedestroy($im);
        if ($img_w > 0 && $img_h > 0) {
          $ratio = $img_h / $img_w;
          unset($im);
          $w = intval($h / $ratio);
        }
      }
      $table .= '<td rowspan="2" align="left">'
        .getLdapImage($entry[$cn_name], $w, $h, _T($role)).'</td>';
      } else {
      $table .= '<td rowspan="2"></td>';
      }
      
      $extp = '<table '
        . ' cellpadding="1"'
      . ' cellspacing="1"'
        . ' width="100%"'
        . ' height="100%"'
        . '>'
        . '<tr valign="bottom">'
      . '<td>Tel: '.$ph.'</td>'
        . '<td align="right">'.(strlen($px)?'Port: '.$px:'')
        . '</td>'
        . '</tr>'
        . '</table>';
      
      $surt = '<table '
        . ' cellpadding="2"'
        . ' cellspacing="2"'
        . ' width="100%"'
        . ' height="100%"'
        . '>'
        . '<tr><td>'.$o.'</td></tr>'
        . '<tr><td>'.$street.'</td></tr>'
        . '<tr><td>'.$postalcode . ' ' . $l.' - '.$st.'</td></tr>'
        . '<tr><td><a href="mailto:'.htmlspecialchars($mail).'">'
        . $mail.'</a></td></tr>'
        . '</table>';
      
      $table .= '<td valign="top">'
        . '<table'
        . ' cellpadding="0"'
        . ' cellspacing="0"'
        . ' width="100%"'
        . ' height="100%"'
        . '>'
        . '<tr valign="top"><td><b>'._T($role).'</b></td></tr>'
        . ( strlen($desc)
            ? '<tr valign="top"><td><i>'._T($desc).'</i></td></tr>' : '' )
        . '<tr valign="top"><td>&nbsp;</td></tr>'
        . '<tr valign="bottom"><td>'.$surt.'</td></tr>'
        . '</table>'
        . '</td>';
      
      $table .= '</tr>';
      
      $table .= $altTr.'<td valign="bottom" colspan="2">'
        .$extp.'</td></tr>';
      
      $table .= '<tr><td colspan="2">&nbsp;</td></tr>';
    }
    
  }
  $table .= '</table>';

  if (!$compact) {
    $defaultTable = '<table>';
    $defaultTable .= getAltTr($trToggle);
    $defaultTable .= '<td>';
    $defaultTable .= '<h2>'._T('Directory')
      /* get all vcard hack */
      . notMac('<sup><small>')
      . '['
      . '<a href="vcard.php?cn=all"'
      . ' title="'._T('Get the VCard').' (.vcf)"'
      . '>'
      . 'vCard'
      . '</a>'
      . '&nbsp;'
      . '<a href="vcard.php?cn=all&format=zip"'
      . ' title="'._T('Get the VCard').' (.zip)"'
      . '>'
      . 'vCard.zip'
      . '</a>'
      . ']'
      . notMac('</small></sup>');
    $defaultTable .= '</h2>';
  
    $c_tbl = array();
    $c_d = 0;
    for($i = 0 ; $i < count($master_table) ; $i++) {
      $tbl = explode('/', $master_table[$i]);
      /* close high depths or if different */
      for( ; $c_d > count($tbl) || ( $c_d != count($tbl)
                                     && $c_d > 0 && $tbl[$c_d - 1]
                                     != $c_tbl[$c_d - 1] )
             ; $c_d--) {
        $defaultTable .= '</ul>';
      }
      /* open to reach depth */
      for ($j = 0 ; $c_d < count($tbl) ; $c_d++, $j++) {
        if ($j > 0) {
          $defaultTable .= '<li>'.$tbl[$c_d - 1].'</li>';
        }
        $defaultTable .= '<ul>';
        $c_tbl[$c_d] = $tbl[$c_d];
      }
      $defaultTable .= '<li><a href="#ndx_'.($i + 1).'">'
        . $tbl[$c_d - 1].'</a></li>';
    }
    for( ; $c_d > 0 ; $c_d--) {
      $defaultTable .= '</ul>';
    }
    $defaultTable .= '</td>';
    $defaultTable .= '</tr>';
    $defaultTable .= '</table>';
 }
  
} else if ($find === true) {
  $defaultTable = _T('too many results');
} else if ($ldap->getErrorMessage() !== false) {
  $defaultTable =
    '<font color="red"><h1>'._T($ldap->getErrorMessage()).'</h1></font>';
} else {
  $defaultTable = _T('error');
}
unset($touch);
unset($ldap);

/* Top html data */
require($xtmGlobal->inc('bottom.inc'));

/* End */
include('include/exit.inc');
?>
