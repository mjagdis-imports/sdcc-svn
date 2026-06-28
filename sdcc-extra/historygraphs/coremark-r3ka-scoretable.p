# Configure for SDCC target
set output "coremark-r3ka-score.svg"
datafile = "coremark-r3ka-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 0.1

set arrow from 12085, 2.138+arrowlength to 12085, 2.138
set label "4.1.0" at 12085, 2.138+arrowlength
set arrow from 13131, 3.503+arrowlength to 13131, 3.503
set label "4.2.0" at 13131, 3.503+arrowlength
set arrow from 14208, 3.243+arrowlength to 14208, 3.243
set label "4.3.0" at 14208, 3.243+arrowlength
set arrow from 14648, 3.951+arrowlength to 14648, 3.951
set label "4.4.0" at 14648, 3.951+arrowlength
set arrow from 15246, 3.806+arrowlength to 15246, 3.806
set label "4.5.0" at 15246, 3.806+arrowlength
set arrow from 16640, 4.212+arrowlength to 16640, 4.212
set label "4.6.0" at 16640, 4.212+arrowlength

