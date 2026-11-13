/*-------------------------------------------------------------------------
  port_mos6502.c - port specific file for mos6502

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

static const char *const _crt[]  = { "crt0.rel", NULL };
static const char *const _libs[] = { "mos6502",  NULL };

static char _defaultRules[] =
  {
#include "peeph.rul"
  };

static void
_init (void)
{
  m6502_opts.sub = SUB_MOS6502;
  asm_addTree (&asm_asxxxx_mapping);
}

static const char *
_get_model (void)
{
  if (options.stackAuto)
    return "mos6502-stack-auto";
  else
    return "mos6502";
}

static void
_genAssemblerStart (FILE * of)
{
  fprintf(of, "\t.r6500\n");
  m6502_commonAssemblerStart (of);
}

/* Globals */
PORT mos6502_port =
  {
    TARGET_ID_MOS6502,          /* Unique id for this target */
    "mos6502",                  /* Target name used for -m */
    "MOS 6502",                 /* Target name string, used for --help */
    NULL,                       /* Specific processor for the given target family. specified by -p */
    {                           /* general */
      glue,                     /* Pointer to glue function */
      false,                    /* Emit glue around main */
      MODEL_SMALL | MODEL_LARGE,/* OR of MODEL_* */
      MODEL_LARGE,              /* default model */
      _get_model                /* return the model string, used as library destination */
    },
    {                           /* Assembler */
      m6502_asmCmd,             /* Command to run and arguments */
      NULL,                     /* Alternate macro based form */
      "-plosgffwy",             /* Options with debug */
      "-plosgffw",              /* Options without debug */
      0,                        /* print externs as global */
      ".asm",                   /* assembler file extension */
      NULL                      /* If non-null will be used to execute the assembler. */
    },
    {                           /* Linker */
      m6502_linkCmd,            /** Command to run (eg link-z80) */
      NULL,                     /** Alternate macro based form. */
      NULL,                     /** If non-null will be used to execute the link. */
      ".rel",                   /* object file extension */
      1,                        /* need linker script */
      _crt,                     /* crt */
      _libs                     /* libs */
    },
    {                           /* Peephole optimizer */
      _defaultRules,
      m6502_getInstructionSize,
      NULL,
      NULL,
      NULL,
      m6502_notUsed,
      NULL,
      m6502_notUsedFrom,
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
    { 0x00, 0x00, 0x00, 0x00 },   /* far, near, xstack, code */
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
      1                         /* sp points to next free stack location */
    },
    {
      -1,                       /* shifts never use support routines */
      false,                    /* do not use support routine for int x int -> long multiplication */
      false                     /* do not use support routine for unsigned long x unsigned char -> unsigned long long multiplication */
    },
    {
      m6502_emitDebuggerSymbol,
      {
	m6502_dwarfRegNum,
	0,                          /* cfiSame */
	0,                          /* cfiUndef */
	4,                          /* addressSize */
	14,                         /* regNumRet */
	15,                         /* regNumSP */
	-1,                         /* regNumBP */
	1                           /* offsetSP */
      },
    },
    {
      256,                      /* maxCount */
      2,                        /* sizeofElement */
      {8,16,32},                /* sizeofMatchJump[] */
      {8,16,32},                /* sizeofRangeCompare[] */
      5,                        /* sizeofSubtract */
      10                        /* sizeofDispatch */
    },
    "_",                        /* Prefix to add to a C function (eg "_") */
    _init,                      /* First chance to initialise and set any port specific variables */
    m6502_parseOptions,         /* Optional list of automatically parsed options */
    m6502_options,              /* Parses one option + its arguments */
    NULL,                       /* Initialise port spectific paths */
    m6502_finaliseOptions,      /* Called after all the options have been parsed */
    m6502_setDefaultOptions,    /* Called before any options are parsed. */
    m6502_assignRegisters,
    m6502_getRegName,           /* Returns the register name of a symbol */
    NULL,                       /* getRegByName */
    NULL,                       /* rtrackUpdate - Try to keep track of register contents. */
    m6502_keywords,             /* list of keywords that are used by this target */
    _genAssemblerStart,         /* genAssemblerStart */
    m6502_genAssemblerEnd,      /* genAssemblerEnd */
    m6502_genIVT,               /* local IVT generation code */
    m6502_genXINIT,             /* genXINIT code */
    NULL,                       /* genInitStartup */
    m6502_reset_regparm,        /* reset the register count */
    m6502_regparm,              /* will return 1 if can be passed in register */
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
    true,                       /* __far is a subset of generic. */
    false,                      /* generic is a subset of __far. */
    1,                          /* reset labelKey to 1 */
    1,                          /* globals & local statics allowed */
    3,                          /* Number of registers handled in the tree-decomposition-based register allocator in SDCCralloc.hpp */
    PORT_MAGIC
  };

