# Configure for SDCC target
set output "dhrystone-r4k-size.svg"
datafile = "dhrystone-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

sdcc_4_6_0_revision = 16640
sdcc_4_6_0_size = 7544

set arrow from sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength to sdcc_4_6_0_revision, sdcc_4_6_0_size
set label "4.6.0" at sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength

# Arrows for SDCC releases
set arrow from 16640, 8196+arrowlength to 16640, 8196
set label "4.6.0" at 16640, 8196+arrowlength

