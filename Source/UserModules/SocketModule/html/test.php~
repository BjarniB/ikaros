

<?php
$server_ip   = '127.0.0.1';
$server_port = 9100;
$beat_period = 5;
$message     = 'PyHB';
print "Sending heartbeat to IP $server_ip, port $server_port\n";
print "press Ctrl-C to stop\n";
if ($socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP)) {
    socket_sendto($socket, $message, strlen($message), 0, $server_ip, $server_port);
    print "Time: " . date("%r") . "\n";
} else {
  print("can't create socket\n");
}
?>



