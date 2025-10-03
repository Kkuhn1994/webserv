<?php
move_uploaded_file($_FILES['upload']['tmp_name'], '/tmp/' . $_FILES['upload']['name']);
?>