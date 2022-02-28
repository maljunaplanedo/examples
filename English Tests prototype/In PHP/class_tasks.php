<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

$id = (int)$_REQUEST["id"];
$mysqli = mysqli_make();
$class_name = htmlspecialchars($mysqli->query("SELECT `name` from `classes` WHERE `id`=$id")
	->fetch_assoc()["name"]);

html_begin("Задания $class_name");
page_head();

?>

<a href="classes.php">Назад</a>

<br>Задания <?=$class_name;?>: <br>

<?php

$class_tasks = $mysqli->query("SELECT `id`, `task_id` FROM `class_tasks` WHERE".
	" `class_id`=$id ORDER BY `add_time` DESC");

?>

<ul>
	
	<?php

	while ($class_task = $class_tasks->fetch_assoc()) {
		$task = $mysqli->query("SELECT `type`, `name` FROM `tasks` WHERE `id`=".$class_task["task_id"])
			->fetch_assoc();
		$task["name"] = htmlspecialchars($task["name"]);
		?>
		<li>

			<a href=<?="task.php?id=".$class_task["task_id"];?>><?=$task["type"]." ".$task["name"];?></a>
			<a href=<?="class_task.php?class_id=$id&id=".$class_task["id"];?>>Результаты учеников</a>
			<a href=<?="remove_class_task.php?class_id=$id&id=".$class_task["id"];?>>Удалить задание</a>
		</li>
		<?php
	}
	?>

</ul>

<a href=<?="add_class_task_page.php?class_id=$id";?>>Задать задание классу</a>

<?php

$mysqli->close();
html_end();