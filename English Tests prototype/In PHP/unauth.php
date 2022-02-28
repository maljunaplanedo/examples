<?php

require_once dirname(__FILE__)."/src/include.php";


if (isset($_COOKIE["auth_hash"])) {
	$mysqli = mysqli_make();
	$auth_hash = $_COOKIE["auth_hash"];
	setcookie("auth_hash", "-", time() - 1);
	$mysqli->query("DELETE FROM `auths` WHERE `auth_hash`='$auth_hash'");
	$mysqli->close();
}

header("Location: auth_page.php");