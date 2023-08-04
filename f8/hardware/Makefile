SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8

.PHONY: all tests clean

all: f8-testsystem

clean:
	rm -f f8-testsystem tests/*/*.lst tests/*/*.sym tests/*/*.vcd tests/*/*.vmem

f8-testsystem: *.v
	iverilog -g2009 -o $@ testsystem.v

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTSYSTEMVCDS = $(TESTDIRS:=/testsystem.vcd)
TESTLOGS = $(TESTDIRS:=/test.log)

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

%.vmem: %.ihx
	objcopy --input-target ihex --output-target verilog $< $@

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

