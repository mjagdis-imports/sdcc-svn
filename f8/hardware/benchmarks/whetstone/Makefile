SDCC = /home/philipp/sdcc-branches/f8/sdcc/bin/sdcc -mf8 -I/home/philipp/sdcc-branches/f8/sdcc/device/include -L/home/philipp/sdcc-branches/f8/sdcc/device/lib/build/f8
YOSYS = yosys
YOSYS_CONFIG = yosys-config

.PHONY: all clean

all: image.ihx

clean:
	rm -f *.rel *.map *.lst *.noi *.lk *.lnk *.sym *.asm *.ihx *.o *.rst

image.ihx: whetstone.rel portme.rel
	$(SDCC) $(SDCCFLAGS) *.rel -o image.ihx

%.rel: %.c
	$(SDCC) --fverbose-asm -c -DNOSTRUCTASSIGN $(SDCCFLAGS) $<

