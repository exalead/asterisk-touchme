<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="<?php echo $currLang_; ?>">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>TouchMe</title>

<script language="javascript">
<!--
function focusMe() {
  document.form.<?php
  echo isset($callee) && strlen($callee) ? 'password' : 'login';
  ?>.focus();
}
// -->
</script>

</head>

<body onLoad="focusMe()">

<h2>
<font color="blue">Asterisk</font>TouchMe<br />
</h2>

<h4>
<nl>

<li>
<?php echo _T('Please enter your login and password').':'; ?>
<br />
<table border="0"><tr><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>
<form name="form" method="post" action="index.php">
<?php echo $additional_login_vars; ?>
<?php echo _T('Username'); ?>:
<input type="text" size="8" name="login" value="<?php
 if (isset($_REQUEST['login'])) {
   echo $_REQUEST['login'];
 } else if (isset($_COOKIE['touchMeLogin'])) {
   echo $_COOKIE['touchMeLogin'];
 }
?>" />
<br />
<?php echo _T('Password'); ?>:
<input type="password" size="8" name="password" />
<br />
<input type="submit" value="<?php echo _T('Login'); ?>!" />
</form>
</td></tr></table>
</li>

<?php
/* DISABLED */
if (1) {
?>

<?php echo '-'._T('Or').'-'; ?><br /><br />

<li>
<?php echo _T('Browse the directory wihout logging').':'; ?>
<br />
<table border="0"><tr><td>&nbsp;&nbsp;&nbsp;&nbsp;</td><td>
<form name="form" method="post" action="index.php">
<?php echo $additional_login_vars; ?>
<input type="hidden" size="8" name="login" value="*" />
<input type="hidden" size="8" name="password" value="" />
<input type="submit" value="<?php echo _T('Enter'); ?>!" />
</form>
</td></tr></table>
</li>

<?php
/* DISABLED */
}
?>

</nl>
</h4>

<?php
if (isset($_REQUEST['msg'])) {
  echo '<h2><font color="red">'.$_REQUEST['msg'].'</font></h2>';
}
?>

</body>
</html>
