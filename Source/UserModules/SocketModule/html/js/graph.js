
var allowSend = false;

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

                    var interaction = $('input[name="interact"]:checked').val();

                    if (interaction == "ADD") {

                      var curveNr = $('input[name="curveID"]').val();
                      

                            var x =  e.xAxis[0].value, // current time
                            y = e.yAxis[0].value,
                            z = e.yAxis[0].value;
                            this.series[curveNr].addPoint([x, y], true, false);
                          }
                          

                        }


                      }
                    },
                    xAxis: {
                      crosshair: {
                        color: 'green',
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
                    return Highcharts.numberFormat(this.x, 2) +'<br/>'+
                    Highcharts.numberFormat(this.y, 2);
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

    }
  }



  $.post("index.php?send=curves",
  {
    'servo' : array
  },
  function(data,status){
          //window.alert(data + "\nStatus: " + status);
        });


}



function play (freq, endAt) {
  allowSend = true;
  $.get( "index.php?action=play", function( data ) {
    $( ".result" ).html( data );

  });

  crossHairAnim(0,30, 500);
}; 


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

    $.get( "index.php?action=play", function( data ) {
      $( ".result" ).html( data );
  //alert( "Load was performed." );
});
  }

}, freq/4)
}; 
