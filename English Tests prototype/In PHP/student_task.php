<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

$id = (int)$_REQUEST["id"];

$mysqli = mysqli_make();


$student_task = $mysqli->query("SELECT * FROM `students_tasks` WHERE `id`=$id");

if ($student_task->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$student_task = $student_task->fetch_assoc();

$student_id = (int)$student_task["student_id"];

if ($me === -1 || ($me["type"] == 1 && $student_id != $me["id"])) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

if ($me["type"] == 1) {
	$possible_running_task = $mysqli->query("SELECT `id` FROM `students_tasks`".
		" WHERE `student_id`=$student_id AND `status`=1");
	if ($possible_running_task->num_rows === 1) {
		$mysqli->close();
		header("Location: task_runner_page.php?id=".$possible_running_task->fetch_assoc()["id"]);
		exit();
	}
}

$task = (int)$student_task["task_id"];
$task = $mysqli->query("SELECT * FROM `tasks` WHERE `id`=$task");

if ($task->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$task = $task->fetch_assoc();
$task["name"] = htmlspecialchars($task["name"]);

if ($student_task["status"] == 1)
	external_update_time($mysqli, $task, $student_task);

html_begin("Задание ".$task["name"]);
page_head();

if ($me["type"] == 0 || $student_task["status"] === 2) {
	$task["name"] = htmlspecialchars($task["name"]);
	$task["given"] = htmlspecialchars($task["given"]);
	$student_task["answers"] = htmlspecialchars($student_task["answers"]);
	$task["answers"] = htmlspecialchars($task["answers"]);

	?>
	<a href=<?="student.php?id=$student_id";?>>Назад</a><br>
	Задание: <?php if ($me["type"] == 0) echo "<a href='task.php?id=".$task["id"]."'>"; ?> 
	<?=$task["name"];?>
	<?php if ($me["type"] == 0) echo "</a>"; ?><br>
	Статус: <?=$student_task["status"];?><br>
	Результат: <?=$student_task["result"];?><br>
	Условия: <?=$task["given"];?><br>
	Ответы: <?=$student_task["answers"];?><br>
	Правильные ответы: <?=$task["answer"];?><br>
	Время начала: <?=$student_task["begin_time"];?><br>
	Продолжительность выполнения: <?=$task["duration"];?><br>
	<?php
}
else {
	?>
	<a href=<?="task_runner_page.php?id=$id";?>>Перейти к выполнению</a>
	<?php
}

html_end();
$mysqli->close();