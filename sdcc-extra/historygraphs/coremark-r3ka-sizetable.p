# Configure for SDCC target
set output "coremark-r3ka-size.svg"
datafile = "coremark-r3ka-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 200

# Arrows for SDCC releases
set arrow from 12085, 16000+arrowlength to 12085, 16000
set label "4.1.0" at 12085, 16000+arrowlength
set arrow from 13131, 14929+arrowlength to 13131, 14929
set label "4.2.0" at 13131, 14929+arrowlength
set arrow from 14208, 14117+arrowlength to 14208, 14117
set label "4.3.0" at 14208, 14117+arrowlength
set arrow from 14648, 14887+arrowlength to 14648, 14887
set label "4.4.0" at 14648, 14887+arrowlength
set arrow from 15246, 14212+arrowlength to 15246, 14212
set label "4.5.0" at 15246, 14212+arrowlength
set arrow from 16640, 13010+arrowlength to 16640, 13010
set label "4.6.0" at 16640, 13010+arrowlength

