# Configure for SDCC target
set output "coremark-stm8-size.svg"
datafile = "coremark-stm8-sizetable"

# Configure arrow length (to make the plot look nice)
arrowlength = 200

sdcc_3_5_0_revision = 9256
sdcc_3_5_0_size = 15866
sdcc_3_6_0_revision = 9618
sdcc_3_6_0_size = 15949
sdcc_3_9_0_revision = 11214
sdcc_3_9_0_size = 14518
sdcc_4_0_0_revision = 11533
sdcc_4_0_0_size = 13829
sdcc_4_1_0_revision = 12085
sdcc_4_1_0_size = 13739
sdcc_4_2_0_revision = 13131
sdcc_4_2_0_size = 12981
sdcc_4_3_0_revision = 14208
sdcc_4_3_0_size = 12469
sdcc_4_4_0_revision = 14648
sdcc_4_4_0_size = 12851
sdcc_4_5_0_revision = 15246
sdcc_4_5_0_size = 1249
sdcc_4_6_0_revision = 16640
sdcc_4_6_0_size = 12428

set arrow from sdcc_3_5_0_revision, sdcc_3_5_0_size+arrowlength to sdcc_3_5_0_revision, sdcc_3_5_0_size
set label "3.5.0" at sdcc_3_5_0_revision, sdcc_3_5_0_size+arrowlength
set arrow from sdcc_3_6_0_revision, sdcc_3_6_0_size+arrowlength to sdcc_3_6_0_revision, sdcc_3_6_0_size
set label "3.6.0" at sdcc_3_6_0_revision, sdcc_3_6_0_size+arrowlength
set arrow from sdcc_3_9_0_revision, sdcc_3_9_0_size+arrowlength to sdcc_3_9_0_revision, sdcc_3_9_0_size
set label "3.9.0" at sdcc_3_9_0_revision, sdcc_3_9_0_size+arrowlength
set arrow from sdcc_4_0_0_revision, sdcc_4_0_0_size+arrowlength to sdcc_4_0_0_revision, sdcc_4_0_0_size
set label "4.0.0" at sdcc_4_0_0_revision, sdcc_4_0_0_size+arrowlength
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

