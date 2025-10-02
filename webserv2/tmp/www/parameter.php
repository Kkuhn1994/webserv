<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<h1>Hello from WebServ CGI!</h1>\n";
$name = $_REQUEST['name'];
$age = $_REQUEST['age'];
echo "<p>Blabla</p>\n";
echo "<h1>Hello my name is " . $name . " and i am " . $age . " years old!</h1>\n";
// echo "<p>Query String: " . ($_SERVER['QUERY_STRING'] ?? 'none') . "</p>\n";
echo "<p>Query String: " . ($_SERVER['QUERY_STRING'] ?? 'none') . "</p>\n";
?>