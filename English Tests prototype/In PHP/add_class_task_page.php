<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if (!isset($_REQUEST["class_id"])) {
	header("Location: index.php");
	exit();
}

$class_id = (int)$_REQUEST["class_id"];

only_teachers();

$mysqli = mysqli_make();
$class_name = $mysqli->query("SELECT `name` from `classes` WHERE `id`=$class_id");

if ($class_name->num_rows !== 1) {
	$mysqli->close();
	header("Location: index.php");
	exit();
}
$class_name = htmlspecialchars($class_name->fetch_assoc()["name"]);

html_begin("Задать задание ".$class_name);

page_head();

$tasks = $mysqli->query("SELECT `id`, `name` FROM `tasks`");

?>

<a href=<?="class_tasks.php?id=$class_id";?>>Назад</a>
<form name="new_class_task" action="add_class_task.php" method="post">
	<select name="task_id">
		<?php

		while ($task = $tasks->fetch_assoc()) {
			$task["name"] = htmlspecialchars($task["name"]);
			?>
			
			<option value=<?=$task["id"];?>><?=$task["name"];?></option>

			<?php
		}

		?>
	</select>
	<input type="hidden" name="class_id" value=<?=$class_id;?>>
	<input type="submit" value="Задать">
</form>




<?php

$mysqli->close();
html_end();