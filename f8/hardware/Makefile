# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all tests simpletests postsynthcputests alltests clean

all: f8-testsystem f8-simpletestsystem

clean:
	rm -f f8-testsystem f8-simpletestsystem f8-postsynthcputestsystem cpu_flat.v cpu_synth.v tests/*/*.lst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.vcd tests/*/*.log

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
IOSOURCES = gpio.v interruptcontroller.v timer.v watchdog.v
TESTSYSTEMSOURCES = testsystem.v memory.v $(CPUSOURCES) $(IOSOURCES)
SIMPLETESTSYSTEMSOURCES = simpletestsystem.v memory.v $(CPUSOURCES)
POSTSYNTHCPUTESTSYSTEMSOURCES = postsynthcputestsystem.v memory.v cpu_synth.v $(IOSOURCES)

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

f8-simpletestsystem: $(SIMPLETESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ simpletestsystem.v

f8-postsynthcputestsystem: $(POSTSYNTHCPUTESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ postsynthcputestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`
	

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTSYSTEMVCDS = $(TESTDIRS:=/testsystem.vcd)
SIMPLETESTSYSTEMVCDS = $(TESTDIRS:=/simpletestsystem.vcd)
TESTLOGS = $(TESTDIRS:=/test.log)
SIMPLETESTLOGS = $(TESTDIRS:=/simpletest.log)
POSTSYNTHCPUTESTLOGS = $(TESTDIRS:=/postsynthcputest.log)

cpu_flat.v: $(CPUSOURCES)
	$(SV2V) --exclude=always --exclude=logic cpu.v > $@

cpu_synth.v: cpu_flat.v
	$(YOSYS) -p "read_verilog -sv cpu_flat.v; synth_ice40 -noflatten -top cpu; write_verilog cpu_synth.v" # Without -noflatten we get lots of test failures

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

%/postsynthcputest.vcd %/postsynthcputest.log &: f8-postsynthcputestsystem %/test.vmem
	cd $*; ../../f8-postsynthcputestsystem > postsynthcputest.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

simpletests: $(SIMPLETESTLOGS)
	for simpletestlog in $(SIMPLETESTLOGS); do \
		echo -n "simpletestsystem": $$simpletestlog ""; grep ERROR $$simpletestlog | tr -d '\n'; echo ""; \
	done

postsynthcputests: $(POSTSYNTHCPUTESTLOGS)
	for testlog in $(POSTSYNTHCPUTESTLOGS); do \
		echo -n "postsynthcputestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests simpletests postsynthcputests

