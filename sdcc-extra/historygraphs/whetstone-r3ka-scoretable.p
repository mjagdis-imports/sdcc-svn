# Configure for SDCC target
set output "whetstone-r3ka-score.svg"
datafile = "whetstone-r3ka-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 2

set arrow from 13131, 36.851+arrowlength to 13131, 36.851
set label "4.2.0" at 13131, 36.851+arrowlength
set arrow from 14208, 40.783+arrowlength to 14208, 40.783
set label "4.3.0" at 14208, 40.783+arrowlength
set arrow from 14648, 40.997+arrowlength to 14648, 40.997
set label "4.4.0" at 14648, 40.897+arrowlength
set arrow from 15246, 40.558+arrowlength to 15246, 40.558
set label "4.5.0" at 15246, 40.558+arrowlength
set arrow from 16640, 63.363+arrowlength to 16640, 63.363
set label "4.6.0" at 16640, 63.363+arrowlength

