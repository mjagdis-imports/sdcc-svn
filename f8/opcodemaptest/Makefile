# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.12/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.40 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

all: genopcodemap

clean:
	rm -rf testdata_*

.PHONY: data clean icehead ice2head

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

# The big target. Takes a lot of time and memory if there are many maps. Rule of thumb when using make -j <n> data: System should have (n*3) GB of RAM.
data: $(ICESYNTHSIZES) $(ICESYNTH2SIZES)

# Find the smallest sizes.
icehead: data
	find . -name icesynth.size -exec cat {} \; | sort -n | head

# Find the smallest sizes.
icehead2: data
	find . -name icesynth2.size -exec cat {} \; | sort -n | head

# To generate a large number of random maps, use e.g. for i in {0..10000}; do ./genopcodemap; done
# To generate nearby maps to a given one, use e.g. (for 500 in distances 1 to 5) for i in {0..100}; do for j in {1..5}; do ./genopcodemap -d $j <basemap>; done; done
# Find which map has a given size: grep -r --include "*.size" 4622 .

