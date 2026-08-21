# Configure for SDCC target
set output "dhrystone-r4k-score.svg"
datafile = "dhrystone-r4k-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

set arrow from 16640, 5341+arrowlength to 16640, 5341
set label "4.6.0" at 16640, 5341+arrowlength

