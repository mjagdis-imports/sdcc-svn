# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.11/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.38 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

all: genopcodemap

.PHONY: data clean

genopcodemap: genopcodemap.c
	gcc --std=c2x -Wall -pedantic $< -o $@

OPCODEMAPS = $(wildcard testdata_*/*/*/*/opcodemap.v)
CASES = $(OPCODEMAPS:testdata_%/opcodemap.v=testdata_%)
ICESYNTHSIZES = $(CASES:testdata_%=testdata_%/icesynth.size)
ICESYNTH2SIZES = $(CASES:testdata_%=testdata_%/icesynth2.size)

%/cpu_nosv.v: ../hardware/cpu.v ../hardware/alu.v ../hardware/opcode.v
	cd $*; cp $(CURDIR)/../hardware/cpu.v ./; cp $(CURDIR)/../hardware/alu.v ./; cp $(CURDIR)/../hardware/opcode.v ./; $(SV2V) --exclude=always --exclude=logic cpu.v > cpu_nosv.v

%/cpu2_nosv.v: ../hardware/cpu2.v ../hardware/alu2.v ../hardware/opcode.v
	cd $*; cp $(CURDIR)/../hardware/cpu2.v ./; cp $(CURDIR)/../hardware/alu2.v ./; cp $(CURDIR)/../hardware/opcode.v ./; $(SV2V) --exclude=always --exclude=logic cpu2.v > cpu2_nosv.v

%/icesynth.yosyslog: %/cpu_nosv.v %/opcodemap.v
	cd $*; $(YOSYS) -l icesynth.yosyslog -p "read_verilog -sv cpu_nosv.v; synth_ice40 -top cpu -json icesynth.json"

%/icesynth2.yosyslog: %/cpu2_nosv.v %/opcodemap.v
	cd $*; $(YOSYS) -l icesynth2.yosyslog -p "read_verilog -sv cpu2_nosv.v; synth_ice40 -top cpu -json icesynth2.json"

%/icesynth.size: %/icesynth.yosyslog
	grep "Number of cells:" $< | sed -e 's/.*: *//' > $@

%/icesynth2.size: %/icesynth2.yosyslog
	grep "Number of cells:" $< | sed -e 's/.*: *//' > $@

data: $(ICESYNTHSIZES) $(ICESYNTH2SIZES)

