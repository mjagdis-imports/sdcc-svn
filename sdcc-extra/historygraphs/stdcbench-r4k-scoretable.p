set output "stdcbench-r4k-score.svg"
set terminal svg size 640,480
set style data lines
set key bottom right
set xlabel "SDCC revision"
set ylabel "stdcbench score"

set title "stdcbench on RCM4110 board (Rabbit 4000 at 29.49 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 1

set arrow from 16640, 90+arrowlength to 16640, 90
set label "4.6.0" at 16640, 90+arrowlength

plot "stdcbench-r4k-scoretable" using 1:4 title "default", "stdcbench-r4k-scoretable" using 1:2 title "size", "stdcbench-r4k-scoretable" using 1:3 title "speed"

