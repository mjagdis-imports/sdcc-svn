# Configure for SDCC target
set output "dhrystone-stm8-score.svg"
datafile = "dhrystone-stm8-scoretable"

# Configure arrow length (to make the plot look nice)
arrowlength = 500

sdcc_3_5_0_revision = 9256
sdcc_3_5_0_score = 4225
sdcc_3_6_0_revision = 9618
sdcc_3_6_0_score = 4538

set arrow from sdcc_3_5_0_revision, sdcc_3_5_0_score+arrowlength to sdcc_3_5_0_revision, sdcc_3_5_0_score
set label "3.5.0" at sdcc_3_5_0_revision, sdcc_3_5_0_score+arrowlength
set arrow from sdcc_3_6_0_revision, sdcc_3_6_0_score+arrowlength to sdcc_3_6_0_revision, sdcc_3_6_0_score
set label "3.6.0" at sdcc_3_6_0_revision, sdcc_3_6_0_score+arrowlength
set arrow from 10233, 10233+arrowlength to 10233, 10233
set label "3.7.0" at 10233, 10233+arrowlength
set arrow from 10582, 10817+arrowlength to 10582, 10817
set label "3.8.0" at 10582, 10817+arrowlength
set arrow from 11214, 10841+arrowlength to 11214, 10841
set label "3.9.0" at 11214, 10841+arrowlength
set arrow from 11533, 10841+arrowlength to 11533, 10841
set label "4.0.0" at 11533, 10841+arrowlength
set arrow from 12085, 10836+arrowlength to 12085, 10836
set label "4.1.0" at 12085, 10836+arrowlength
set arrow from 13131, 11013+arrowlength to 13131, 11013
set label "4.2.0" at 13131, 11013+arrowlength
set arrow from 14208, 11116+arrowlength to 14208, 11116
set label "4.3.0" at 14208, 11116+arrowlength
set arrow from 14648, 11574+arrowlength to 14648, 11574
set label "4.4.0" at 14648, 11574+arrowlength
set arrow from 15246, 11536+arrowlength to 15246, 11536
set label "4.5.0" at 15246, 11536+arrowlength
set arrow from 16640, 11611+arrowlength to 16640, 11611
set label "4.6.0" at 16640, 11611+arrowlength

