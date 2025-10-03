<?php
move_uploaded_file(
    $_FILES['upload']['tmp_name'] ?? '',
    __DIR__ . '/' . basename($_FILES['upload']['name'] ?? '')
);

header("Content-Type: text/plain");

echo "FILES:\n";
print_r($_FILES);

echo "\nPOST:\n";
print_r($_POST);

echo "\nSERVER:\n";
print_r($_SERVER);
?>