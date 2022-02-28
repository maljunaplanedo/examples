<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

$id = (int)$_REQUEST["id"];

if ($me === -1 || ($me["type"] == 1 && $me["id"] != $id)) {
	header("Location: index.php");
	exit();
}

$mysqli = mysqli_make();

$student = $mysqli->query("SELECT `name`, `surname`, `class_id` FROM `users` WHERE `id`=$id");

if ($student->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}

$student = $student->fetch_assoc();

if ($me["type"] == 1) {
	$possible_running_task = $mysqli->query("SELECT `id` FROM `students_tasks`".
		" WHERE `student_id`=$id AND `status`=1");
	if ($possible_running_task->num_rows === 1) {
		$mysqli->close();
		header("Location: task_runner_page.php?id=".$possible_running_task->fetch_assoc()["id"]);
		exit();
	}
}

$name = htmlspecialchars($student["name"]);
$surname = htmlspecialchars($student["surname"]);
$class_id = (int)$student["class_id"];

html_begin($name." ".$surname);
page_head();
?>

<h5><?=$name." ".$surname;?></h5>

<?php
if ($me["type"] == 0) {
	?>
		<a href=<?="class_students.php?id=$class_id";?>>Ко всем ученикам</a><br>
	<?php
}
?>

Заданные задания:
<ul>
<?php

$tasks = $mysqli->query("SELECT `id`, `class_task_id`, `task_id`, `status`, `result`, `begin_time`".
	" FROM `students_tasks` WHERE `student_id`=$id ORDER BY `add_time` DESC");

while ($task = $tasks->fetch_assoc()) {

	$task_template = $mysqli->query("SELECT `name`, `type`, `duration` FROM `tasks` WHERE `id`="
		.$task["task_id"])->fetch_assoc();

	if ($task["status"] == 1)
		external_update_time($mysqli, $task_template, $task);

	$task_template["name"] = htmlspecialchars($task_template["name"]);

	?> <li> <?php

	if ($me["type"] == 0) {
		?>
		<a href=<?="task.php?id=".$task["task_id"];?>>
		<?php
	}
	echo $task_template["type"]." ".$task_template["name"];
	if ($me["type"] == 0)
		echo "</a>";

	echo "<br>Статус: ".$task["status"]."<br>";
	echo "Результат: ".$task["result"]."<br>";
	?>
	<a href=<?="student_task.php?id=".$task["id"];?>>Перейти к решению</a>
	<a href=<?="class_task.php?class_id=$class_id&id=".$task["class_task_id"];?>>Результаты класса</a>

	</li>
	<?php 
}

?>
</ul>


<?php
html_end();
$mysqli->close();