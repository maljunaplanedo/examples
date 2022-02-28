<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if ($me !== -1) {
	header("Location: index.php");
	exit();
}

html_begin("Регистрация");

?>

<form name="reg" action="reg.php" method="get">
	<input type="text" name="invite_code" placeholder="Пригласительный код">
	<input type="text" name="login" placeholder="Логин">
	<input type="password" name="password" placeholder="Пароль">
	<input type="submit" name="submit" value="Регистрация">
</form>

<?php
if (isset($_REQUEST["error"])) {
?>

<p>
	<?php

	$error_message = $_REQUEST["error"];

	if ($error_message === "notexist")
		echo "Такого пригласительного кода не существует";
	else if ($error_message === "loginexists")
		echo "Такой логин уже существует";
	else if ($error_message === "loginnotvalid")
		echo "Такой логин не может быть использован.";
	else if ($error_message === "passwordnotvalid")
		echo "Такой пароль не может быть использован.";
	else
		echo "Ошибка";
	?>
</p>

<?php } ?>

<p>Уже зарегестрированы? <a href="auth_page.php">Войти</a></p>

<?php

html_end();
?>