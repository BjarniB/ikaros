
var pause = true; // if we should stop sending updates when making changes
var shiftDown = false; // if shift key is down
var ctrlDown = false; // if ctrl  key is down

var tickEnd; //movement ends at this tick
var selectedSeries; // curve that was selected last

// HTML elements
var port, string;
var interactionDiv, shiftDiv, setupDiv, shiftDiv, ctrlDiv;
var playBTN, pauseBTN, getRecordedBtn;
var chart;


$(document).ready(function() {

  // get html elemets
  port = document.getElementById('port').value;
  getRecordedBtn = document.getElementById('getRecordedBtn');
  string = document.getElementById('stringInput').value;
  interactionDiv = document.getElementById('interaction');
  shiftDiv = document.getElementById('shift');
  setupDiv = document.getElementById('setup');
  playBTN = document.getElementById('playButton');
  pauseBTN = document.getElementById('pauseButton');
  shiftDiv = document.getElementById('shiftPress');
  ctrlDiv = document.getElementById('ctrlPress');
  chart = $('#container').highcharts();


  $('#container').mousemove(mouseMove);
  initChart();
});


// send request to PHP server to parse a string to array of curves
function loadFromStr() {
  
 $.post("handler.php?format=fromString",
 {
  'curves' : string
},
function(data,status){
  var curves = JSON.parse(data);
  genCurves(curves);
});

}

// send array of curves to handler.php
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

// RECEIVE CURVE DATA FROM SERVER
function getCurves(){

  getRecordedBtn.innerHTML="Listening for keyframes at "+port;

  $.get( "handler.php?get=curves&port="+ port, function( data, status) {

    window.alert("Received keyframes!");
    var curves = JSON.parse(data);
    genCurves(curves);
  });

}


  // generates and adds curves from a string
  function genCurves(curves) {

    interactionDiv.style.display = 'inline';
    setupDiv.style.display = 'none';

    var chart = $('#container').highcharts();

    // set min and max values for y axes
    chart.yAxis[0].setExtremes(0,360);

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


    }
  }


// play specific timeunit 
  function playTick (tick) {
    $.get( "handler.php?send=play&tick="+tick, function( data ) {
      $( ".result" ).html( data );
    });
    pause = false;
  }



// on mouse move
function mouseMove(event) {

  if (ctrlDown) {

    var chart = $('#container').highcharts();
    var x = chart.xAxis[0].toValue(event.pageX)-2;

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





//######## BUTTONS


function playClick () {
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

  playBTN.style.background='grey';
  pauseBTN.style.background='blue';
  pause = true;

  $.get( "handler.php?send=pause", function( data ) {
    $( ".result" ).html( data );
  });
}


function smoothReset () {
  var chart = $('#container').highcharts();
  var curves = chart.series;

  tickEnd += 50;
  endX = tickEnd;

  for (var i = 0; i < curves.length; i++) {

    var endY = curves[i].data[0].y;
    curves[i].addPoint([endX, endY], true, false);
  }

}

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

//function to handle when ctrl/shift is being pressed
$(function() {                           
 $(document).on('keyup keydown', function(e){
  shiftDown = e.shiftKey;
  ctrlDown = e.ctrlKey;

  var shiftColor = (shiftDown ? "green" : "grey");
  var ctrlColor = (ctrlDown ? "green" : "grey");

  shiftDiv.style.background=shiftColor;
  ctrlDiv.style.background=ctrlColor;

} );

});


//############# init chart

function initChart() {
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
        if (shiftDown) {
          var x =  e.xAxis[0].value, y = e.yAxis[0].value;
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
}],
tooltip: {
  formatter: function() {

    selectedSeries = this.series;
    return Highcharts.numberFormat(this.x, 2) +'<br/>'+
    Highcharts.numberFormat(this.y, 2);},

    positioner:function(){
      return { x : this.x, y : 10 };
    }
  },
  title: {
    text: 'Dynamixel interactor'
  }
  
});
}