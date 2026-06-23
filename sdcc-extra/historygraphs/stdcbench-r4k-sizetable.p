# Configure for SDCC target
set output "stdcbench-r4k-size.svg"
datafile = "stdcbench-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

# Arrows for SDCC releases
set arrow from 16640, 23111+arrowlength to 16640, 23111
set label "4.6.0" at 16640, 23111+arrowlength

