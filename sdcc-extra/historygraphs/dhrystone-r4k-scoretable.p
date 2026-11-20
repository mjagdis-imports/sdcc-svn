# Configure for SDCC target
set output "dhrystone-r4k-score.svg"
datafile = "dhrystone-r4k-scoretable"

set title "Dhrystone on RCM4110 board (Rabbit 4000 at 29.49 MHz)"
show title

# Configure arrow length (to make the plot look nice)
arrowlength = 50

sdcc_4_6_0_revision = 16640
sdcc_4_6_0_score = 5341

set arrow from sdcc_4_6_0_revision, sdcc_4_6_0_score+arrowlength to sdcc_4_6_0_revision, sdcc_4_6_0_score
set label "4.6.0" at sdcc_4_6_0_revision, sdcc_4_6_0_score+arrowlength

