#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body>";
echo "<h1>PHP CGI Test</h1>";
echo "<p>Current time: " . date('Y-m-d H:i:s') . "</p>";
echo "<p>Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";
echo "</body></html>";
?>
