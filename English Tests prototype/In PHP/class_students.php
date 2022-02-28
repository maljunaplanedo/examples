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

html_begin("Ученики $class_name");
page_head();

?>

<a href="classes.php">Назад</a>

<br>Ученики <?=$class_name;?>: <br>

<?php

$students = $mysqli->query("SELECT `id`, `name`, `surname` FROM `users` WHERE `class_id`=$id");
?>

<ul>
	<?php
	
	while ($student = $students->fetch_assoc()) {
		$student["surname"] = htmlspecialchars($student["surname"]);
		$student["name"] = htmlspecialchars($student["name"]);
		?>

		<li>
			
			<a href=<?="student.php?id=".$student["id"];?>
				><?=$student["surname"]." ".$student["name"];?></a>
			<a href=<?="remove_student.php?class_id=$id&id=".$student["id"];?>
				>Удалить</a>

		</li>

		<?php
	}

	?>
</ul>

<a href=<?="add_student_page.php?class_id=".$id;?>>Добавить ученика</a>

<br>Незарегестрированные ученики <?=$class_name;?>: <br>

<?php

$unconf_students = $mysqli->query("SELECT `id`, `code`, `name`, `surname`".
	" FROM `unconf_users` WHERE `class_id`=$id");
?>

<ul>
	<?php
	
	while ($student = $unconf_students->fetch_assoc()) {
		$student["surname"] = htmlspecialchars($student["surname"]);
		$student["name"] = htmlspecialchars($student["name"]);
		?>

		<li>
			
			<?=$student["surname"]." ".$student["name"];?>
			Пригласительный код: <?=$student["code"];?>
			<a href=<?="remove_unconf_student.php?class_id=$id&id=".$student["id"];?>
				>Удалить</a>

		</li>

		<?php
	}

	?>
</ul>


<?php

$mysqli->close();
html_end();