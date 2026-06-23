# Configure for SDCC target
set output "whetstone-r4k-size.svg"
datafile = "whetstone-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

# Arrows for SDCC releases
set arrow from 16640, 9595+arrowlength to 16640, 9595
set label "4.6.0" at 16640, 9595+arrowlength

