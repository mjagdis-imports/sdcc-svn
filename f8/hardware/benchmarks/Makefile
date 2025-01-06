YOSYS = yosys
YOSYS_CONFIG = yosys-config
P_R = /home/philipp/p_r/p_r
SDCC = /home/philipp/sdcc-trunk/sdcc/bin/sdcc -mf8 -I/home/philipp/sdcc-trunk/sdcc/device/include -L/home/philipp/sdcc-trunk/sdcc/device/lib/build/f8
SDCCFLAGS = --opt-code-size --max-allocs-per-node 20000

.PHONY: all whetstone dhrystone clean

all: whetstone/image.ihx dhrystone/image.ihx stdcbench/image.ihx whetstone/icebreaker2_synth.bin dhrystone/icebreaker2_synth.bin stdcbench/icebreaker2_synth.bin whetstone/icesynthtest2.vcd dhrystone/icesynthtest2.vcd

clean:
	cd whetstone; $(MAKE) clean
	cd dhrystone; $(MAKE) clean
	cd stdcbench; $(MAKE) -f examples/Makefile.SDCC-F8 clean
	rm -f */*.ihx */test*.vmem */*_synth.json */*_synth.asc */*_synth.bin */gatematea1evb*.bit

whetstone/image.ihx: whetstone/*.c
	cd whetstone; $(MAKE) SDCC="$(SDCC)" SDCCFLAGS="$(SDCCFLAGS)"

dhrystone/image.ihx: dhrystone/*.c dhrystone/*.h
	cd dhrystone; $(MAKE) SDCC="$(SDCC)" SDCCFLAGS="$(SDCCFLAGS)"

stdcbench/image.ihx: stdcbench/*.c stdcbench/*.h
	cd stdcbench; $(MAKE) -f examples/Makefile.SDCC-F8 CC="$(SDCC)" SDCCFLAGS="$(SDCCFLAGS)"
	cd stdcbench; cp stdcbench.ihx image.ihx

%/test.vmem: %/image.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%/test.even.vmem: %/test.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%/test.odd.vmem: %/test.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8


%/system_2_icesynth.v: ../system_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv $(CURDIR)/../system_2_nosv.v; synth_ice40 -noflatten -top system; write_verilog system_2_icesynth.v"

%/f8-icesynthtest2system: ../icesynthtestsystem_2.v %/system_2_icesynth.v
	cd $*; iverilog -g2009 -Wall -Ptestsystem.SIMULATIONTIME=30000 -o f8-icesynthtestsystem_2 $(CURDIR)/../icesynthtestsystem_2.v `$(YOSYS_CONFIG) --datdir/ice40/cells_sim.v`

%/icesynthtest_2.vcd %/icesynthtest_2.log &: %/f8-icesynthtestsystem_2
	cd $*; ./f8-icesynthtestsystem_2 > icesynthtest_2.log


%/icebreaker_2_synth.json: $(CURDIR)/../boards/icebreaker_2_nosv.v %/test.odd.vmem %/test.even.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv $(CURDIR)/../boards/icebreaker_2_nosv.v; synth_ice40 -top icebreaker -json icebreaker_2_synth.json"

%/icebreaker_2_synth.asc: %/icebreaker_2_synth.json $(CURDIR)/../boards/icebreaker.pcf
	nextpnr-ice40 --up5k --json $< --pcf $(CURDIR)/../boards/icebreaker.pcf --freq 2 --asc $@

%/icebreaker_2_synth.bin: %/icebreaker_2_synth.asc
	icepack $< $@

%/gatematea1evb_2_synth.v: $(CURDIR)/../boards/gatematea1evb_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv $(CURDIR)/../boards/gatematea1evb_2_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb_2_synth.v"

# Needs non-free p_r tool (to be replaced by nextpnr - GateMate support in nextpnr is planned for Q4 2024).
%/gatematea1evb_2_synth_00.cfg.bit: %/gatematea1evb_2_synth.v $(CURDIR)/../boards/gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb_2_synth.v -o gatematea1evb_2_synth -ccf $(CURDIR)/../boards/gatematea1evb.ccf

