# Configure for SDCC target
set output "stdcbench-r3ka-size.svg"
datafile = "stdcbench-r3ka-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 200

set arrow from 12085, 25331+arrowlength to 12085, 25331
set label "4.1.0" at 12085, 25331+arrowlength
set arrow from 13131, 24510+arrowlength to 13131, 24510
set label "4.2.0" at 13131, 24510+arrowlength
set arrow from 14208, 26941+arrowlength to 14208, 26941
set label "4.3.0" at 14208, 26941+arrowlength
set arrow from 14648, 24891+arrowlength to 14648, 24891
set label "4.4.0" at 14648, 24891+arrowlength
set arrow from 15246, 24436+arrowlength to 15246, 24436
set label "4.5.0" at 15246, 24436+arrowlength
set arrow from 16640, 22949+arrowlength to 16640, 22949
set label "4.6.0" at 16640, 22949+arrowlength

