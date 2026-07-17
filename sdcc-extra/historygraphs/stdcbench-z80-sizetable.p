set output "stdcbench-z80-size.svg"
set terminal svg size 640,480
set style data lines
set xlabel "revision"
set ylabel "size [B]"

# Configure arrow length (to make the plot look nice)
arrowlength = 500

trans(x) = x < 10000 ? x : x - 18000
set yrange [8000:14000]
set ytics ("8000" 8000, "9000" 9000, "10000" 10000, "29000" 11000, "30000" 12000, "31000" 13000, "32000" 14000)
set arrow from graph 0, first 6750 to graph 1, first 6750 nohead lt 500 lw 20 lc bgnd
set label "c90lib module enabled" at 10221, 10000 front
set arrow from 9256, 9140 to 9256, 8640 front
set label "3.5.0" at 9256, 9140 front
set arrow from 9618, 8589+arrowlength to 9618, 8589 front
set label "3.6.0" at 9618, 8589+arrowlength front
set arrow from 10582, trans(30206)+arrowlength to 10582, trans(30206)
set label "3.8.0" at 10582, trans(30206)+arrowlength
set arrow from 11214, trans(30282)+arrowlength to 11214, trans(30282)
set label "3.9.0" at 11214, trans(30282)+arrowlength
set arrow from 11533, trans(31542) to 11533, trans(31042)
set label "4.0.0" at 11533, trans(31542)
set arrow from 12085, trans(30588) to 12085, trans(30088)
set label "4.1.0" at 12085, trans(30588)
set arrow from 13131, trans(29260) to 13131, trans(28760)
set label "4.2.0" at 13131, trans(29260)
set arrow from 14208, trans(32142) to 14208, trans(31642)
set label "4.3.0" at 14208, trans(32142)
set arrow from 14648, trans(30052) to 14648, trans(29552)
set label "4.4.0" at 14648, trans(30052)
set arrow from 15246, trans(28576)+arrowlength to 15246, trans(28576)
set label "4.5.0" at 15246, trans(28576)+arrowlength
set arrow from 16640, trans(28201)+arrowlength to 16640, trans(28201)
set label "4.6.0" at 16640, trans(28201)+arrowlength

plot "stdcbench-z80-sizetable" using 1:(trans($4)) title "default", "stdcbench-z80-sizetable" using 1:(trans($2)) title "size", "stdcbench-z80-sizetable" using 1:(trans($3)) title "speed", 10000 lt rgb "white" lw 20 notitle

