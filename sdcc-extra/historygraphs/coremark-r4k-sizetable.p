# Configure for SDCC target
set output "coremark-r4k-size.svg"
datafile = "coremark-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

# Arrows for SDCC releases
set arrow from 16640, 12796+arrowlength to 16640, 12796
set label "4.6.0" at 16640, 12796+arrowlength

