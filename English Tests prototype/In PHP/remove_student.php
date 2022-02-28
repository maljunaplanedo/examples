<?php

require_once dirname(__FILE__)."/src/include.php";

if (!isset($_REQUEST["id"]) || !isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

page_init();
global $me;

only_teachers();

$student_id = (int)$_REQUEST["id"];
$class_id = (int)$_REQUEST["class_id"];
$mysqli = mysqli_make();

$mysqli->query("DELETE FROM `users` WHERE `id`=$student_id");
$mysqli->query("DELETE FROM `students_tasks` WHERE `student_id`=$student_id");

$mysqli->close();

header("Location: class_students.php?id=$class_id");