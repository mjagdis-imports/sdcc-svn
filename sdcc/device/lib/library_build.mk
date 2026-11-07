LIB_TYPE	= @LIB_TYPE@

#PORT_SOURCES = $(addprefix ../,$(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )
#PORT_OBJECTS = $(patsubst %.c,%.rel,$(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )

#$(addsuffix suffix,names)

PORT_OBJECTS = $(addsuffix .rel, $(PORT_SDCC) $(PORT_FLOAT) $(PORT_LIBC) $(PORT_LIBM) )

all: $(PORTDIR)/$(LIB) $(PORT_CRT)

$(PORTDIR)/crt0.rel: crt0.rel
	cp crt0.rel $(PORTDIR)/crt0.rel

$(PORTDIR)/$(LIB): $(PORT_OBJECTS) Makefile
ifeq ($(LIB_TYPE), SDCCLIB)
	rm -f $@; \
	$(top_builddir)/bin/sdcclib -a $@ $(PORT_OBJECTS)
else
  ifeq ($(LIB_TYPE), AR)
	$(SDAR) -rcSD $@ $(PORT_OBJECTS)
  else
    ifeq ($(LIB_TYPE), RANLIB)
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

