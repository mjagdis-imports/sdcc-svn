/*-------------------------------------------------------------------------
  m6502.h - header for SDCC mos6502 port

  Copyright (C) 2022-2025, Gabriele Gorla

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

#include <stdio.h>
#include <stdbool.h>
#include "common.h"

typedef enum
  {
    SUB_MOS6502,
    SUB_MOS65C02,
    SUB_HUC6280
  }
MOS6502_SUB_PORT;

typedef struct
  {
    MOS6502_SUB_PORT sub;
  }
MOS6502_OPTS;

extern const char *m6502_linkCmd[];
extern const char *m6502_asmCmd[];
extern const char *const m6502_crt[];
extern const char m6502_builtins[];
extern char *m6502_keywords[];
extern OPTION m6502_options[];


extern MOS6502_OPTS m6502_opts;

int m6502_process_pragma (const char *s);
void m6502_commonAssemblerStart (FILE * of);
const char * m6502_get_model (void);
bool m6502_hasExtBitOp (int op, sym_link *left, int right);
bool m6502_hasNativeMulFor (iCode *ic, sym_link *left, sym_link *right);
int m6502_genIVT (struct dbuf_s * oBuf, symbol ** interrupts, int maxInterrupts);
void m6502_genXINIT (FILE * of);
void m6502_reset_regparm (struct sym_link *ftype);
int m6502_regparm (sym_link *l, bool reentrant);
int m6502_dwarfRegNum (const struct reg_info *reg);
int m6502_getInstructionSize (lineNode *line);
bool m6502_parseOptions (int *pargc, char **argv, int *i);
void m6502_finaliseOptions (void);
void m6502_genAssemblerEnd (FILE * of);
int m6502_oclsExpense (struct memmap *oclass);
const char * m6502_getRegName (const struct reg_info *reg);
void m6502_setDefaultOptions (void);

void m6502_assignRegisters (ebbIndex *);
void m6502_emitDebuggerSymbol (const char *);

#define IS_MOS6502  (m6502_opts.sub == SUB_MOS6502)
#define IS_MOS65C02 (m6502_opts.sub == SUB_MOS65C02)
#define IS_HUC6280  (m6502_opts.sub == SUB_HUC6280)

