set output "stdcbench-z80-score.svg"
datafile = "stdcbench-z80-scoretable"

set terminal svg size 640,480
set style data lines
set key bottom right
set xlabel "SDCC revision"
set ylabel "stdcbench score"

set title "stdcbench on Z80-MBC2 board (Z80 at 8 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 1

set arrow from graph 0, first 6.5 to graph 1, first 6.5 nohead lt 500 lw 20 lc bgnd
set label "c90lib module enabled" at 10221, 6.5 front

set arrow from 9618, 6+arrowlength to 9618, 6
set label "3.6.0" at 9618, 6+arrowlength
set arrow from 11214, 12+arrowlength to 11214, 12
set label "3.9.0" at 11214, 12+arrowlength
set arrow from 11533, 12+arrowlength to 11533, 12
set label "4.0.0" at 11533, 12+arrowlength
set arrow from 12085, 12+arrowlength to 12085, 12
set label "4.1.0" at 12085, 12+arrowlength
set arrow from 13131, 13+arrowlength to 13131, 13
set label "4.2.0" at 13131, 13+arrowlength
set arrow from 14208, 8+arrowlength to 14208, 8
set label "4.3.0" at 14208, 8+arrowlength
set arrow from 14648, 8+arrowlength to 14648, 8
set label "4.4.0" at 14648, 8+arrowlength
set arrow from 15246, 8+arrowlength to 15246, 8
set label "4.5.0" at 15246, 8+arrowlength
set arrow from 16640, 9+arrowlength to 16640, 9
set label "4.6.0" at 16640, 9+arrowlength

plot datafile using 1:4 title "default", datafile using 1:2 title "size", datafile using 1:3 title "speed"

