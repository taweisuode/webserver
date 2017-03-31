<html>
<head>
<meta http-equiv="content-type" content="text/html;charset=utf-8">
<title>鹏哥的第一个web服务器</title>
</head>
<body>
<?php
$array = array(
	"id" => "1",
	"name"=> "pengge",
    "aaa" => "sdsdd",
    "yes" => "sdsdfsfsff"
);
echo "<pre>";
var_dump($_GET);
var_dump($_SERVER);
var_dump($array);
?>
</body>
</html>
