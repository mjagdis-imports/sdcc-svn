include $(srcdir)/ports.mk

# Deleting all files created by building the program
# --------------------------------------------------
clean:
	rm -f *core *[%~] *.[oa] *.rel *.lst *.sym *.asm *.ihx *.dump* *.cdb
	rm -f .[a-z]*~
	rm -f *.dump*
	rm -rf build
	for port in $(ALLPORTS) ; do\
	  if [ -f $$port/Makefile ]; then\
	    $(MAKE) -C $$port PORT=$$port clean ;\
	  fi;\
	done


# Deleting all files created by configuring or building the program
# -----------------------------------------------------------------
distclean: clean
	rm -f Makefile *.dep
	for port in $(ALLPORTS) ; do\
	  if [ -f $$port/Makefile ]; then\
	    $(MAKE) -C $$port distclean ;\
	  fi;\
	done

# Like clean but some files may still exist
# -----------------------------------------
mostlyclean: clean


# Deleting everything that can reconstructed by this Makefile. It deletes
# everything deleted by distclean plus files created by bison, etc.
# -----------------------------------------------------------------------
realclean: distclean
