<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if ($me !== -1) {
	header("Location: index.php");
	exit();
}

html_begin("Авторизация");

?>

<form name="auth" method="post" action="auth.php">
	<input type="text" name="login" placeholder="Логин">
	<input type="password" name="password" placeholder="Пароль">
	<input type="submit" name="submit" value="Войти">
</form>

<?php

if (isset($_REQUEST["error"])) {

?>

<p>
	<?php

		$error_message = $_REQUEST["error"];

		if ($error_message === "wrong")
			echo "Неправильный логин или пароль.";
		else
			echo "Ошибка.";
	?>
</p>

<?php
}

?>

<p>
Еще нет аккаунта? <a href="reg_page.php">Зарегистрируйтесь</a> по пригласительному коду, выданному учителем.
</p>

<?php

html_end();