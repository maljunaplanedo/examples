<?php

require_once dirname(__FILE__)."/src/include.php";

page_init();
global $me;

only_teachers();

html_begin("Система тестирования");

page_head();

?>

<a href="classes.php">Классы</a>
<a href="tasks.php">Задания</a>

<?php

html_end();