<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<h1>Hello from WebServ CGI!</h1>\n";
echo "<p>Request Method: " . ($_SERVER['REQUEST_METHOD'] ?? 'GET') . "</p>\n";
echo "<p>Query String: " . ($_SERVER['QUERY_STRING'] ?? 'none') . "</p>\n";
?>
