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
 *                              Authentication
 *
 *****************************************************************************/

/* Auth providers */
require_once('auths/ldap.inc');
require_once('auths/kerberos.inc');
require_once('auths/imap.inc');
require_once('auths/ssh.inc');
require_once('auths/ftp.inc');
require_once('auths/anonymous.inc');

/**
 * Auth class.
 **/
class TouchMeAuth {
  var $method;
  var $dispatchMethod;
  var $auth;
  var $magic;
  var $phone_name;
  var $acls;
  
  function __construct($args) {
    global $xtmGlobal;
    $this->cookieMagicName = 'touchMeMagic';
    $this->cookieLoginName = 'touchMeLogin';
    $this->cookiePhoneName = 'touchMePhone';
    $this->cookieCNName = 'touchMeCN';
    $this->method = $args['methods'];
    $this->magic = $args['magic'];
    $this->dispatchMethod = array('ldap' => 'authUserPassLdap',
                                  'kerberos'
                                  => 'authUserPassKerberos',
                                  'imap' => 'authUserPassImap',
                                  'ssh' => 'authUserPassSSH',
                                  'ftp' => 'authUserPassFTP',
                                  'anonymous' => 'authUserPassAnonymous'
                                  );
    $this->acls = $args['acls'];
  }
  
  function __destruct() {
  }

  function authUserPass(&$errMsg) {
    /* scan all available methods */
    if (count($this->method) > 0) {
      foreach($this->method as $name => $args) {
        if (isset($this->dispatchMethod[$name])) {
          $fun = $this->dispatchMethod[$name];
          $ret = $fun($args, $errMsg);
          if ($ret !== false) {
            return $ret;
          }
        } else {
          $errMsg =
            'fatal: unknown authentication method in the configuration';
        }
      }
    } else {
         $errMsg =
           'fatal: no authentication method defined in the configuration';
   }
    return false;
  }
  
  function getMagic($user, $phone, $cn) {
    return substr(md5($this->magic.':'.$user.':'.$phone.':'.$cn), 0, 8);
  }

  function logout() {
    $this->clearCookies();
    $this->auth = array();
  }
  
  function clearCookies() {
    $etime = time() - 1;  /* already expired ; clear (almost) everything */
    setcookie($this->cookieMagicName, '', $etime);
    setcookie($this->cookiePhoneName, '', $etime);
    setcookie($this->cookieCNName, '', $etime);
  }
  
  /* Check credentials */
  function checkCredentials(&$errMsg) {
    global $xtmGlobal;
    global $_COOKIE;
    
    /* Initialize */
    $this->auth = array();
    
    /* Cookie information available ? */
    if (isset($_COOKIE[$this->cookieLoginName])
        && isset($_COOKIE[$this->cookieMagicName])
        && isset($_COOKIE[$this->cookiePhoneName])
        && isset($_COOKIE[$this->cookieCNName])
        && strlen($_COOKIE[$this->cookieLoginName]) > 0
        && strlen($_COOKIE[$this->cookieMagicName]) > 0
        && strlen($_COOKIE[$this->cookiePhoneName]) > 0) {
      /* Magic cookie set */
      $userMagic = $_COOKIE[$this->cookieMagicName];
      $magic = $this->getMagic($_COOKIE[$this->cookieLoginName],
                               $_COOKIE[$this->cookiePhoneName],
                               $_COOKIE[$this->cookieCNName]);
      if ($magic == $userMagic) {
        $this->auth['login'] = $_COOKIE[$this->cookieLoginName];
        $this->auth['phone'] = $_COOKIE[$this->cookiePhoneName];
        $this->auth['cn'] = $_COOKIE[$this->cookieCNName];
        return true;
      } else {
        $errMsg = 'bad cookie';
      }
    }
  
    /* Not passed ; check username/password available */
    if ( ($login = $this->authUserPass($errMsg)) !== false) {
      $this->auth['login'] = $login;

      /* non-anonymous */
      if ($login != '') {
        /* ensure user has a phone numer */
        $ldap = $xtmGlobal->getLdap();
        $infos = $ldap->getUserInfos($this->auth['login']);
        if ($infos !== false) {  /* user exists */
          /* ensure user has a phone number */
          $touch = $xtmGlobal->getTouch();
          $num = $infos['telephonenumber'];
          $num = $touch->getReducedNumber($num);
          unset($touch);
          if (isset($infos['cn'])) {
            $cn = $infos['cn'];
          } else {
            $cn = $num;
          }
          /* store phone number and fancy display name */
          $this->auth['phone'] = $num;
          $this->auth['cn'] = $cn;
        } else {
          $errMsg = $ldap->getErrorMessage();
          $this->auth = array();
        }
      } else {
        $this->auth['login'] = 'guest';
        $this->auth['phone'] = '*';  /* guest */
        $this->auth['cn'] = 'guest';
      }

      /* infos fetched */
      if (count($this->auth) > 0) {
        /* send cookie and notify success */
        $etime = time() + 60*60*24*31*6;  /* valid for 6 months */
        $magic = $this->getMagic($this->auth['login'],
                                 $this->auth['phone'],
                                 $this->auth['cn']);
        setcookie($this->cookieLoginName, $this->auth['login'], $etime);
        setcookie($this->cookiePhoneName, $this->auth['phone'], $etime);
        setcookie($this->cookieCNName, $this->auth['cn'], $etime);
        setcookie($this->cookieMagicName, $magic, $etime);
        return true;  /* SUCCESS! */
      } else {
        $errMsg = $ldap->getErrorMessage();
      }
    } else {
      $this->clearCookies();
    }
    
    /* Failed */
    $this->auth = array();
    return false;
  }
  
  function getCredentialsElement($name, $arg=false) {
    if (isset($this->auth[$name])) {
      return $this->auth[$name];
    } else if (substr($name, 0, 4) == 'acl:') {
      if (isset($this->acls[$this->auth['cn']])) {
        $acnname = substr($name, 4);
        $acls = $this->acls[$this->auth['cn']];
        if (isset($acls[$acnname])) {
          return $acls[$acnname];
        }
      } else if ($arg !== false && $arg == $this->auth['cn']
                 && isset($this->acls['@myself'])) {
        $acnname = substr($name, 4);
        $acls = $this->acls['@myself'];
        if (isset($acls[$acnname])) {
          return $acls[$acnname];
        }
      }
    }
    return false;
  }
  
  /**
   * Ensure that the client is authenticated.
   **/
  function ensureCredentials($blank=false) {
    global $xtmGlobal;
    $errMsg = '';
    $oauth = $xtmGlobal->getAuth();
    if (!$oauth->checkCredentials($errMsg)) {
      if (!$blank) {
        header('Location: login.php'
               . ( strlen($errMsg) != 0 ? '?msg='.urlencode($errMsg) : '' )
               );
          ?>
<html>
<head>
</head>
<body>
   Please wait ..
</body>
</html>
<?php
    } else {
  ?>
<html>
<head>
<script language="javascript">
<!--
   window.parent.location = 'index.php';
// -->
</script>
</head>
<body>
   <?php echo _T('Please authenticate').' ..'; ?>
   <?php echo $errMsg; ?>
</body>
</html>
    <?php
    }
      exit(0);  /* EXIT */
    }
  }
}

?>
