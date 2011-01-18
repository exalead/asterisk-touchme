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
 *                              LDAP helpers
 *
 *****************************************************************************/

/**
 * Ldap client.
 **/
class TouchMeLdapClient {
  var $host;
  var $port;
  var $base;
  var $phone;
  var $uid;
  var $ds;
  var $errmsg;
  var $user;
  var $password;
  var $regexp;
  
  function __construct($server, $port=389, $base, $phone, $uid,
                       $user='', $password='', $regexp=false) {
    $this->host = $server;
    $this->port = $port;
    $this->base = $base;
    $this->phone = $phone;
    $this->uid = $uid;
    $this->user = $user;
    $this->password = $password;
    if (is_array($regexp)) {
      $this->regexp = $regexp;
    } else {
      $this->regexp = false;
    }
  }
  
  function __destruct() {
    $this->close();
  }
  
  function connect() {
    if (isset($this->ds)) {
      return true;
    }
    $ds = ldap_connect($this->host, $this->port);
    if ($ds) {
      $this->ds = $ds;
      ldap_set_option($this->ds, LDAP_OPT_PROTOCOL_VERSION, 3);
      @ldap_set_option($this->ds, LDAP_OPT_NETWORK_TIMEOUT, 1);
      ldap_set_option($this->ds, LDAP_OPT_TIMELIMIT, 1);
      $ldapbind=@ldap_bind($this->ds, $this->user, $this->password);
      if($ldapbind) {
        /* Detect base dn if not set (code by Proformatique) */
        if (!isset($this->base) || $this->base == "" ) {
          $result_read = ldap_read($this->ds,"","(objectclass=*)",
                                   array("defaultNamingContext"));
          $entry_read = ldap_first_entry($this->ds, $result_read);
          $basedn = ldap_get_attributes($this->ds, $entry_read);
          if (isset($basedn['defaultNamingContext'])) {
            $this->base = $basedn['defaultNamingContext'][0];
          }
          if ($this->base == "" ) {
            $result_read = ldap_read($this->ds,"","(objectclass=*)",
                                     array("namingContexts"));
            $entry_read = ldap_first_entry($this->ds, $result_read);
            $basedn = ldap_get_attributes($this->ds, $entry_read);
            $this->base = $basedn['namingContexts'][0];
          }
        }
        return true;
      } else {
        $this->errmsg = 'ldap bind failed';
      }
      return true;
    } else {
      $this->errmsg = 'ldap connection failed';
    }
    return false;
  }

  function clearError() {
    if (isset($this->errmsg)) {
      unset($this->errmsg);
    }
  }

  function getErrorMessage() {
    if (isset($this->errmsg)) {
      return $this->errmsg;
    }
    return false;
  }

  function close() {
    if (isset($this->ds)) {
      ldap_close($this->ds);
      unset($this->ds);
    }
  }

  function debugObj($o) {
    if (is_array($o)) {
      $s = '<table border="1" cellspacing="0" cellpadding="0">';
      foreach($o as $k => $v) {
        $s .= '<tr><td>'.$this->debugObj($k).'</td>'
          . '<td>'.$this->debugObj($v).'</tr>';
      }
      $s .= '</table>';
      return $s;
    } else if (is_string($o)) {
      return htmlspecialchars($o);
    } else {
      return htmlspecialchars(serialize($o));
    }
  }

  function ldap_get_entries_binary($sr) {
    $arr = array();
    for($index = 0, $e = ldap_first_entry($this->ds, $sr)
          ; is_resource($e)
          ; $e = ldap_next_entry($this->ds, $e), $index++) {
      $attr = ldap_get_attributes($this->ds, $e);
      if ($attr !== false) {
        $arr[$index] = array();
        for($i = 0 ; $i < $attr['count'] ; $i++) {
          $name = strtolower($attr[$i]);  /* case insensitive */
          $val = ldap_get_values_len($this->ds, $e, $name);
          $arr[$index][$name] = $val;
        }
      }
    }
    $arr['count'] = $index;
    return $arr;
  }

  /* false: error
     true: too manu results
     array: result(s)
  */
  function findUserNumber($login, $exact=false, $findWhat='sn', $max=99) {
    $this->clearError();
    if ($this->connect()) {
      $arr = explode('@', $login);
      $user = $arr[0];
      if (!is_array($findWhat)) {
        $filter = '('.$findWhat
          . '=' . $user . ( $exact ? '' : '*' ) . ')';
      } else {
        $filter = '(|';
        foreach($findWhat as $e) {
          $filter .= '(' . $e . '=' . $user . ( $exact ? '' : '*' ) . ')';
        }
        $filter .= ')';
      }

      /* Transform request (optional) */
      if ($this->regexp !== false) {
        $filter = TouchMeCall::multiple_ereg_replace($this->regexp, $filter);
      }

      /* Execute query */
      $sr = @ldap_search($this->ds, $this->base, $filter/*, $field*/);
      if ($sr) {
        $lcount = ldap_count_entries($this->ds, $sr);
        if ($lcount <= $max) {
          /* less that 4 entries: fetch binary fields too */
          if (false /*$lcount <= 4*/) {
            $info = $this->ldap_get_entries_binary($sr);
            $info['_mode'] = 'binary';
          } else {
            $info = ldap_get_entries($this->ds, $sr);
          }
          {
            global $xtmDebug;
            if (isset($xtmDebug)) {
              echo $this->debugObj($info);
            }
          }
          if ($info['count'] >= 1) {
            $a = array();
            for($i = 0 ; $i < $info['count'] && count($a) <= $max ; $i++) {
              if (isset($info[$i])) {
                $entry = array();
                foreach($info[$i] as $k => $v) {
                  if (isset($v[0]) && is_string($v)) {
                    $entry[$k] = $v;
                  } else if (isset($v[0])) {
                    $entry[$k] = $v[0];
                  }
                }
                $a[] = $entry;
              }
            }
            return $a;
          } else {
            $this->errmsg = 'name not found in ldap server';
          }
        } else {
          $this->errmsg = 'too many entries';
          return true;  /* success anyway */
        }
      } else {
        $this->errmsg = 'login name lookup error with ldap server: '
          . ldap_error($this->ds) . ' (' . $this->base .',' .$filter. ')';
      }
    }
    return false;
  }
  
  function getUserNumber($login) {
    $this->clearError();
    $a = $this->findUserNumber($login, true, $this->uid, 1);
    if (is_array($a)) {
      if (count($a) == 1) {
        if (isset($a[0][$this->phone])) {
          return $a[0][$this->phone];
        } else {
          $this->errmsg = 'this user has no phone number';
        }
      } else if( $info['count'] > 1) {
        $this->errmsg = 'multiple login names found in ldap server';
      }
    } else if ($a === true) {
      $this->errmsg = 'multiple login names found in ldap server';
   }
    return false;
  }
  
  function getUserInfos($login) {
    $this->clearError();
    $a = $this->findUserNumber($login, true, $this->uid, 1);
    if (is_array($a)) {
      if (count($a) == 1) {
        if (isset($a[0][$this->phone])) {
          return $a[0];
        } else {
          $this->errmsg = 'this user has no phone number';
        }
      } else if( $info['count'] > 1) {
        $this->errmsg = 'multiple login names found in ldap server';
      }
    } else if ($a === true) {
      $this->errmsg = 'multiple login names found in ldap server';
    }
    return false;
  }

  function edit($ed, $allow=false, $new_entry=false) {
    if (!is_array($ed) || count($ed) == 0) {
      $this->errmsg = 'missing parameters';
      return false;
    }
    $this->clearError();
    if (isset($ed['cn']) && isset($ed['dn'])) {
      $dn = $ed['dn'];
      $mod = array();
      foreach($ed as $k => $v) {
        if ($k == 'cn' || $k == 'dn') {
          continue;
        }
        /* explicitly restrict edit: skip field */
        if (is_array($allow) && !isset($allow[$k])) {
          continue;
        }
        if (isset($v) && strlen($v) > 0) {
          $mod[$k] = array($v);
        } else {
          $mod[$k] = array();
        }
      }
      if (count($mod) > 0) {
        if ($new_entry) {
          /* replace dn */
          $old_dn = $dn;
          $dn = '';
          foreach(explode(',', $old_dn) as $e) {
            $a = explode('=', $e);
            if (count($a) == 2 && $a[0] == 'cn') {
              $a[1] = $ed['cn'];
            }
            if (strlen($dn) > 0) {
              $dn .= ',';
            }
            $dn .= $a[0].'='.$a[1];
          }
          /* execute query */       
          $success = @ldap_add($this->ds, $dn, $mod);
        } else {
          /* execute query */       
          $success = @ldap_modify($this->ds, $dn, $mod);
        }
        if ($success) {
          return true;
        } else {
          echo $this->debugObj(array('dn' => $dn));
          echo $this->debugObj(array('args' => $mod));
          $this->errmsg = ldap_error($this->ds);
        }
      } else {
        $this->errmsg = 'no rights to edit fields';
      }
    } else {
      $this->errmsg = 'missing cn or dn field';
    }
    return false;
  }
  
}

?>
