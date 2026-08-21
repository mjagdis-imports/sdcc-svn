# Configure for SDCC target
set output "dhrystone-mcs51-size.svg"
datafile = "dhrystone-mcs51-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

set arrow from 9618, 12795+arrowlength to 9618, 12795
set label "3.6.0" at 9618, 12795+arrowlength
set arrow from 10233, 13104+arrowlength to 10233, 13104
set label "3.7.0" at 10233, 13104+arrowlength
set arrow from 10582, 12886+arrowlength to 10582, 12886
set label "3.8.0" at 10582, 12886+arrowlength
set arrow from 11214, 13573+arrowlength to 11214, 13573
set label "3.9.0" at 11214, 13573+arrowlength
set arrow from 11533, 13580 to 11533, 13580
set label "4.0.0" at 11533, 13580+arrowlength
set arrow from 12085, 13611+arrowlength to 12085, 13611
set label "4.1.0" at 12085, 13611+arrowlength
set arrow from 13131, 13629+arrowlength to 13131, 13629
set label "4.2.0" at 13131, 13629+arrowlength
set arrow from 14208, 13601+arrowlength to 14208, 13601
set label "4.3.0" at 14208, 13601+arrowlength
set arrow from 14648, 13491+arrowlength to 14648, 13491
set label "4.4.0" at 14648, 13491+arrowlength
set arrow from 15246, 12947+arrowlength to 15246, 12947
set label "4.5.0" at 15246, 12947+arrowlength
set arrow from 16640, 13039+arrowlength to 16640, 13039
set label "4.6.0" at 16640, 13039+arrowlength

