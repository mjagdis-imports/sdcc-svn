# Configure for SDCC target
set output "dhrystone-r3ka-score.svg"
datafile = "dhrystone-r3ka-scoretable"

set title "Dhrystone on RCM3319 board (Rabbit 3000A at 44.24 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 100

sdcc_4_1_0_revision = 12085
sdcc_4_1_0_score = 4107
sdcc_4_2_0_revision = 13131
sdcc_4_2_0_score = 4925
sdcc_4_3_0_revision = 14208
sdcc_4_3_0_score = 4995
sdcc_4_4_0_revision = 14648
sdcc_4_4_0_score = 5542
sdcc_4_5_0_revision = 15246
sdcc_4_5_0_score = 5327
sdcc_4_6_0_revision = 16640
sdcc_4_6_0_score = 5645

set arrow from sdcc_4_1_0_revision, sdcc_4_1_0_score+arrowlength to sdcc_4_1_0_revision, sdcc_4_1_0_score
set label "4.1.0" at sdcc_4_1_0_revision, sdcc_4_1_0_score+arrowlength
set arrow from sdcc_4_2_0_revision, sdcc_4_2_0_score+arrowlength to sdcc_4_2_0_revision, sdcc_4_2_0_score
set label "4.2.0" at sdcc_4_2_0_revision, sdcc_4_2_0_score+arrowlength
set arrow from sdcc_4_3_0_revision, sdcc_4_3_0_score+arrowlength to sdcc_4_3_0_revision, sdcc_4_3_0_score
set label "4.3.0" at sdcc_4_3_0_revision, sdcc_4_3_0_score+arrowlength
set arrow from sdcc_4_4_0_revision, sdcc_4_4_0_score+arrowlength to sdcc_4_4_0_revision, sdcc_4_4_0_score
set label "4.4.0" at sdcc_4_4_0_revision, sdcc_4_4_0_score+arrowlength
set arrow from sdcc_4_5_0_revision, sdcc_4_5_0_score+arrowlength to sdcc_4_5_0_revision, sdcc_4_5_0_score
set label "4.5.0" at sdcc_4_5_0_revision, sdcc_4_5_0_score+arrowlength
set arrow from sdcc_4_6_0_revision, sdcc_4_6_0_score+arrowlength to sdcc_4_6_0_revision, sdcc_4_6_0_score
set label "4.6.0" at sdcc_4_6_0_revision, sdcc_4_6_0_score+arrowlength

