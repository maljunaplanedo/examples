<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if (!isset($_REQUEST["id"]) || !isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

only_teachers();

$id = (int)$_REQUEST["id"];
$class_id = (int)$_REQUEST["class_id"];

$mysqli = mysqli_make();

$class_task = $mysqli->query("SELECT * FROM `class_tasks` WHERE `id`=$id");
$class_name = $mysqli->query("SELECT `name` FROM `classes` WHERE `id`=$class_id");

if ($class_task->num_rows !== 1 || $class_name->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$class_task = $class_task->fetch_assoc();
$class_name = htmlspecialchars($class_name->fetch_assoc()["name"]);

$task_template_name = $mysqli->query("SELECT `name`, `duration`".
	" FROM `tasks` WHERE `id`=".$class_task["task_id"]);
if ($task_template_name->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}
$task_template = $task_template_name->fetch_assoc();
$task_template_name = htmlspecialchars($task_template_name->fetch_assoc()["name"]);


html_begin("Результаты $class_name по заданию $task_template_name");

?>
<a href=<?="class_tasks.php?id=$class_id";?>>Назад</a><br>
<?="Результаты $class_name по заданию <a href='task.php?id=".
	$class_task["task_id"]."'>$task_template_name</a>:";?>
<ul>
<?php

$results = $mysqli->query("SELECT `student_id`, `status`, `result`, `begin_time`".
	" FROM `students_tasks` WHERE `class_task_id`=$id ORDER BY `result` DESC");

while ($result = $results->fetch_assoc()) {

	$student = $mysqli->query("SELECT `name`, `surname` FROM".
		" `users` WHERE `id`=".$result["student_id"])->fetch_assoc();
	$student["name"] = htmlspecialchars($student["name"]);
	$student["surname"] = htmlspecialchars($student["surname"]);

	if ($result["status"] == 1)
		external_update_time($mysqli, $task_template, $result);

	?>
	<li>
		<a href=<?="student.php?id=".$result["student_id"];?>><?=$student["name"]." ".$student["surname"];?></a>
		Результат: <?=$result["result"];?>
		Состояние: <?=$result["status"];?>
	</li>
	<?php

}


?>
</ul>
<?php

html_end();
$mysqli->close();

