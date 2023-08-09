SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8

.PHONY: all tests clean

all: f8-testsystem

clean:
	rm -f f8-testsystem f8-simpletestsystem tests/*/*.lst tests/*/*.sym tests/*/*.vcd tests/*/*.vmem

f8-testsystem: *.v
	iverilog -g2009 -o $@ testsystem.v

f8-simpletestsystem: *.v
	iverilog -g2009 -o $@ simpletestsystem.v

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTSYSTEMVCDS = $(TESTDIRS:=/testsystem.vcd)
SIMPLETESTSYSTEMVCDS = $(TESTDIRS:=/simpletestsystem.vcd)
TESTLOGS = $(TESTDIRS:=/test.log)
SIMPLETESTLOGS = $(TESTDIRS:=/simpletest.log)

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

%.vmem: %.ihx
	objcopy --input-target ihex --output-target verilog $< $@

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

%/simpletest.vcd %/simpletest.log &: f8-simpletestsystem %/test.vmem
	cd $*; ../../f8-simpletestsystem > simpletest.log

tests: $(TESTLOGS) $(SIMPLETESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done
	for simpletestlog in $(SIMPLETESTLOGS); do \
		echo -n "simpletestsystem": $$simpletestlog ""; grep ERROR $$simpletestlog | tr -d '\n'; echo ""; \
	done

