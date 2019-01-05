/*-------------------------------------------------------------------------
  main.c - Padauk specific definitions.

  Philipp Klaus Krause <pkk@spth.de> 2012-2018

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

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!
-------------------------------------------------------------------------*/

#include "common.h"

#include "ralloc.h"

extern DEBUGFILE dwarf2DebugFile;
extern int dwarf2FinalizeFile(FILE *);

static char *pdk_keywords[] = {
  "at",
  "data",
  "interrupt",
  "naked",
  "reentrant",
  "sfr",
  "sfr16",
  NULL
};

static void
pdk_genAssemblerEnd (FILE *of)
{
  if (options.out_fmt == 'E' && options.debug)
    dwarf2FinalizeFile (of);
}

static void
pdk_init (void)
{
  asm_addTree (&asm_asxxxx_mapping);
}

static void
pdk_reset_regparm (struct sym_link *funcType)
{
}

static int
pdk_reg_parm (sym_link *l, bool reentrant)
{
  return (0);
}

static bool
pdk_parseOptions (int *pargc, char **argv, int *i)
{
  if (!strcmp (argv[*i], "--out-fmt-elf"))
  {
    options.out_fmt = 'E';
    debugFile = &dwarf2DebugFile;
    return TRUE;
  }
  return FALSE;
}

static void
pdk_finaliseOptions (void)
{
  port->mem.default_local_map = data;
  port->mem.default_globl_map = data;
}

static void
pdk_setDefaultOptions (void)
{
  options.out_fmt = 'i';        /* Default output format is ihx */
}

static const char *
pdk_getRegName (const struct reg_info *reg)
{
  if (reg)
    return reg->name;
  return "err";
}

static bool
_hasNativeMulFor (iCode *ic, sym_link *left, sym_link *right)
{
  return (false);
}

/** $1 is always the basename.
    $2 is always the output file.
    $3 varies
    $l is the list of extra options that should be there somewhere...
    MUST be terminated with a NULL.
*/
static const char *_linkCmd[] =
{
  "sdldstm8", "-nf", "\"$1\"", NULL
};

/* $3 is replaced by assembler.debug_opts resp. port->assembler.plain_opts */
static const char *pdkAsmCmd[] =
{
  "sdasstm8", "$l", "$3", "\"$1.asm\"", NULL
};

static const char *const _libs_pdk14[] = { "pdk14", NULL, };

PORT pdk14_port =
{
  TARGET_ID_PDK14,
  "pdk14",
  "PDK14",                       /* Target name */
  0,                             /* Processor name */
  {
    glue,
    false,
    NO_MODEL,
    NO_MODEL,
    0,                          /* model == target */
  },
  {                             /* Assembler */
    pdkAsmCmd,
    0,
    "-plosgffwy",               /* Options with debug */
    "-plosgffw",                /* Options without debug */
    0,
    ".asm"
  },
  {                             /* Linker */
    _linkCmd,
    0,                          //LINKCMD,
    0,
    ".rel",
    1,
    0,                          /* crt */
    _libs_pdk14,                /* libs */
  },
  {                             /* Peephole optimizer */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  },
  /* Sizes: char, short, int, long, long long, ptr, fptr, gptr, bit, float, max */
  {
    1,                          /* char */
    2,                          /* short */
    2,                          /* int */
    4,                          /* long */
    8,                          /* long long */
    2,                          /* near ptr */
    2,                          /* far ptr */
    2,                          /* generic ptr */
    2,                          /* func ptr */
    0,                          /* banked func ptr */
    1,                          /* bit */
    4,                          /* float */
  },
  /* tags for generic pointers */
  { 0x00, 0x40, 0x60, 0x80 },   /* far, near, xstack, code */
  {
    "XSEG",
    "STACK",
    "CODE",                     /* code */
    "DATA",                     /* data */
    NULL,                       /* idata */
    NULL,                       /* pdata */
    NULL,                       /* xdata */
    NULL,                       /* bit */
    "RSEG (ABS)",               /* reg */
    "GSINIT",                   /* static initialization */
    NULL,                       /* overlay */
    "GSFINAL",                  /* gsfinal */
    "HOME",                     /* home */
    NULL,                       /* xidata */
    NULL,                       /* xinit */
    "CONST",                    /* const_name */
    "CABS (ABS)",               /* cabs_name */
    "DABS (ABS)",               /* xabs_name */
    NULL,                       /* iabs_name */
    "INITIALIZED",              /* name of segment for initialized variables */
    "INITIALIZER",              /* name of segment for copies of initialized variables in code space */
    NULL,
    NULL,
    1                           /* CODE  is read-only */
  },
  { 0, 0 },
  {                             /* stack information */
     +1,                        /* direction: stack grows up */
     0,
     7,                         /* isr overhead */
     2,                         /* call overhead */
     0,
     2,
     1,                         /* sp points to next free stack location */
  },     
  { -1, true },
  { 0,
    {
      0,
      0,                        /* cfiSame */
      0,                        /* cfiUndef */
      0,                        /* addressSize */
      0,                        /* regNumRet */
      0,                        /* regNumSP */
      0,                        /* regNumBP */
      0,                        /* offsetSP */
    },
  },
  {
    256,                        /* maxCount */
    1,                          /* sizeofElement */
    {2, 0, 0},                  /* sizeofMatchJump[] - the 0s here ensure that we only generate jump tables for 8-bit operands, which is all the backend can handle */
    {4, 0, 0},                  /* sizeofRangeCompare[] */
    1,                          /* sizeofSubtract */
    2,                          /* sizeofDispatch */
  },
  "_",
  pdk_init,
  pdk_parseOptions,
  0,
  0,
  pdk_finaliseOptions,           /* finaliseOptions */
  pdk_setDefaultOptions,         /* setDefaultOptions */
  pdk_assignRegisters,
  pdk_getRegName,
  0,
  0,
  pdk_keywords,
  0,
  pdk_genAssemblerEnd,
  0,
  0,                            /* no genXINIT code */
  0,                            /* genInitStartup */
  pdk_reset_regparm,
  pdk_reg_parm,
  0,                            /* process_pragma */
  0,                            /* getMangledFunctionName */
  _hasNativeMulFor,             /* hasNativeMulFor */
  0,                            /* hasExtBitOp */
  0,                            /* oclsExpense */
  true,
  true,                         /* little endian */
  0,                            /* leave lt */
  0,                            /* leave gt */
  1,                            /* transform <= to ! > */
  1,                            /* transform >= to ! < */
  1,                            /* transform != to !(a == b) */
  0,                            /* leave == */
  false,                        /* Array initializer support. */
  0,                            /* no CSE cost estimation yet */
  0,                            /* builtin functions */
  GPOINTER,                     /* treat unqualified pointers as "generic" pointers */
  1,                            /* reset labelKey to 1 */
  1,                            /* globals & local statics allowed */
  2,                            /* Number of registers handled in the tree-decomposition-based register allocator in SDCCralloc.hpp */
  PORT_MAGIC
};

