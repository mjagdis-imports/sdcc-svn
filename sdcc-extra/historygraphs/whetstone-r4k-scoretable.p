# Configure for SDCC target
set output "whetstone-r4k-score.svg"
datafile = "whetstone-r4k-scoretable"

set title "Whetstone on RCM4110 board (Rabbit 4000 at 29.49 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 2

set arrow from 16640, 65.547+arrowlength to 16640, 65.547
set label "4.6.0" at 16640, 65.547+arrowlength
