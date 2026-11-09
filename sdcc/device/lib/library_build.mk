
#PORT_SOURCES = $(addprefix ../,$(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )
#PORT_OBJECTS = $(patsubst %.c,%.rel,$(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )

#$(addsuffix suffix,names)

LIB_TYPE=debug

PORT_OBJECTS = $(addsuffix .rel, $(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )

all: $(PORTDIR)/$(LIB) $(PORTDIR)/$(PORT_CRT)

$(PORTDIR)/$(PORT_CRT): $(PORT_CRT)
	cp $(PORT_CRT) $(PORTDIR)/$(PORT_CRT)

$(PORTDIR)/$(LIB): $(PORT_OBJECTS) Makefile
	echo lib $(LIB_TYPE)
ifeq ($(LIB_TYPE), SDCCLIB)
	rm -f $@; \
	$(top_builddir)/bin/sdcclib -a $(PORTDIR)/libsdcc.lib $(addsuffix .rel, $(PORT_SDCC) )
	$(top_builddir)/bin/sdcclib -a $(PORTDIR)/libsdccf.lib $(addsuffix .rel, $(PORT_FLOAT) )
	$(top_builddir)/bin/sdcclib -a $(PORTDIR)/libc.lib $(addsuffix .rel, $(PORT_LIBC) )
	$(top_builddir)/bin/sdcclib -a $(PORTDIR)/libm.lib $(addsuffix .rel, $(PORT_LIBM) )
	$(top_builddir)/bin/sdcclib -a $@ $(PORT_OBJECTS)
else
  ifeq ($(LIB_TYPE), AR)
	$(SDAR) -rcSD $(PORTDIR)/libsdcc.lib $(addsuffix .rel, $(PORT_SDCC) )
	$(SDAR) -rcSD $(PORTDIR)/libsdccf.lib $(addsuffix .rel, $(PORT_FLOAT) )
	$(SDAR) -rcSD $(PORTDIR)/libc.lib $(addsuffix .rel, $(PORT_LIBC) )
	$(SDAR) -rcSD $(PORTDIR)/libm.lib $(addsuffix .rel, $(PORT_LIBM) )
	$(SDAR) -rcSD $@ $(PORT_OBJECTS)
  else
    ifeq ($(LIB_TYPE), RANLIB)
	$(SDAR) -rcD $(PORTDIR)/libsdcc.lib $(addsuffix .rel, $(PORT_SDCC) )
	$(SDAR) -rcD $(PORTDIR)/libsdccf.lib $(addsuffix .rel, $(PORT_FLOAT) )
	$(SDAR) -rcD $(PORTDIR)/libc.lib $(addsuffix .rel, $(PORT_LIBC) )
	$(SDAR) -rcD $(PORTDIR)/libm.lib $(addsuffix .rel, $(PORT_LIBM) )
	$(SDAR) -rcD $@ $(PORT_OBJECTS)
    else
	rm -f $@
	for i in $(basename $(PORT_OBJECTS)); do echo $$i >>$@; done
	cp $(PORT_OBJECTS) $(PORTDIR)
    endif
  endif
endif

clean:
	rm -f *.rel *.sym *.lst *~ $(CLEANSPEC) *.dump* *.asm *.lib

distclean: clean
	rm -f Makefile

Makefile: $(srcdir)/Makefile.in
	cd $(top_builddir); ./config.status device/lib/$(PORT)/Makefile

%.rel: %.s
	$(AS) $(ASFLAGS) $@ $<

%.rel: %.c
	$(CC) $(CFLAGS) ${CPPFLAGS} ${EXTRA_CFLAGS} -c $< -o $@

