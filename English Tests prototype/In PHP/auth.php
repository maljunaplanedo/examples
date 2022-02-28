<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

if ($me !== -1 || !isset($_REQUEST["login"]) || !isset($_REQUEST["password"])) {
	header("Location: index.php");
	exit();
}

function _die($message) {
	global $mysqli;
	if (isset($mysqli))
		$mysqli->close();
	header("Location: auth_page.php?error=$message");
	exit();
}

$login = $_REQUEST["login"];
$password = $_REQUEST["password"];

if (strlen($login) === 0 || strlen($password) === 0)
	_die("wrong");

$mysqli = mysqli_make();
$login = $mysqli->escape_string($login);
$password = $mysqli->escape_string($password);

$candidate = $mysqli->query("SELECT `id`, `password` FROM `users` WHERE `login`='$login'");

if ($candidate->num_rows !== 1)
	_die("wrong");

$candidate = $candidate->fetch_assoc();
$password_hash = $candidate["password"];

$candidate = (int)($candidate["id"]);

if (!password_verify($password, $password_hash))
	_die("wrong");


$auth_hash = bin2hex(random_bytes(30));

$mysqli->query("INSERT INTO `auths` (`auth_hash`, `user_id`) VALUES ('$auth_hash', $candidate)");
setcookie("auth_hash", $auth_hash, time() + AUTH_HASH_LIFETIME);

$mysqli->close();

header("Location: index.php");