<?php

require_once dirname(__FILE__)."/src/include.php";

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

page_init();
global $me;

only_teachers();

$class_id = (int)$_REQUEST["id"];
$mysqli = mysqli_make();

$mysqli->query("DELETE FROM `classes` WHERE `id`=$class_id");
$mysqli->query("DELETE FROM `users` WHERE `class_id`=$class_id");
$mysqli->query("DELETE FROM `class_tasks` WHERE `class_id`=$class_id");
$mysqli->query("DELETE FROM `students_tasks` WHERE `class_id`=$class_id");

$mysqli->close();

header("Location: classes.php");