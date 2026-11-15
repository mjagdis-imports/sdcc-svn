/*-------------------------------------------------------------------------
  subport.h - subport header for SDCC hc08 port

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

#include <stdio.h>
#include <stdbool.h>
#include "common.h"

typedef enum
  {
    SUB_HC08,
    SUB_S08
  }
HC08_SUB_PORT;

typedef struct
  {
    HC08_SUB_PORT sub;
  }
HC08_OPTS;

extern const char *hc08_linkCmd[];
extern const char *hc08_asmCmd[];
//extern const char *const hc08_crt[];
//extern const char *const hc08_libs[];
extern const char hc08_peepRules[];
//extern const char hc08_builtins[];
extern char *hc08_keywords[];
extern OPTION hc08_options[];

extern HC08_OPTS hc08_opts;

void hc08_commonAssemblerStart (FILE * of);
const char * hc08_get_model (void);
bool hc08_hasExtBitOp (int op, sym_link *left, int right);
bool hc08_hasNativeMulFor (iCode *ic, sym_link *left, sym_link *right);
int hc08_genIVT (struct dbuf_s * oBuf, symbol ** interrupts, int maxInterrupts);
void hc08_genXINIT (FILE * of);
void hc08_reset_regparm (struct sym_link *ftype);
int hc08_regparm (sym_link *l, bool reentrant);
int hc08_dwarfRegNum (const struct reg_info *reg);
int hc08_getInstructionSize (lineNode *line);
bool hc08_parseOptions (int *pargc, char **argv, int *i);
void hc08_finaliseOptions (void);
void hc08_genAssemblerEnd (FILE * of);
int hc08_oclsExpense (struct memmap *oclass);
const char * hc08_getRegName (const struct reg_info *reg);
void hc08_setDefaultOptions (void);
void hc08_genExtraAreas (FILE * asmFile, bool mainExists);
void hc08_genAssemblerStart (FILE * of);
bool hc08_cseCostEstimation (iCode *ic, iCode *pdic);

void hc08_assignRegisters (ebbIndex *);
void hc08_emitDebuggerSymbol (const char *);

#define IS_HC08 (hc08_opts.sub == SUB_HC08)
#define IS_S08 (hc08_opts.sub == SUB_S08)

