<?php
$array = $_POST['servo'];
echo $array[0][0][0] ." has ".count($array[0][0]) ." points";
//foreach ($array as $item) {

	// [curve nr][point][0,1,2] (0= curve name, 1= point.x, 2=point.y)
    //echo $array[0][0][1];
//}
$myfile = fopen("newfile.txt", "w") or die("Unable to open file!");
$write = "ID/1 X/1 Y/1\n";
for ($i = 0; $i < count($array); $i++) {
	for ($n = 0; $n < count($array[$i]); $n++) {
    	$write .= $i ." ".round($array[$i][$n][1],2)." ".round($array[$i][$n][2],2)."\n";
    }
} 
fwrite($myfile, $write);
fclose($myfile);


?>
