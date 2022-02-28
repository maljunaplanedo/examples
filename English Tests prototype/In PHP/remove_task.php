<?php

require_once dirname(__FILE__)."/src/include.php";

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

page_init();
global $me;

only_teachers();

$task_id = (int)$_REQUEST["id"];
$mysqli = mysqli_make();

$mysqli->query("DELETE FROM `tasks` WHERE `id`=$task_id");
$mysqli->query("DELETE FROM `students_tasks` WHERE `task_id`=$task_id");
$mysqli->query("DELETE FROM `class_tasks` WHERE `task_id`=$task_id");

$mysqli->close();

header("Location: tasks.php");