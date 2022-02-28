<?php

define("DEBUG", true);

if (DEBUG) {
	define("SQL_ADDRESS", "localhost");
	define("SQL_LOGIN", "root");
	define("SQL_PASSWORD", "root");
	define("SQL_DATABASE_NAME", "ajax_try");

	define("THIS_URL", "http://localhost");
}
else {
	date_default_timezone_set("Europe/Minsk");

	define("SQL_ADDRESS", "sql110.epizy.com");
	define("SQL_LOGIN", "epiz_26071478");
	define("SQL_PASSWORD", "EUHgyBbBjpYWxvb");
	define("SQL_DATABASE_NAME", "epiz_26071478_saske_blog");

	define("THIS_URL", "http://heyguys228party1337.epizy.com");
}

define("INVITE_CODE_LEN", 10);
define("AUTH_HASH_LIFETIME", 2592000);


function who_am_i() {

	if (!isset($_COOKIE["auth_hash"]))
		return -1;

	$auth_hash = $_COOKIE["auth_hash"];
	$mysqli = mysqli_make();

	$auth = $mysqli->query("SELECT `user_id` FROM `auths` WHERE `auth_hash`='$auth_hash'");

	if ($auth->num_rows !== 1) {
		setcookie("auth_hash", "-", time() - 1);
		return -1;
	}

	$auth = (int)($auth->fetch_assoc()["user_id"]);
	$self = $mysqli->query("SELECT * FROM `users` WHERE `id`=$auth")->fetch_assoc();

	$mysqli->close();

	return $self;
}

function page_init() {

	global $me;
	$me = who_am_i();

}

function mysqli_make() {
	return new mysqli(SQL_ADDRESS, SQL_LOGIN, SQL_PASSWORD, SQL_DATABASE_NAME);
}

function html_begin($title, $enable_testing_system = -1) {

	?>

	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<title><?=$title;?></title>
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>
		<?php
		if ($enable_testing_system != -1) {
			?>
				<script type="text/javascript">window.STUDENT_TASK_ID = 
							<?=$enable_testing_system;?>;
				</script>
				<script type="text/javascript" src="scripts/task_runner.js"></script>
			<?php
		}
		?>
	</head>
	<body>
	
	<?php

}

function html_end() {

	?>
	</body>
	</html>
	<?php
}

function page_head() {
	?>
	<div class="header">
	<?php

	global $me;

	if ($me["type"] == 0) {
	?>
	<a href="index.php">
	<?php } ?>
		<h1>LOGO</h1>
	<?php
	if ($me["type"] == 0) { ?>
	</a>
	<?php } ?>
	<p>

	<?php
	if ($me === -1) {
		?>
		
		<a href="auth_page.php">Войти</a> <a href="reg_page.php">Зарегистрироваться</a>

		<?php
	}
	else {
		?>
		<?=htmlspecialchars($me["name"])." ".htmlspecialchars($me["surname"]);?><br>
		<a href="unauth.php">Выйти</a>

		<?php
	}
	?>

	</p></div>

	<?php

}


function only_teachers() {

	global $me;
	if ($me === -1) {
		header("Location: auth_page.php");
		exit();
	}
	else if ($me["type"] == 1) {
		header("Location: student.php?id=".$me["id"]);
		exit();
	}

}


function external_update_time(&$mysqli, &$task, &$student_task) {

	if ($student_task["status"] == 0)
		return (int)2e9;
	if ($student_task["status"] == 2)
		return (int)-2e9;

	$current_time = time();
	$end_time = (int)$student_task["begin_time"] + (int)$task["duration"];

	if ($current_time > $end_time) {
		$mysqli->query("UPDATE `students_tasks` SET `status`=2 WHERE `id`=".$student_task["id"]);
		$student_task["status"] = 2;
	}
	return $end_time - $current_time;
}