# Plot the graphs
set key autotitle columnhead
set terminal svg size 640,480
set style data lines
set xlabel "revision"
set ylabel "Dhrystones per second"
set key bottom right

# Configure arrow length (to make the plot look nice)
arrowlength = 100

plot datafile using 1:4, datafile using 1:2, datafile using 1:3

