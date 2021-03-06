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
 *                                 Translations
 *
 *****************************************************************************/

/* We are in UTF-8 */
@header( "Content-type: text/html; charset=\"utf-8\"" );

function get_ISO_8859_1_UnaccentMap() {
  /* Unaccent ISO-8859-1/ISO-8859-15 */
  return array(
               array(array('�', '�', '�', '�', '�', '�'), 'a'),
               array(array('�'), 'c'),
               array(array('�', '�', '�', '�'), 'e'),
               array(array('�', '�', '�', '�'), 'i'),
               array(array('�'), 'n'),
               array(array('�', '�', '�', '�', '�'), 'o'),
               array(array('�', '�', '�', '�'), 'u'),
               array(array('�', '�'), 'y'),
               array(array('�', '�', '�', '�', '�', '�'), 'A'),
               array(array('�'), 'C'),
               array(array('�', '�', '�', '�'), 'E'),
               array(array('�', '�', '�', '�'), 'I'),
               array(array('�'), 'N'),
               array(array('�', '�', '�', '�', '�'), 'O'),
               array(array('�', '�', '�', '�'), 'U'),
               array(array('�'), 'Y')
               );
}

/* Strings  */
$langs_ = array(
                'Dialing' => array('fr' => 'Num�rotation'),
                'dial' => array('fr' => 'num�roter'),
                'unable to dial' => array('fr' => 'impossible de num�roter'),
                'bad number format' => array('fr'
                                             => 'mauvais format de num�ro'),
                'bad characters' => array('fr' => 'caract�res ill�gaux'),
                'Search' => array('fr' => 'Chercher'),
                'Call' => array('fr' => 'Appeller'),
                'Hangup' => array('fr' => 'Raccrocher'),
                'Error' => array('fr' => 'Erreur'),
                'error' => array('fr' => 'erreur'),
                'unexpected error' => array('fr' => 'erreur inattendue'),
                'unknown error' => array('fr' => 'erreur inconnue'),
                'missing parameter' =>array('fr' => 'param�tre incorrect'),
                'please be more precise' => array('fr' => 'soyez plus precis'),
                'Please enter your login and password' =>
                array('fr' =>
                      'Veuillez entrer votre nom d \'utilisateur et votre '
                      . 'mot de passe'),
                'Browse the directory wihout logging' =>
                array('fr' => 'Parcourir l\'annuaire sans s\'authentifier'),
                'Login' => array('fr' => 'Login'),
                'Enter' => array('fr' => 'Entrer'),
                'Or' => array('fr' => 'Ou'),
                'Username' => array('fr' => 'Utilisateur'),
                'Password' => array('fr' => 'Mot de passe'),
                'Please authenticate' => array('fr' =>
                                               'Veuillez vous identifier'),
                'Name' => array('fr' => 'Nom'),
                'Phone' => array('fr' => 'T�l�phone'),
                'Email' => array('fr' => 'Adresse email'),
                'Company' => array('fr' => 'Soci�t�'),
                'City' => array('fr' => 'Ville'),
                'Country' => array('fr' => 'Pays'),
                'Voicemail' => array('fr' => 'Messagerie Vocale'),
                'Directory' => array('fr' => 'Annuaire'),
                'open' => array('fr' => 'ouvrir'),
                'Cancel all redirections' => array('fr' =>
                                                   'Annuler tous les renvois'),
                'Redirect to' => array('fr' => 'Renvoi vers'),
                'Redirect' => array('fr' => 'Renvoyer'),
                'No matches' => array('fr' => 'Aucun r�sultat'),
                /* Internal errors */
                'can not call myself' => array('fr' => 'impossible de '
                                               .'s\'appeller soi-m�me'),
                'malformed reply' => array('fr' => 'r�ponse malform�e'),
                'bad reply' => array('fr' => 'r�ponse incorrecte'),
                'illegal caller number' => array('fr' => 'num�ro d\'appellant '
                                                 .'incorrect'),
                'bad number format' => array('fr' => 'mauvais format de '
                                             .'num�ro'),
                'lock is busy' => array('fr' => 'verrou d�ja pris'),
                'bad caller id' => array('fr' => 'identifiant d\'appellant '
                                         .'incorrect'),
                'a call is already in progress' => array('fr' => 'un appel '
                                                         .'est d�ja en cours'),
                'line not active' => array('fr' => 'ligne non d�croch�e'),
                'too many results' => array('fr' => 'trop de r�sultats'),
                'Edit' => array('fr' => 'Editer'),
                'Duplicate' => array('fr' => 'Dupliquer'),
                'Modify' => array('fr' => 'Modifier'),
                'Unable to get ldap edit access' =>
                array('fr' => 'impossible d\'acc�der en �criture au ldap'),
                'Malformed parameters' => array('fr' =>
                                                'param�tres malform�s'),
                'missing cn or dn field' => array('fr' =>
                                                  'champs cn or dn manquants'),
                'Height' => array('fr' => 'Hauteur'),
                'Rotate' => array('fr' => 'Rotation'),
                'Get the VCard' => array('fr' => 'T�l�charger la VCard'),
                'Send an email' => array('fr' => 'Envoyer un email'),
                'Browse this site' => array('fr' => 'Ouvrir ce site'),
                'Room location' => array('fr' => 'Emplacement de la salle'),
                'Room' => array('fr' => 'Salle'),
                
                /* Character mappings */
                '__conf' => array('fr'
                                  => array('charset' => 'iso-8859-15',
                                           'charmap'
                                           => get_ISO_8859_1_UnaccentMap()
                                           )
                                  )
                );

/* Dispatch current language */
$currLang_ = 'en';
if (isset($_SERVER["QUERY_STRING"]) && strpos($_SERVER["QUERY_STRING"],
                                              "_lang_=") !== false) {
  $pos = strpos($_SERVER["QUERY_STRING"], "_lang_=");
  $lang = substr($_SERVER["QUERY_STRING"], $pos, 2);
  $currLang_ = $lang;
  if (isset($available[$lang]['charset'])) {
    $currLangCharset_ = $available[$lang]['charset'];
  }
  if (isset($available[$lang]['charmap'])) {
    $currLangCharmap_ = $available[$lang]['charmap'];
  }
} else if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
  $available = $langs_['__conf'];
  $arr = explode(',', $_SERVER['HTTP_ACCEPT_LANGUAGE']);
  foreach($arr as $v) {
    $av = explode(';', $v);
    $lang = strtolower(trim($av[0]));
    if (strlen($lang) == 2 && isset($available[$lang])) {
      $currLang_ = $lang;
      if (isset($available[$lang]['charset'])) {
        $currLangCharset_ = $available[$lang]['charset'];
      }
      if (isset($available[$lang]['charmap'])) {
        $currLangCharmap_ = $available[$lang]['charmap'];
      }
      break;
    }
  }
}

/* Translate */
function _T($str) {
  global $langs_, $currLang_, $currLangCharset_;
  if (is_array($str)) {
    $a = array();
    foreach($str as $v) {
      $a[] = _T($v);
    }
    return $a;
  } else if (isset($langs_[$str])) {
    if (isset($langs_[$str][$currLang_])) {
      if (isset($currLangCharset_)) {
        return iconv($currLangCharset_, 'utf-8', $langs_[$str][$currLang_]);
      } else {
        return $langs_[$str][$currLang_];
      }
    } else {
      return $str;
    }
  } else {
    return $str;
  }
}

function _CLEAN($str) {
  global $currLangCharmap_;
  if (isset($currLangCharmap_)) {
    foreach($currLangCharmap_ as $e) {
      $str = str_replace($e[0], $e[1], $str);
    }
  }
  return $str;
}

function _TERR($str) {
  return implode(': ', _T(explode(': ', $str)));
}

function _ADDLANG($a) {
  global $langs_;
  foreach($a as $k => $v) {
    $langs_[$k] = $v;
  }
}

function _SETLANG($lang) {
  global $currLang_;
  $currLang_ = $lang;
}

?>
