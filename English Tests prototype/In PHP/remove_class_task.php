<?php

require_once dirname(__FILE__)."/src/include.php";

if (!isset($_REQUEST["id"]) || !isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

page_init();
global $me;

only_teachers();

$class_task_id = (int)$_REQUEST["id"];
$class_id = (int)$_REQUEST["class_id"];
$mysqli = mysqli_make();

$mysqli->query("DELETE FROM `class_tasks` WHERE `id`=$class_task_id");
$mysqli->query("DELETE FROM `students_tasks` WHERE `class_task_id`=$class_task_id");

$mysqli->close();

header("Location: class_tasks.php?id=$class_id");