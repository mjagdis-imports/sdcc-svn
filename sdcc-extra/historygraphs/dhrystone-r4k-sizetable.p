# Configure for SDCC target
set output "dhrystone-r4k-size.svg"
datafile = "dhrystone-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

# Arrows for SDCC releases
set arrow from 16640, 8196+arrowlength to 16640, 8196
set label "4.6.0" at 16640, 8196+arrowlength

