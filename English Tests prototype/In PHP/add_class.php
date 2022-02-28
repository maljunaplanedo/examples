<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

if (!isset($_REQUEST["name"])) {
	header("Location: index.php");
	exit();
}

$name = $_REQUEST["name"];

if (strlen($name) === 0 || strlen($name) > 255) {
	header("Location: add_class_page.php?error=wrongname");
	exit();
}

$mysqli = mysqli_make();

$name = $mysqli->escape_string($name);

$mysqli->query("INSERT INTO `classes` (`name`)".
	" VALUES ('$name')");
$mysqli->close();

header("Location: classes.php");