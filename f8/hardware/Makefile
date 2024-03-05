# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config
SDCC = /home/philipp/sdcc-branches/f8/sdcc/bin/sdcc -I/home/philipp/sdcc-branches/f8/sdcc/device/include -L/home/philipp/sdcc-branches/f8/sdcc/device/lib/build/f8

.PHONY: all tests postsynthtests fullpostsynthtests alltests clean

all: f8-testsystem

clean:
	rm -f f8-testsystem f8-postsynthtestsystem *_nosv.v *_synth.v *_synth.asc *_synth.bin *_synth.json tests/*/*.lst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.v tests/*/f8-fullpostsynthtestsystem tests/*/*.vcd tests/*/*.log

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
SYSTEMSOURCES = system.v $(CPUSOURCES) $(IOSOURCES)
TESTSYSTEMSOURCES = $(SYSTEMSOURCES)
POSTSYNTHTESTSYSTEMSOURCES = postsynthtestsystem.v system.v memory.v ram_synth.v cpu_synth.v io_synth.v

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

f8-postsynthtestsystem: $(POSTSYNTHTESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ postsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-fullpostsynthtestsystem: %/system_synth.v
	cd $*; iverilog -g2009 -Wall -o f8-fullpostsynthtestsystem ../../fullpostsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTLOGS = $(TESTDIRS:=/test.log)
POSTSYNTHTESTLOGS = $(TESTDIRS:=/postsynthtest.log)
FULLPOSTSYNTHTESTLOGS = $(TESTDIRS:=/fullpostsynthtest.log)

cpu_nosv.v: $(CPUSOURCES)
	$(SV2V) --exclude=always --exclude=logic cpu.v > $@

io_nosv.v: $(IOSOURCES)
	$(SV2V) --exclude=always --exclude=logic io.v > $@

ram_nosv.v: ram.v
	$(SV2V) --exclude=always --exclude=logic ram.v > $@

system_nosv.v: $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic system.v > $@

icebreaker_nosv.v: icebreaker.v $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic icebreaker.v > $@

cpu_synth.v: cpu_nosv.v
	$(YOSYS) -p "read_verilog -sv cpu_nosv.v; synth_ice40 -noflatten -top cpu; write_verilog cpu_synth.v" # Without -noflatten we get lots of test failures

io_synth.v: io_nosv.v
	$(YOSYS) -p "read_verilog -sv io_nosv.v; synth_ice40 -noflatten -top iosystem; write_verilog io_synth.v"

ram_synth.v: ram_nosv.v
	$(YOSYS) -p "read_verilog -sv ram_nosv.v; synth_ice40 -noflatten -top ram; write_verilog ram_synth.v"

test.vmem: tests/cblink/test.vmem
	cp tests/cblink/test.vmem $@

icebreaker_synth.json: icebreaker_nosv.v test.vmem
	$(YOSYS) -p "read_verilog -sv icebreaker_nosv.v; synth_ice40 -noflatten -top icebreaker -json icebreaker_synth.json"

icebreaker_synth.asc: icebreaker_synth.json icebreaker.pcf
	nextpnr-ice40 --up5k --json icebreaker_synth.json --pcf icebreaker.pcf --freq 3 --asc $@

icebreaker_synth.bin: icebreaker_synth.asc
	icepack icebreaker_synth.asc $@
	
icebreaker_synth.v: icebreaker_synth.json
	yosys -p 'read_json $^; write_verilog $@'

%/system_synth.v: system_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_synth.v"

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

tests/cblink/test.ihx: tests/cblink/test.c
	cd tests/cblink; $(SDCC) -mf8 test.c --data-loc 0x3f00

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

%/postsynthtest.vcd %/postsynthtest.log &: f8-postsynthtestsystem %/test.vmem
	cd $*; ../../f8-postsynthtestsystem > postsynthtest.log

%/fullpostsynthtest.vcd %/fullpostsynthtest.log &: %/f8-fullpostsynthtestsystem
	cd $*; ./f8-fullpostsynthtestsystem > fullpostsynthtest.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

postsynthtests: $(POSTSYNTHTESTLOGS)
	for testlog in $(POSTSYNTHTESTLOGS); do \
		echo -n "postsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

fullpostsynthtests: $(FULLPOSTSYNTHTESTLOGS)
	for testlog in $(FULLPOSTSYNTHTESTLOGS); do \
		echo -n "fullpostsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests postsynthtests fullpostsynthtests

