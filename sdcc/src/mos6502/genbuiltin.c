/*-------------------------------------------------------------------------
  genbuiltin.c - builtin functions for MOS6502

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

#include "m6502.h"
#include "ralloc.h"
#include "gen.h"


const char m6502_builtins[] =
  "extern float __builtin_fabsf (float f) __builtin__;\n"
  ;


static void
genBuiltInFabs(const iCode *ic, int nparams, operand **pparams)
{
  operand *src, *result;
  int offset;

  m6502_emitComment (TRACEGEN, "  %s", __func__);

  src = pparams[0];
  result = IC_RESULT (ic);

  m6502_aopOp (src, ic);
  m6502_aopOp (result, ic);

  bool needpulla = storeRegTempIfSurv (m6502_reg_a);


  for(offset=0;offset<(AOP_SIZE(result)-1);offset++)
    m6502_transferAopAop(AOP(src), offset, AOP(result), offset);

  m6502_loadRegFromAop (m6502_reg_a, AOP (src), 3);
  m6502_emitOp("and","#0x7f");
  m6502_storeRegToAop (m6502_reg_a, AOP (result), 3);

  m6502_loadOrFreeRegTemp(m6502_reg_a, needpulla);

  m6502_freeAsmop (src, NULL);
  m6502_freeAsmop (result, NULL);
}

static void
genBuiltInMemset(const iCode *ic, int nparams, operand **pparams)
{
  operand *dst, *val, *len;
  symbol *loop_label = m6502_safeNewiTempLabel (NULL);
  bool needpulla = false;
  bool needpully = false;
  bool use_dptr = false;
  //  int offset;

  m6502_emitComment (TRACEGEN, "  %s", __func__);

  dst = pparams[0];
  val = pparams[1];
  len = pparams[2];

  m6502_aopOp (dst, ic);
  m6502_aopOp (val, ic);
  m6502_aopOp (len, ic);

  needpulla = storeRegTempIfSurv (m6502_reg_a);
  needpully = storeRegTempIfSurv (m6502_reg_y);


  if(AOP_TYPE(dst)!=AOP_DIR)
    {
      //storeOperToDPTR (operand *oper, int size, iCode *ic)
      m6502_loadRegFromAop (m6502_reg_a, AOP (dst), 0);
      m6502_storeRegToDPTR(m6502_reg_a, 0);
      m6502_loadRegFromAop (m6502_reg_a, AOP (dst), 1);
      m6502_storeRegToDPTR(m6502_reg_a, 1);
      use_dptr=true;
    }

  m6502_loadRegFromAop (m6502_reg_a, AOP (val), 0);
  m6502_loadRegFromAop (m6502_reg_y, AOP (len), 0);
  m6502_safeEmitLabel(loop_label);
  m6502_rmwWithReg ("dec", m6502_reg_y);

  if(use_dptr)
    m6502_emitOp ("sta", INDFMT_IY, "DPTR");
  else
    m6502_emitOp("sta", INDFMT_IY, AOP(dst)->aopu.aop_dir);
  m6502_emitBranch ("bne", loop_label);

  m6502_loadOrFreeRegTemp(m6502_reg_y, needpully);
  m6502_loadOrFreeRegTemp(m6502_reg_a, needpulla);

  m6502_freeAsmop (dst, NULL);
  m6502_freeAsmop (val, NULL);
  m6502_freeAsmop (len, NULL);
}

/*-----------------------------------------------------------------*/
/* genBuiltIn - calls the appropriate function to generate code    */
/* for a built in function                                         */
/*-----------------------------------------------------------------*/
void
m6502_genBuiltIn (iCode *ic)
{
  operand *bi_parms[MAX_BUILTIN_ARGS];
  int nbi_parms;
  iCode *bi_iCode;
  symbol *bif;

  m6502_emitComment (TRACEGEN, "  %s", __func__);

  /* get all the arguments for a built in function */
  //  if(!regalloc_dry_run)
  bi_iCode = getBuiltinParms (ic, &nbi_parms, bi_parms);

  /* which function is it */
  bif = OP_SYMBOL (IC_LEFT (bi_iCode));

  //wassertl (!ic->prev || ic->prev->op != SEND || !ic->prev->builtinSEND, "genBuiltIn() must be called on first SEND icode only.");

  if (!strcmp (bif->name, "__builtin_fabsf"))
    {
      genBuiltInFabs (bi_iCode, nbi_parms, bi_parms);
    }
  else if (!strcmp (bif->name, "__builtin_memset"))
    {
      genBuiltInMemset (bi_iCode, nbi_parms, bi_parms);
    }
  else
    {
      m6502_emitComment (ALWAYS, "ERROR: %s - unknowns builtin %s", __func__, bif->name);
      //      wassertl (0, "Unknown builtin function encountered");
    }
}

