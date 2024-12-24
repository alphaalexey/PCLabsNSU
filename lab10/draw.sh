#!/usr/bin/gnuplot

set xlabel "Operations count"
set ylabel "Time"
set grid

set arrow from 112, graph 0 to 112, graph 1 nohead lt rgb "blue"
set label "112" at 112, graph 0.2 center

set arrow from 224, graph 0 to 224, graph 1 nohead lt rgb "blue"
set label "224" at 224, graph 0.2 center

plot 'log.txt' using 1:2 with lines linecolor rgb "red" title 'Graph'

pause -1
