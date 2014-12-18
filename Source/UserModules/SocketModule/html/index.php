<HTML>
	<HEAD>
		<TITLE>IKAROS DYNAMIXEL UI INTERACTOR</TITLE>
		<script type="text/javascript"
		src="http://code.jquery.com/jquery-1.10.1.min.js"></script>
		<script src="http://code.highcharts.com/highcharts.js"></script>
		<script src="http://code.highcharts.com/modules/exporting.js"></script>
		<script src="js/draggable-points-master/draggable-points.js"></script>
		<script src="js/graph.js"></script>
		<script src="js/theme/dark.js"></script>

		<link rel="stylesheet" type="text/css" href="style/style.css">


	</HEAD>
	<BODY>
		<div id="container" style="min-width: 728px; height: 400px; margin: 0 auto"></div>

		
</head>
<body>
	<div id="shiftPress" style="visibility: hidden">SHIFT IS PRESSED</div>

		<!--<input type="text" name="curveName" value="curve name"><button onclick="addCurve()">Add curve</button><br>
		<br>-->
		<div id="setup">
			PORT <input type="text" id="port" value="8080"> <button class="button" onClick="getCurves()">Get recorded data</button><br>
			<input type="textbox" id="stringInput"/><button class="button" onClick="loadFromStr()">Load from string</button>
			


		</div>
		<div id="interaction" style="visibility: hidden">
			<button id="pauseButton" class="button" onClick="pause()">Pause</button>
			<button id="playButton" class="button" onClick="play()" style="background: grey">Play</button><br><br>
			<button class="button" onClick="saveFile()"> Save to file </button>

			<br><br>

			PRESS SHIFT TO ADD A POINT<br>
			

		</div>

		

	</BODY>
</HTML>
