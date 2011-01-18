<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html lang="<?php echo $currLang_; ?>">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>TouchMe</title>

</head>

<body>

<h1>
<font color="red"><?php
  if (isset($message)) {
    echo $message;
  } else {
    echo 'Forbidden';
  }
?><br />
</h1>

</body>
</html>
