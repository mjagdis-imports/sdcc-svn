# Requires sdcc regression tests to have been run once before (e.g. via "make test-f8" in sdcc/support/regression).

.PHONY: all clean

SDCCREGTESTPATH = ../../../sdcc/support/regression
TESTSOURCES = $(wildcard ../../../sdcc/support/regression/cases/tst_*.c) $(wildcard ../../../sdcc/support/regression/cases/*/*.c)
TESTCASES = $(TESTSOURCES:../../../sdcc/support/regression/cases/%.c=%)
TEST2LOGS = $(TESTCASES:%=gen/%/test2.log)

all: $(TEST2LOGS)

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
	$(SDCC) --data-loc 0x2000 $^ -o $@

