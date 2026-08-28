/*-------------------------------------------------------------------------
  main.c - m6502 specific general function

  Copyright (C) 2003, Erik Petrich

  Hacked for the MOS6502:
  Copyright (C) 2020, Steven Hugg  hugg@fasterlight.com
  Copyright (C) 2021-2026, Gabriele Gorla

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; eitherversion 2, or (at your option) any
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

#include "ralloc.h"
#include "gen.h"
#include "dbuf_string.h"
#include "m6502.h"
#include "peep.h"

#define OPTION_SMALL_MODEL      "--model-small"
#define OPTION_LARGE_MODEL      "--model-large"
//#define OPTION_XDATA_OVR        "--xdata-overlay"
#define OPTION_XDATA_SPILL      "--no-zp-spill"
//#define OPTION_CODE_SEG         "--codeseg"
//#define OPTION_CONST_SEG        "--constseg"
//#define OPTION_DATA_SEG         "--dataseg"
#define OPTION_NO_STD_CRT0      "--no-std-crt0"

extern char * iComments2;
extern DEBUGFILE dwarf2DebugFile;
extern int dwarf2FinalizeFile(FILE *);

OPTION m6502_options[] = {
  {0, OPTION_SMALL_MODEL, NULL, "8-bit address space for data"},
  {0, OPTION_LARGE_MODEL, NULL, "16-bit address space for data (default)"},
  //    {0, OPTION_XDATA_OVR,   &options.xdata_overlay, "place overlay segment in 16-bit address space"},
  {0, OPTION_XDATA_SPILL, &options.xdata_spill,   "place register spills in 16-bit address space"},
  //    {0, OPTION_CODE_SEG,        &options.code_seg, "<name> use this name for the code segment", CLAT_STRING},
  //    {0, OPTION_CONST_SEG,       &options.const_seg, "<name> use this name for the const segment", CLAT_STRING},
  //    {0, OPTION_DATA_SEG,        &options.data_seg, "<name> use this name for the data segment", CLAT_STRING},
  {0, OPTION_NO_STD_CRT0,     &options.no_std_crt0, "Do not link default crt0.rel"},
  {0, NULL }
};

static struct
{
  // Determine if we can put parameters in registers
  struct
  {
    int n;
    struct sym_link *ftype;
  } regparam;
}
  _G;

MOS6502_OPTS m6502_opts;

/* list of key words used by m6502 */
char *m6502_keywords[] = {
  "at",
  "critical",
  "interrupt",
  "naked",
  "reentrant",
  "code",
  "data",
  "near",
  "zp",
  "xdata",
  "far",
  //  "overlay",
  //  "using",
  //  "generic",
  NULL
};

/** $1 is always the basename.
    $2 is always the output file.
    $3 varies
    $l is the list of extra options that should be there somewhere...
    $L is the list of extra options that should be passed on the command line...
    MUST be terminated with a NULL.
*/
const char *m6502_linkCmd[] =
  {
    "sdld6808", "-nf", "$1", "$L", NULL
  };

/* $3 is replaced by assembler.debug_opts resp. port->assembler.plain_opts */
const char *m6502_asmCmd[] =
  {
    "sdas6500", "$l", "$3", "$2", "$1.asm", NULL
  };

const char *const m6502_crt[] = { "crt0.rel", NULL, };


void
m6502_reset_regparm (struct sym_link *ftype)
{
  _G.regparam.n = 0;
  _G.regparam.ftype = ftype;
}

int
m6502_regparm (sym_link *l, bool reentrant)
{
  if (IFFUNC_HASVARARGS (_G.regparam.ftype))
    return 0;

  if (IS_STRUCT (l))
    return 0;

  int size = getSize(l);

  /* If they fit completely, the first two bytes of parameters can go */
  /* into A and X, otherwise, they go on the stack. Examples:         */
  /*   foo(char p1)                    A <- p1                        */
  /*   foo(char p1, char p2)           A <- p1, X <- p2               */
  /*   foo(char p1, char p2, char p3)  A <- p1, X <- p2, stack <- p3  */
  /*   foo(int p1)                     XA <- p1                       */
  /*   foo(long p1)                    stack <- p1                    */
  /*   foo(char p1, int p2)            A <- p1, stack <- p2           */
  /*   foo(int p1, char p2)            XA <- p1, stack <- p2          */

  if (_G.regparam.n>=2)
    return 0;

  if ((_G.regparam.n+size)>2)
    {
      _G.regparam.n = 2;
      return 0;
    }

  _G.regparam.n += size;
  return 1+_G.regparam.n-size;
}

bool
m6502_parseOptions (int *pargc, char **argv, int *i)
{
  return false;
}

void
m6502_finaliseOptions (void)
{
  if (options.noXinitOpt)
    port->genXINIT = 0;

  if (options.model == MODEL_LARGE)
    {
      port->mem.default_local_map = xdata;
      port->mem.default_globl_map = xdata;
    }
  else
    {
      port->mem.default_local_map = data;
      port->mem.default_globl_map = data;
    }

  if(options.data_loc > 240)
    werror (W_DATA_LOC_RANGE);

  istack->ptrType = FPOINTER;
}

void
m6502_setDefaultOptions (void)
{
  options.nopeep = 0;
  options.stackAuto = 0;
  //  options.intlong_rent = 1;
  //  options.float_rent = 1;
  //  options.noRegParams = 0;
  options.code_loc = 0x8000;
  options.data_loc = 0x0001;    /* Zero page, We can't use the byte at address zero in C, since NULL pointers have special meaning */
  options.xdata_loc = 0x0200;   /* immediately following stack */
  options.stack_loc = 0x01ff;

  options.omitFramePtr = 1;     /* no frame pointer (we use SP */
                                /* offsets instead)            */
  options.out_fmt = 'i';        /* Default output format is ihx */
  //  options.xdata_overlay = 0;    /* Overlay in ZP */
  options.xdata_spill = 0;      /* Spill in ZP   */
}

const char *
m6502_getRegName (const struct reg_info *reg)
{
  if (reg)
    return reg->name;
  return "err";
}

void
m6502_commonAssemblerStart (FILE * of)
{
  fprintf(of, ";; Ordering of segments for the linker.\n");
  tfprintf (of, "\t!area\n", DATA_NAME);
  tfprintf (of, "\t!area\n", OVERLAY_NAME);
  //  if (options.xdata_overlay==0)
  //      tfprintf (of, "\t!area    (PAG, OVR)\n", OVERLAY_NAME);

  tfprintf (of, "\t!area\n", HOME_NAME);
  tfprintf (of, "\t!area\n", STATIC_NAME);
  tfprintf (of, "\t!area\n", "GSFINAL");
  tfprintf (of, "\t!area\n", CODE_NAME);
  tfprintf (of, "\t!area\n", CONST_NAME);
  tfprintf (of, "\t!area\n", XINIT_NAME);

  tfprintf (of, "\t!area\n", "_DATA");
  tfprintf (of, "\t!area\n", XIDATA_NAME);
  //  if(options.xdata_overlay)
  //      tfprintf (of, "\t!area    (OVR)\n", OVERLAY_NAME);
  tfprintf (of, "\t!area\n", XDATA_NAME);

  if (!options.noOptsdccInAsm)
    {
      fprintf (of, "\t.optsdcc -m%s\n", port->target);
    }
  fprintf (of, "\n");
}

void
m6502_genAssemblerEnd (FILE * of)
{
  if (options.out_fmt == 'E' && options.debug)
    {
      dwarf2FinalizeFile (of);
    }
}

/* Generate interrupt vector table. */
int
m6502_genIVT (struct dbuf_s * oBuf, symbol ** interrupts, int maxInterrupts)
{
  dbuf_printf (oBuf, "\t.area\tCODEIVT (ABS)\n");
  dbuf_printf (oBuf, "\t.org\t0xFFFA\n");

  wassertl(maxInterrupts <= 2, "Too many interrupt vectors");
  if (maxInterrupts > 1 && interrupts[1])
    dbuf_printf (oBuf, "\t.dw\t%s\n", interrupts[1]->rname);
  else
    dbuf_printf (oBuf, "\t.dw\t0xffff\n");
  dbuf_printf (oBuf, "\t.dw\t%s", "__sdcc_gs_init_startup\n");
  if (maxInterrupts > 0 && interrupts[0])
    dbuf_printf (oBuf, "\t.dw\t%s\n", interrupts[0]->rname);
  else
    dbuf_printf (oBuf, "\t.dw\t0xffff\n");

  return true;
}

/* Generate code to copy XINIT to XISEG */
void
m6502_genXINIT (FILE * of)
{
  // This is not called but it must be defined to avoid
  // SDCCmem.c line 445 from putting DATA into BSS and
  // then generating code to fill it in.
}

/* Indicate which extended bit operations this port supports */
bool
m6502_hasExtBitOp (int op, sym_link *left, int right)
{
  switch (op)
    {
    case GETBYTE:
    case GETWORD:
      return true;
    case ROT:
      {
        unsigned int lbits = bitsForType (left);
        if (lbits % 8)
          return false;
        if (lbits==8)
          return true;
        if (lbits==16)
          return true;
        if (lbits==32)
          return true;
        if (lbits > (unsigned)port->support.shift*8)
          return false;
        if (right % lbits  == 1 || right % lbits == lbits - 1)
          return true;
      }
      return false;
    }
  return false;
}

/* Indicate the expense of an access to an output storage class */
int
m6502_oclsExpense (struct memmap *oclass)
{
  if (IN_DIRSPACE (oclass))     /* direct addressing mode is fastest */
    return -2;
  if (IN_FARSPACE (oclass))     /* extended addressing mode is almost at fast */
    return -1;
  if (oclass == istack)         /* stack is the slowest */
    return 2;

  return 0; /* anything we missed */
}

/*----------------------------------------------------------------------*/
/* m6502_dwarfRegNum - return the DWARF register number for a register.  */
/*   These are defined for the M6502 in "Motorola 8- and 16-bit Embedded */
/*   Application Binary Interface (M8/16EABI)"                          */
/*----------------------------------------------------------------------*/
int
m6502_dwarfRegNum (const struct reg_info *reg)
{
  switch (reg->rIdx)
    {
    case A_IDX: return 0;
    case Y_IDX: return 1;
    case X_IDX: return 2;
    case CND_IDX: return 17;
    case SP_IDX: return 15;
    }
  return -1;
}

bool
m6502_hasNativeMulFor (iCode *ic, sym_link *left, sym_link *right)
{
  return false;
}

typedef struct asmLineNode
{
  int size;
}
  asmLineNode;

static asmLineNode *
newAsmLineNode (void)
{
  asmLineNode *aln;

  aln = Safe_alloc ( sizeof (asmLineNode));
  aln->size = 0;

  return aln;
}

/*
  processor flags
  N 0x80
  V 0x40
  B 0x10
  D 0x08
  I 0x04
  Z 0x02
  C 0x01
*/

/* These must be kept sorted by opcode name */
const m6502opcodedata m6502opcodeDataTable[] =
  {
    {".db",   M6502OP_INH, -1,       0 },    // used by the code generator only in the jump table
    {"adc",   M6502OP_REG, A_IDX,    0xc3 },
    {"and",   M6502OP_REG, A_IDX,    0x82 },
    {"asl",   M6502OP_RMW, -1,       0x83 },
    {"bbr",   M6502OP_BBR, -1,       0 },    // Rockwell and WDC only - check huc
    {"bbs",   M6502OP_BBR, -1,       0 },    // Rockwell and WDC only
    {"bcc",   M6502OP_BR,  -1,       0 },
    {"bcs",   M6502OP_BR,  -1,       0 },
    {"beq",   M6502OP_BR,  -1,       0 },
    {"bit",   M6502OP_CMP, -1,       0xc2 },
    {"bmi",   M6502OP_BR,  -1,       0 },
    {"bne",   M6502OP_BR,  -1,       0 },
    {"bpl",   M6502OP_BR,  -1,       0 },
    {"bra",   M6502OP_BR,  -1,       0 },    // 65C02 and 6280
    {"brk",   M6502OP_INH, -1,       0 },
    {"bsr",   M6502OP_BR,  -1,       0 },    // 6280 only
    {"bvc",   M6502OP_BR,  -1,       0 },
    {"bvs",   M6502OP_BR,  -1,       0 },
    {"cla",   M6502OP_INH,  1,       0 },    // 6280 only - clears A no flags affected
    {"clc",   M6502OP_INH, -1,       0x01 },
    {"cld",   M6502OP_INH, -1,       0x08 },
    {"cli",   M6502OP_INH, -1,       0x04 },
    {"clv",   M6502OP_INH, -1,       0x40 },
    {"clx",   M6502OP_INH, -1,       0 },    // 6280 only - clears X no flags affected
    {"cly",   M6502OP_INH, -1,       0 },    // 6280 only - clears Y no flags affected
    {"cmp",   M6502OP_CMP, -1,       0x83 },
    {"cpx",   M6502OP_CMP, -1,       0x83 },
    {"cpy",   M6502OP_CMP, -1,       0x83 },
    {"dec",   M6502OP_RMW, -1,       0x82 },
    {"dex",   M6502OP_IDD, X_IDX,    0x82 },
    {"dey",   M6502OP_IDD, Y_IDX,    0x82 },
    {"eor",   M6502OP_REG, A_IDX,    0x82 },
    {"inc",   M6502OP_RMW, -1,       0x82 },
    {"inx",   M6502OP_IDI, X_IDX,    0x82 },
    {"iny",   M6502OP_IDI, Y_IDX,    0x82 },
    {"jmp",   M6502OP_JMP, -1,       0 },
    {"jsr",   M6502OP_JMP, -1,       0 },
    {"lda",   M6502OP_LD , A_IDX,    0x82 },
    {"ldx",   M6502OP_LD , X_IDX,    0x82 },
    {"ldy",   M6502OP_LD , Y_IDX,    0x82 },
    {"lsr",   M6502OP_RMW, -1,       0x83 },
    {"nop",   M6502OP_INH, -1,       0 },
    {"ora",   M6502OP_REG, A_IDX,    0x82 },
    {"pha",   M6502OP_SPH, -1,       0 },
    {"php",   M6502OP_SPH, -1,       0 },
    {"phx",   M6502OP_SPH, -1,       0 },    // 65C02 and 6280
    {"phy",   M6502OP_SPH, -1,       0 },    // 65C02 and 6280
    {"pla",   M6502OP_SPL, A_IDX,    0x82 },
    {"plp",   M6502OP_SPL, -1,       0xdf },
    {"plx",   M6502OP_SPL, X_IDX,    0x82 }, // 65C02 and 6280
    {"ply",   M6502OP_SPL, Y_IDX,    0x82 }, // 65C02 and 6280
    {"rmb",   M6502OP_REG, -1,       0 },    // Rockwell, WDC and 6280 only
    {"rol",   M6502OP_RMW, -1,       0x83 },
    {"ror",   M6502OP_RMW, -1,       0x83 },
    {"rti",   M6502OP_INH, -1,       0xdf },
    {"rts",   M6502OP_INH, -1,       0 },
    {"sax",   M6502OP_INH, -1,       0 },    // 6280 only - no flags
    {"say",   M6502OP_INH, -1,       0 },    // 6280 only - no flags
    {"sbc",   M6502OP_REG, A_IDX,    0xc3 },
    {"sec",   M6502OP_INH, -1,       0x01 },
    {"sed",   M6502OP_INH, -1,       0x08 },
    {"sei",   M6502OP_INH, -1,       0x04 },
    {"set",   M6502OP_INH, -1,       0 },    // 6280 only - set T=1
    {"smb",   M6502OP_REG, -1,       0 },    // Rockwell, WDC only and 6280
//    {"st0",   M6502OP_INH, -1,       0 },    // 6280 only
//    {"st1",   M6502OP_INH, -1,       0 },    // 6280 only
//    {"st2",   M6502OP_INH, -1,       0 },    // 6280 only
    {"sta",   M6502OP_ST , -1,       0 },
    {"stp",   M6502OP_INH, -1,       0 },    // WDC only
    {"stx",   M6502OP_ST , -1,       0 },
    {"sty",   M6502OP_ST , -1,       0 },
    {"stz",   M6502OP_ST , -1,       0 },    // 65C02 and 6280
    {"sxy",   M6502OP_INH, -1,       0 },    // 6280 only - no flags
    {"tai",   M6502OP_MT,  -1,       0 },    // 6280 only - no flags
//    {"tam",   M6502OP_INH, -1,       0 },    // 6280 only - no flags
    {"tax",   M6502OP_INH, X_IDX,    0x82 },
    {"tay",   M6502OP_INH, Y_IDX,    0x82 },
    {"tdd",   M6502OP_MT,  -1,       0 },    // 6280 only - no flags
    {"tia",   M6502OP_MT,  -1,       0 },    // 6280 only - no flags
    {"tii",   M6502OP_MT,  -1,       0 },    // 6280 only - no flags
    {"tin",   M6502OP_MT,  -1,       0 },    // 6280 only - no flags
//    {"tma",   M6502OP_INH, -1,       0 },    // 6280 only - no flags
    {"trb",   M6502OP_REG, -1,       0 },    // 65C02 and 6280
    {"tsb",   M6502OP_REG, -1,       0 },    // 65C02 and 6280
//    {"tst",   M6502OP_INH, -1,       0xc2 },    // 6280 only - weird format
    {"tsx",   M6502OP_INH, X_IDX,    0x82 },
    {"txa",   M6502OP_INH, A_IDX,    0x82 },
    {"txs",   M6502OP_INH, -1,       0 },
    {"tya",   M6502OP_INH, A_IDX,    0x82 },
    {"wai",   M6502OP_INH, -1,       0 },    // WDC only
    {"zzz",   0,           -1,       0 }     // end of table
  };

static int
m6502_opcodeCompare (const void *key, const void *member)
{
  return strncmp((const char *)key, ((m6502opcodedata *)member)->name,3);
}


const m6502opcodedata *m6502_getOpcodeData (const char *inst)
{
  return   bsearch (inst, m6502opcodeDataTable,
                    sizeof(m6502opcodeDataTable)/sizeof(m6502opcodedata)-1,
                    sizeof(m6502opcodedata), m6502_opcodeCompare);
}

int
m6502_opcodeSize (const m6502opcodedata *opcode, const char *arg)
{
  switch (opcode->type)
    {
    case M6502OP_INH: /* Inherent addressing mode */
    case M6502OP_SPH:
    case M6502OP_SPL:
    case M6502OP_IDD:
    case M6502OP_IDI:
      return 1;

    case M6502OP_BR:  /* Branch (1 byte signed offset) */
      return 2;

    case M6502OP_BBR:  /* Branch on bit (1 byte signed offset) */
      return 3;

    case M6502OP_RMW: /* read/modify/write instructions */
      if (!strcmp(arg, "a"))  /* accumulator */
	return 1;
      if (arg[0] == '*') /* Zero page */
	return 2;
      return 3;  /* absolute */

    case M6502OP_REG: /* standard instruction */
    case M6502OP_CMP:
    case M6502OP_LD:
    case M6502OP_ST:
      if (arg[0] == '#') /* Immediate addressing mode */
	return 2;
      if (arg[0] == '*') /* Zero page */
	return 2;
      if (arg[0] == '[') /* indirect */
	return 2;
      return 3; /* Otherwise, must be extended addressing mode */

    case M6502OP_JMP:
      return 3;

    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "unknown instruction type in m6502_opcodeSize");
      return 3;
    }
}

/*--------------------------------------------------------------------*/
/* Given an instruction and its first two operands, compute the       */
/* instruction size. There are a few cases where it's too complicated */
/* to distinguish between an 8-bit offset and 16-bit offset; in these */
/* cases we conservatively assume the 16-bit offset size.             */
/*--------------------------------------------------------------------*/
static int
m6502_instructionSize (const char *inst, const char *op1, const char *op2)
{
  const m6502opcodedata *opcode = m6502_getOpcodeData(inst);

  if (!opcode)
    return 999;

  //  printf("op: %s - %s - %s\n",inst,op1, op2);

  return m6502_opcodeSize(opcode, op1);
}


static asmLineNode *
m6502_asmLineNodeFromLineNode (lineNode *ln)
{
  asmLineNode *aln = newAsmLineNode();
  char *op, op1[256], op2[256];
  int opsize;
  const char *p;
  char inst[8];

  p = ln->line;

  while (*p && isspace(*p)) p++;
  for (op = inst, opsize=1; *p; p++)
    {
      if (isspace(*p) || *p == ';' || *p == ':' || *p == '=')
        break;
      else
        if (opsize < sizeof(inst))
          *op++ = tolower(*p), opsize++;
    }
  *op = '\0';

  if (*p == ':' || *p == '=')
    return aln;

  while (*p && isspace(*p)) p++;
  if (*p == '=')
    return aln;

  if (*p==';')
    {
      op1[0]=0;
      op2[0]=0;
      aln->size = m6502_instructionSize(inst, op1, op2);
      return aln;
    }

  for (op = op1, opsize=1; *p && *p != ',' && *p != ';'; p++)
    {
      if (!isspace(*p) && opsize < sizeof(op1))
        *op++ = tolower(*p), opsize++;
    }
  *op = '\0';

  if (*p == ',')
    p++;
  if (*p == ';')
    {
      op2[0]=0;
      aln->size = m6502_instructionSize(inst, op1, op2);
      return aln;
    }

  for (op = op2, opsize=1; *p && *p != ',' && *p != ';' ; p++)
    {
      if (!isspace(*p) && opsize < sizeof(op2))
        *op++ = tolower(*p), opsize++;
    }
  *op = '\0';

  aln->size = m6502_instructionSize(inst, op1, op2);

  return aln;
}

int
m6502_getInstructionSize (lineNode *line)
{
  if (!line->aln)
    line->aln = (asmLineNodeBase *) m6502_asmLineNodeFromLineNode (line);

  return line->aln->size;
}

const char *
m6502_get_model (void)
{
  if (IS_MOS65C02)
    {
      if (options.stackAuto)
        return "mos65c02-stack-auto";
      else
        return "mos65c02";
    }
  else
    {
      if (options.stackAuto)
        return "mos6502-stack-auto";
      else
        return "mos6502";
    }
}

