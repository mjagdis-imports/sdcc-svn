# Plot the graphs
set key autotitle columnhead
set terminal svg size 640,480
set style data lines
set xlabel "revision"
set ylabel "size [B]"

plot datafile using 1:4, datafile using 1:2, datafile using 1:3

