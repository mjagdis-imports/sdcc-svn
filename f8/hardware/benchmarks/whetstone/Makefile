SDCC = /home/philipp/sdcc-branches/f8/sdcc/bin/sdcc -mf8 -I/home/philipp/sdcc-branches/f8/sdcc/device/include -L/home/philipp/sdcc-branches/f8/sdcc/device/lib/build/f8
YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all clean

all: image.ihx icebreaker2_synth.bin

clean:
	rm -f *.rel *.map *.lst *.noi *.lk *.lnk *.sym *.asm *.ihx *.o *.rst test*.vmem icebreaker2_synth.json icebreaker2_synth.asc icebreaker2_synth.bin

image.ihx: whetstone.rel portme.rel
	$(SDCC) $(SDCCFLAGS) *.rel -o image.ihx

%.rel: %.c
	$(SDCC) --fverbose-asm -c -DNOSTRUCTASSIGN $(SDCCFLAGS) $<

test.vmem: image.ihx
	objcopy --change-addresses -0x4000 --input-target ihex --output-target verilog image.ihx test.vmem

test.even.vmem: test.vmem
	srec_cat test.vmem -Vmem --split 2 0 -o test.even.vmem -Vmem 8

test.odd.vmem: test.vmem
	srec_cat test.vmem -Vmem --split 2 1 -o test.odd.vmem -Vmem 8

icebreaker2_synth.json: $(CURDIR)/../../icebreaker2_nosv.v test.odd.vmem test.even.vmem
	$(YOSYS) -p "read_verilog -sv $(CURDIR)/../../icebreaker2_nosv.v; synth_ice40 -top icebreaker -json icebreaker2_synth.json"

icebreaker2_synth.asc: icebreaker2_synth.json $(CURDIR)/../../icebreaker.pcf
	nextpnr-ice40 --up5k --json icebreaker2_synth.json --pcf $(CURDIR)/../../icebreaker.pcf --freq 2 --asc $@

icebreaker2_synth.bin: icebreaker2_synth.asc
	icepack $< $@

