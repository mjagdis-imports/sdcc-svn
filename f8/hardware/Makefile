# Use installed SDCC. Requires at least 4.5.0.
SDAS = sdasf8
SDLD = sdldf8
SDCC = sdcc -mf8
# If SDCC is not installed, but compiled, and in a known location, use something like
# SDAS = /home/philipp/sdcc-trunk/sdcc/bin/sdasf8
# SDLD = /home/philipp/sdcc-trunk/sdcc/bin/sdldf8
# SDCC = /home/philipp/sdcc-trunk/sdcc/bin/sdcc -mf8 -I/home/philipp/sdcc-trunk/sdcc/device/include -L/home/philipp/sdcc-trunk/sdcc/device/lib/build/f8

# sv2v is not in Debian. Use custom build of sv2v 0.0.12 or later.
SV2V = /home/philipp/sv2v-0.0.12/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

include boards/boards.mk

.PHONY: all tests_1 tests_2 icesynthtests_1 gmsynthtests_1 icesynthtests_2 gmsynthtests_2 alltests clean icebreaker-images gatematea1evb-images sdcctests_2 benchmarks2

all: f8-testsystem_1 f8-testsystem_2

# Images for the iCEBreaker FPGA board.
icebreaker-images: tests/cblink/icebreaker_2_synth.bin tests/chello/icebreaker_2_synth.bin
# Images for the GateMateA1-EVB FPGA board.
gatematea1evb-images: tests/cblink/gatematea1evb_2_synth_00.cfg.bit tests/chello/gatematea1evb_2_synth_00.cfg.bit
# Images for the UPDuino v2.0 board.
upduino2-images: tests/cblink/upduino2_2_synth.bin
# Images for the Tang Nano 9K board.
tangnano9k-images: tests/cblink/tangnano9k_2.fs

clean:
	rm -f f8-testsystem_* *_nosv.v *_synth.v *_synth.asc *_synth.bin *_synth.json tests/*/*.lk tests/*/*.asm tests/*/*.lst tests/*/*.rst tests/*/*.sym tests/*/*.rel tests/*/*.ihx tests/*/*.vmem tests/*/*.v tests/*/f8-icesynthtestsystem_* tests/*/*.vcd tests/*/*.log tests/*/*.bin tests/*/*.map tests/*/*.json tests/*/*.cfg tests/*/*.prn tests/*/*.sdf tests/*/*.cdf tests/*/*.pos tests/*/*.used tests/*/*.pin tests/*/*.cfg.bit tests/*/*_synth* tests/*/*.fs tests/*/*.net tests/*/*.id
	cd sdcctests && $(MAKE) clean
	cd benchmarks && $(MAKE) clean

CPUSOURCES = cpu.v alu.v opcodemap.v opcode.v
CPU2SOURCES = cpu2.v alu2.v opcodemap.v opcode.v
IOSOURCES = io.v gpio.v interruptcontroller.v timer.v watchdog.v
IO2SOURCES = io2.v interruptcontroller.v timer.v watchdog2.v
SYSTEM_1_SOURCES = clkdiv.v system_1.v memory.v rom.v ram.v dualportram.v $(CPUSOURCES) $(IOSOURCES)
SYSTEM_2_SOURCES = clkdiv.v system_2.v memory2.v rom2.v ram2.v dualportram.v $(CPU2SOURCES) $(IO2SOURCES)
TESTSYSTEM_1_SOURCES = testsystem_1.v $(SYSTEM_1_SOURCES)
TESTSYSTEM_2_SOURCES = testsystem_2.v $(SYSTEM_2_SOURCES)
SYNTHTESTSYSTEMSOURCES = system.v memory.v ram_synth.v cpu_synth.v io_synth.v
ICESYNTHTESTSYSTEMSOURCES = icesynthtestsystem.v $(SYNTHTESTSYSTEMSOURCES)

f8-testsystem_1: $(TESTSYSTEM_1_SOURCES)
	iverilog -g2009 -Wall -o $@ testsystem_1.v

f8-testsystem_2: $(TESTSYSTEM_2_SOURCES)
	iverilog -g2009 -Wall -pfileline=1 -o $@ testsystem_2.v

f8-testsystem_2-long: $(TESTSYSTEM_2_SOURCES)
	iverilog -g2009 -Wall -pfileline=1 -Ptestsystem.SIMULATIONTIME=400000 -o $@ testsystem_2.v

%/f8-icesynthtestsystem_1: icesynthtestsystem.v %/system_1_icesynth.v
	cd $*; iverilog -g2009 -Wall -o f8-icesynthtestsystem_1 ../../icesynthtestsystem.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-gmsynthtestsystem_1: gmsynthtestsystem.v %/system_1_gmsynth.v
	cd $*; iverilog -g2009 -Wall -o f8-gmsynthtestsystem_1 ../../gmsynthtestsystem.v `$(YOSYS_CONFIG) --datdir/gatemate/cells_sim.v`

%/f8-icesynthtestsystem_2: icesynthtest2system.v %/system_2_icesynth.v
	cd $*; iverilog -g2009 -Wall -o f8-icesynthtestsystem_2 ../../icesynthtest2system.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/f8-gmsynthtestsystem_2: gmsynthtest2system.v %/system_2_gmsynth.v
	cd $*; iverilog -g2009 -Wall -o f8-gmsynthtestsystem_2 ../../gmsynthtest2system.v `$(YOSYS_CONFIG) --datdir/gatemate/cells_sim.v`

TESTDIRS = ${shell find tests/* -type d -print}
TESTS = $(TESTDIRS:=/test.s)
TESTIHXS = $(TESTDIRS:=/test.ihx)
TESTVMEMS = $(TESTDIRS:=/test.vmem)
TESTLOGS_1 = $(TESTDIRS:=/test_1.log)
TESTLOGS_2 = $(TESTDIRS:=/test_2.log)
ICESYNTHTESTLOGS_1 = $(TESTDIRS:=/icesynthtest_1.log)
GMSYNTHTESTLOGS_1 = $(TESTDIRS:=/gmsynthtest_1.log)
ICESYNTHTESTLOGS_2 = $(TESTDIRS:=/icesynthtest_2.log)
GMSYNTHTESTLOGS_2 = $(TESTDIRS:=/gmsynthtest_2.log)

system_1_nosv.v: $(SYSTEMSOURCES)
	$(SV2V) --exclude=always --exclude=logic system_1.v > $@

system_2_nosv.v: $(SYSTEM2SOURCES)
	$(SV2V) --exclude=always --exclude=logic system_2.v > $@

%/system_1_icesynth.v: system_1_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_1_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_1_icesynth.v"

%/system_1_gmsynth.v: system_1_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_1_nosv.v; synth_gatemate -nomx8 -noflatten -top system; write_verilog system_1_gmsynth.v"

%/system_2_icesynth.v: system_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_2_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_2_icesynth.v"

%/system_2_gmsynth.v: system_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../system_2_nosv.v; synth_gatemate -nomx8 -noflatten -top system; write_verilog system_2_gmsynth.v"

%.rel: %.s
	$(SDAS) -plosgff $@ $<

%.ihx: %.rel
	$(SDLD) -b HOME=0x4000 -i $@ $<

tests/cblink/test.ihx: tests/cblink/test.c
	cd tests/cblink; $(SDCC) test.c --data-loc 0x3c00

tests/chello/test.ihx: tests/chello/test.c
	cd tests/chello; $(SDCC) test.c --data-loc 0x3c00

%.vmem: %.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%.even.vmem: %.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%.odd.vmem: %.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8

%/test_1.vcd %/test_1.log &: f8-testsystem_1 %/test.vmem
	cd $*; ../../f8-testsystem_1 > test_1.log

%/test_2.vcd %/test_2.log &: f8-testsystem_2 %/test.even.vmem %/test.odd.vmem
	cd $*; ../../f8-testsystem_2 > test_2.log

%/icesynthtest_1.vcd %/icesynthtest_1.log &: %/f8-icesynthtestsystem_1
	cd $*; ./f8-icesynthtestsystem_1 > icesynthtest_1.log

%/gmsynthtest_1.vcd %/gmsynthtest_1.log &: %/f8-gmsynthtestsystem_1
	cd $*; ./f8-gmsynthtestsystem_1 > gmsynthtest_1.log

%/icesynthtest_2.vcd %/icesynthtest_2.log &: %/f8-icesynthtestsystem_2
	cd $*; ./f8-icesynthtestsystem_2 > icesynthtest_2.log

%/gmsynthtest_2.vcd %/gmsynthtest_2.log &: %/f8-gmsynthtestsystem_2
	cd $*; ./f8-gmsynthtestsystem_2 > gmsynthtest_2.log

tests_1: $(TESTLOGS_1)
	for testlog in $(TESTLOGS_1); do \
		echo -n "testsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

tests_2: $(TESTLOGS_2)
	for testlog in $(TESTLOGS_2); do \
		echo -n "testsystem2:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done
	
icesynthtests_1: $(ICESYNTHTESTLOGS_1)
	for testlog in $(ICESYNTHTESTLOGS_1); do \
		echo -n "icesynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

gmsynthtests_1: $(GMSYNTHTESTLOGS_1)
	for testlog in $(GMSYNTHTESTLOGS_1); do \
		echo -n "gmsynthtestsystem:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

icesynthtests_2: $(ICESYNTHTESTLOGS_2)
	for testlog in $(ICESYNTHTESTLOGS_2); do \
		echo -n "icesynthtest2system:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

gmsynthtests_2: $(GMSYNTHTESTLOGS_2)
	for testlog in $(GMSYNTHTESTLOGS_2); do \
		echo -n "gmsynthtest2system:" $$testlog ""; grep ERROR $$testlog | tr -d '\n'; echo ""; \
	done

alltests: tests_1 tests_2 icesynthtests_1 gmsynthtests_1 icesynthtests_2 gmsynthtests_2

sdcctests_2: f8-testsystem_2-long
	cd sdcctests; $(MAKE) SDCC="$(SDCC)" tests2

sdccicesynthtests_2: system2_nosv.v icesynthtestsystem.v
	cd sdcctests; $(MAKE) SDCC="$(SDCC)" YOSYS="$(YOSYS)" YOSYS_CONFIG="$(YOSYS_CONFIG)" icesynthtests2

benchmarks_2: icebreaker2_nosv.v icesynthtest2system.v system_2_nosv.v
	cd benchmarks; $(MAKE) SDCC="$(SDCC)" YOSYS="$(YOSYS)" YOSYS_CONFIG="$(YOSYS_CONFIG)"

