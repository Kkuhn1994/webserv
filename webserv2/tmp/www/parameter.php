<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<h1>Hello from WebServ CGI!</h1>\n";
$name = $_REQUEST['name'];
$age = $_REQUEST['age'];
echo "<p>Hello my name is " . $name . "and i am " . $age . "years old!</p>\n";
?>