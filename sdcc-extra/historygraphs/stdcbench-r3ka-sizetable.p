# Configure for SDCC target
set output "stdcbench-r3ka-size.svg"
datafile = "stdcbench-r3ka-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 200

sdcc_4_1_0_revision = 12085
sdcc_4_1_0_size = 25367
sdcc_4_2_0_revision = 13131
sdcc_4_2_0_size = 24571
sdcc_4_3_0_revision = 14208
sdcc_4_3_0_size = 26999
sdcc_4_4_0_revision = 14648
sdcc_4_4_0_size = 24948
sdcc_4_5_0_revision = 15246
sdcc_4_5_0_size = 24494
sdcc_4_6_0_revision = 16640
sdcc_4_6_0_size = 22949

set arrow from sdcc_4_1_0_revision, sdcc_4_1_0_size+arrowlength to sdcc_4_1_0_revision, sdcc_4_1_0_size
set label "4.1.0" at sdcc_4_1_0_revision, sdcc_4_1_0_size+arrowlength
set arrow from sdcc_4_2_0_revision, sdcc_4_2_0_size+arrowlength to sdcc_4_2_0_revision, sdcc_4_2_0_size
set label "4.2.0" at sdcc_4_2_0_revision, sdcc_4_2_0_size+arrowlength
set arrow from sdcc_4_3_0_revision, sdcc_4_3_0_size+arrowlength to sdcc_4_3_0_revision, sdcc_4_3_0_size
set label "4.3.0" at sdcc_4_3_0_revision, sdcc_4_3_0_size+arrowlength
set arrow from sdcc_4_4_0_revision, sdcc_4_4_0_size+arrowlength to sdcc_4_4_0_revision, sdcc_4_4_0_size
set label "4.4.0" at sdcc_4_4_0_revision, sdcc_4_4_0_size+arrowlength
set arrow from sdcc_4_5_0_revision, sdcc_4_5_0_size+arrowlength to sdcc_4_5_0_revision, sdcc_4_5_0_size
set label "4.5.0" at sdcc_4_5_0_revision, sdcc_4_5_0_size+arrowlength
set arrow from sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength to sdcc_4_6_0_revision, sdcc_4_6_0_size
set label "4.6.0" at sdcc_4_6_0_revision, sdcc_4_6_0_size+arrowlength

