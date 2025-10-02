#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body>";
echo "<h1>PHP Parameters Test</h1>";
echo "<p>GET parameters:</p><ul>";
foreach($_GET as $key => $value) {
    echo "<li>$key = $value</li>";
}
echo "</ul>";
echo "<p>POST data length: " . strlen(file_get_contents('php://input')) . "</p>";
echo "</body></html>";
?>
