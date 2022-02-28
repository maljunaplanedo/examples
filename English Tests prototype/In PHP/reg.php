<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

function _die($message) {
	global $mysqli;
	if (isset($mysqli))
		$mysqli->close();
	header("Location: reg_page.php?error=$message");
	exit();
}

if ($me !== -1 || !isset($_REQUEST["invite_code"]) ||
		!isset($_REQUEST["login"]) || !isset($_REQUEST["password"])) {

	header("Location: index.php");
	exit();
}

$invite_code = $_REQUEST["invite_code"];
$login = $_REQUEST["login"];
$password = $_REQUEST["password"];

if (strlen($invite_code) != INVITE_CODE_LEN)
	_die("notexist");
if (strlen($login) === 0)
	_die("loginnotvalid");
if (strlen($password) === 0)
	_die("passwordnotvalid");


$mysqli = mysqli_make();
$invite_code = $mysqli->escape_string($invite_code);
$login = $mysqli->escape_string($login);
$password = $mysqli->escape_string($password);

$answer = $mysqli->query("SELECT * FROM `unconf_users` WHERE `code`='$invite_code'");
if ($answer->num_rows !== 1)
	_die("notexist");	

$answer = $answer->fetch_assoc();
$name = $answer["name"];
$surname = $answer["surname"];
$type = $answer["type"];
$class_id = $answer["class_id"];

$namesake = $mysqli->query("SELECT * FROM `users` WHERE `login`='$login'");
if ($namesake->num_rows !== 0)
	_die("loginexists");

$password = password_hash($password, PASSWORD_DEFAULT);

$mysqli->query("DELETE FROM `unconf_users` WHERE `code`='$invite_code'");
$mysqli->query("INSERT INTO `users` (`login`, `password`, `name`, `surname`, `type`, `class_id`)".
	" VALUES ('$login', '$password', '$name', '$surname', $type, $class_id)");

$mysqli->close();

header("Location: auth_page.php");