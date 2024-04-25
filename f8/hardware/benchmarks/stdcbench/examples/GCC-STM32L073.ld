ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 192K
  RAM  (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* highest address of the user mode stack */
_estack = 0x20002000;

SECTIONS
{
    /* for Cortex devices, the beginning of the startup code is stored in the .isr_vector section, which goes to FLASH */
    .isr_vector :
    {
	. = ALIGN(4);
        KEEP(*(.isr_vector))            /* Startup code */
	. = ALIGN(4);
    } >FLASH
    
    /* the program code is stored in the .text section, which goes to Flash */
    .text :
    {
	    . = ALIGN(4);
	    
        *(.text)                   /* normal code */
        *(.text.*)                 /* -ffunction-sections code */
        *(.rodata)                 /* read-only data (constants) */
        *(.rodata*)                /* -fdata-sections read only data */
        *(.glue_7)                 /* TBD - needed ? */
        *(.glue_7t)                /* TBD - needed ? */

	/* Necessary KEEP sections (see http://sourceware.org/ml/newlib/2005/msg00255.html) */
	KEEP (*(.init))
	KEEP (*(.fini))
	
	    . = ALIGN(4);
        _etext = .;
	    /* This is used by the startup in order to initialize the .data section */
        _sidata = _etext;
    } >FLASH
    
    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to RAM. */
    .data  : AT ( _sidata )
    {
	    . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data secion */
        _sdata = . ;
        _data = . ;
        
        *(.data)
        *(.data.*)
        *(.RAMtext)

	    . = ALIGN(4);
	    /* This is used by the startup in order to initialize the .data secion */
   	    _edata = . ;
    } >RAM
    
    /* This is the uninitialized data section */
    .bss :
    {
	    . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        _sbss = .;
        _bss = .;
        
        *(.bss)
        *(.bss.*) /* patched by elias - allows the use of -fdata-sections */
        *(COMMON)
        
	    . = ALIGN(4);
	    /* This is used by the startup in order to initialize the .bss secion */
   	 _ebss = . ;
    } >RAM
    
    PROVIDE ( end = _ebss );
    PROVIDE ( _end = _ebss );
    
    __exidx_start = .;
    __exidx_end = .;
    
    /* after that it's only debugging information. */
    
    /* remove the debugging information from the standard libraries */
    /DISCARD/ :
    {
     libc.a ( * )
     libm.a ( * )
     libgcc.a ( * )
     }
}

