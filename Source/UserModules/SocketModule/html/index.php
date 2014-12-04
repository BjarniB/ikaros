<?php
// Constants
define("X", 0);
define("Y", 1);


$array = $_POST['servo'];
$send = toStr($array);

$receive = toMatrix($send);


echo $receive[1][0][X];

// generate sample curves
if (isset($_GET['get']) && $_GET['get'] == "curves") {
	
	$genCurves;
	$genCurves[0] = "0# 0:1 1:20";
	$genCurves[1] = "1# 0:5 1:80";
	$genCurves[2] = "2# 0:5 1:80 2.5:200 3:120";
	$genCurves[2] = "2# 0:5 1:80 2.5:200 7:120";

	$ret = toMatrix($genCurves);
	echo  json_encode($ret);


} else {
	$server_ip   = '127.0.0.1';
	$server_port = 9100;
	print "Sending heartbeat to IP $server_ip, port $server_port\n";
	print "press Ctrl-C to stop\n";
	if ($socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)) {
		for ($i = 0; $i < count($send); $i++) {
			$send[$i] .= "\0";
			socket_sendto($socket, $send[$i], strlen($send[$i]), 0, $server_ip, $server_port);
		}
	} else {
		print("can't create socket\n");
	}

}

// functions

// convert matrix of curves to array of strings
// returns an array of string representations for each curve
// $array[curveNbr][pointNbr][x/y] -> $ret = curveNbr#x0:y0 x1:y1 x2:y2 ...
function toStr($array) {
	$ret = "";

	for ($i = 0; $i < count($array); $i++) {

		$ret[$i] .= $i."#";
		for ($n = 0; $n < count($array[$i]); $n++) {
			$ret[$i] .= " ".round($array[$i][$n][X],2).":".round($array[$i][$n][Y],2); 

		}
	}

	return $ret;
}

// convert array of strings to matrix
// returns a matrix for a an array of string representations of curves
// $string = curveNbr#x0:y0 x1:y1 x2:y2 ... -> $array[curveNbr][pointNbr][x/y]
function toMatrix($string) {
	

	$array;


	for ($i = 0; $i < count($string); $i++) {
		$split = explode("# ", $string[$i]);

		$curveId = $split[0];
		$points = explode(" ", $split[1]);


		for ($n = 0; $n < count($points); $n++) {
			$cords = explode(":", $points[$n]);



			$array[$curveId][$n][X] = $cords[0];
			$array[$curveId][$n][Y] = $cords[1];


		}
		
	}

	return $array;
}

?>
