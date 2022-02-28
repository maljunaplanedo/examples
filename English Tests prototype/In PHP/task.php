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

$task = $mysqli->query("SELECT * FROM `tasks` WHERE `id`=$task_id");
if ($task->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}
$task = $task->fetch_assoc();

$mysqli->close();

$task["name"] = htmlspecialchars($task["name"]);
$task["given"] = htmlspecialchars($task["given"]);
$task["answer"] = htmlspecialchars($task["answer"]);

html_begin("Заданиe ".$task["name"]);

page_head();

?>
<a href="tasks.php">Все задания</a><br>

Название: <?=$task["name"];?><br>
Содержание: <?=$task["given"];?><br>
Ответы: <?=$task["answer"];?><br>
Время выполнения: <?=$task["duration"];?><br>
<a href="remove_task.php?id=<?=$task_id;?>">Удалить</a>


<?php

html_end();