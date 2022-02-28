<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

if (!isset($_REQUEST["name"]) || !isset($_REQUEST["surname"]) || !isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

$name = $_REQUEST["name"];
$surname = $_REQUEST["surname"];
$class_id = (int)$_REQUEST["class_id"];

function _die($message) {
	global $class_id;
	global $mysqli;
	$mysqli->close();
	header("Location: add_student_page.php?class_id=$class_id&error=$message");
	exit();
}

$mysqli = mysqli_make();
$class_exists = $mysqli->query("SELECT * FROM `classes` WHERE `id`=$class_id");
if ($class_exists->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$class_name = htmlspecialchars($class_exists->fetch_assoc()["name"]);

if (strlen($name) === 0 || strlen($name) > 255)
	_die("wrongname");
else if (strlen($surname) === 0 || strlen($surname) > 255)
	_die("wrongsurname");

$name = $mysqli->escape_string($name);
$surname = $mysqli->escape_string($surname);

$invite_code = bin2hex(random_bytes(INVITE_CODE_LEN / 2));
$mysqli->query("INSERT INTO `unconf_users` (`code`, `name`, `surname`, `type`, `class_id`)".
	" VALUES ('$invite_code', '$name', '$surname', 1, $class_id)");

$mysqli->close();
?>

Ученик <?=$name." ".$surname;?> добавлен в класс <?=$class_name?>. Для регистрации дайте ему этот пригласительный код: <?=$invite_code;?>

<a href=<?="class_students.php?id=$class_id";?>>К списку учеников <?=$class_name;?></a>