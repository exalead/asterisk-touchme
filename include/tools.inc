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
 *                               Tools
 *
 *****************************************************************************/

function getDialer($number) {
  global $caller, $xtmGlobal;
  if ($number != $caller && $caller != '*') {
    return '<a href="#">'
      . '<img'
      . ' src="' . $xtmGlobal->image('dial.gif') . '"'
      . ' alt="'._T('dial').' '.htmlspecialchars($number).'"'
      . ' title="'._T('Call').'"'
      . ' onmouseover="window.status=\''._T('dial').' '.$number.'\'; '
      .   'return true;"'
      . ' onmouseout="window.status=\'\'; return true;"'
      . ' onclick="submitMe(\''.$number.'\');"'
      . ' border="0" '
      . '/>'
      . '</a>';
  } else {
    return '';
  }
}

function getRoomLocator($number) {
  global $xtmConfig;
  
  if (isset($xtmConfig['ldap']['roomnumber_locator']) && strlen($number) > 0) {
    $loc = $xtmConfig['ldap']['roomnumber_locator'];
    $loc = str_replace('%1', $number, $loc);
    return '<a '
      . 'href="'.htmlspecialchars($loc).'"'
      . ' title="'._T("Room location").'"'
      . ' onmouseover="window.status=\''._T('dial').' '.$number.'\'; '
      .   'return true;"'
      . ' onmouseout="window.status=\'\'; return true;"'
      . ' onclick="window.open(\''.htmlspecialchars($loc).'\', null, '
      . '\'height=600,width=600,status=no,toolbar=no,location=no,'
      . 'menubar=no,titlebar=no\'); return false"'
      . '>'
      . $number
      . '</a>';
  } else {
    return '';
  }
}

function getLdapImage($cn, $w, $h, $title=false) {
  $url = 'image.php?cn='.htmlspecialchars($cn);
  return '<img src="'.$url.'" '
    . 'border="0" width="'.$w.'" height="'.$h.'"'
    . ' alt="'.htmlspecialchars($cn).'"'
    . ' onclick="window.open(\''.$url.'\', null, '
    . '\'height=200,width=320,status=no,toolbar=no,location=no,'
    . 'menubar=no,titlebar=no\'); return false"'
    . ( $title !== false
        ? ( ' title="'.htmlspecialchars($cn)
            . ( strlen($title) > 0 ? ' ('.$title.')' : '' )
            . '"'
            )
        : '' )
    .' />';
}

function getLdapSelector($type, $id=false, $name=false) {
  global $xtmConfig;
  $choice = '';
  if ($type[0] == '@') {
    $onchange='';
    $def = $xtmConfig['ldap']['custom_edit'][substr($type, 1)];
    if (isset($def['list'])) {
      $g = $def;
      $def = $def['list'];
      if (isset($g['list_onchange'])) {
        $onchange .= '; '.$g['list_onchange'];
      }
      if (isset($g['list_sync_with']) && $id !== false) {
        foreach($g['list_sync_with'] as $sid)  {
          $onchange .= '; document.getElementsByName(\'sel_ldap_edit['.$sid.']\')[0].selectedIndex = selectedIndex; document.getElementsByName(\'sel_ldap_edit['.$sid.']\')[0].onchange()';
        }
      }
    }
    $choice = '<select'
      . ($id !== false ? ' id="sel_'.$id.'"' : '')
      . ($id !== false ? ' onchange="'.$id.'.value=value'.$onchange.'"' : '')
      . ($name !== false ? ' name="'.$name.'"' : '')
      . '>';
    $choice .= '<option value=""><option>';
    foreach($def as $e) {
      if (is_array($e)) {
        $name = $e[0];
        $val = $e[1];
        if (substr($val, 0, 1) == '*') {  /* hack: '* foo' */
          $val = $name.substr($val, 1);
        }
      } else {
        $name = $e;
        $val = $e;
      }
      $choice .= '<option value="'.htmlspecialchars($name).'">'
        . htmlspecialchars(_T($val)).'</option>';
    }
    $choice .= '</select>';
  }
  return $choice;
}

function prettyNumber($num) {
  if (strlen($num) == 10) {
    return substr($num, 0, 2). '.'
      . substr($num, 2, 2). '.'
      . substr($num, 4, 2). '.'
      . substr($num, 6, 2). '.'
      . substr($num, 8, 2);
  } else if (strlen($num) > 10) {
    return substr($num, 0, 3).'.'.substr($num, 3);
  }
  return $num;
}

function getNumberForm($number) {
  global $caller;
  if ($number != $caller && $caller != '*') {
    return '<a href="#" onclick="submitMe(\''.$number.'\');"'
      . ' title="'._T('Call').'"'
      . '>'
      . prettyNumber($number).'</a>';
  } else {
    return prettyNumber($number);
  }
}

function getAltTr(&$toggle) {
  global $xtmGlobal;
  $toggle = ( ( $toggle + 1 ) % 2 );
  return '<tr background="'.($toggle == 0 ? $xtmGlobal->image('bk1.gif')
                             : $xtmGlobal->image('bk2.gif')).'">';
}

function getSelTr($busy=false) {
  global $xtmGlobal;
  return '<tr background="'.($busy ? $xtmGlobal->image('bkb.gif')
                             : $xtmGlobal->image('bks.gif')).'">';
}

function getMailForm($email) {
  $arr = explode('@', $email);
  $display = $arr[0];
  return '<a href="mailto:'.$email.'"'
    .' title="'._T('Send an email').'"'
    .'>'.$display.'</a>';
}

function cmp_n($a, $b) {
  return $b - $a;
}
function getSortName_(&$e, $k1, $k2) {
  $s = '';
  if (isset($e[$k1])) {
    $s .= $e[$k1];
  }
  if (isset($e[$k2])) {
    $s .= $e[$k2];
  }
  return $s;
}
function getCat($c) {
  global $xtmConfig;
  if (is_array($xtmConfig['ldap']['directory_sort'])) {
    foreach($xtmConfig['ldap']['directory_sort'] as $s => $r) {
      $c = ereg_replace($s, chr($r), $c);
    }
  }
  return $c;
}
function cmp_f($a, $b) {
  global $xtmConfig;
  global $slb_;
  $ca = getCat($a['_category']);
  $cb = getCat($b['_category']);
  if ($cb == $ca) {
    $k1 = $xtmConfig['ldap']['sn_name'];
    $k2 = $xtmConfig['ldap']['gn_name'];
    return strcasecmp(getSortName_($a, $k1, $k2), getSortName_($b, $k1, $k2));
  } else {
/*     $order = $slb_[$cb] - $slb_[$ca]; */
/*     if ($order != 0) { */
/*       return $order; */
/*     } else { */
      return strcasecmp($ca, $cb);
/*     } */
  }
}
function sortLdapBy(&$find, $dn, $ou, $skip_ou, $department='') {
  global $slb_;
  $slb_ = array();
  $moreCat = '';
  foreach($find as &$entry) {
    $categ = '';
    if (isset($entry[$dn])) {
      foreach(explode(',', $entry[$dn]) as $e) {
        $a = explode('=', $e);
        if (count($a) == 2 && $a[0] == $ou && !isset($skip_ou[$a[1]])) {
          if (strlen($categ) > 0) {
            $categ .= '/';
          }
          $categ .= _T($a[1]);
        }
      }
      if ($department != '' && isset($entry[$department])) {
        $categ .= '/'._T($entry[$department]);
      }
/*       if (strlen($categ) == 0) { */
/*         $categ = 'unknown'; */
/*       } */
      $categ2 = getCat($categ);
      if (!isset($slb_[$categ2])) {
        $slb_[$categ2] = 1;
      } else {
        $slb_[$categ2]++;
      }
      $entry['_category'] = $categ;
    }
  }
  $tounset = array();
  foreach($find as $key => &$entry) {
    if (strlen($entry['_category']) == 0) {
      $tounset[] = $key;
    }
  }
  foreach($tounset as $key) {
    unset($find[$key]);
  }
  unset($tounset);
  uasort($slb_, "cmp_n");
  usort($find, "cmp_f");
}

function sortLdapStructure(&$find) {
  global $xtmConfig;
  sortLdapBy($find, $xtmConfig['ldap']['dn_name'],
             $xtmConfig['ldap']['ou_name'],
             $xtmConfig['ldap']['skip_ou'],
             $xtmConfig['ldap']['department_name']);
}

function notMac($s) {
  global $_SERVER;
  if (strpos($_SERVER['HTTP_USER_AGENT'], 'AppleWebKit/') === false) {
    return $s;
  } else {
    return '';
  }
}

function bootstrapNames(&$ldap) {
  global $cn_name,
    $dn_name,
    $phone_name, 
    $phone_ext_name, 
    $fax_name, 
    $o_name, 
    $l_name, 
    $st_name, 
    $street_name, 
    $postal_name, 
    $image_name, 
    $mail_name, 
    $sn_name, 
    $role_name, 
    $gn_name, 
    $description_name, 
    $department_name, 
    $roomnumber_name;
  $cn_name = $ldap['cn_name'];
  $dn_name = $ldap['dn_name'];
  $phone_name = $ldap['phone_name'];
  $phone_ext_name = $ldap['phone_ext_name'];
  $fax_name = $ldap['fax_name'];
  $o_name = $ldap['organization_name'];
  $l_name = $ldap['city_name'];
  $st_name = $ldap['country_name'];
  $street_name = $ldap['street_name'];
  $postal_name = $ldap['postal_name'];
  $image_name = $ldap['image_name'];
  $mail_name = $ldap['mail_name'];
  $sn_name = $ldap['sn_name'];
  $role_name = $ldap['role_name'];
  $gn_name = $ldap['gn_name'];
  $description_name = $ldap['description_name'];
  $department_name = $ldap['department_name'];
  $roomnumber_name = $ldap['roomnumber_name'];
}

/* Common names */
bootstrapNames($xtmConfig['ldap']);
?>