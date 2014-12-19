/**
 * Highcharts plugin to provide interpolation on line charts
 *
 * Author: Stephane Vanraes
 * License: MIT License
 * Requires: Highcharts 3.0+
 *
 * Usage:
 * 		add the following to the chart options
 *		interpolate: {
 *			enabled: true,
 *			onSeries: -series-number-, [optional]
 *			callback: function (series, xvalue, yvalue)
 *		}
 * See the wiki page for more information.
 * */
(function (H) {
    H.wrap(H.Pointer.prototype, 'runPointActions', function (proceed) {
        proceed.apply(this, [].slice.call(arguments, 1));

        // Check if interpolation is turned on
        if (!this.chart.options.interpolation || !this.chart.options.interpolation.enabled) {
            // No interpolation
            return;
        }
        var chart = this.chart,
            // Options
            options = chart.options.interpolation,
            callback = options.callback,
            // Mouse-event
            e = arguments[1],
            // Data
            series = chart.series[options.onSeries || (chart.hoverSeries ? chart.hoverSeries._i : false) || 0],
            data = series.data,
            // Nearby points
            p1,
            p2,
            // Axis
            xAxis = series.xAxis,
            // X-Value
            x = chart.inverted ? e.chartY : e.chartX,
            xval = xAxis.toValue(x),
            // Y-Value
            yval;

        // Calculate Y Value
        // first find the surrounding points
        for (i = 0; i < data.length - 1; i++) {
            if (xval > data[i].x && xval < data[i+1].x) {
                p1 = data[i];
                p2 = data[i+1];
            }
        }
        // Check if points where found
        if (p1 && p2) {
            // Find linear function
            var a =  ((p2.y - p1.y) / (p2.x - p1.x)),
                b =  (p1.y - a * p1.x);
            // Use function
            yval = (a*xval) + b;
        } else {
            xval = undefined;
            yval = undefined;
        }

        // Run the callback function
        if (callback) {
            callback(series, xval, yval);
        }
    });
}(Highcharts));
