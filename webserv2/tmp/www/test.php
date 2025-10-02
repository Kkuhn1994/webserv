<?php
echo "<h1>Hello from WebServ CGI!</h1>\n";
echo "<p>Request Method: " . ($_SERVER['REQUEST_METHOD'] ?? 'GET') . "</p>\n";
echo "<p>Query String: " . ($_SERVER['QUERY_STRING']) . "</p>\n";
?>
