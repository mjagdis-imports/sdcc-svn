# Configure for SDCC target
set output "dhrystone-r3ka-score.svg"
datafile = "dhrystone-r3ka-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

set arrow from 12085, 4107+arrowlength to 12085, 4107
set label "4.1.0" at 12085, 4107+arrowlength
set arrow from 13131, 4925+arrowlength to 13131, 4925
set label "4.2.0" at 13131, 4925+arrowlength
set arrow from 14208, 4995+arrowlength to 14208, 4995
set label "4.3.0" at 14208, 4995+arrowlength
set arrow from 14648, 5542+arrowlength to 14648, 5542
set label "4.4.0" at 14648, 5542+arrowlength
set arrow from 15246, 5327+arrowlength to 15246, 5327
set label "4.5.0" at 15246, 5327+arrowlength
set arrow from 16640, 5645+arrowlength to 16640, 5645
set label "4.6.0" at 16640, 5645+arrowlength

