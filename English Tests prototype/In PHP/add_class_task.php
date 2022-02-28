<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

if (!isset($_REQUEST["task_id"]) || !isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

$task_id = (int)$_REQUEST["task_id"];
$class_id = (int)$_REQUEST["class_id"];

$mysqli = mysqli_make();
$class_exists = $mysqli->query("SELECT `id` FROM `classes` WHERE `id`=$class_id");
$task_exists = $mysqli->query("SELECT `id` FROM `tasks` WHERE `id`=$task_id");

if ($class_exists->num_rows !== 1 || $task_exists->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$add_time = time();

$mysqli->query("INSERT INTO `class_tasks` (`class_id`, `task_id`, `add_time`)".
	" VALUES ($class_id, $task_id, $add_time)");
$my_id = (int)$mysqli->query("SELECT `id` FROM `class_tasks` WHERE".
	" `class_id`=$class_id AND `task_id`=$task_id AND `add_time`=$add_time")->fetch_assoc()["id"];

$class_students = $mysqli->query("SELECT `id` FROM `users` WHERE `class_id`=$class_id");
while ($student = (int)$class_students->fetch_assoc()["id"]) {
	$mysqli->query("INSERT INTO `students_tasks`".
		" (`class_task_id`, `task_id`, `student_id`, `add_time`, `answers`)".
		" VALUES ($my_id, $task_id, $student, $add_time, '')");

}

$mysqli->close();
header("Location: class_tasks.php?id=$class_id");