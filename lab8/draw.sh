#!/usr/bin/gnuplot

set xlabel "KiB"
set ylabel "Time"
set grid

set arrow from 48, graph 0 to 48, graph 1 nohead lt rgb "blue"
set label "L1, 48KiB" at 48, graph 0.5 center

set arrow from 1328, graph 0 to 1328, graph 1 nohead lt rgb "blue"
set label "L2+L1, 1280+48KiB" at 1328, graph 0.5 center

set arrow from 25904, graph 0 to 25904, graph 1 nohead lt rgb "blue"
set label "L3+L2+L1, 24576+1280+48KiB" at 25904, graph 0.5 center

set multiplot layout 3,1
plot 'log.txt' every ::0::128 using 1:2 with lines linecolor rgb "red" title 'Random access'
plot 'log.txt' every ::1000::1500 using 1:2 with lines linecolor rgb "red" title 'Random access'
plot 'log.txt' every ::0::27648 using 1:2 with lines linecolor rgb "red" title 'Random access'
unset multiplot

pause -1
