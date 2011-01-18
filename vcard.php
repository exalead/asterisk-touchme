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
 *                                vCard
 *
 *****************************************************************************/

function qp_encode($s) {
  global $xtmConfig;
  if ($xtmConfig['ldap']['encoding'] != 'utf-8') {
    $s = iconv('utf-8', $xtmConfig['ldap']['encoding'], $s);
  }
  return str_replace(array(';', ':', ',', "\r", "\n", "\t"),
                     array("\\;", "\\:", "\\,", "", "\\n", "\\t"),
                     $s);
  //return str_replace('%', '=', rawurlencode($s));
}

function no_spaces($s) {
  return str_replace(array(' ', "\t", "\r", "\n"), array('', '', '', ''), $s);
}

function qp_entry(&$e, $name) {
  if (isset($e[$name])) {
    return qp_encode($e[$name]);
  } else {
    return '';
  }
}

function get_vline($name, $data, $atts=false) {
  return strtoupper($name).($atts !== false ? ';'
                            . strtoupper(is_array($atts)
                                         ? implode(';', $atts)
                                         : $atts
                                         )
                            : '')
    .':'.$data."\r\n";
}

function get_entryline(&$e, $name, $where, $atts=false) {
  return get_vline($name, qp_entry($e, $where), $atts);
}

function getZipName() {
  $file = tempnam(sys_get_temp_dir(), "phonebook"); 
  @unlink($file);
  $file .= '.zip';
  return $file;
}

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

/* Args */
unset($q);
unset($busyWait);
$q = '';  /* match all */
if (isset($_REQUEST['cn'])) {
  $q = $_REQUEST['cn'];
}
$zip = false;
$csv = false;  /* not yet done */
if (isset($_REQUEST['format'])) {
  if ($_REQUEST['format'] == 'zip') {
    $zip = true;
  }
  else if ($_REQUEST['format'] == 'csv') {
    $csv = true;
  }
  else {
    echo "Bad format: ".$_REQUEST['format'];
    include('include/exit.inc');
    exit(0);
  }
}
/* Create a new ldap connection */
$touch = $xtmGlobal->getTouch();
//$ldap = $xtmGlobal->getLdap();
$what = $cn_name;
$max_hits = 1 /*$xtmConfig['ldap']['search_max']*/;
if ($q == 'all') {  /* 'get all vcard' hack */
  $max_hits = 999;
  $q = '';
}
$find = $xtmGlobal->findUserNumber($q, true, $what, $max_hits);
/* Args */
if (is_array($find) && count($find) >= 1) {
  /* Headers */
  if ($zip) {
    Header('Content-Type: application/zip; '
           . 'filename="phonebook.zip"');
    Header('Content-Disposition: attachment; filename="phonebook.zip"');
    $zipFile = getZipName();
    $zipArc = new ZipArchive;
    $zipRes = $zipArc->open($zipFile, ZipArchive::CREATE);
    if ($zipRes !== TRUE) {
      echo "Can not produce ZIP file";
      include('include/exit.inc');
      exit(0);
    }
  } else if ($csv) {
    Header('Content-Type: application/x-csv; '
           . 'filename="phonebook.csv"');
    Header('Content-Disposition: attachment; filename="phonebook.csv"');
  } else if (count($find) == 1) {
    $entry = $find[0];
    Header('Content-Type: text/x-vcard; charset="'
           .$xtmConfig['ldap']['encoding'].'"; filename="'
           .htmlspecialchars($entry[$cn_name]).'.vcf"');
    Header('Content-Disposition: attachment; filename="'
           .htmlspecialchars($entry[$cn_name]).'.vcf"');
    Header('Content-ID: <'.md5($q).'@localhost>');
  } else {
    Header('Content-Type: text/x-vcard; charset="'
           .$xtmConfig['ldap']['encoding'].'"; '
           . 'filename="phonebook.vcf"');
    Header('Content-Disposition: attachment; filename="phonebook.vcf"');
  }
  //Header('Content-Transfer-Encoding: Quoted-Printable');

  /* Body */
  foreach($find as $entry) {
    /* Different schema: load globals */
    if (isset($entry['_config'])) {
      bootstrapNames($entry['_config']);
    }

    /* Number */
    if (isset($entry[$phone_name])) {
      $num = $touch->getReducedNumber($entry[$phone_name]);
    } else {
      $num = '';
    }

    /* email address not set => skip */
    if ($max_hits > 1 && ( !isset($entry[$mail_name])
                           || strlen($entry[$mail_name]) == 0) ) {
      continue ;
    }

    $vcard = get_vline('begin', 'VCARD')
      . get_vline('version', '2.1')
      . get_vline('charset', $xtmConfig['ldap']['encoding'])
      . notMac(get_vline('source', 'ldap://'.qp_entry($entry, $dn_name)))
      . get_entryline($entry, 'name', $cn_name)
      . get_entryline($entry, 'fn', $cn_name)
      . get_vline('n', strtoupper(qp_entry($entry, $sn_name)) /* family name */
                  .';'.qp_entry($entry, $gn_name)  /* first name */
                  )
      . get_vline('email', qp_entry($entry, $mail_name), 'type=internet')
      . get_entryline($entry, 'adr', $st_name)
      . get_vline('adr', ';'
                  .';'.qp_entry($entry, $street_name)
                  .';'.qp_entry($entry, $l_name)
                  . ';'
                  .';'.qp_entry($entry, $postal_name)
                  .';'.qp_entry($entry, $st_name), 'work')
      . get_entryline($entry, 'role', $role_name)
      . notMac(get_entryline($entry, 'adr.Street', $street_name))
      . get_entryline($entry, 'adr.Locality', $l_name, 'work')
      . notMac(get_entryline($entry, 'adr.PostalCode', $postal_name, 'work'))
      . notMac(get_entryline($entry, 'adr.CountryName', $st_name, 'work'))
      . get_entryline($entry, 'org.Name', $o_name)
      . get_entryline($entry, 'org.Unit', $department_name)
      . get_vline('org', qp_entry($entry, $o_name)
                  .';'.qp_entry($entry, $department_name))
      . get_entryline($entry, 'title', $role_name)
      . get_entryline($entry, 'note', $description_name)
      
      . get_vline('tel', qp_entry($entry, $phone_name), array('work', 'voice'))
      . get_vline('tel', qp_entry($entry, $fax_name), array('work', 'fax'));
    
    $first = true;
    foreach($phone_ext_name as $other) {
      if (isset($entry[$other]) && strlen($entry[$other]) > 0) {
        if ($first) {
          $vcard .= get_vline('tel', qp_encode($entry[$other]),
                              array('cell', 'voice'));
        } else {
          $vcard .= get_vline('tel', qp_encode($entry[$other]),
                              array(qp_encode($other), 'voice', 'msg'));
        }
      }
    }
    if (isset($entry[$image_name])) {
      $vcard .= get_vline('photo', "\r\n "
                          .rtrim(chunk_split(base64_encode($entry[$image_name])
                                              ."\r\n", 72, "\r\n ")),
                          array('encoding=BASE64', 'type=JPEG'));
    }
    
    $vcard .= get_vline('rev', date('c'))
      . get_vline('end', 'VCARD');
    
    /* flush card */
    if ($zip) {
      $zipArc->addFromString($entry[$cn_name].'.vcf', $vcard);
    } else if ($csv) {
      echo qp_entry($entry, $sn_name)
      .';'
      .qp_entry($entry, $gn_name)
      .';'
      .no_spaces(qp_entry($entry, $phone_name))
      .';'
      .no_spaces(qp_entry($entry, $phone_ext_name[0]))
      .';'
      .qp_entry($entry, $role_name)
      .';'
      .qp_entry($entry, $st_name)
      ."\r\n"
      ;
    } else {
      echo $vcard;
    }
  }

  /* Output zipp file */
  if ($zip) {
    $zipArc->close();
    $fp = fopen($zipFile, "rb");
    unlink($zipFile);
    fpassthru($fp);
  }
  
} else {
  Header("HTTP/1.0 404 vCard entry not found");
  echo "<html><head><title>vCard entry not found</title>'
. '</head><body><h1>vCard entry not found</h1></body></html>";
}
  
/* End */
include('include/exit.inc');
?>
