# Use custom f8 assembler and linker (latest from f8 branch)
SDAS = /home/philipp/sdcc-branches/f8/sdcc/bin/sdasf8
SDLD = /home/philipp/sdcc-branches/f8/sdcc/bin/sdldf8
# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config
P_R = /home/philipp/p_r/p_r
SDCC = /home/philipp/sdcc-branches/f8/sdcc/bin/sdcc -mf8 -I/home/philipp/sdcc-branches/f8/sdcc/device/include -L/home/philipp/sdcc-branches/f8/sdcc/device/lib/build/f8

.PHONY: all tests tests2 icesynthtests gmsynthtests icesynthtests2 gmsynthtests2 alltests clean icebreaker-images gatematea1evb-images sdcctests

all: f8-testsystem f8-testsystem2

# Images for the iCEBreaker FPGA board.
icebreaker-images: tests/cblink/icebreaker_synth.bin tests/chello/icebreaker_synth.bin
# Images for the GateMateA1-EVB FPGA board.
gatematea1evb-images: tests/cblink/gatematea1evb_synth_00.cfg.bit tests/chello/gatematea1evb_synth_00.cfg.bit

clean:
	rm -f f8-testsystem f8-testsystem2 *_nosv.v *_synth.v *_synth.asc *_synth.bin *_synth.json tests/*/*.lk tests/*/*.asm tests/*/*.lst tests/*/*.rst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.v tests/*/f8-icesynthtestsystem tests/*/*.vcd tests/*/*.log tests/*/*.bin tests/*/*.map tests/*/*.json tests/*/*.cfg tests/*/*.prn tests/*/*.sdf tests/*/*.cdf tests/*/*.pos tests/*/*.used tests/*/*.pin tests/*/*.cfg.bit tests/*/*_synth.*

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
CPU2SOURCES = cpu2.v alu2.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
IO2SOURCES = io2.v watchdog2.v interruptcontroller.v
SYSTEMSOURCES = clkdiv.v system.v memory.v rom.v ram.v dualportram.v $(CPUSOURCES) $(IOSOURCES)
SYSTEM2SOURCES = clkdiv.v system2.v memory2.v rom2.v ram2.v dualportram.v $(CPU2SOURCES) $(IO2SOURCES)
TESTSYSTEMSOURCES = testsystem.v $(SYSTEMSOURCES)
TESTSYSTEM2SOURCES = testsystem2.v $(SYSTEM2SOURCES)
SYNTHTESTSYSTEMSOURCES = system.v memory.v ram_synth.v cpu_synth.v io_synth.v
ICESYNTHTESTSYSTEMSOURCES = icesynthtestsystem.v $(SYNTHTESTSYSTEMSOURCES)

f8-testsystem: $(TESTSYSTEMSOURCES)
	iverilog -g2009 -Wall -o $@ testsystem.v

f8-testsystem2: $(TESTSYSTEM2SOURCES)
	iverilog -g2009 -Wall -pfileline=1 -o $@ testsystem2.v

f8-testsystem2-long: $(TESTSYSTEM2SOURCES)
	iverilog -g2009 -Wall -pfileline=1 -Ptestsystem.SIMULATIONTIME=400000 -o $@ testsystem2.v

%/f8-icesynthtestsystem: icesynthtestsystem.v %/system_icesynth.v
	cd $*; iverilog -g2009 -Wall -o f8-icesynthtestsystem ../../icesynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-gmsynthtestsystem: gmsynthtestsystem.v %/system_gmsynth.v
	cd $*; iverilog -g2009 -Wall -o f8-gmsynthtestsystem ../../gmsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/gatemate/cells_sim.v`

%/f8-icesynthtest2system: icesynthtest2system.v %/system2_icesynth.v
	cd $*; iverilog -g2009 -Wall -o f8-icesynthtest2system ../../icesynthtest2system.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-gmsynthtest2system: gmsynthtest2system.v %/system2_gmsynth.v
	cd $*; iverilog -g2009 -Wall -o f8-gmsynthtest2system ../../gmsynthtest2system.v `$(YOSYS_CONFIG) --datdir/gatemate/cells_sim.v`

TESTDIRS = ${shell find tests/* -type d -print}
TESTS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTLOGS = $(TESTDIRS:=/test.log)
TESTLOGS2 = $(TESTDIRS:=/test2.log)
ICESYNTHTESTLOGS = $(TESTDIRS:=/icesynthtest.log)
GMSYNTHTESTLOGS = $(TESTDIRS:=/gmsynthtest.log)
ICESYNTHTEST2LOGS = $(TESTDIRS:=/icesynthtest2.log)
GMSYNTHTEST2LOGS = $(TESTDIRS:=/gmsynthtest2.log)

system_nosv.v: $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic system.v > $@

system2_nosv.v: $(SYSTEM2SOURCES)
	$(SV2V) --exclude=always --exclude=logic system2.v > $@

icebreaker_nosv.v: icebreaker.v $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic icebreaker.v > $@

gatematea1evb_nosv.v: gatematea1evb.v $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic gatematea1evb.v > $@

icebreaker2_nosv.v: icebreaker2.v $(SYSTEM2SOURCES)
	$(SV2V) --exclude=always --exclude=logic icebreaker2.v > $@

gatematea1evb2_nosv.v: gatematea1evb2.v $(SYSTEM2SOURCES)
	$(SV2V) --exclude=always --exclude=logic gatematea1evb2.v > $@

# For yosys, -noflatten makes debugging easier but results in less optimization, and p_r can't handle yosys -noflatten output (and needs -nomx8 here).
%/icebreaker_synth.json: icebreaker_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../icebreaker_nosv.v; synth_ice40 -top icebreaker -json icebreaker_synth.json"

%/gatematea1evb_synth.v: gatematea1evb_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../gatematea1evb_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb_synth.v"

%/icebreaker2_synth.json: icebreaker2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../icebreaker2_nosv.v; synth_ice40 -top icebreaker -json icebreaker2_synth.json"

%/gatematea1evb2_synth.v: gatematea1evb2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../gatematea1evb2_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb2_synth.v"
	
%/icebreaker_synth.asc: %/icebreaker_synth.json icebreaker.pcf
	nextpnr-ice40 --up5k --json $*/icebreaker_synth.json --pcf icebreaker.pcf --freq 3 --asc $@

# Needs non-free p_r tool (to be replaced by nextpnr - GateMate support in nextpnr is planned for Q4 2024).
%/gatematea1evb_synth_00.cfg.bit: %/gatematea1evb_synth.v gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb_synth.v -o gatematea1evb_synth -ccf ../../gatematea1evb.ccf

%/icebreaker2_synth.asc: %/icebreaker2_synth.json icebreaker.pcf
	nextpnr-ice40 --up5k --json $*/icebreaker2_synth.json --pcf icebreaker.pcf --freq 3 --asc $@

# Needs non-free p_r tool (to be replaced by nextpnr - GateMate support in nextpnr is planned for Q4 2024).
%/gatematea1evb2_synth_00.cfg.bit: %/gatematea1evb2_synth.v gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb2_synth.v -o gatematea1evb2_synth -ccf ../../gatematea1evb.ccf

%/icebreaker_synth.bin: %/icebreaker_synth.asc
	icepack $< $@

%/icebreaker2_synth.bin: %/icebreaker2_synth.asc
	icepack $< $@
	
icebreaker_synth.v: icebreaker_synth.json
	$(YOSYS) -p 'read_json $^; write_verilog $@'

icebreaker2_synth.v: icebreaker_synth.json
	$(YOSYS) -p 'read_json $^; write_verilog $@'

%/system_icesynth.v: system_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_icesynth.v"

%/system_gmsynth.v: system_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_nosv.v; synth_gatemate -nomx8 -noflatten -top system; write_verilog system_gmsynth.v"

%/system2_icesynth.v: system2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system2_nosv.v; synth_ice40 -noflatten -top system; write_verilog system2_icesynth.v"

%/system2_gmsynth.v: system2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system2_nosv.v; synth_gatemate -nomx8 -noflatten -top system; write_verilog system2_gmsynth.v"

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

tests/cblink/test.ihx: tests/cblink/test.c
	cd tests/cblink; $(SDCC) test.c --data-loc 0x3e00

tests/chello/test.ihx: tests/chello/test.c
	cd tests/chello; $(SDCC) test.c --data-loc 0x3e00

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%.even.vmem: %.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%.odd.vmem: %.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8

%/test.vcd %/test.log &: f8-testsystem %/test.vmem
	cd $*; ../../f8-testsystem > test.log

%/test2.vcd %/test2.log &: f8-testsystem2 %/test.even.vmem %/test.odd.vmem
	cd $*; ../../f8-testsystem2 > test2.log

%/icesynthtest.vcd %/icesynthtest.log &: %/f8-icesynthtestsystem
	cd $*; ./f8-icesynthtestsystem > icesynthtest.log

%/gmsynthtest.vcd %/gmsynthtest.log &: %/f8-gmsynthtestsystem
	cd $*; ./f8-gmsynthtestsystem > gmsynthtest.log

%/icesynthtest2.vcd %/icesynthtest2.log &: %/f8-icesynthtest2system
	cd $*; ./f8-icesynthtest2system > icesynthtest2.log

%/gmsynthtest2.vcd %/gmsynthtest2.log &: %/f8-gmsynthtest2system
	cd $*; ./f8-gmsynthtest2system > gmsynthtest2.log

tests: $(TESTLOGS)
	for testlog in $(TESTLOGS); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

tests2: $(TESTLOGS2)
	for testlog in $(TESTLOGS2); do \
		echo -n "testsystem2:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done
	
icesynthtests: $(ICESYNTHTESTLOGS)
	for testlog in $(ICESYNTHTESTLOGS); do \
		echo -n "icesynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

gmsynthtests: $(GMSYNTHTESTLOGS)
	for testlog in $(GMSYNTHTESTLOGS); do \
		echo -n "gmsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

icesynthtests2: $(ICESYNTHTEST2LOGS)
	for testlog in $(ICESYNTHTEST2LOGS); do \
		echo -n "icesynthtest2system:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

gmsynthtests2: $(GMSYNTHTEST2LOGS)
	for testlog in $(GMSYNTHTEST2LOGS); do \
		echo -n "gmsynthtest2system:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests icesynthtests gmsynthtests tests2 icesynthtests2 gmsynthtests2

sdcctests: f8-testsystem2-long
	cd sdcctests; $(MAKE) SDCC="$(SDCC)"

