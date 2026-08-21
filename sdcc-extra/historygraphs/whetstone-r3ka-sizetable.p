# Configure for SDCC target
set output "whetstone-r3ka-size.svg"
datafile = "whetstone-r3ka-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 200

set arrow from 12085, 13656+arrowlength to 12085, 13656
set label "4.1.0" at 12085, 13656+arrowlength
set arrow from 13131, 12441+arrowlength to 13131, 12441
set label "4.2.0" at 13131, 12441+arrowlength
set arrow from 14208, 12456+arrowlength to 14208, 12456
set label "4.3.0" at 14208, 12456+arrowlength
set arrow from 14648, 10887+arrowlength to 14648, 10887
set label "4.4.0" at 14648, 10887+arrowlength
set arrow from 15246, 10715+arrowlength to 15246, 10715
set label "4.5.0" at 15246, 10715+arrowlength
set arrow from 16640, 9923+arrowlength to 16640, 9923
set label "4.6.0" at 16640, 9923+arrowlength

