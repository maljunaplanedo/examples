<?php

require_once dirname(__FILE__)."/src/include.php";

function return_answer() {
	global $mysqli, $answer;
	if (isset($mysqli))
		$mysqli->close();
	echo json_encode($answer);
	exit();
}

function update_time() {

	global $mysqli, $task, $student_task;
	return external_update_time($mysqli, $task, $student_task);
}

page_init();
global $me;

$answer = [];

if ($me === -1) {
	$answer["command"] = "to_index";
	return_answer();
}

$mysqli = mysqli_make();

$command = $_REQUEST["command"];
$student_task = (int)$_REQUEST["student_task"];

$student_task = $mysqli->query("SELECT * FROM `students_tasks` WHERE `id`=$student_task");
if ($student_task->num_rows !== 1) {
	$answer["command"] = "to_index";
	return_answer();
}
$student_task = $student_task->fetch_assoc();

$task = $mysqli->query("SELECT * FROM `tasks` WHERE `id`=".$student_task["task_id"]);
if ($task->num_rows !== 1) {
	$answer["command"] = "to_index";
	return_answer();
}
$task = $task->fetch_assoc();



if ($me["type"] == 0) {
	$answer["command"] = "to_index";
	return_answer();
}


if ($me["id"] != $student_task["student_id"]) {
	$answer["command"] = "to_index";
	return_answer();
}



$possible_running_task = $mysqli->query("SELECT `id` FROM `students_tasks`".
	" WHERE `student_id`=$student_id AND `status`=1");
if ($possible_running_task->num_rows === 1 && $possible_running_task->fetch_assoc()["id"] !=
	$student_task["id"]) {

	$answer["command"] = "to_index";
	return_answer();
}




$time_left = update_time();
if ($time_left < 0) {
	$answer["command"] = "to_index";
	return_answer();
}


if ($command === "init") {

	$answer["command"] = "ok";
	$answer["task_type"] = (int)$task["type"];
	$answer["task_name"] = htmlspecialchars($task["name"]);
	$answer["given"] = htmlspecialchars($task["given"]);
	if ($student_task["status"] == 0) {
		$begin_time = time();
		$mysqli->query("UPDATE `students_tasks` SET `begin_time`=$begin_time, ".
			"`status`=1 WHERE `id`=".$student_task["id"]);
		$answer["time_left"] = (int)$task["duration"];

		if ($task["type"] == 1) {
			$gap_nom = substr_count($answer["given"], "##");
			$emp = [];
			for ($i = 0; $i < $gap_nom; $i++)
				$emp[] = "";
			$emp = json_encode($emp);

			$student_task["answers"] = $emp;
			$mysqli->query("UPDATE `students_tasks` SET `answers`='$emp' WHERE".
				" `id`=".$student_task["id"]);
		}

	}
	else
		$answer["time_left"] = $time_left;
	$answer["result"] = (int)$student_task["result"];
	$answer["answers"] = $student_task["answers"];

	return_answer();
}

if ($student_task["status"] == 0) {
	$answer["command"] = "to_index";
	return_answer();
}


if ($command === "input") {

	if ($task["type"] == 1) {

		if (!isset($_REQUEST["gap_nom"]) || !isset($_REQUEST["letter"])) {
			$answer["command"] = "to_index";
			return_answer();
		}

		$gap_nom = (int)$_REQUEST["gap_nom"];
		$letter = $_REQUEST["letter"];
		$true_answers = json_decode($task["answer"]);
		$my_answers = json_decode($student_task["answers"]);

		$student_task["result"] = (int)$student_task["result"];

		if ($gap_nom >= count($true_answers) ||
			strlen($true_answers[$gap_nom]) === strlen($my_answers[$gap_nom])) {

			$answer["command"] = "to_index";
			return_answer();
		}

		if ($true_answers[$gap_nom][strlen($my_answers[$gap_nom])] === $letter) {
			$my_answers[$gap_nom] .= $letter;
			$student_task["answers"] = $my_answers;

			if ($my_answers[$gap_nom] == $true_answers[$gap_nom]) {
				$answer["full_word"] = "true";
				$my_answers[$gap_nom] .= '##';
				$student_task["result"] += 100;
			}

			$mysqli->query("UPDATE `students_tasks` SET `answers`='".json_encode($my_answers)."', ".
				"`result`=".$student_task["result"]." WHERE `id`=".$student_task["id"]);

			$answer["command"] = "good";
		}
		else {
			$student_task["result"] -= 30;
			$mysqli->query("UPDATE `students_tasks` SET `result`=".$student_task["result"].
				" WHERE `id`=".$student_task["id"]);

			$answer["command"] = "bad";
		}

	}
	$answer["result"] = (int)$student_task["result"];
	$answer["time_left"] = $time_left;
	return_answer();

}

if ($command === "end") {

	$mysqli->query("UPDATE `students_tasks` SET `status`=2 WHERE `id`=".$student_task["id"]);
	$answer["command"] = "to_index";
	return_answer();
}