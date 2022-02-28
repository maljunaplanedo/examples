<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

html_begin("Добавить задание");

page_head();

?>

<a href="tasks.php">Назад</a>

<form name="new_task" action="add_task.php" method="post">
	
	<input type="text" placeholder="Тип задания" name="type">
	<input type="text" placeholder="Название задания" name="name">
	<textarea name="given" placeholder="Условие"></textarea>
	<textarea name="answer" placeholder="Ответы"></textarea>
	<input type="text" name="duration" placeholder="Время выполнения">
	<input type="submit" name="submit" value="Добавить">

</form>

<?php
if (isset($_REQUEST["error"])) {
	$error = $_REQUEST["error"];
	?>
	<p>
		<?php

		if ($error === "wrongtype")
			echo "Такого типа задания не существует.";
		else if ($error === "wrongname")
			echo "Так назвать задание нельзя.";
		else if ($error === "wronggiven")
			echo "У задания не может быть такое условие.";
		else if ($error === "wronganswer")
			echo "У задания не может быть такой ответ.";
		else if ($error === "wrongduration")
			echo "У задания не может быть такая продолжительность выполнения";
		else
			echo "Ошибка";

		?>
	</p>
	<?php
}

html_end();