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
 *                            Configuration helpers
 *
 *****************************************************************************/

/**
 * Helper class.
 **/
class TouchMeConfig {
  var $config;
  var $ldap;
  var $ldapEdit;
  var $otherLdap;
  var $touch;
  var $auth;
  var $logged;
  
  /* Constructor */
  function __construct() {
    global $xtmConfig;
    $this->config = &$xtmConfig;
    $this->otherLdap = false;
  }

  /* Destructor */
  function __destruct() {
    $this->close();
  }

  /**
   * Get a skin image filename.
   **/
  function image($filename) {
    return 'skins/'.$this->config['skin'].'/images/' . $filename;
  }

  /**
   * Get a skin filename.
   **/
  function inc($filename) {
    return 'skins/'.$this->config['skin'].'/include/' . $filename;
  }

  /**
   * Close all helper objects.
   **/
  function close() {
    if (isset($ldap)) {
      $ldap->close();
      unset($ldap);
    }
    if (isset($touch)) {
      $touch->close();
      unset($touch);
    }
    if (isset($ldapEdit)) {
      $ldapEdit->close();
      unset($ldapEdit);
    }
    if (isset($otherLdap)) {
      $otherLdap->close();
      unset($otherLdap);
    }
  }
  
  /**
   * Get the TouchMeLdapClient() object.
   ***/
  function getLdap() {
    if (!isset($this->ldap)) {
      $this->ldap = new
        TouchMeLdapClient($this->config['ldap']['host'],
                          $this->config['ldap']['port'],
                          $this->config['ldap']['base'],
                          $this->config['ldap']['phone_name'],
                          $this->config['ldap']['user_name'],
                          $this->config['ldap']['user'],
                          $this->config['ldap']['pass'],
                          isset($this->config['ldap']['query_transform_regexp'])
                          ? $this->config['ldap']['query_transform_regexp']
                          : false
                          );
    }
    return $this->ldap;
  }

  /**
   * Get other directories objects.
   ***/
  function getOtherLdap() {
    if ($this->otherLdap === false
        && isset($this->config['ldap']['external_directories'])) {
      $this->otherLdap = array();
      foreach($this->config['ldap']['external_directories'] as &$config) {
        $ol = array();
        $ol['ldap'] =
          new TouchMeLdapClient($config['host'],
                                $config['port'],
                                $config['base'],
                                $config['phone_name'],
                                $config['user_name'],
                                $config['user'],
                                $config['pass'],
                                isset($config['query_transform_regexp'])
                                ? $config['query_transform_regexp'] : false
                                );
        $ol['config'] = &$config;
        $this->otherLdap[] = $ol;
      }
    }
    return $this->otherLdap;
  }

  /**
   * Search in the current ldap, or if nothing was found, in other directories.
   **/
  function findUserNumber($login, $exact=false, $findWhat='sn', $max=99) {
    $ldap = $this->getLdap();
    $ldap->clearError();
    $find = $ldap->findUserNumber($login, $exact, $findWhat, $max);
    if (is_array($find) && count($find) > 0) {
      return $find;
    } else {
      $ldaps = $this->getOtherLdap();
      if (is_array($ldaps)) {
        foreach($ldaps as &$l) {
          $ldap = $l['ldap'];
          $config = $l['config'];
          $ldap->clearError();
          $find = $ldap->findUserNumber($login, $exact, $findWhat, $max);
          if (is_array($find) && count($find) > 0) {
            foreach($find as &$v) {
              $v['_config'] = &$l['config'];
            }
            return $find;
          }
        }
      }
    }
    return false;
  }

  /**
   * Get the findUserNumber() error message.
   **/
  function getFindErrorMessage() {
    $ldap = $this->getLdap();
    $err = $ldap->getErrorMessage();
    if ($err == false) {
      $ldaps = $this->getOtherLdap();
      if (is_array($ldaps)) {
        foreach($ldaps as $l) {
          $ldap = $l['ldap'];
          $err = $ldap->getErrorMessage();
          if ($err !== false) {
            return $err;
          }
        }
      }
    }
    return false;
  }

  /**
   * Get the TouchMeLdapClient() object.
   ***/
  function getLdapForEdit() {
    if (!isset($this->ldapEdit)) {
      $this->ldapEdit = new
        TouchMeLdapClient($this->config['ldap']['host'],
                          $this->config['ldap']['port'],
                          $this->config['ldap']['base'],
                          $this->config['ldap']['phone_name'],
                          $this->config['ldap']['user_name'],
                          $this->config['ldap']['edit_bind'],
                          $this->config['ldap']['edit_pass'],
                          isset($this->config['ldap']['query_transform_regexp'])
                          ? $this->config['ldap']['query_transform_regexp']
                          : false
                          );
    }
    return $this->ldapEdit;
  }
  
  /**
   * Get the authenticated ldap version, used to modify tables.
   **/
  function getLdapAdmin() {
    $ldap = $this->getLdapForEdit();
    if ($ldap !== false) {
      $ldap->close();
      $ldap->connect($this->config['ldap']['edit_bind'],
                     $this->config['ldap']['edit_pass']);
    }
    return $ldap;
  }
    
  /**
   * Get the TouchMe() object.
   ***/
  function getTouch() {
    if (!isset($this->touch)) {
      $this->touch = new
        TouchMeCall($this->config['callmanager']['host'],
                    $this->config['callmanager']['port'],
                    $this->config['callmanager']['user'],
                    $this->config['callmanager']['pass'],
                    $this->config['callmanager']['tmpdir'],
                    /* Valid chars, and all internal class */
                    $this->config['callmanager']['dialValidString'],
                    $this->config['callmanager']['internalRegexp'],
                    /* Internal with SDA recognition */
                    $this->config['callmanager']['internalExternRegexp'],
                    $this->config['callmanager']['externInternalRegexp'],
                    /* Internal with SDA transformation */
                    $this->config['callmanager']['externToInternalReplace'],
                    $this->config['callmanager']['internalToExternReplace'],
                    /* Country and world prefix handling */
                    $this->config['callmanager']['country'],
                    $this->config['callmanager']['countryReplacement'],
                    $this->config['callmanager']['world'],
                    $this->config['callmanager']['worldReplacement'],
                    $this->config['callmanager']['externalPrefix'],
                    /* Other settings */
                    $this->config['callmanager']['pickUpTimeout'],
                    $this->config['callmanager']['extension']
                    );
    }
    return $this->touch;
  }

  function touchLogin() {
    if (!isset($this->logged)) {
      $touch = $this->getTouch();
      $this->logged = $touch->login();
    }
    return $this->logged;
  }
  
  /**
   * Get the TouchMeAuth() object.
   ***/
  function getAuth() {
    if (!isset($this->auth)) {
      $this->auth = new TouchMeAuth($this->config['auth']);
    }
    return $this->auth;
  }

}

?>
