# Configure for SDCC target
set output "whetstone-r4k-score.svg"
datafile = "whetstone-r4k-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 2

set arrow from 16640, 65.547+arrowlength to 16640, 65.547
set label "4.6.0" at 16640, 65.547+arrowlength
