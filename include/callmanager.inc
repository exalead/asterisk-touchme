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
 *                               Call Manager
 *
 *****************************************************************************/

/**
 * "TouchMe" - wrapper for asterisk call manager.
 * Code by Exalead, 2008
 **/
class TouchMeCall {

  var $ch;
  var $errstr;
  var $serverName;
  var $dialValidString;
  var $internalRegexp;
  var $internalExternRegexp;
  var $externToInternalReplace;
  var $internalToExternReplace;
  var $country;
  var $countryReplacement;
  var $world;
  var $worldReplacement;
  var $externalPrefix;
  var $pickUpTimeout;
  var $extension;
  
  function __construct($address, $port, $user, $pass, $lockDir,
                       /* */
                       $dialValidString,
                       $internalRegexp,
                       /* */
                       $internalExternRegexp,
                       $externInternalRegexp,
                       /* */
                       $externToInternalReplace,
                       $internalToExternReplace,
                       /* */
                       $country,
                       $countryReplacement,
                       $world,
                       $worldReplacement,
                       $externalPrefix,
                       /* */
                       $pickUpTimeout,
                       $extension) {
    $this->address = $address;
    $this->port = $port;
    $this->user = $user;
    $this->pass = $pass;
    $this->lockDir = $lockDir;
    $this->dialValidString = $dialValidString;
    $this->internalRegexp = $internalRegexp;
    /* */
    $this->internalExternRegexp = $internalExternRegexp;
    $this->externInternalRegexp = $externInternalRegexp;
    /* */
    $this->externToInternalReplace = $externToInternalReplace;
    $this->internalToExternReplace = $internalToExternReplace;
    /* */
    $this->country = $country;
    $this->countryReplacement = $countryReplacement;
    $this->world = $world;
    $this->worldReplacement = $worldReplacement;
    $this->externalPrefix = $externalPrefix;
    /* */
    $this->pickUpTimeout = $pickUpTimeout;
    $this->extension = $extension;
  }

  function __destruct() {
    $this->close();
  }
  
  function readLine() {
    $this->clearError();
    $line = @fgets($this->ch, 32768);
    if ($line !== false) {
      if (substr($line, -2) == "\r\n") {
        $line = substr($line, 0, strlen($line) - 2);
      }
      else if (substr($line, -1) == "\n" || substr($line, -1) == "\r") {
        $line = substr($line, 0, strlen($line) - 1);
      }
      $this->debug('<< ' . $line);
      return $line;
    }
    $this->throwError(error_get_last());
    return false;
  }

  /* send the request $cmd */
  function sendRequest($cmd) {
    /* form request */
    $this->clearError();
    $str = '';
    foreach($cmd as $key => $value) {
      $line = $key . ': ' . $value;
      $str .= $line . "\r\n";
      $this->debug('>> ' . $line);
    }
    $str .= "\r\n";
    if (@fwrite($this->ch, $str) == false && !@fflush($this->ch)) {
      $this->throwError(error_get_last());
      return false;
    }
    return true;
  }

  /* get a call manager reply chunk */
  function getReply() {
    $this->clearError();
    $reply = array();
    for($line = $this->readLine()
          ; $line !== false && strlen($line) != 0
          ; $line = $this->readLine()) {
      $arr = explode(':', $line);
      if (count($arr) == 2) {
        $field = strtolower($arr[0]);
        $value = ltrim($arr[1]);
        $reply[$field] = $value;
      }
    }
    if ($line !== false) {
      return $reply;
    }
    return false;
  }
  
  /** Take a command and return the result. **/
  function takeThis($cmd) {
    $this->clearError();
    if ($this->sendRequest($cmd)) {
      for(;;) {
        $reply = $this->getReply();
        if ($reply === false || isset($reply['response'])) {
          break;
        }
      }
      if ($reply !== false) {
        return $reply;
      }
    }
    return false;
  }

  /* Clear */
  function clearError() {
    if (isset($this->errstr)) {
      unset($this->errstr);
    }
  }

  /* Throw an error */
  function throwError($msg) {
    $this->errstr = $msg;
  }
  
  function expect($arr, $expect) {
    if ($arr === false) {
      return false;
    }
    if (!isset($arr['response'])) {
      $this->throwError('malformed reply');
      return false;
    }
    if (strtolower($arr['response']) == $expect) {
      return true;
    }
    $moreInfo = (isset($arr['message']) ? ' ('.$arr['message'].')' : '');
    $this->throwError('bad reply: ' . $arr['response'] . $moreInfo);
    return false;
  }
  
  function expectSuccess($arr) {
    return $this->expect($arr, 'success');
  }

  function close() {
    if (isset($this->ch)) {
      fclose($this->ch);
      unset($this->ch);
    }
  }
  
  function login() {
    $this->close();
    $this->clearError();
    $ch = @fsockopen($this->address, $this->port, $errno, $errstr, 1);
    if (is_resource($ch)) {
      $this->ch = $ch;
      stream_set_timeout($this->ch, 10);
      $serverName = $this->readLine();
      if ($serverName !== false) {
        $this->serverName = $serverName;
        if ($this->expectSuccess($this->takeThis(array('Action' => 'Login',
                                                       'Username' =>
                                                       $this->user,
                                                       'Secret' =>
                                                       $this->pass)))) {
           return true;
        }
      }
    } else {
      $this->throwError(error_get_last());
    }
    $this->close();
    return false;
  }

  function logout() {
    return $this->expect($this->takeThis(array('Action' => 'Logoff')),
                         'goodbye');
  }
  
  function ping() {
    return $this->expect($this->takeThis(array('Action' => 'Ping')), 'pong');
  }
   
  function isInternal($number) {
    if ($number !== false) {
      return TouchMeCall::multiple_ereg($this->internalRegexp, $number);
    }
    return false;
  }

  function isInternalExternal($number) {
    if ($number !== false) {
      return TouchMeCall::multiple_ereg($this->internalExternRegexp, $number);
    }
    return false;
  }

  function isExternalInternal($number) {
    return TouchMeCall::multiple_ereg($this->externInternalRegexp, $number);
  }

  /* The dial number on the external world point of view */
  function getExternalNumber($number) {
    if ($number !== false) {
      if ($this->isInternal($number)) {
        if ($this->isInternalExternal($number)) {
          return
            TouchMeCall::multiple_ereg_replace($this->internalToExternReplace,
                                               $number);
        }
      } else {
        return $number;
      }
    }
    return false;
  }

  /* Get the dial number on the internal point of view */
  function getExternalDialNumber($number) {
    if ($number !== false) {
      $number = getExternalNumber($number);
      if ($number !== false) {
        return $this->externalPrefix . $number;
      }
    }
    return false;
  }

  function getDialNumber($number) {
    if ($number !== false) {
      if ($this->isInternal($number)) {
        return $number;
      } else {
        return $this->externalPrefix . $number;
      }
    }
    return false;
  }
  
  function getChannelByNumber($number) {
    if ($number !== false) {
      if ($this->isInternal($number)) {
        return 'SIP/' . $number;
      }
      $this->throwError('illegal caller number: ' . $number);
    }
    return false;
  }
  
  function cleanupNumber($number) {
    if ($number !== false) {
      $number = str_replace(array(' ', "\t", '-', '.', '(', ')'), '', $number);
      if (substr($number, 0, strlen($this->country)) == $this->country) {
        $number = $this->countryReplacement
          . substr($number, strlen($this->country));
      } else if (substr($number, 0, strlen($this->world)) == $this->world) {
        $number = $this->worldReplacement
          . substr($number, strlen($this->world));
      }
      if (TouchMeCall::multiple_ereg($this->dialValidString, $number)) {
        return $number;
      } else {
        $this->throwError('bad number format: ' . $number);
      }
    }
    return false;
  }

  static function multiple_ereg($reg, $str) {
    if (is_array($reg)) {
      for($i = 0 ; $i < count($reg) ; $i++) {
        if (ereg($reg[$i], $str)) {
          return true;
        }
      }
      return false;
    } else {
      return ereg($reg, $str);
    }
  }

  static function multiple_ereg_replace($reg, $str) {
    if (is_array($reg[0])) {
      for($i = 0 ; $i < count($reg) ; $i++) {
        $str = ereg_replace($reg[$i][0], $reg[$i][1], $str);
      }
      return $str;
    } else {
      return ereg_replace($reg[0], $reg[1], $str);
    }
  }
  
  function getInternalNumber($number) {
    $number = $this->cleanupNumber($number);
    if ($number !== false) {
      if ($this->isInternal($number)) {
        return $number;
      } else {
        if ($this->isExternalInternal($number)) {
          return
            TouchMeCall::multiple_ereg_replace($this->externToInternalReplace,
                                               $number);
        }
      }
    }
    return false;
  }

  function getReducedNumber($number) {
    $number = $this->cleanupNumber($number);
    if ($number !== false) {
      $int = $this->getInternalNumber($number);
      if ($int !== false) {
        return $int;
      } else {
        return $number;
      }
    }
    return false;
  }
  
  function contact($caller, $callee, $caller_title='Pick up') {
    $this->clearError();
    $caller = $this->getReducedNumber($caller);
    $callee = $this->cleanupNumber($callee);
    $channel = $this->getChannelByNumber($caller);
    $dial = $this->getDialNumber($callee);
    if ($caller !== false && $callee !== false && $channel !== false
        && $dial !== false) {
      if ($caller != $callee) {
        if (($lock = $this->getLock($caller)) !== false) {
          $cmd = array(
                       'Action' => 'Originate',
                       'Context' => $this->extension,
                       'Priority' => '1',
                       'Channel' => $channel,
                       'Callerid' => $caller_title
                       . '<' . $this->getInternalNumber($caller).'>',
                       'Exten' => $dial,
                       'Timeout' => $this->pickUpTimeout
                       );
          $ret = $this->expectSuccess($this->takeThis($cmd));
          fclose($lock);
          /* Success ? */
          if ($ret) {
            return true;
          }
        } else {
          $this->throwError('a call is already in progress');
        }
      } else {
        $this->throwError('can not call myself');
      }
    } else {
      $this->throwError('missing parameter');
    }
    return false;
  }

  function findStatusOfCaller_($caller) {
    $this->clearError();
    $caller = $this->getInternalNumber($caller);
    $channel = $this->getChannelByNumber($caller);
    if ($caller !== false && $channel !== false) {
      /* Humm, list *all* status to get the one we want .. */
      $cmd = array('Action' => 'Status'
                   /*, 'Channel' => $channel.'*'*/
                   );
      if ($this->expectSuccess($this->takeThis($cmd))) {
        /* read until we find an "Event: StatusComplete" */
        $channel = false;
        while( ( $msg = $this->getReply() ) !== false) {
          /* Example:
             Event: Status
             Privilege: Call
             Channel: SIP/9924-08298ba0
             CallerID: 9924
             CallerIDName: 9924
             Account:
             State: Ringing
             Uniqueid: 1205999370.28044
          */
          if (isset($msg['event']) && strtolower($msg['event']) == 'status'
              && isset($msg['channel'])
              && ereg('^SIP/'.$caller.'-', $msg['channel'])) {
            $channel = $msg;
          }
          /* End of list */
          if (isset($msg['event'])
              && strtolower($msg['event']) == 'statuscomplete') {
            break;
          }
        }
        if ($channel == false) {
          $this->throwError('line not active');
        }
        return $channel;
      }
    }
    return false;
  }
  
  function findChannelOfCaller_($caller) {
    $status = $this->findStatusOfCaller_($caller);
    if ($status !== false && isset($status['channel'])) {
      return $status['channel'];
    }
    return false;
  }
  
  function findStateOfCaller_($caller) {
    $status = $this->findStatusOfCaller_($caller);
    if ($status !== false && isset($status['state'])) {
      return $status['state'];
    }
    return false;
  }

  function hangup($caller) {
    $this->clearError();
    $caller = $this->getInternalNumber($caller);
    $channel = $this->getChannelByNumber($caller);
    if ($caller !== false && $channel !== false) {
      $real_channel = $this->findChannelOfCaller_($caller);
      if ($real_channel !== false) {
        $cmd = array('Action' => 'Hangup',
                     'Channel' => $real_channel);
        return $this->expectSuccess($this->takeThis($cmd));
      } else {
        // $this->throwError('error..');
      }
    }
    return false;
  }

  function getLineStatus($caller) {
    $this->clearError();
    $caller = $this->getInternalNumber($caller);
    $channel = $this->getChannelByNumber($caller);
    if ($caller !== false && $channel !== false) {
      $real_channel = $this->findChannelOfCaller_($caller);
      if ($real_channel !== false) {
        return true;
      }
    }
    return false;
  }

  function isBusy($caller) {
    $this->clearError();
    $caller = $this->getInternalNumber($caller);
    $channel = $this->getChannelByNumber($caller);
    if ($caller !== false && $channel !== false) {
      $real_channel = $this->findChannelOfCaller_($caller);
      if ($real_channel !== false) {
        return true;
      }
    }
    return false;
  }

  function getState($caller) {
    $this->clearError();
    $caller = $this->getInternalNumber($caller);
    $state = $this->findStateOfCaller_($caller);
    return $state;
  }

  function getLock($caller) {
    $dirname = $this->lockDir. '/.callme_lock';
    $filename = $dirname . '/' . $caller;
    $fp = @fopen($filename, "w+");
    if (!is_resource($fp)) {
      @mkdir($dirname);
      $fp = @fopen($filename, "w+");
    }
    if (!is_resource($fp)) {
      $this->throwError(error_get_last());
      return false;
    }
    if (!flock($fp, LOCK_EX | LOCK_NB)) {
      fclose($fp);
      $this->throwError('lock is busy');
      return false;
    }
    return $fp;
  }
  
  static function log($message) {
    syslog(LOG_DEBUG, $message);
  }

  static function debug($message) {
    //syslog(LOG_DEBUG, $message);
    //echo "* " . $message . "<br />";
  }

  function getErrorMessage() {
    if (isset($this->errstr)) {
      return $this->errstr;
    }
    return 'unknown error';
  }
  
  function checkCredentials($number, $caller) {
    if ($caller !== false) {
      $number = $this->cleanupNumber($number);
      if ($number !== false) {
        $internal = $this->getInternalNumber($number);
        if ($internal == $caller) {
          return true;
        } else {
          $this->throwError('bad caller id: ' . $internal .' != ' . $caller);
        }
      }
    }
    return false;
  }
}

?>
