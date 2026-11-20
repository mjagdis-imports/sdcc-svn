# Configure for SDCC target
set output "coremark-stm8-score.svg"
datafile = "coremark-stm8-scoretable"

set title "Coremark on STM8AF board (STM8AF at 16 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 0.3

sdcc_3_5_0_revision = 9256
sdcc_3_5_0_score = 2.949
sdcc_3_6_0_revision = 9618
sdcc_3_6_0_score = 2.986
sdcc_3_9_0_revision = 11214
sdcc_3_9_0_score = 5.950
sdcc_4_0_0_revision = 11533
sdcc_4_0_0_score = 6.151
sdcc_4_1_0_revision = 12085
sdcc_4_1_0_score = 6.127
sdcc_4_2_0_revision = 13131
sdcc_4_2_0_score = 6.505
sdcc_4_3_0_revision = 14208
sdcc_4_3_0_score = 6.411
sdcc_4_4_0_revision = 14648
sdcc_4_4_0_score = 6.639
sdcc_4_5_0_revision = 15246
sdcc_4_5_0_score = 6.559
sdcc_4_6_0_revision = 16640
sdcc_4_6_0_score = 6.512

set arrow from sdcc_3_5_0_revision, sdcc_3_5_0_score+arrowlength to sdcc_3_5_0_revision, sdcc_3_5_0_score
set label "3.5.0" at sdcc_3_5_0_revision, sdcc_3_5_0_score+arrowlength
set arrow from sdcc_3_6_0_revision, sdcc_3_6_0_score+arrowlength to sdcc_3_6_0_revision, sdcc_3_6_0_score
set label "3.6.0" at sdcc_3_6_0_revision, sdcc_3_6_0_score+arrowlength
set arrow from sdcc_3_9_0_revision, sdcc_3_9_0_score+arrowlength to sdcc_3_9_0_revision, sdcc_3_9_0_score
set label "3.9.0" at sdcc_3_9_0_revision, sdcc_3_9_0_score+arrowlength
set arrow from sdcc_4_0_0_revision, sdcc_4_0_0_score+arrowlength to sdcc_4_0_0_revision, sdcc_4_0_0_score
set label "4.0.0" at sdcc_4_0_0_revision, sdcc_4_0_0_score+arrowlength
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

