set output "coremark-r4k-size.svg"
set terminal svg size 640,480
set style data lines
set xlabel "revision"
set ylabel "size [B]"
set arrow from 16640, 12896 to 16640, 12796
set label "4.6.0" at 16640, 12896
plot "coremark-r4k-sizetable" using 1:4 title "default", "coremark-r4k-sizetable" using 1:2 title "size", "coremark-r4k-sizetable" using 1:3 title "speed"

