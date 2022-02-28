<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

html_begin("Задания");

page_head();

?>

<a href="index.php">Назад</a>

<ul>
	
	<?php

	$mysqli = mysqli_make();
	$tasks = $mysqli->query("SELECT `id`, `type`, `name` FROM `tasks`");

	while ($task = $tasks->fetch_assoc()) {
		$task["name"] = htmlspecialchars($task["name"]);
		?>

		<li>
			<a href=<?="task.php?id=".$task["id"];?>><?=$task["type"];?> <?=$task["name"];?></a>
			<a href=<?="remove_task.php?id=".$task["id"];?>>Удалить</a>	
		</li>

		<?php
	}

	$mysqli->close();

	?>

</ul>

<a href="add_task_page.php">Добавить задание</a>

<?php
html_end();