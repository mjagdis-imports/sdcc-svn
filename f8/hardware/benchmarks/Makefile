YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all whetstone dhrystone clean

all: whetstone/image.ihx dhrystone/image.ihx stdcbench/image.ihx whetstone/icebreaker2_synth.bin dhrystone/icebreaker2_synth.bin stdcbench/icebreaker2_synth.bin

clean:
	cd whetstone; $(MAKE) clean
	cd dhrystone; $(MAKE) clean
	cd stdcbench; $(MAKE) -f examples/Makefile.SDCC-F8 clean
	rm -f */*.ihx */test*.vmem */icebreaker2_synth.json */icebreaker2_synth.asc */icebreaker2_synth.bin

whetstone/image.ihx:
	cd whetstone; $(MAKE) SDCC="$(SDCC)"

dhrystone/image.ihx:
	cd dhrystone; $(MAKE) SDCC="$(SDCC)"

stdcbench/image.ihx:
	cd stdcbench; $(MAKE) -f examples/Makefile.SDCC-F8 CC="$(SDCC)"
	cd stdcbench; cp stdcbench.ihx image.ihx

%/test.vmem: %/image.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog $< $@

%/test.even.vmem: %/test.vmem
	srec_cat $< -Vmem --split 2 0 -o $@ -Vmem 8

%/test.odd.vmem: %/test.vmem
	srec_cat $< -Vmem --split 2 1 -o $@ -Vmem 8

%/icebreaker2_synth.json: $(CURDIR)/../icebreaker2_nosv.v %/test.odd.vmem %/test.even.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv $(CURDIR)/../icebreaker2_nosv.v; synth_ice40 -top icebreaker -json icebreaker2_synth.json"

%/icebreaker2_synth.asc: %/icebreaker2_synth.json $(CURDIR)/../icebreaker.pcf
	nextpnr-ice40 --up5k --json $< --pcf $(CURDIR)/../icebreaker.pcf --freq 2 --asc $@

%/icebreaker2_synth.bin: %/icebreaker2_synth.asc
	icepack $< $@

