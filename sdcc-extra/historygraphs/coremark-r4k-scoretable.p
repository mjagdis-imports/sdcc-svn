# Configure for SDCC target
set output "coremark-r4k-score.svg"
datafile = "coremark-r4k-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 0.1

set arrow from 16640, 3.997+arrowlength to 16640, 3.997
set label "4.6.0" at 16640, 3.997+arrowlength

