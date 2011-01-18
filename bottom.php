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

/* Includes */
require_once('touchme.inc');
require_once('include/tools.inc');

/* Auth */
$auth = $xtmGlobal->getAuth();
$auth->ensureCredentials(true);

/* caller phone */
$caller = $auth->getCredentialsElement('phone');
$callerName = $auth->getCredentialsElement('cn');

/* ACLs */
$editAll = $auth->getCredentialsElement('acl:edit');

/* fwd declaration */
$scriptFoundValue = '';

/* Args */
unset($q);
unset($busyWait);
if (isset($_REQUEST['q']) && strlen($_REQUEST['q']) != 0) {
  $q = $_REQUEST['q'];
  $max_hits = $xtmConfig['ldap']['search_max'];
  if (isset($currLangCharset_)) {
    $q = iconv('utf-8', $currLangCharset_, $q);
  }
  if (substr($q, 0, 6) == 'debug:') {  /* guess what */
    $xtmDebug = true;
    $q = substr($q, 6);
  }
  if (strtolower($q) == 'all' || $q == '*') {
    $q = '';
    $max_hits = 999;
  } else {
    $q = _CLEAN($q);
  }

  /* No bad characters ? */
  if (strlen($q) == 0 || eregi($xtmConfig['ldap']['valid_chars'], $q)) {
    /* Create a new ldap connection */
    $touch = $xtmGlobal->getTouch();
    //$ldap = $xtmGlobal->getLdap();
    if (isset($xtmDebug)) {
      echo "number: ".$q.'<br />';
      echo "internal: ".($touch->isInternal($q)?'yes (state='
                         .$touch->getState($q).')'
                         :'no').'<br />';
      echo "internal external: ".($touch->isInternalExternal($q)?'yes (ext='
                                  .$touch->getExternalDialNumber($q).')':'no')
        .'<br />';
      echo "external internal: ".($touch->isExternalInternal($q)?'yes (int='
                                  .$touch->getInternalNumber($q).')':'no')
        .'<br />';
    }
  
    $what = $sn_name;
    if (strpos($q, ' ') !== false) {
      $what = array($cn_name);
      $q = trim($q);
    } else if (ereg($xtmConfig['callmanager']['dialValidString'], $q)) {
      $what = array($phone_name);
      $what = array_merge($what, $phone_ext_name);
    }

    /* Execute query iff not too small */
    if (isset($xtmConfig['ldap']['min_query_size'])
        && strlen($q) < $xtmConfig['ldap']['min_query_size']) {
      $find = true;
    } else {
      $find = $xtmGlobal->findUserNumber($q, false, $what, $max_hits);
    }

    /* Found ? */
    if (is_array($find)) {
      $other_a = array($o_name, $l_name, $st_name);
      $endspan = count($other_a) + 2;
      $table = '<table border="0"'
        . ' cellpadding="8"'
        . ' cellspacing="1">';
      $first = (count($find) == 1);
      $trToggle = 0;
      $table .= getAltTr($trToggle)
        . '<th colspan="2" align="left">'._T('Name').'</th>'
        . '<th align="left">'._T('Phone').'</th>'
        . '<th align="left" width="64">&nbsp;</th>'
        . '<th align="left">'._T('Email').'</th>'
        . '<th align="left">'._T('Room').'</th>'
        . '<th align="left">'._T('Company').'</th>'
        . '<th align="left">'._T('City').'</th>'
        . '<th align="left">'._T('Country').'</th>'
        . ( $editAll ? '<th align="left">'._T('Edit').'</th>' : '<th></th>' )
        . '</tr>';
      foreach($find as $entry) {
        /* Different schema: load globals */
        $is_external = isset($entry['_config']);
        if ($is_external) {
          bootstrapNames($entry['_config']);
          /* Recompute */
          $other_a = array($o_name, $l_name, $st_name);
        }

        /* Skip invalid entries */
        if (!isset($entry[$cn_name])) {
          continue ;
        }

        /* Can edit ? */
        if (!$is_external) {
          $edit = $auth->getCredentialsElement('acl:edit', $entry[$cn_name]);
        } else {
          $edit = false;
        }
        if (isset($entry[$phone_name])) {
          $num = $touch->getReducedNumber($entry[$phone_name]);
        } else {
          $num = '';
        }
        $isDefault = ($first && strlen($num) != 0);
        $altTr = getAltTr($trToggle);
        if (!$isDefault || $is_external) {
          $table .= $altTr;
        } else {
          $busy = false;
          if ($xtmGlobal->touchLogin()) {
            if ($touch->isBusy($num)) {
              $busy = true;
              $busyWait = $num;
            }
          } else {
            $te = $touch->getErrorMessage();
            $message = _T('unexpected error') . ': ' . _TERR($te);
          }
          $table .= getSelTr($busy);
        }

/*         if (isset($entry[$mail_name]) */
/*             && isset($xtmConfig['file-image']['dir']) */
/*             && strlen($xtmConfig['file-image']['dir']) > 0) { */
/*           $imgFilename = $xtmGlobal->image($xtmConfig['file-image']['dir'] */
/*                                            . '/' */
/*                                            . strtolower($entry[$mail_name])); */
/*           if (file_exists('./' . $imgFilename)) { */
/*             $w = $xtmConfig['ldap-image']['width']; */
/*             $h = $xtmConfig['ldap-image']['height']; */
/*             $im = getimagesize($imgFilename); */
/*             if (is_array($im)) { */
/*               $img_w = $im[0]; */
/*               $img_h = $im[1]; */
/*               if ($img_w > 0 && $img_h > 0) { */
/*                 $ratio = $img_h / $img_w; */
/*                 unset($im); */
/*                 $w = intval($h / $ratio); */
/*               } */
/*             } */
/*             $table .= '<td><a href="'.htmlspecialchars($imgFilename) */
/*               . '" target="_new">' */
/*               . '<img src="'.htmlspecialchars($imgFilename) */
/*               . '" border="0" width="'.$w.'" height="'.$h.'" ' */
/*               . 'onclick="window.open(\''.$imgFilename.'\', null, ' */
/*               . '\'height=320,width=400,status=no,toolbar=no,location=no,' */
/*               . 'menimg ubar=no,titlebar=no\'); return false"/></a></td>'; */
/*           } else { */
/*             $table .= '<td></td>'; */
/*           } */
/*        } else */

        $additional = 0;
        $add_table = '';
        $span = 3;
        foreach($phone_ext_name as $other) {
          if (isset($entry[$other]) && strlen($entry[$other]) > 0) {
            $num2 = $touch->getReducedNumber($entry[$other]);
            $isDefault = ($first && strlen($num2) != 0);
            if (!$isDefault) {
              $add_table .= $altTr;
            } else {
              $add_table .= getSelTr();
              $scriptFoundValue = $num2;
              $first = false;
            }
            $add_table .= '<td align="right">'.getNumberForm($num2).'</td>';
            $add_table .= '<td>'.getDialer($num2).'</td>';
            $add_table .= '<td colspan="'.$endspan.'"></td>';
            $add_table .= '</tr>';
            $additional++;
          }
        }
        
        $role = isset($entry[$role_name]) ? $entry[$role_name] : '';
        if (isset($entry[$image_name])) {
          $w = $xtmConfig['ldap-image']['width'];
          $h = $xtmConfig['ldap-image']['height'];
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
         
          $table .= '<td rowspan="'.($additional + 1).'">'
            .getLdapImage($entry[$cn_name], $w, $h, _T($role)).'</td>';
        } else {
          $table .= '<td rowspan="'.($additional + 1).'"></td>';
        }
        $table .= '<td rowspan="'.($additional + 1).'">'
          .htmlspecialchars($entry[$cn_name]).'</td>';
        $table .= '<td align="right">'.getNumberForm($num).'</td>';
        $table .= '<td>'.getDialer($num).'</td>';
        if ($isDefault) {
          $scriptFoundValue = $num;
          $first = false;
        }
        $table .= '<td>'
          . (isset($entry['mail']) ? getMailForm($entry['mail']) : '')
          . notMac('<sup><small>')
          . '&nbsp;['
          . '<a href="vcard.php?cn='.htmlspecialchars(urlencode($entry[$cn_name])).'"'
          . ' title="'._T('Get the VCard').'"'
          . '>'
          . 'vCard'
/*           . '<small>' */
/*           .   '<small>[</small>V<small>card]</small>' */
/*           . '</small>' */
          . '</a>'
          . ']'
          . notMac('</small></sup>')
          . '</td>';
        $table .= '<td align="center">'
          .(isset($entry[$roomnumber_name])
            ? getRoomLocator($entry[$roomnumber_name]) : '')
          .'</td>';
        foreach($other_a as $other) {
          $table .= '<td>'
            . (isset($entry[$other]) ? $entry[$other] : '')
            . '</td>';
        }
        if (is_array($edit)) {
          $table .= '<td><a href="edit.php?q='
            . htmlspecialchars($entry[$cn_name]).'"'
            . ' title="'._T('Edit').'"'
            . '>'._T('Edit');
          if (isset($edit['@duplicate']) && $edit['@duplicate'] == 'text') {
            $table .= '&nbsp;<a href="edit.php?q='
              . htmlspecialchars($entry[$cn_name]).'&duplicate"'
              . ' title="'._T('Duplicate').'"'
              . '>'._T('Duplicate');
          }
          $table .= '</td>';
        } else {
          $table .= '<td></td>';
        }
        $table .= '</tr>';

        if ($additional > 0) {
          $table .= $add_table;
        }

      }
      $table .= '</table>';
    } else if ($find === true) {
      $defaultTable = _T('too many results');
    } else if ($q == 'license') {
      $fp = fopen('COPYING', 'rb');
        $defaultTable = '<tt>'.nl2br(fread($fp, 8192)).'<tt>';
        fclose($fp);
    }
    /* time for easter eggs :p (added 03/24/08) */
    else if (md5($q) == '369f6c3fac0d235d099af98ca23ca4c9') {
      $defaultTable = '<object width="425" height="355"><param name="movie" value="http://www.youtube.com/v/MiuimDNlyuQ&hl=en"></param><param name="wmode" value="transparent"></param><embed src="http://www.youtube.com/v/MiuimDNlyuQ&hl=en" type="application/x-shockwave-flash" wmode="transparent" width="425" height="355"></embed></object>';
    } else if (md5($q) == 'a4757d7419ff3b48e92e90596f0e7548') {
      $defaultTable = '<img src="'.$xtmGlobal->image('fb.jpg')
        .'" border="0" alt="god"/>';
    } else if (md5($q) == 'e84afaab83ecb301b3d97ce4174d2773') {
      $defaultTable = 42;
    } else if ($xtmGlobal->getFindErrorMessage() !== false) {
      $defaultTable =
        '<font color="red"><h1>'
        ._T($xtmGlobal->getFindErrorMessage())
        .'</h1></font>';
    } else {
      $defaultTable = '<i>'._T("No matches").'!</i>';
    }
    unset($touch);
    //unset($ldap);
  } else {
    $message = _T('bad characters');
  }
} else {
  $table = '<table border="0"'
    . ' cellpadding="1"'
    . ' cellspacing="1">';
  $table .= getAltTr($trToggle)
    . '<th>'._T('Name').'</th>'
    . '<th width="64">&nbsp;</th>'
    . '<th>'._T('Phone').'</th>'
    . '</tr>';
  $trToggle = 0;
  foreach(array(
                'Voicemail' => '*98'
                /*,'' => false,
                  'Cancel all redirections' => '*20'*/
                ) as $title => $num) {
    if ($num !== false) {
      $table .= getAltTr($trToggle);
      $table .= '<td>'._T($title).'</td>';
      $table .= '<td width="64"></td>';
      $table .= '<td>'.getNumberForm($num).'</td>';
      $table .= '</tr>';
    } else {
      $table .= '<tr><td colspan="3">&nbsp;</td></tr>';
    }
  }
  foreach(array('Directory' => 'directory.php') as $title => $url) {
    $table .= getAltTr($trToggle);
    $table .= '<td>'._T($title).'</td>';
    $table .= '<td width="64"></td>';
    $table .= '<td>'.'<a href="'.$url.'"'
      . ' target="new_"'
      . ' title="'._T('Browse this site').'"'
      . '>'
      . _T('open').'</a>'.'</td>';
    $table .= '</tr>';
  }

  /* note: TODO FIXME: does not work (Originate call) */
  if (false) {
  $table .= getAltTr($trToggle)
    . '<form onsubmit="return false">'
    . '<td>'
    . _T('Redirect to').':'
    . '</td>'
    . '<td></td>'
    . '<td>'
    . '*21<input size="10" name="number" value="00123452626">'
    . '<input type="button" value="'._T('Redirect').'" '
    .   'onclick="submitMe(\'*21\' + number.value);">'
    . '</td>'
    . '</form>'
    . '</tr>';
  }
  
  $table .= '</table>';
  
  $defaultTable = $table;
  unset($table);
}

/* Top html data */
require($xtmGlobal->inc('bottom.inc'));

/* End */
include('include/exit.inc');
?>
