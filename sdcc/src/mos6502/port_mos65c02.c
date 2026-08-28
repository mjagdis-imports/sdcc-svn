/*-------------------------------------------------------------------------
  port_mos65c02.c - port specific file for mos65c02

  Copyright (C) 2003, Erik Petrich

  Hacked for the MOS6502:
  Copyright (C) 2020, Steven Hugg  hugg@fasterlight.com
  Copyright (C) 2021-2026, Gabriele Gorla

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2, or (at your option) any
  later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  -------------------------------------------------------------------------*/

#include "common.h"
#include "m6502.h"
#include "peep.h"

static char _m65c02_defaultRules[] =
  {
#include "peeph.rul"
  };

static void
mos65c02_init (void)
{
  m6502_opts.sub = SUB_MOS65C02;
  asm_addTree (&asm_asxxxx_mapping);
}

static void
mos65c02_genAssemblerStart (FILE * of)
{
  fprintf(of, "\t.r65c02\n");
  m6502_commonAssemblerStart (of);
}

static const char *const _crt[] = { "crt0.rel", NULL, };
static const char * const _libs_m65c02[] = { "mos65c02", NULL, };

/* Globals */
PORT mos65c02_port =
  {
    TARGET_ID_MOS65C02,
    "mos65c02",
    "WDC 65C02",                /* Target name */
    NULL,                       /* Processor name */
    {
      glue,
      false,                    /* Emit glue around main */
      MODEL_SMALL | MODEL_LARGE,
      MODEL_LARGE,
      m6502_get_model,
    },
    {
      m6502_asmCmd,
      NULL,
      "-plosgffwy",             /* Options with debug */
      "-plosgffw",              /* Options without debug */
      0,
      ".asm",
      NULL                      /* no do_assemble function */
    },
    {                           /* Linker */
      m6502_linkCmd,
      NULL,
      NULL,
      ".rel",                   /* object file extension */
      1,                        /* need linker script */
      _crt,                     /* crt */
      _libs_m65c02,             /* libs */
    },
    {                           /* Peephole optimizer */
      _m65c02_defaultRules,
      m6502_getInstructionSize,
      NULL,
      NULL,
      NULL,
      m6502_notUsed,
      NULL,
      m6502_notUsedFrom,
      NULL,
      NULL,
      NULL,
    },
    /* Sizes: char, short, int, long, long long, ptr, fptr, gptr, bit, float, max */
    // TODO: banked func ptr
    {
      1,                        /* char */
      2,                        /* short */
      2,                        /* int */
      4,                        /* long */
      8,                        /* long long */
      2,                        /* near ptr */
      2,                        /* far ptr */
      2,                        /* generic ptr */
      2,                        /* func ptr */
      0,                        /* banked func ptr */
      1,                        /* bit */
      4,                        /* float */
      64,                       /* bit-precise integer types up to _BitInt (64) */
    },
    /* tags for generic pointers */
    { 0x00, 0x00, 0x00, 0x00 }, /* far, near, xstack, code */
    {
      "XSEG",                   /* xstack_name */
      "STACK",                  /* istack_name */
      "CODE",                   /* code */
      "ZP      (PAG)",          /* data */
      NULL,                     /* idata */
      NULL,                     /* pdata */
      "BSS",                    /* xdata */
      NULL,                     /* xconst_name */
      NULL,                     /* bit */
      "RSEG    (ABS)",          /* reg */
      "GSINIT",                 /* static initialization */
      "OSEG    (PAG, OVR)",     /* overlay */
      "GSFINAL",                /* gsfinal */
      "_CODE",                  /* home */
      "DATA",                   /* initialized xdata */
      "XINIT",                  /* a code copy of DATA */
      "RODATA",                 /* const_name */
      "CABS    (ABS)",          /* cabs_name - const absolute data */
      "DABS    (ABS)",          /* xabs_name - absolute xdata */
      NULL,                     /* iabs_name */
      NULL,                     /* name of segment for initialized variables */
      NULL,                     /* name of segment for copies of initialized variables in code space */
      NULL,                     /* default location for auto vars */
      NULL,                     /* default location for globl vars */
      1,                        /* CODE  is read-only */
      false,                    /* doesn't matter, as port has no __sfr anyway */
      1                         /* No fancy alignments supported. */
    },
    { NULL, NULL },             /* No extra areas */
    0,                          /* default ABI revision */
    {                           /* stack information */
      -1,                       /* stack grows down */
      0,                        /* bank_overhead (switch between register banks) */
      6,                        /* isr overhead */
      2,                        /* call overhead */
      0,                        /* reent overhead */
      0,                        /* banked overhead (switch between code banks) */
      1                         /* sp is offset by 1 from last item pushed */
    },
    {
      -1,                       /* shifts never use support routines */
      false,                    /* do not use support routine for int x int -> long multiplication */
      false,                    /* do not use support routine for unsigned long x unsigned char -> unsigned long long multiplication */
    },
    {
      m6502_emitDebuggerSymbol,
      {
	m6502_dwarfRegNum,
	NULL,
	NULL,
	4,                          /* addressSize */
	14,                         /* regNumRet */
	15,                         /* regNumSP */
	-1,                         /* regNumBP */
	1,                          /* offsetSP */
      },
    },
    {
      256,                      /* maxCount */
      2,                        /* sizeofElement */
      {8,16,32},                /* sizeofMatchJump[] */
      {8,16,32},                /* sizeofRangeCompare[] */
      5,                        /* sizeofSubtract */
      10,                       /* sizeofDispatch */
    },
    "_",
    mos65c02_init,
    m6502_parseOptions,
    m6502_options,
    NULL,
    m6502_finaliseOptions,
    m6502_setDefaultOptions,
    m6502_assignRegisters,
    m6502_getRegName,
    0,
    NULL,
    m6502_keywords,
    mos65c02_genAssemblerStart, /* genAssemblerStart */
    m6502_genAssemblerEnd,      /* genAssemblerEnd */
    m6502_genIVT,               /* local IVT generation code */
    m6502_genXINIT,             /* genXINIT code */
    NULL,                       /* genInitStartup */
    m6502_reset_regparm,
    m6502_regparm,
    m6502_process_pragma,       /* process_pragma */
    NULL,                       /* getMangledFunctionName */
    m6502_hasNativeMulFor,      /* hasNativeMulFor */
    m6502_hasExtBitOp,          /* hasExtBitOp */
    m6502_oclsExpense,          /* oclsExpense */
    true,                       /* use_dw_for_init */
    true,                       /* little endian */
    0,                          /* leave lt */
    0,                          /* leave gt */
    1,                          /* transform <= to ! > */
    1,                          /* transform >= to ! < */
    1,                          /* transform != to !(a == b) */
    0,                          /* leave == */
    false,                      /* No array initializer support. */
    NULL,                       /* CSE cost estimation */
    m6502_builtins,             /* builtin functions */
    GPOINTER,                   /* treat unqualified pointers as "generic" pointers */
    true,
    false,
    1,                          /* reset labelKey to 1 */
    1,                          /* globals & local statics allowed */
    3,                          /* Number of registers handled in the tree-decomposition-based register allocator in SDCCralloc.hpp */
    PORT_MAGIC
  };

