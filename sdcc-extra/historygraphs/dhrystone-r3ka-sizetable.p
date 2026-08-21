# Configure for SDCC target
set output "dhrystone-r3ka-size.svg"
datafile = "dhrystone-r3ka-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

set arrow from 12085, 9920+arrowlength to 12085, 9920
set label "4.1.0" at 12085, 9920+arrowlength
set arrow from 13131, 8868+arrowlength to 13131, 8868
set label "4.2.0" at 13131, 8868+arrowlength
set arrow from 14208, 8836+arrowlength to 14208, 8836
set label "4.3.0" at 14208, 8836+arrowlength
set arrow from 14648, 8393+arrowlength to 14648, 8393
set label "4.4.0" at 14648, 8393+arrowlength
set arrow from 15246, 8196+arrowlength to 15246, 8196
set label "4.5.0" at 15246, 8196+arrowlength
set arrow from 16640, 7690+arrowlength to 16640, 7690
set label "4.6.0" at 16640, 7690+arrowlength

