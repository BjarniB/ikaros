
var pause = true; // 
var shiftDown = false; // if shift key is down
var ctrlDown = false; // if ctrl  key is down

var tickEnd; //movement ends at this tick
var selectedSeries; // curve that was selected last


$(function () {
 $(document).ready(function() {
  Highcharts.setOptions({
   global: {
    useUTC: false
  }
});

  var chart;
  $('#container').mousemove(move);
  $('#container').highcharts({
   chart: {
    animation: false,
    events: {

      // send keyframes on plot redraw
      redraw: function(e) {
        if (!pause)
          sendCurves();
      },

      click: function (e) {
                    // if shift key is pressed
                    if (shiftDown) {
                      var chart = $('#container').highcharts();

                      var axisIndex;
                      $(chart.yAxis).each(function(i, yAxis){
                       if (yAxis == selectedSeries.yAxis)
                        axisIndex = i;
                    });


                      var x =  e.xAxis[0].value,
                      y = e.yAxis[axisIndex].value,
                      z = e.yAxis[axisIndex].value;
                      selectedSeries.addPoint([x, y], true, false);



                    }


                  }


                }
              },
              xAxis: {

                type: 'tick',
                text: 'Tick',
                min: 0,
                tickPixelInterval: 150
              },
              yAxis: [{
               title: {
                text: 'Angle',
                min: 0,
                max: 360,



              },
              tickPositions: [0, 90, 180, 270, 360]
              ,
              plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
              }]
            },
            { // Secondary yAxis (speed / torque)
              gridLineWidth: 0,
              title: {
                text: 'speed / torque',
                min: 0,
                max: 1,
                style: {
                  color: Highcharts.getOptions().colors[0]
                }
                
              },
              labels: {
                style: {
                  color: Highcharts.getOptions().colors[0]
                }
              },
              opposite: true

            }
            ],
            tooltip: {
             formatter: function() {

              selectedSeries = this.series;

              return Highcharts.numberFormat(this.x, 2) +'<br/>'+
              Highcharts.numberFormat(this.y, 2);
            },
            positioner:function(){

              return { x : this.x, y : 10 };
            }
          },
          legend: {
           enabled: true
         },
         title: {
          text: 'Dynamixel interactor'
        }
      });
});



});

// send request to PHP server to parse a string to array of curves
function loadFromStr() {
  var string = document.getElementById('stringInput').value;
  $.post("handler.php?format=fromString",
  {
    'curves' : string
  },
  function(data,status){
    var curves = JSON.parse(data);
    genCurves(curves);
  });

}


// send array of curves 
function sendCurves(){
  var chart = $('#container').highcharts();
  var curves = chart.series;

  var array = [[[]]];


  for (var i = 0; i < curves.length; i++) {
    array[i] = new Array();

    for (var n = 0; n < curves[i].data.length; n++) {

      var curve = curves[i];
      var point = curve.data[n];

      array[i][n] = new Array();


      array[i][n][0] = point.x;
      array[i][n][1] = point.y;

      tickEnd = point.x;

    }
  }



  $.post("handler.php?send=curves",
  {
    'curves' : array
  },
  function(data,status){
          //window.alert(data + "\nStatus: " + status);
        });


}



      // POLL CURVE DATA FROM SERVER
      function getCurves(){

        var port = document.getElementById('port').value;

        var getRecordedBtn = document.getElementById('getRecordedBtn');
        getRecordedBtn.innerHTML="Listening for keyframes at "+port;

        

        $.get( "handler.php?get=curves&port="+ port, function( data, status) {

          window.alert("Received keyframes!");

          var curves = JSON.parse(data);
          
          genCurves(curves);



        });



      }


        // generates and adds curves from a string
        function genCurves(curves) {

          var interactionDiv = document.getElementById('interaction');
          var shiftDiv = document.getElementById('shift');
          var setupDiv = document.getElementById('setup');




          interactionDiv.style.display = 'inline';
          setupDiv.style.display = 'none';

          var chart = $('#container').highcharts();

          // set min and max values for y axes
          chart.yAxis[0].setExtremes(0,360);
          chart.yAxis[1].setExtremes(0,1);

          for (var i = 0; i < curves.length; i++) {


            chart.addSeries({
              name: "servo "+i,
              draggableX: true,
              draggableY: true,
              dragMinY: 0,
              dragMaxY: 360,
              dragMinX: 0,
              type: 'spline'

            });

            var index = chart.series.length-1;

            for (var n = 0; n < curves[i].length; n++) {
              var x = parseFloat(curves[i][n][0]);
              var y = parseFloat(curves[i][n][1]);


              chart.series[index].addPoint([x, y], true, false);

              tickEnd = x;
            }

            var color = chart.series[index].color;


            // Use to add different style curves for servo and torque
            /*
            chart.addSeries({
              name: "speed "+i,
              yAxis: 1,
              draggableX: true,
              draggableY: true,
              dragMinY: 0,
              dragMaxY: 1,
              dragMinX: 0,
              type: 'spline',
              color: color,
              visible: false,
              dashStyle: 'ShortDot',
              data: [[0,1], [tickEnd,1]]

            });


            


            chart.addSeries({
              name: "torque "+i,
              yAxis: 1,
              draggableX: true,
              draggableY: true,
              dragMinY: 0,
              dragMaxY: 1,
              dragMinX: 0,
              type: 'spline',
              color: color,
              visible: false,
              dashStyle: 'Dash',
              data: [[0,1], [tickEnd,1]]

            });
*/

}
}




function playClick () {
  var playBTN = document.getElementById('playButton');
  var pauseBTN = document.getElementById('pauseButton');
  playBTN.style.background='blue';
  pauseBTN.style.background='grey';


  var chart = $('#container').highcharts();
  var curves = chart.series;

 chart.xAxis[0].removePlotLine('timeline_cross');

  pause = false;


  $.get( "handler.php?send=play", function( data ) {
    $( ".result" ).html( data );
  });


  sendCurves();
}; 

function pauseClick () {
  var playBTN = document.getElementById('playButton');
  var pauseBTN = document.getElementById('pauseButton');
  playBTN.style.background='grey';
  pauseBTN.style.background='blue';
  pause = true;

  $.get( "handler.php?send=pause", function( data ) {
    $( ".result" ).html( data );
  });
}
function playTick (tick) {

  $.get( "handler.php?send=play&tick="+tick, function( data ) {
    $( ".result" ).html( data );
  });
  pause = false;
}

function smoothReset () {
  var chart = $('#container').highcharts();
  var curves = chart.series;


  tickEnd += 50;
  endX = tickEnd;

  for (var i = 0; i < curves.length; i++) {


    var endY = curves[i].data[0].y;

    curves[i].addPoint([endX, endY], true, false);
  }nd



  addedSmoothing = true;
}




//function to handle if shift key event
$(function() {                           
 $(document).on('keyup keydown', function(e){
  shiftDown = e.shiftKey;
  ctrlDown = e.ctrlKey;



  var shiftColor = (shiftDown ? "green" : "grey");
  var ctrlColor = (ctrlDown ? "green" : "grey");



  var shiftDiv = document.getElementById('shiftPress');
  var ctrlDiv = document.getElementById('ctrlPress');

  shiftDiv.style.background=shiftColor;
  ctrlDiv.style.background=ctrlColor;





} );

});

function saveFile () {  

  var chart = $('#container').highcharts();
  var curves = chart.series;

  var array = [[[]]];




  for (var i = 0; i < curves.length; i++) {
    array[i] = new Array();

    for (var n = 0; n < curves[i].data.length; n++) {

      var curve = curves[i];
      var point = curve.data[n];

      array[i][n] = new Array();


      array[i][n][0] = point.x;
      array[i][n][1] = point.y;



    }
  }

  

  $.post("handler.php?format=toString",
  {
    'curves' : array
  },
  function(data,status){

    var saveCurves = "data:text/csv;charset=utf-8,";
    saveCurves += data;

    var encodedUri = encodeURI(saveCurves);
    window.open(encodedUri);
  });




}

function move(event) {

  if (ctrlDown) {



    var chart = $('#container').highcharts();


    var x = chart.xAxis[0].toValue(event.pageX)-2;


  //var x =  Math.round(e.xAxis[0].value);

  playTick(x);
  
  chart.xAxis[0].removePlotLine('timeline_cross');
  chart.xAxis[0].addPlotLine({
    value: x,
    color: 'red',
    width: 1,
    id: 'timeline_cross'
  });
}


}