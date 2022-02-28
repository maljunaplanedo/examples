<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

html_begin("Добавить класс");

page_head();

?>

<a href="classes.php">Назад</a>

<form name="new_class" action="add_class.php" method="post">
	
	<input type="text" placeholder="Название класса" name="name">
	<input type="submit" name="submit" value="Добавить">

</form>

<?php
if (isset($_REQUEST["error"])) {
	$error = $_REQUEST["error"];
	?>
	<p>
		<?php

		if ($error === "wrongname")
			echo "Так назвать класс нельзя.";
		else
			echo "Ошибка";
		?>
	</p>
	<?php
}

html_end();