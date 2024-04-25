.PHONY: all whetstone dhrystone clean

all: whetstone dhrystone

clean:
	cd whetstone; $(MAKE) clean
	cd dhrystone; $(MAKE) clean

whetstone:
	cd whetstone; $(MAKE) SDCC="$(SDCC)" YOSYS="$(YOSYS)" YOSYS_CONFIG="$(YOSYS_CONFIG)"

dhrystone:
	cd dhrystone; $(MAKE) SDCC="$(SDCC)" YOSYS="$(YOSYS)" YOSYS_CONFIG="$(YOSYS_CONFIG)"

