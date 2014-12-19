<?php
// Constants
define("X", 0);
define("Y", 1);


// server at Ikaros
$server_ip = '127.0.0.1';
$send_port = '9200'; // send to this port

// default receive port
$recieve_port = '1337'; // receive at this port

// create the socket
$socket = createSocket();



// handle get commands
if (isset($_GET['get'])) {

	// use userdefined port if set from WEB UI
	if (isset($_GET['port']))
		$recieve_port = $_GET['port'];

	// Bind the socket to server sender
	bindSocket($socket, $server_ip, $recieve_port);


	//Waiting to receive
	while(1)
	{
		$r = socket_recvfrom($socket, $buf, 512, 0, $remote_ip, $remote_port);

		$buf = fromStr($buf);
		echo json_encode($buf); 
		break;
	}


// handle send commands
} else if (isset($_GET['send'])){


	switch ($_GET['send']) {

		case "pause":
			$send = "-2"; // Pause flag 
			break;

		case "play":
			$send = "-1"; // Play flag 

			if (isset($_GET['tick']))
				$send = "P".$_GET['tick']; // P - play tick flag
			break;

		case "curves":
			$send = $_POST['curves'];
			$send = toStr($send);
			break;
		
		default:
			$send = "send command not recognized";
	}
	
	$send .= "\0";
	sendData($socket, $server_ip, $send_port, $send);


// handle format requests
} else if (isset($_GET['format'])) {
	$array = $_POST['curves'];
	$string = "";


	switch ($_GET['format']) {

		case "toString":
			$string = toStr($array);
			break;

		case "fromString":
			$array = fromStr($array);
			$string = json_encode($array);
			break;

		default:
			$string = "format command not recognized";
	}
	
	echo $string;
} 


socket_close($sock);

// functions

// convert matrix of curves to array of strings
// returns an array of string representations for each curve
// $array[curveNbr][pointNbr][x/y] -> $ret = curveNbr#x0:y0 x1:y1 x2:y2 ...
function toStr($array) {
	
	$ret = "";
	for ($i = 0; $i < count($array); $i++) {
		if ($i > 0)
			$ret .= "#";

		
		for ($n = 0; $n < count($array[$i]); $n++) {
			if ($n > 0)
				$ret .= " ";
			$ret .= round($array[$i][$n][X],2).":".round($array[$i][$n][Y],2);

		}
		
	}

	return $ret;
}



// convert array of strings to matrix
// returns a matrix for a an array of string representations of curves
// $string = curveNbr#x0:y0 x1:y1 x2:y2 ... -> $array[curveNbr][pointNbr][x/y]
function fromStr($string) {
	

	$array;

	$curve = explode("#", $string);
	
	for ($i = 0; $i < count($curve); $i++) {
		
		$points = explode(" ", $curve[$i]);

		for ($n = 0; $n < count($points); $n++) {
			$cords = explode(":", $points[$n]);

			$array[$i][$n][X] = $cords[0];
			$array[$i][$n][Y] = $cords[1];

		}

	}

	return $array;
}



// creates and returns a variable of the socket
function createSocket() {
// create socket
	if(!($sock = socket_create(AF_INET, SOCK_DGRAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Couldn't create socket: [$errorcode] $errormsg \n");
	}

	return $sock;

}

// sends data to a socket
function sendData($socket, $ip, $port, $msg) {
	socket_sendto($socket, $msg, strlen($msg), 0, $ip, $port);
}

// Bind a socket to a dest address
function bindSocket($socket, $ip, $port) {
	if( !socket_bind($socket, $ip , $port) )
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not bind socket : [$errorcode] $errormsg \n");
	}
}


	?>
