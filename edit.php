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
 *                               Editing
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
$callerLogin = $auth->getCredentialsElement('login');

/* Remote user info */
$remoteUser = $callerLogin . '@'
  . $_SERVER["REMOTE_ADDR"] . ':' . $_SERVER["REMOTE_PORT"];

$defaultHeight = 256;

function getAllow($edit) {
  $allow = array();
  foreach($edit as $key => $type) {
    if ($type != 'hidden') {
      $allow[$key] = true;
    }
  }
  return $allow;
}

/* Display table */
$table = '';

/* Process */
if (isset($_REQUEST['ldap_do_edit'])
    && isset($_REQUEST['ldap_edit'])) {
  $ed = $_REQUEST['ldap_edit'];
  $duplicate = isset($_REQUEST['duplicate'])
    && intval($_REQUEST['duplicate']) != 0;
 
  /* Check ACLs */
  if (!isset($ed[$cn_name])) {
    $message = 'Forbidden operation for '.$remoteUser;
    Header('HTTP/1.1 403 '.$message);
    require($xtmGlobal->inc('forbidden.inc'));
    include('include/exit.inc');
    exit(-1);
  }
  $edit = $auth->getCredentialsElement('acl:edit', $ed[$cn_name]);
  if (!is_array($edit)
      || ($duplicate && ( !isset($edit['@duplicate'])
                          || $edit['@duplicate'] != 'text')
          )
      ) {
    $message = 'Forbidden operation for '.$remoteUser;
    Header('HTTP/1.1 403 '.$message);
    require($xtmGlobal->inc('forbidden.inc'));
    include('include/exit.inc');
    exit(-1);
  }
  $allow = getAllow($edit);
  if ($duplicate) {  /* allow to change CN (here, a new entry) */
    $allow[$cn_name] = true;
  }
  
  /* TEMPORARY -- QUICK'N DIRTY IMPORT */
  /*   if (isset($xtmConfig['file-image']['dir']) */
  /*       && strlen($xtmConfig['file-image']['dir']) > 0 */
  /*       && !isset($_FILES['jpegphoto'])) { */
  /*     $imgFilename = $xtmGlobal->image($xtmConfig['file-image']['dir'] */
  /*                                      . '/' */
  /*                                      . strtolower($ed[$mail_name])); */
  /*     if (file_exists('./'.$imgFilename)) { */
  /*       $fp = fopen('./'.$imgFilename, 'rb'); */
  /*       if ($fp) { */
  /*         $data = fread($fp, filesize($imgFilename)); */
  /*         $tmp = tempnam('/tmp', 'tmpimg'); */
  /*         fclose($fp); */
  /*         $fp = fopen($tmp, 'wb'); */
  /*         if ($fp) { */
  /*           fwrite($fp, $data); */
  /*           fclose($fp); */
  /*           $_FILES['jpegphoto'] = array('tmp_name' => $tmp, */
  /*                                        'name' => $imgFilename); */
  /*         } */
  /*       } */
  /*     } */
  /*   } */

  /* Height if editing image */
  $h = $defaultHeight;
  if (isset($_REQUEST['ldap_edit_img_h']) && isset($allow['@height'])) {
    $h = intval($_REQUEST['ldap_edit_img_h']);
    if ($h <= 0 || $h > 1024) {
      $h = $defaultHeight;
    }
  }
  $r = 0;
  if (isset($_REQUEST['ldap_edit_img_r']) && isset($allow['@rotate'])) {
    $r = intval($_REQUEST['ldap_edit_img_r']);
    if ($r != 90 && $r != 180 && $r != 270) {
      $r = 0;
    }
  }

  /* Any image ? */
  if (isset($_REQUEST['ldap_edit_file'])) {
    $files = $_REQUEST['ldap_edit_file'];
    foreach($files as $file) {
      if (isset($_FILES[$file]) && is_array($_FILES[$file])
          && isset($_FILES[$file]['tmp_name'])
          && strlen($_FILES[$file]['tmp_name'])) {
        /* read image and resample it */
        $tmpname = $_FILES[$file]['tmp_name'];
        if (strlen($tmpname) == 0) {
          $message = 'Image too big ? (php.ini)';
        } else if (eregi('\.png$', $_FILES[$file]['name'])) {
          $im = @imagecreatefrompng($tmpname);
        } else if (eregi('\.gif$', $_FILES[$file]['name'])) {
          $im = @imagecreatefromgif($tmpname);
        } else if (eregi('\.(jpeg|jpg)$', $_FILES[$file]['name'])) {
          $im = @imagecreatefromjpeg($tmpname);
        } else {
          $message = _T('Unknown image format');
        }
        if (isset($im) && is_resource($im)) {
          if ($r != 0) {
            $im = imagerotate($im, $r, 0);
            if (!is_resource($im)) {
              $message = _T('Malformed rotated image');
            }
          }
        } else {
          if (!isset($message)) {
            $message = _T('Malformed image');
          }
        }
        if (isset($im) && $im) {
          $img_w = imagesx($im);
          $img_h = imagesy($im);
          if ($img_w > 0 && $img_h > 0) {
            $ratio = $img_h / $img_w;
            $w = intval($h / $ratio);
            $im2 = imagecreatetruecolor($w, $h);
            imageinterlace($im2);
            if ($im2
                && imagecopyresampled($im2, $im, 0, 0, 0, 0, $w, $h,
                                      $img_w, $img_h)
                && imagejpeg($im2, $tmpname)) {
              $fp = fopen($tmpname, 'rb');
              if ($fp) {
                $img_data = fread($fp, filesize($tmpname));
                fclose($fp);
                /* insert data */
                $ed[$file] = $img_data;
              }
            }
          }
          /* cleanup */
          if ($im2) {
            imagedestroy($im2);
          }
          imagedestroy($im);
        }
      }
    }
  }

  if ($duplicate && $_REQUEST['duplicate_cn'] == $ed[$cn_name]) {
    $message = _TERR('This CN already exists, '
                     . 'please change it to duplicate the entry');
  } else if (is_array($ed)) {
    $ldap_adm = $xtmGlobal->getLdapAdmin();
    if ($ldap_adm !== false) {
      syslog(LOG_NOTICE, 'editing "' . $ed[$cn_name] . '" ldap entry for '
             .$remoteUser);
      if ($ldap_adm->edit($ed, $allow, $duplicate) === false) {
        $message = _TERR('Unable to edit entry: '
                         . $ldap_adm->getErrorMessage());
        syslog(LOG_WARNING, 'error while editing "' . $ed[$cn_name]
               . '" ldap entry for ' .$remoteUser . ':'
               . $ldap_adm->getErrorMessage());
      }
    } else {
      $message = _T('Unable to get ldap edit access');
    }
    $ldap_adm->close();
    unset($ldap_adm);
  } else {
    $message = _T('Malformed parameters');
  }

  if (!isset($message)) {
    if (isset($_REQUEST['q']) && strlen($_REQUEST['q']) != 0) {
      $q = $_REQUEST['q'];
    }
    Header('Location: bottom.php'.(isset($q)?'?q='.urlencode($q):''));
    exit(0);
  }
  
}

/* Args */
unset($q);
unset($busyWait);
if (isset($_REQUEST['q']) && strlen($_REQUEST['q']) != 0) {
  $q = $_REQUEST['q'];
  $duplicate = isset($_REQUEST['duplicate']);
  if (isset($currLangCharset_)) {
    $q = iconv('utf-8', $currLangCharset_, $q);
  }
  if (substr($q, 0, 6) == 'debug:') {  /* guess what */
    $xtmDebug = true;
    $q = substr($q, 6);
  }
  $q = _CLEAN($q);

  /* No bad characters ? */
  if (eregi($xtmConfig['ldap']['valid_chars'], $q)) {
    /* (Re)Check ACLs */
    $edit = $auth->getCredentialsElement('acl:edit', $q);
    if (!is_array($edit)
        || ($duplicate && ( !isset($edit['@duplicate'])
                            || $edit['@duplicate'] != 'text')
            )
        ) {
      $message = 'Forbidden operation for '.$remoteUser;
      Header('HTTP/1.1 403 '.$message);
      require($xtmGlobal->inc('forbidden.inc'));
      include('include/exit.inc');
      exit(-1);
    }
    $allow = getAllow($edit);

    /* allow to change CN (here, a new entry) */
    if ($duplicate && isset($edit[$cn_name])) {
      $edit[$cn_name] = 'text';
    }

    /* Create a new ldap connection */
    $ldap = $xtmGlobal->getLdap();
    $what = $cn_name;
    $find = $ldap->findUserNumber($q, true, $what, 1);
    unset($ldap);

    /* User found */
    if (is_array($find)) {
      $entry = $find[0];
      $table .= '<form method="POST" enctype="multipart/form-data">'
        . '<input type="hidden" name="MAX_FILE_SIZE" value="10000000" />'
        . ( $duplicate
            ? '<input type="hidden" name="duplicate" value="1" />'
            . '<input type="hidden" name="duplicate_cn" value="'
            .htmlspecialchars($entry[$cn_name]).'" />'
            : '' )
        ;
      $table .= '<table border="0"'
        . ' cellpadding="1"'
        . ' cellspacing="1">';
      $id = 0;
      foreach($edit as $key => $atype) {
        if ($key[0] == '@') {  /* special */
          continue;
        }
        $nicetitle = $key;
        $id++;
        unset($comment);
        if (is_array($atype)) {
          $type = $atype['type'];
          if (isset($atype['comment'])) {
            $comment = _T($atype['comment']);
          }
          if (isset($atype['title'])) {
            $nicetitle = $atype['title'];
          }
        } else {
          $type = $atype;
        }
        $value = isset($entry[$key]) ? $entry[$key] : '';
        $binary = $type == 'image';
        $hidden = $type == 'hidden';
        $textarea = $type == 'textarea';
        unset($choice);
        if ($type[0] == '@') {
          $choice = getLdapSelector($type, 'edit_'.$id,
                                    'sel_ldap_edit['.$key.']');
        }

        if ($hidden) {
          $field = '<input type="hidden" '
            . 'name="ldap_edit['.$key.']" '
            . 'value="'. htmlspecialchars($value) . '" />'
            . htmlspecialchars($value);
        } else if ($binary) {
          $field = ( $value ? getLdapImage($q, 64, 64) : '')
            . '<input type="hidden" name="ldap_edit_file['.$key.']" '
            . 'value="'.$key.'" />'
               . '<input type="file" name="'.$key.'" size="16" />';
        } else if ($textarea) {
          $field =
            '<textarea rows="1" cols="40" id="edit_'.$id
            .'" name="ldap_edit['.$key.']">'
            . htmlspecialchars($value)
            .'</textarea>';
        } else {
          $field =
            '<input id="edit_'.$id.'" name="ldap_edit['.$key.']" value="'
            . htmlspecialchars($value) . '" />';
        }

        if ($duplicate && $key == $cn_name) {
          $field = '<table bgcolor="red"><tr><td>'.$field.'</td></tr></table>';
        }
        
        $table .= '<tr>'
          . '<td title="'.htmlspecialchars(_T($nicetitle).' ('.$key.')').'">'
          . htmlspecialchars(_T($nicetitle))
          . '</td>'
          . '<td>=</td>'
          . '<td>'
          . $field
          . (isset($comment) ? htmlspecialchars($comment) : '')
          . (isset($choice) ? $choice : '')
          . '</td>'
          . '</tr>';
      }

      $table .= '<tr><td>'
        . _T('Height').'</td>'
        . '<td></td>'
        . '<td><input type="'.(isset($allow['@height']) ? 'text' : 'hidden')
        . '" name="ldap_edit_img_h" value="'.$defaultHeight.'">'
        . (isset($allow['@height']) ? '' : $defaultHeight)
        . '</td></tr>';
      $table .= '<tr><td>'
        . _T('Rotate').'</td>'
        . '<td></td>'
        . '<td>'
        . getLdapSelector('@rotate', false, 'ldap_edit_img_r')
        . '</td></tr>';
      
      $table .= '<tr><td></td><td></td><td align="right">'
        . '<input type="submit" name="ldap_do_edit" value="'._T('Modify').'">'
        . '</td></tr>';
      
      $table .= '</table>';
      $table .= '</form>';
    } else if ($find === true) {
      $defaultTable = _T('too many results');
    } else {
      $defaultTable = '<i>'._T("No matches").'!</i>';
    }
  } else {
    $message = _T('bad characters');
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
