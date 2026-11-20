# Configure for SDCC target
set output "whetstone-z80-score.svg"
datafile = "whetstone-z80-scoretable"

set title "Whetstone on Z80-MBC2 board (Z80 at 8 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 0.2

set arrow from 9618, 2.740+arrowlength to 9618, 2.740
set label "3.6.0" at 9618, 2.740+arrowlength
set arrow from 11214, 3.225+arrowlength to 11214, 3.225
set label "3.9.0" at 11214, 3.225+arrowlength
set arrow from 11533, 3.236+arrowlength to 11533, 3.236
set label "4.0.0" at 11533, 3.236+arrowlength
set arrow from 12085, 3.264+arrowlength to 12085, 3.264
set label "4.1.0" at 12085, 3.264+arrowlength
set arrow from 13131, 4.501+arrowlength to 13131, 4.501
set label "4.2.0" at 13131, 4.501+arrowlength
set arrow from 14208, 4.899+arrowlength to 14208, 4.899
set label "4.3.0" at 14208, 4.899+arrowlength
set arrow from 14648, 4.98+arrowlength to 14648, 4.98
set label "4.4.0" at 14648, 4.98+arrowlength
set arrow from 15246, 4.753+arrowlength to 15246, 4.753
set label "4.5.0" at 15246, 4.753+arrowlength
set arrow from 16640, 5.529+arrowlength to 16640, 5.529
set label "4.6.0" at 16640, 5.529+arrowlength

