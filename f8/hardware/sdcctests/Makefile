# Requires sdcc regression tests to have been run once before (e.g. via "make test-f8" in sdcc/support/regression).

.PHONY: all clean

SDCCREGTESTPATH = ../../../sdcc/support/regression
TESTSOURCES = $(wildcard ../../../sdcc/support/regression/tests/*.c)
TESTCASES = $(TESTSOURCES:../../../sdcc/support/regression/tests/%.c=%)
TEST2LOGS = $(TESTCASES:%=tst_%/test2.log)

all: $(TEST2LOGS)

clean:
	rm -rf tst_* testfwk.rel

%/test2.vcd %/test2.log &: ../f8-testsystem2 %/test.even.vmem %/test.odd.vmem
	cd $*; ../../f8-testsystem2 > test2.log

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%.even.vmem: %.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%.odd.vmem: %.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8

tst_%/test.rel: $(SDCCREGTESTPATH)/cases/tst_%.c $(SDCCREGTESTPATH)/fwk/include/testfwk.h
	rm -rf tst_$*; mkdir tst_$*; $(SDCC) -I$(SDCCREGTESTPATH)/fwk/include -c $< -o $@

FWKLIB = testfwk.rel statics.rel extern1.rel extern2.rel

testfwk.rel: testfwk.c $(SDCCREGTESTPATH)/fwk/include/testfwk.h
	$(SDCC) -I$(SDCCREGTESTPATH)/fwk/include -c $< -o $@

statics.rel: $(SDCCREGTESTPATH)/fwk/lib/statics.c
	$(SDCC) -c $< -o $@

extern1.rel: $(SDCCREGTESTPATH)/fwk/lib/extern1.c
	$(SDCC) -c $< -o $@

extern2.rel: $(SDCCREGTESTPATH)/fwk/lib/extern2.c
	$(SDCC) -c $< -o $@

%.ihx: %.rel $(FWKLIB)
	$(SDCC) --data-loc 0x2000 $^ -o $@

