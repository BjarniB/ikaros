
var allowSend = false;
var shiftDown = false; // if shift key is down
var selectedSeries;

var tickEnd; //movement ends at this tick





$(function () {
 $(document).ready(function() {
  Highcharts.setOptions({
   global: {
    useUTC: false
  }
});

  var chart;
  $('#container').highcharts({
   chart: {
                //animation: Highcharts.svg, // don't animate in old IE
                animation: false,
                events: {
                  redraw: function(e) {
                    if (allowSend)
                      sendCurves();
                  },

                  click: function (e) {




                    // if shift key is pressed
                    if (shiftDown) {

                      var x =  e.xAxis[0].value,
                      y = e.yAxis[0].value,
                      z = e.yAxis[0].value;
                      selectedSeries.addPoint([x, y], true, false);
                    }


                  }


                }
              },
              xAxis: {
                crosshair: {
                  color: 'red',
                  snap: true
                },
                type: 'tick',
                text: 'Tick',
                min: 0,
                tickPixelInterval: 150
              },
              yAxis: [{
               title: {
                text: 'Angle',
                min: 0,
                max: 360
              },
              plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
              }]
            }],
            tooltip: {
             formatter: function() {

              selectedSeries = this.series;

              var shiftDiv = document.getElementById('shiftPress');
              shiftDiv.innerHTML="ADDING POINT TO "+selectedSeries.name;

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

         exporting: {
           enabled: true
         },
         title: {
          text: 'Dynamixel interactor'
        }
      });
});



});


function loadFromStr() {
  var string = document.getElementById('stringInput').value;
  $.post("handler.php?format=fromString",
  {
    'string' : string
  },
  function(data,status){
    //alert(data);
    var curves = JSON.parse(data);
    //window.alert(data + "\nStatus: " + status);

    genCurves(curves);
  });

}


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

        $.get( "handler.php?get=curves&port="+ port, function( data, status) {

          window.alert(data + "\nStatus: " + status);

          var curves = JSON.parse(data);
          
          genCurves(curves);



        });



      }


        // generates and adds curves from a string
        function genCurves(curves) {

        var interactionDiv = document.getElementById('interaction');
        var setupDiv = document.getElementById('setup');


        interactionDiv.style.visibility='visible';
        setupDiv.style.visibility='hidden';


         var chart = $('#container').highcharts();

          // set min and max values for y axis (0,360)
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

            for (var n = 0; n < curves[i].length; n++) {
              var x = parseFloat(curves[i][n][0]);
              var y = parseFloat(curves[i][n][1]);

              var index = chart.series.length-1;


              chart.series[index].addPoint([x, y], true, false);

              tickEnd = x;
            }


          }
        }


        

        function play () {
          var play = document.getElementById('playButton');
          var pause = document.getElementById('pauseButton');
          play.style.background='blue';
          pause.style.background='grey';



          var chart = $('#container').highcharts();
          var curves = chart.series;

          var freq = 30;


          allowSend = true;
          

          $.get( "handler.php?action=play", function( data ) {
            $( ".result" ).html( data );


          });

          crossHairAnim(0,freq, tickEnd);
        }; 

        function pause () {
              var play = document.getElementById('playButton');
          var pause = document.getElementById('pauseButton');
          play.style.background='grey';
          pause.style.background='blue';
          allowSend = false;
        }



        function crossHairAnim (i, freq, endAt) {          
         setTimeout(function () {   
           var chart = $('#container').highcharts();
           chart.xAxis[0].removePlotLine('timeline_cross');
           chart.xAxis[0].addPlotLine({
            value: i,
            color: 'red',
            width: 1,
            id: 'timeline_cross'
          });
           i+=(1/4);
           if (i < endAt) crossHairAnim(i, freq, endAt);
           else {

            $.get( "handler.php?action=play", function( data ) {
              $( ".result" ).html( data );
            });
          }

        }, freq/4)
       }; 

//function to handle if shift key event
$(function() {                           
 $(document).on('keyup keydown', function(e){
  shiftDown = e.shiftKey;


  var shiftDiv = document.getElementById('shiftPress');


  if(shiftDown)
    shiftDiv.style.visibility='visible';
  else
    shiftDiv.style.visibility='hidden';

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