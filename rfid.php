<?php
$karta = $_GET["cardnr"];

switch ($karta){
        case "7A6AEFA2":
                echo "open:5";
                break;
        case "04373DA96E1C80":
                echo "open:10";
                break;
        case "6EEC2A23":
                echo "open:1";
                break;
        default:
                echo "open:0";
}
?>

