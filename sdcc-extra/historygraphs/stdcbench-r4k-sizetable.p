# Configure for SDCC target
set output "stdcbench-r4k-size.svg"
datafile = "stdcbench-r4k-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 100

sdcc_4_6_0_revision = 16640
sdcc_4_6_0_size = 23111

set arrow from sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength to sdcc_4_6_0_revision, sdcc_4_6_0_size
set label "4.6.0" at sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength

