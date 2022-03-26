# Regression test specification for the stm8-large target running with uCsim

# simulation timeout in seconds
SIM_TIMEOUT = 60

# path to uCsim
ifdef SDCC_BIN_PATH
  UCSTM8C = $(SDCC_BIN_PATH)/ucsim_stm8$(EXEEXT)

  AS_STM8C = $(SDCC_BIN_PATH)/sdasstm8$(EXEEXT)
else
  ifdef UCSIM_DIR
    UCSTM8A = $(UCSIM_DIR)/stm8.src/ucsim_stm8$(EXEEXT)
  else
    UCSTM8A = $(top_builddir)/sim/ucsim/stm8.src/ucsim_stm8$(EXEEXT)
    UCSTM8B = $(top_builddir)/bin/ucsim_stm8$(EXEEXT)
  endif

  EMU = $(WINE) $(shell if [ -f $(UCSTM8A) ]; then echo $(UCSTM8A); else echo $(UCSTM8B); fi)

  AS = $(WINE) $(top_builddir)/bin/sdasstm8$(EXEEXT)

ifndef CROSSCOMPILING
  SDCCFLAGS += --nostdinc -I$(top_srcdir)
  LINKFLAGS += --nostdlib -L$(top_builddir)/device/lib/build/stm8-large
endif
endif

ifdef CROSSCOMPILING
  SDCCFLAGS += -I$(top_srcdir)
endif

SDCCFLAGS += -mstm8 --model-large --less-pedantic --out-fmt-ihx
LINKFLAGS += stm8.lib

OBJEXT = .rel
BINEXT = .ihx

# otherwise `make` deletes testfwk.rel and `make -j` will fail
.PRECIOUS: $(PORT_CASES_DIR)/%$(OBJEXT)

# Required extras
EXTRAS = $(PORT_CASES_DIR)/testfwk$(OBJEXT) $(PORT_CASES_DIR)/support$(OBJEXT)
include $(srcdir)/fwk/lib/spec.mk

# Rule to link into .ihx
%$(BINEXT): %$(OBJEXT) $(EXTRAS) $(FWKLIB) $(PORT_CASES_DIR)/fwk.lib
	$(SDCC) $(SDCCFLAGS) $(LINKFLAGS) $(EXTRAS) $(PORT_CASES_DIR)/fwk.lib $< -o $@

%$(OBJEXT): %.asm
	$(AS) -plosgff $<

%$(OBJEXT): %.c
	$(SDCC) $(SDCCFLAGS) -c $< -o $@

$(PORT_CASES_DIR)/%$(OBJEXT): $(PORTS_DIR)/$(PORT)/%.c
	$(SDCC) $(SDCCFLAGS) -c $< -o $@

$(PORT_CASES_DIR)/%$(OBJEXT): $(srcdir)/fwk/lib/%.c
	$(SDCC) $(SDCCFLAGS) -c $< -o $@

$(PORT_CASES_DIR)/fwk.lib: $(srcdir)/fwk/lib/fwk.lib
	cat < $(srcdir)/fwk/lib/fwk.lib > $@

_clean:
