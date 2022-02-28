<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

if (!isset($_REQUEST["type"]) || !isset($_REQUEST["name"]) || !isset($_REQUEST["given"]) ||
	!isset($_REQUEST["answer"]) || !isset($_REQUEST["duration"])) {
	header("Location: index.php");
	exit();
}

function _die($message) {
	header("Location: add_task_page.php?error=$message");
	exit();
}

$type = (int)$_REQUEST["type"];
$name = $_REQUEST["name"];
$given = $_REQUEST["given"];
$answer = $_REQUEST["answer"];
$duration = (int)$_REQUEST["duration"];

if ($type !== 1)
	_die("wrongtype");
else if (strlen($name) === 0 || strlen($name) > 255)
	_die("wrongname");
else if (strlen($given) === 0 || strlen($given) > 60000)
	_die("wronggiven");
else if (strlen($answer) === 0 || strlen($answer) > 60000)
	_die("wronganswer");
else if ($duration > 604800)
	_die("wrongduration");

$mysqli = mysqli_make();
$name = $mysqli->escape_string($name);
$given = $mysqli->escape_string($given);
$answer = $mysqli->escape_string($answer);
$mysqli->query("INSERT INTO `tasks` (`type`, `name`, `given`, `answer`, `duration`)".
	" VALUES ($type, '$name', '$given', '$answer', $duration)");
$mysqli->close();

header("Location: tasks.php");