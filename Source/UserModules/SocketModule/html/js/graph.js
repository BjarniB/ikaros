
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
    type: 'spline',
                animation: Highcharts.svg, // don't animate in old IE
                marginRight: 1,
                events: {


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
            title: {
             text: 'Interactive dynamixel curves'
         },
         xAxis: {
             type: 'tick',
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
          return '<b>'+ this.series.name +'</b><br/>'+
          Highcharts.numberFormat(this.x, 2) +'<br/>'+
          Highcharts.numberFormat(this.y, 2);
      }
  },
  legend: {
     enabled: true
 },
 exporting: {
     enabled: true
 },
 navigator : {
    enabled : true
},
series: [{
 name: 'Servo 0',
 data: [30, 200, 150,80],
 draggableX: true,
 draggableY: true,
 dragMinY: 0,
 dragMaxY: 360,
 dragMinX: 0,
 type: 'spline',
 minPointLength: 2

},
{
 name: 'Servo 1',
 data: [70, 300, 50, 50],
 draggableX: true,
 draggableY: true,
 dragMinY: 0,
 dragMaxY: 360,
 dragMinX: 0,
 type: 'spline',
 minPointLength: 2

},
{
 name: 'Servo 2',
 data: [70, 200, 70, 50],
 draggableX: true,
 draggableY: true,
 dragMinY: 0,
 dragMaxY: 360,
 dragMinX: 0,
 type: 'spline',
 minPointLength: 2

}]
});
});

});
