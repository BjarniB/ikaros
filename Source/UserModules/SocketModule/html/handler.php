<?php
// Constants
define("X", 0);
define("Y", 1);



// generate sample curves
if ($_GET['get'] == "curves") {
	
	$genCurves;
	$genCurves .= "0:100 100:120 400:180";
	$genCurves .= "#";
	$genCurves .= "0:150 100:100 400:180";
	$genCurves .= "#";
	$genCurves .= "0:150 100:80 400:180";

	$ret = fromStr($genCurves);

	//echo json_encode($ret);

	$port = 8080;
	if (isset($_GET['port']))
		$port = $_GET['port'];

	//echo $port;




	if(!($sock = socket_create(AF_INET, SOCK_DGRAM, 0)))
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Couldn't create socket: [$errorcode] $errormsg \n");
	}

//echo "Socket created \n";

// Bind the source address
	if( !socket_bind($sock, "127.0.0.1" , $port) )
	{
		$errorcode = socket_last_error();
		$errormsg = socket_strerror($errorcode);

		die("Could not bind socket : [$errorcode] $errormsg \n");
	}

//echo "Socket bind OK \n";

//Do some communication, this loop can handle multiple clients
	while(1)
	{
		//echo "Waiting for data ... \n";

    //Receive some data
		$r = socket_recvfrom($sock, $buf, 512, 0, $remote_ip, $remote_port);

		$buf = fromStr($buf);
		echo json_encode($buf); 
		break;
    //echo "$remote_ip : $remote_port -- " . $buf;

	}

	socket_close($sock);
	

} else if ($_GET['send'] == "curves"){
	$array = $_POST['curves'];
	$send = toStr($array);
	$send .= "\0";
	

	$server_ip   = '127.0.0.1';
	$server_port = 9100;
	print "Sending curves to IP $server_ip, port $server_port\n";

	if ($socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)) {
		
		socket_sendto($socket, $send, strlen($send), 0, $server_ip, $server_port);

		
	} else {
		print("can't create socket\n");
	}



} else if ($_GET['format'] == "toString") {
	$array = $_POST['curves'];
	$string = toStr($array);
	
	echo $string;
} else if ($_GET['format'] == "fromString") {
	$string = $_POST['string'];
	$array = fromStr($string);
	
	echo json_encode($array);

}



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

?>
