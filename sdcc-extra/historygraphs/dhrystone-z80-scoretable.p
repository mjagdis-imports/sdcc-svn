# Configure for SDCC target
set output "dhrystone-z80-score.svg"
datafile = "dhrystone-z80-scoretable"

set title "Dhrystone on Z80-MBC2 board (Z80 at 8 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 10

sdcc_3_5_0_revision = 9256
sdcc_3_5_0_score = 476
sdcc_3_6_0_revision = 9618
sdcc_3_6_0_score = 494
sdcc_3_9_0_revision = 11214
sdcc_3_9_0_score = 466
sdcc_4_0_0_revision = 11533
sdcc_4_0_0_score = 467
sdcc_4_1_0_revision = 12085
sdcc_4_1_0_score = 481
sdcc_4_2_0_revision = 13131
sdcc_4_2_0_score = 605
sdcc_4_3_0_revision = 14208
sdcc_4_3_0_score = 608
sdcc_4_4_0_revision = 14648
sdcc_4_4_0_score = 629
sdcc_4_5_0_revision = 15246
sdcc_4_5_0_score = 520
sdcc_4_6_0_revision = 16640
sdcc_4_6_0_score = 533

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


