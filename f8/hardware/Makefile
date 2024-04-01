# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config
P_R = /tmp/gatemate/cc-toolchain-linux/bin/p_r/p_r
SDCC = /home/philipp/sdcc-branches/f8/sdcc/bin/sdcc -I/home/philipp/sdcc-branches/f8/sdcc/device/include -L/home/philipp/sdcc-branches/f8/sdcc/device/lib/build/f8

.PHONY: all tests icesynthtests gmsynthtests alltests clean icebreaker-images gatematea1evb-images

all: f8-testsystem

# Images for the iCEBreaker FPGA board.
icebreaker-images: tests/cblink/icebreaker_synth.bin tests/chello/icebreaker_synth.bin
# Images for the GateMateA1-EVB FPGA board.
gatematea1evb-images: tests/cblink/gatematea1evb_synth_00.cfg.bit tests/chello/gatematea1evb_synth_00.cfg.bit

clean:
	rm -f f8-testsystem *_nosv.v *_synth.v *_synth.asc *_synth.bin *_synth.json tests/*/*.lk tests/*/*.asm tests/*/*.lst tests/*/*.rst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.v tests/*/f8-icesynthtestsystem tests/*/*.vcd tests/*/*.log tests/*/*.bin tests/*/*.map tests/*/*.json tests/*/*.cfg tests/*/*.prn tests/*/*.sdf tests/*/*.cdf tests/*/*.pos tests/*/*.used tests/*/*.pin tests/*/*.cfg.bit

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
SYSTEMSOURCES = system.v memory.v ram.v $(CPUSOURCES) $(IOSOURCES)
SYNTHTESTSYSTEMSOURCES = system.v memory.v ram_synth.v cpu_synth.v io_synth.v
ICESYNTHTESTSYSTEMSOURCES = icesynthtestsystem.v $(SYNTHTESTSYSTEMSOURCES)

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

%/f8-icesynthtestsystem: icesynthtestsystem.v %/system_icesynth.v
	cd $*; iverilog -g2009 -Wall -o f8-icesynthtestsystem ../../icesynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-gmsynthtestsystem: gmsynthtestsystem.v %/system_gmsynth.v
	cd $*; iverilog -g2009 -Wall -o f8-gmsynthtestsystem ../../gmsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/gatemate/cells_sim.v`

TESTDIRS = ${shell find tests/* -type d -print}
TESTSS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTLOGS = $(TESTDIRS:=/test.log)
ICESYNTHTESTLOGS = $(TESTDIRS:=/icesynthtest.log)
GMSYNTHTESTLOGS = $(TESTDIRS:=/gmsynthtest.log)

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

gatematea1evb_nosv.v: gatematea1evb.v $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic gatematea1evb.v > $@

cpu_synth.v: cpu_nosv.v
	$(YOSYS) -p "read_verilog -sv cpu_nosv.v; synth_ice40 -noflatten -top cpu; write_verilog cpu_synth.v" # -noflatten makes debugging easier.

io_synth.v: io_nosv.v
	$(YOSYS) -p "read_verilog -sv io_nosv.v; synth_ice40 -noflatten -top iosystem; write_verilog io_synth.v"

ram_synth.v: ram_nosv.v
	$(YOSYS) -p "read_verilog -sv ram_nosv.v; synth_ice40 -noflatten -top ram; write_verilog ram_synth.v"

%/icebreaker_synth.json: icebreaker_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../icebreaker_nosv.v; synth_ice40 -noflatten -top icebreaker -json icebreaker_synth.json"

%/gatematea1evb_synth.v: gatematea1evb_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../gatematea1evb_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb_synth.v" # But p_r can't handle yosys -noflatten output, and needs -nomx8 here.

%/icebreaker_synth.asc: %/icebreaker_synth.json icebreaker.pcf
	nextpnr-ice40 --up5k --json $*/icebreaker_synth.json --pcf icebreaker.pcf --freq 3 --asc $@

# Needs non-free p_r tool (to be replaced by nextpnr - GateMate support in nextpnr is planned for Q4 2024).
%/gatematea1evb_synth_00.cfg.bit: %/gatematea1evb_synth.v gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb_synth.v -o gatematea1evb_synth -ccf ../../gatematea1evb.ccf

%/icebreaker_synth.bin: %/icebreaker_synth.asc
	icepack $< $@
	
icebreaker_synth.v: icebreaker_synth.json
	$(YOSYS) -p 'read_json $^; write_verilog $@'

%/system_icesynth.v: system_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_icesynth.v"

%/system_gmsynth.v: system_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_nosv.v; synth_gatemate -nomx8 -noflatten -top system; write_verilog system_gmsynth.v"

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

tests/cblink/test.ihx: tests/cblink/test.c
	cd tests/cblink; $(SDCC) -mf8 test.c --data-loc 0x3e00

tests/chello/test.ihx: tests/chello/test.c
	cd tests/chello; $(SDCC) -mf8 test.c --data-loc 0x3e00

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

%/icesynthtest.vcd %/icesynthtest.log &: %/f8-icesynthtestsystem
	cd $*; ./f8-icesynthtestsystem > icesynthtest.log

%/gmsynthtest.vcd %/gmsynthtest.log &: %/f8-gmsynthtestsystem
	cd $*; ./f8-gmsynthtestsystem > gmsynthtest.log
	
tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

icesynthtests: $(ICESYNTHTESTLOGS)
	for testlog in $(ICESYNTHTESTLOGS); do \
		echo -n "icesynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

gmsynthtests: $(GMSYNTHTESTLOGS)
	for testlog in $(GMSYNTHTESTLOGS); do \
		echo -n "gmsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests icesynthtests gmsynthtests

