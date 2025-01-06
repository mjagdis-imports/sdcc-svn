# Non-free P&P tool for GateMate (to be replaced when nextpnr GateMate support is ready)
P_R = /home/philipp/p_r/p_r
# nextpnr-himbaechel is not yet in Debian, and nextpnr-gowin, which is in Debian doesn't support block RAM.
NEXTPNR_HIMBÄCHEL = /home/philipp/nextpnr/nextpnr-himbaechel

boards/icebreaker_1_nosv.v: boards/icebreaker_1.v $(SYSTEM_1_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@

boards/gatematea1evb_1_nosv.v: boards/gatematea1evb_1.v $(SYSTEM_1_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@

boards/icebreaker_2_nosv.v: boards/icebreaker_2.v $(SYSTEM_2_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@

boards/upduino2_2_nosv.v: boards/upduino2_2.v $(SYSTEM_2_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@

boards/gatematea1evb_2_nosv.v: boards/gatematea1evb_2.v $(SYSTEM_2_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@

boards/tangnano9k_2_nosv.v: boards/tangnano9k_2.v $(SYSTEM_2_SOURCES)
	$(SV2V) --exclude=always --exclude=logic $< > $@


# For yosys, -noflatten makes debugging easier but results in less optimization, and p_r can't handle yosys -noflatten output (and needs -nomx8 here).
%/icebreaker_1_synth.json: boards/icebreaker_1_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/icebreaker_1_nosv.v; synth_ice40 -abc2 -top icebreaker -json icebreaker_1_synth.json"

%/icebreaker_2_synth.json: boards/icebreaker_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/icebreaker_2_nosv.v; synth_ice40 -abc2 -top icebreaker -json icebreaker_2_synth.json"

%/upduino2_2_synth.json: boards/upduino2_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/upduino2_2_nosv.v; synth_ice40 -abc2 -top icebreaker -json upduino2_2_synth.json"

%/gatematea1evb_1_synth.v: boards/gatematea1evb_1_nosv.v %/test.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/gatematea1evb_1_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb_1_synth.v"

%/gatematea1evb_2_synth.v: boards/gatematea1evb_2_nosv.v %/test.even.vmem %/test.odd.vmem
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/gatematea1evb_2_nosv.v; synth_gatemate -nomx8 -top gatematea1evb; write_verilog gatematea1evb_2_synth.v"

%/tangnano9k_2_synth.json: boards/tangnano9k2_nosv.v %/test.even.vmem %/test.odd.vmem # Current nextpnr 0.7 doesn't support bram yet, but with -nobram the design doesn't fit. Also nextpnr from trunk won't work with -noflatten.
	cd $*; $(YOSYS) -p "read_verilog -sv ../../boards/tangnano9k_2_nosv.v; synth_gowin -top tangnano9k -json tangnano9k_2_synth.json"

%/icebreaker_1_synth.asc: %/icebreaker_1_synth.json boards/icebreaker.pcf
	nextpnr-ice40 --up5k --json $*/icebreaker_1_synth.json --pcf boards/icebreaker.pcf --freq 2 --asc $@

%/icebreaker_2_synth.asc: %/icebreaker_2_synth.json boards/icebreaker.pcf
	nextpnr-ice40 --up5k --json $*/icebreaker_2_synth.json --pcf boards/icebreaker.pcf --freq 2 --asc $@

%/upduino2_2_synth.asc: %/upduino2_2_synth.json boards/upduino2.pcf
	nextpnr-ice40 --up5k --json $*/upduino2_2_synth.json --pcf boards/upduino2.pcf --freq 2 --asc $@

%/gatematea1evb_1_synth_00.cfg.bit: %/gatematea1evb_1_synth.v boards/gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb_1_synth.v -o gatematea1evb_1_synth -ccf ../../boards/gatematea1evb.ccf

%/gatematea1evb_2_synth_00.cfg.bit: %/gatematea1evb_2_synth.v boards/gatematea1evb.ccf
	cd $*; $(P_R) -cCP -i gatematea1evb_2_synth.v -o gatematea1evb_2_synth -ccf ../../boards/gatematea1evb.ccf

%/tangnano9k_2.json: %/tangnano9k_2_synth.json boards/tangnano9k.cst
	$(NEXTPNR_HIMBÄCHEL) --json $< --write $@ --device GW1NR-LV9QN88PC6/I5 --vopt family=GW1N-9C --vopt cst=boards/tangnano9k.cst

%/icebreaker_1_synth.bin: %/icebreaker_1_synth.asc
	icepack $< $@

%/icebreaker_2_synth.bin: %/icebreaker_2_synth.asc
	icepack $< $@

%/upduino2_2_synth.bin: %/upduino2_2_synth.asc
	icepack $< $@

%/tangnano9k_2.fs: %/tangnano9k_2.json
	gowin_pack -d GW1N-9C -o $@ $^

