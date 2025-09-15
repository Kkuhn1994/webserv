<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<h1>CGI Test Successful!</h1>\n";
echo "<p>Method: " . $_SERVER['REQUEST_METHOD'] . "</p>\n";
echo "<p>Query: " . $_SERVER['QUERY_STRING'] . "</p>\n";
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $input = file_get_contents('php://stdin');
    echo "<p>POST Data: " . htmlspecialchars($input) . "</p>\n";
}
?>
