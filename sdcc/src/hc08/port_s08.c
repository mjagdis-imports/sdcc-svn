/*-------------------------------------------------------------------------
  port_s08.c - port specific file for mos6502

  Copyright (C) 2003, Erik Petrich
  Copyright (C) 2026, Gabriele Gorla

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
#include "subport.h"

static const char *const _libs[] = { "libsdcc", "libsdccf", "libc", "libm", NULL };

static void
_init (void)
{
  hc08_opts.sub = SUB_S08;
  asm_addTree (&asm_asxxxx_mapping);
}

/* Globals */
PORT s08_port =
{
  TARGET_ID_S08,
  "s08",
  "S08",                        /* Target name */
  NULL,                         /* Processor name */
    {                           /* general */
    glue,                       /* Pointer to glue function */
    false,                      /* Emit glue around main */
      MODEL_SMALL | MODEL_LARGE,/* OR of MODEL_* */
      MODEL_LARGE,              /* default model */
    NULL,                       /* model == target */
  },
    {                           /* Assembler */
      hc08_asmCmd,              /* Command to run and arguments */
      NULL,                     /* Alternate macro based form */
    "-plosgffwy",               /* Options with debug */
    "-plosgffw",                /* Options without debug */
      0,                        /* print externs as global */
      ".asm",                   /* assembler file extension */
      NULL                      /* If non-null will be used to execute the assembler. */
  },
  {                             /* Linker */
      hc08_linkCmd,             /* Command to run (eg link-z80) */
      NULL,                     /* Alternate macro based form. */
      NULL,                     /* If non-null will be used to execute the link. */
      ".rel",                   /* object file extension */
      1,                        /* need linker script */
    NULL,                       /* crt */
    _libs,                      /* libs */
  },
  {                             /* Peephole optimizer */
      hc08_peepRules,
      hc08_getInstructionSize,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
  },
    /* Sizes: char, short, int, long, long long, near ptr, far ptr, gptr, func ptr, banked func ptr, bit, float, BitInt (in bits) */
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
      64                        /* bit-precise integer types up to _BitInt (64) */
  },
  /* tags for generic pointers */
  { 0x00, 0x00, 0x00, 0x00 },           /* far, near, xstack, code */
  {
      "XSEG",                   /* xstack_name */
      "STACK",                  /* istack_name */
    "CSEG    (CODE)",
    "DSEG    (PAG)",
    NULL, /* "ISEG" */
    NULL, /* "PSEG" */
    "XSEG",
    NULL,                // xconst_name
    NULL, /* "BSEG" */
    "RSEG    (ABS)",
    "GSINIT  (CODE)",
    "OSEG    (PAG, OVR)",
    "GSFINAL (CODE)",
    "HOME    (CODE)",
    "XISEG",              // initialized xdata
    "XINIT   (CODE)",     // a code copy of xiseg
    "CONST   (CODE)",     // const_name - const data (code or not)
    "CABS    (ABS,CODE)", // cabs_name - const absolute data (code or not)
    "XABS    (ABS)",      // xabs_name - absolute xdata
    "IABS    (ABS)",      // iabs_name - absolute data
      NULL,                     /* name of segment for initialized variables */
      NULL,                     /* name of segment for copies of initialized variables in code space */
      NULL,                     /* default location for auto vars */
      NULL,                     /* default location for globl vars */
      1,                        /* CODE  is read-only */
      false,                    /* doesn't matter, as port has no __sfr anyway */
      1                         /* No fancy alignments supported. */
  },
  {
    hc08_genExtraAreas,
    NULL 
  },
    0,                          /* default ABI revision */
    {                           /* stack information */
    -1,         /* direction (-1 = stack grows down) */
    0,          /* bank_overhead (switch between register banks) */
    4,          /* isr overhead */
    2,          /* call overhead */
    0,          /* reentoverhead */
    0,          /* banked overhead (switch between code banks) */
    1           /* sp is offset by 1 from last item pushed */
  },
  {
    5,
    false,                    /* do not use support routine for int x int -> long multiplication */
    false                     /* do not use support routine for unsigned long x unsigned char -> unsigned long long multiplication */
  },
  {
    hc08_emitDebuggerSymbol,
    {
      hc08_dwarfRegNum,
      0,                        /* cfiSame */
      0,                        /* cfiUndef */
      4,                        /* addressSize */
      14,                       /* regNumRet */
      15,                       /* regNumSP */
      -1,                       /* regNumBP */
      1                         /* offsetSP */
    },
  },
  {
    256,        /* maxCount */
    2,          /* sizeofElement */
    {8,16,32},  /* sizeofMatchJump[] */
    {8,16,32},  /* sizeofRangeCompare[] */
    5,          /* sizeofSubtract */
    10          /* sizeofDispatch */
  },
  "_",                        /* Prefix to add to a C function (eg "_") */
  _init,                      /* First chance to initialise and set any port specific variables */
  hc08_parseOptions,            /* Optional list of automatically parsed options */
  hc08_options,                 /* Parses one option + its arguments */
  NULL,                         /* Initialise port spectific paths */
  hc08_finaliseOptions,         /* Called after all the options have been parsed */
  hc08_setDefaultOptions,       /* Called before any options are parsed. */
  hc08_assignRegisters,
  hc08_getRegName,              /* Returns the register name of a symbol */
  NULL,                         /* getRegByName */
  NULL,                         /* rtrackUpdate - Try to keep track of register contents. */
  hc08_keywords,                /* list of keywords that are used by this target */
  hc08_genAssemblerStart,       /* genAssemblerStart */
  hc08_genAssemblerEnd,         /* genAssemblerEnd */
  hc08_genIVT,                  /* local IVT generation code */
  hc08_genXINIT,                /* genXINIT code */
  NULL,                         /* genInitStartup */
  hc08_reset_regparm,           /* reset the register count */
  hc08_regparm,                 /* will return 1 if can be passed in register */
  NULL,                         /* process_pragma */
  NULL,                         /* getMangledFunctionName */
  hc08_hasNativeMulFor,         /* hasNativeMulFor */
  hc08_hasExtBitOp,             /* hasExtBitOp */
  hc08_oclsExpense,             /* oclsExpense */
  true,                         /* use_dw_for_init */
  false,                        /* little endian */
  0,                            /* leave lt */
  0,                            /* leave gt */
  1,                            /* transform <= to ! > */
  1,                            /* transform >= to ! < */
  1,                            /* transform != to !(a == b) */
  0,                            /* leave == */
  false,                        /* No array initializer support. */
  hc08_cseCostEstimation,
  "",                           /* no builtin functions */
  GPOINTER,                     /* treat unqualified pointers as "generic" pointers */
  true,                         /* __far is a subset of generic. */
  false,                        /* generic is a subset of __far. */
  1,                            /* reset labelKey to 1 */
  1,                            /* globals & local statics allowed */
  3,                            /* Number of registers handled in the tree-decomposition-based register allocator in SDCCralloc.hpp */
  PORT_MAGIC
};

