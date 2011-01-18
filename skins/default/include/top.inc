<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="<?php echo $currLang_; ?>">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>TouchMe</title>

<script language="javascript">
<!--

var prevValue = '';
var prevTimeout;
function pressMe() {
  if (form.callee.value != prevValue) {
    prevValue = form.callee.value;
    parent.parent.frames[1].document.form.q.value = form.callee.value;
    parent.parent.frames[1].document.form.submit();
    checkMe(500);
  }
}

function checkMe(timeout) {
  if (prevTimeout) {
    clearTimeout(prevTimeout);
  }
  prevTimeout = setTimeout("pressMe()", timeout);
}

function focusMe() {
  document.form.callee.focus();
<?php
    if (isset($redirect)) {
      echo "document.form.info.value = '';\n";
      echo "document.form.submit();\n";
    }
?>
}

// -->
</script>

</head>

<body bgcolor="white" onLoad="focusMe()">

<!-- main form -->
<table border="0" width="100%" height="100%">
<tr><td>
<!-- main form -->

  <table border="0">
  <tr><td>
  <img src="skins/default/images/logo.gif" alt="Logo" border="0" />
  </td><td>
  &nbsp;
  </td><td>

<h2>
<font color="blue"><i>Astrisk</i></font>Touch<font color="teal">Me@<?php if ($caller != '*') { echo $caller; } else { echo 'guest'; } ?></font><br />
</h2>

</td></tr>
</table>

<!-- dial form -->
<table border="0">
<tr>

<td>
<form name="form" onkeyup="checkMe(50)" 
<?php if ($caller != '*') { ?>
method="post" onsubmit="pressMe()"
<?php } else { ?>
onsubmit="return false"
<?php } ?>
>
<input type="hidden" name="info" value="1" />
<input type="hidden" name="found" value="" />
<?php echo _T('Search'); ?>:
<input type="text" name="callee" value="<?php
  echo $callee;
?>" <?php
  if (isset($info)) {
    echo ' readonly style="background-color:#CCCCCC"';
  }
?> />
<?php if ($caller != '*') { ?>
<input type="submit"
   title="<?php echo _T('Call'); ?>"
   value="<?php echo _T('Call'); ?>" />
<?php } ?>
</form>
</td>

<?php if ($caller != '*') { ?>
<td>
<form name="hangup" method="post">
<input type="hidden" name="hangup" value="1" />
<input type="submit"
   title="<?php echo _T('Hangup'); ?>"
   value="<?php echo _T('Hangup'); ?>" />
</form>
</td>
<?php } ?>

</tr>
</table>

<?php
if (isset($message)) {
  if (isset($noError)) {
    echo "<font color='blue'>" . $message . "</font>";
  } else {
    echo "<font color='red'>" . $message . "</font>";
  }
}
?>

<!-- /main form -->
</td></tr>
<tr>
<td align="right"><a href="top.php?logout">logout</a></td></tr><tr>
<td align="right"><small>v<?php echo $touchmeVersion; ?></small></td>
</tr>
</table>
<!-- /main form -->

</body>
</html>
