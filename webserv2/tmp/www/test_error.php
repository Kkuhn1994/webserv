#!/opt/homebrew/bin/php
<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html><body><h1>Error Test</h1>";
// This will cause an error
$undefined_variable = $nonexistent_array['key'];
echo "</body></html>";
?>
