# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all tests postsynthtests alltests clean

all: f8-testsystem

clean:
	rm -f f8-testsystem f8-postsynthtestsystem cpu_flat.v cpu_synth.v io_flat.v io_synth.v ram_flat.v ram_synth.v tests/*/*.lst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.vcd tests/*/*.log

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
TESTSYSTEMSOURCES = testsystem.v memory.v ram.v $(CPUSOURCES) $(IOSOURCES)
POSTSYNTHTESTSYSTEMSOURCES = postsynthtestsystem.v memory.v ram_synth.v cpu_synth.v io_synth.v

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

f8-postsynthtestsystem: $(POSTSYNTHTESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ postsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`
	

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTSYSTEMVCDS = $(TESTDIRS:=/testsystem.vcd)
TESTLOGS = $(TESTDIRS:=/test.log)
POSTSYNTHTESTLOGS = $(TESTDIRS:=/postsynthtest.log)

cpu_flat.v: $(CPUSOURCES)
	$(SV2V) --exclude=always --exclude=logic cpu.v > $@

io_flat.v: $(IOSOURCES)
	$(SV2V) --exclude=always --exclude=logic io.v > $@

ram_flat.v: ram.v
	$(SV2V) --exclude=always --exclude=logic ram.v > $@

cpu_synth.v: cpu_flat.v
	$(YOSYS) -p "read_verilog -sv cpu_flat.v; synth_ice40 -noflatten -top cpu; write_verilog cpu_synth.v" # Without -noflatten we get lots of test failures

io_synth.v: io_flat.v
	$(YOSYS) -p "read_verilog -sv io_flat.v; synth_ice40 -noflatten -top iosystem; write_verilog io_synth.v"

ram_synth.v: ram_flat.v
	$(YOSYS) -p "read_verilog -sv ram_flat.v; synth_ice40 -noflatten -top ram; write_verilog ram_synth.v"

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

%/postsynthtest.vcd %/postsynthtest.log &: f8-postsynthtestsystem %/test.vmem
	cd $*; ../../f8-postsynthtestsystem > postsynthtest.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

postsynthtests: $(POSTSYNTHTESTLOGS)
	for testlog in $(POSTSYNTHTESTLOGS); do \
		echo -n "postsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests postsynthtests

