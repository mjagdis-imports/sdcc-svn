# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all tests simpletests postsynthtests alltests clean

all: f8-testsystem f8-simpletestsystem

clean:
	rm -f f8-testsystem f8-simpletestsystem f8-postsynthtestsystem cpu_flat.v cpu_synth.v io_flat.v io_synth.v tests/*/*.lst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.vcd tests/*/*.log

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
TESTSYSTEMSOURCES = testsystem.v memory.v $(CPUSOURCES) $(IOSOURCES)
SIMPLETESTSYSTEMSOURCES = simpletestsystem.v memory.v $(CPUSOURCES)
POSTSYNTHTESTSYSTEMSOURCES = postsynthtestsystem.v memory.v cpu_synth.v io_synth.v

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

f8-simpletestsystem: $(SIMPLETESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ simpletestsystem.v

f8-postsynthtestsystem: $(POSTSYNTHTESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ postsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`
	

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTSYSTEMVCDS = $(TESTDIRS:=/testsystem.vcd)
SIMPLETESTSYSTEMVCDS = $(TESTDIRS:=/simpletestsystem.vcd)
TESTLOGS = $(TESTDIRS:=/test.log)
SIMPLETESTLOGS = $(TESTDIRS:=/simpletest.log)
POSTSYNTHTESTLOGS = $(TESTDIRS:=/postsynthtest.log)

cpu_flat.v: $(CPUSOURCES)
	$(SV2V) --exclude=always --exclude=logic cpu.v > $@

io_flat.v: $(IOSOURCES)
	$(SV2V) --exclude=always --exclude=logic io.v > $@

cpu_synth.v: cpu_flat.v
	$(YOSYS) -p "read_verilog -sv cpu_flat.v; synth_ice40 -noflatten -top cpu; write_verilog cpu_synth.v" # Without -noflatten we get lots of test failures

io_synth.v: io_flat.v
	$(YOSYS) -p "read_verilog -sv io_flat.v; synth_ice40 -noflatten -top iosystem; write_verilog io_synth.v" #
	
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

%/postsynthtest.vcd %/postsynthtest.log &: f8-postsynthtestsystem %/test.vmem
	cd $*; ../../f8-postsynthtestsystem > postsynthtest.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

simpletests: $(SIMPLETESTLOGS)
	for simpletestlog in $(SIMPLETESTLOGS); do \
		echo -n "simpletestsystem": $$simpletestlog ""; grep ERROR $$simpletestlog | tr -d '\n'; echo ""; \
	done

postsynthtests: $(POSTSYNTHTESTLOGS)
	for testlog in $(POSTSYNTHTESTLOGS); do \
		echo -n "postsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests simpletests postsynthtests

