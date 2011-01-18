<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="<?php echo $currLang_; ?>">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">

<script language="javascript">
<!--

function submitMe(value) {
  parent.parent.frames[0].document.form.found.value = '';
  parent.parent.frames[0].document.form.callee.value = value;
  parent.parent.frames[0].document.form.submit();
  return false;
}

function focusMe() {
  parent.parent.frames[0].document.form.found.value = '<?php
  if (isset($scriptFoundValue)) {
    echo $scriptFoundValue;
  }
?>';
}

<?php
if (isset($busyWait)) {
  echo 'setTimeout("window.location = \'bottom.php?q='
    . $busyWait . '\';", 30000);' . "\n";
}
?>

// -->
</script>

</head>

<body background="<?php echo $xtmGlobal->image('backtable.jpg'); ?>" onLoad="focusMe()">

<?php
if (isset($defaultTable)) {
  echo $defaultTable;
}
if (isset($table)) {
  echo $table;
}
if (isset($message)) {
  echo '<h1>'.$message.'</h1>';
}
?>

<!-- hidden form -->
<form action="bottom.php" name="form" method="post">
<input type="hidden" name="q">
</form>

</body>
</html>
