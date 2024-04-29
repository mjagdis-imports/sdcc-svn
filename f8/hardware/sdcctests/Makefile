# Requires sdcc regression tests to have been run once before (e.g. via "make test-f8" in sdcc/support/regression).

.PHONY: all tests2 icesynthtests2 clean

SDCCREGTESTPATH = ../../../sdcc/support/regression
TESTSOURCES = $(wildcard ../../../sdcc/support/regression/cases/tst_*.c) $(wildcard ../../../sdcc/support/regression/cases/*/*.c)
TESTCASES = $(TESTSOURCES:../../../sdcc/support/regression/cases/%.c=%)
TEST2LOGS = $(TESTCASES:%=gen/%/test2.log)
ICESYNTHTEST2LOGS = gen/tst_printf_d/icesynthtest2.log

all: tests2

tests2: $(TEST2LOGS)

icesynthtests2: $(ICESYNTHTEST2LOGS)

FWKLIB = testfwk.rel statics.rel extern1.rel extern2.rel

clean:
	rm -rf $(FWKLIB) gen/*

%/test2.vcd %/test2.log &: ../f8-testsystem2-long %/test.even.vmem %/test.odd.vmem
	cd $*; $(CURDIR)/../f8-testsystem2-long > test2.log

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%.even.vmem: %.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%.odd.vmem: %.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8

gen/%/test.rel: $(SDCCREGTESTPATH)/cases/%.c $(SDCCREGTESTPATH)/fwk/include/testfwk.h
	rm -rf gen/$*; mkdir -p gen/$*; $(SDCC) -I$(SDCCREGTESTPATH)/../.. -I$(SDCCREGTESTPATH)/fwk/include -c $< -o $@

testfwk.rel: testfwk.c $(SDCCREGTESTPATH)/fwk/include/testfwk.h
	$(SDCC) -I$(SDCCREGTESTPATH)/fwk/include -c $< -o $@

statics.rel: $(SDCCREGTESTPATH)/fwk/lib/statics.c
	$(SDCC) -c $< -o $@

extern1.rel: $(SDCCREGTESTPATH)/fwk/lib/extern1.c
	$(SDCC) -c $< -o $@

extern2.rel: $(SDCCREGTESTPATH)/fwk/lib/extern2.c
	$(SDCC) -c $< -o $@

%.ihx: $(FWKLIB) %.rel
	$(SDCC) --data-loc 0x2800 $^ -o $@

%/icesynthtest2.vcd %/icesynthtest2.log &: %/f8-icesynthtest2system
	cd $*; ./f8-icesynthtest2system > icesynthtest2.log

%/system2_icesynth.v: ../system2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv $(CURDIR)/../system2_nosv.v; synth_ice40 -noflatten -top system; write_verilog system2_icesynth.v"

%/f8-icesynthtest2system: ../icesynthtest2system.v %/system2_icesynth.v
	cd $*; iverilog -g2009 -Wall -Ptestsystem.SIMULATIONTIME=30000 -o f8-icesynthtest2system $(CURDIR)/../icesynthtest2system.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

