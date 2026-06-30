set output "stdcbench-r3ka-score.svg"
set terminal svg size 640,480
set style data lines
set key bottom right
set xlabel "revision"
set ylabel "stdcbench score"

# Configure arrow length (to make the plot look nice)
arrowlength = 5

set arrow from 12085, 124+arrowlength to 12085, 124
set label "4.1.0" at 12085, 124+arrowlength
set arrow from 13131, 126+arrowlength to 13131, 126
set label "4.2.0" at 13131, 126+arrowlength
set arrow from 14208, 85+arrowlength to 14208, 85
set label "4.3.0" at 14208, 85+arrowlength
set arrow from 14648, 66+arrowlength to 14648, 66
set label "4.4.0" at 14648, 66+arrowlength
set arrow from 15246, 65+arrowlength to 15246, 65
set label "4.5.0" at 15246, 65+arrowlength
set arrow from 16640, 133+arrowlength to 16640, 133
set label "4.6.0" at 16640, 133+arrowlength

plot "stdcbench-r3ka-scoretable" using 1:4 title "default", "stdcbench-r3ka-scoretable" using 1:2 title "size", "stdcbench-r3ka-scoretable" using 1:3 title "speed"

