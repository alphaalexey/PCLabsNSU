#!/usr/bin/gnuplot

set xlabel "Fragments"
set ylabel "Time"
set grid

# set arrow from 12, graph 0 to 12, graph 1 nohead lt rgb "blue"
# set label "12" at 12, graph 0.2 center

set arrow from 8, graph 0 to 8, graph 1 nohead lt rgb "blue"
set label "8" at 8, graph 0.2 center

set arrow from 16, graph 0 to 16, graph 1 nohead lt rgb "blue"
set label "16" at 16, graph 0.2 center

plot 'log.txt' using 1:2 with lines linecolor rgb "red" title 'Graph'

pause -1
