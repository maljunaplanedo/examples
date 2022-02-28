<?php

require_once dirname(__FILE__)."/src/include.php";

if (!isset($_REQUEST["id"])) {
	header("Location: index.php");
	exit();
}

$id = (int)$_REQUEST["id"];

page_init();
global $me;

if ($me["type"] != 1) {
	header("Location: index.php");
	exit();
}

html_begin("Тестирующая система", $id);
page_head();
html_end();