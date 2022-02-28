<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

html_begin("Классы");

page_head();

?>

<a href="index.php">Назад</a>

<ul>
	
	<?php

	$mysqli = mysqli_make();
	$classes = $mysqli->query("SELECT `id`, `name` FROM `classes`");

	while ($_class = $classes->fetch_assoc()) {
		$_class["name"] = htmlspecialchars($_class["name"]);
		?>

		<li>
			<?=$_class["name"];?>
			<a href=<?="class_students.php?id=".$_class["id"];?>>Ученики</a>
			<a href=<?="class_tasks.php?id=".$_class["id"];?>>Задания</a>
			<a href=<?="remove_class.php?id=".$_class["id"];?>>Удалить</a>	
		</li>

		<?php
	}

	$mysqli->close();

	?>

</ul>

<a href="add_class_page.php">Добавить класс</a>

<?php
html_end();