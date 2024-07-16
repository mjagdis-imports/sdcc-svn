# sv2v is not in Debian. Use custom build of sv2v 0.0.11
SV2V = /home/philipp/sv2v-0.0.12/bin/sv2v
# Debian has yosys 0.33, which is too old. A custom build of yosys 0.40 has been installed to /usr/local.
YOSYS = yosys
YOSYS_CONFIG = yosys-config

all: opcodemaptool

clean:
	rm -rf testdata_* opcodemaps opcodemapstable

.PHONY: olddata newdata clean oldicehead oldice2head

# new tool: opcodemaptool

opcodemaptool: opcodemaptool.cc
	g++ -Wall -pedantic $< -o $@ -lboost_serialization

NEWOPCODEMAPS = $(wildcard opcodemaps/*/opcodemap.v)
NEWCASES = $(NEWOPCODEMAPS:opcodemaps/%/opcodemap.v=opcodemaps/%)
NEWICESYNTHSIZES = $(NEWCASES:opcodemaps/%=opcodemaps/%/icesynth.size)
NEWICESYNTH2SIZES = $(NEWCASES:opcodemaps/%=opcodemaps/%/icesynth2.size)

# The big target. Takes a lot of time and memory if there are many maps. Rule of thumb when using make -j <n> data: System should have (n*3) GB of RAM.
newdata: $(NEWICESYNTHSIZES) $(NEWICESYNTH2SIZES)

# old tool: genopcodemap
	
genopcodemap: genopcodemap.c
	gcc --std=c2x -Wall -pedantic $< -o $@

OLDOPCODEMAPS = $(wildcard testdata_*/*/*/*/opcodemap.v)
OLDCASES = $(OLDOPCODEMAPS:testdata_%/opcodemap.v=testdata_%)
OLDICESYNTHSIZES = $(OLDCASES:testdata_%=testdata_%/icesynth.size)
OLDICESYNTH2SIZES = $(OLDCASES:testdata_%=testdata_%/icesynth2.size)

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
olddata: $(OLDICESYNTHSIZES) $(OLDICESYNTH2SIZES)

# Find the smallest sizes.
oldicehead: olddata
	find . -name icesynth.size -exec cat {} \; | sort -n | head

# Find the smallest sizes.
oldicehead2: olddata
	find . -name icesynth2.size -exec cat {} \; | sort -n | head

# To generate a large number of random maps, use e.g. for i in {0..10000}; do ./genopcodemap; done
# To generate nearby maps to a given one, use e.g. (for 500 in distances 1 to 5) for i in {0..100}; do for j in {1..5}; do ./genopcodemap -d $j <basemap>; done; done
# Find which map has a given size: grep -r --include "*.size" 4622 .

