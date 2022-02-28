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

$mysqli->close();
html_begin("Добавить ученика в ".$class_name);

page_head();

?>

<a href=<?="class_students.php?id=$class_id";?>>Назад</a>

<form name="new_student" action="add_student.php" method="post">
	
	<input type="text" placeholder="Имя" name="name">
	<input type="text" placeholder="Фамилия" name="surname">
	<input type="hidden" value="<?=$class_id;?>" name="class_id">
	<input type="submit" name="submit" value="Добавить">

</form>

<?php
if (isset($_REQUEST["error"])) {
	$error = $_REQUEST["error"];
	?>
	<p>
		<?php

		if ($error === "wrongname")
			echo "У ученика не может быть такое имя.";
		else if ($error === "wrongsurname")
			echo "У ученика не может быть такой фамилии.";
		else
			echo "Ошибка";
		?>
	</p>
	<?php
}

html_end();