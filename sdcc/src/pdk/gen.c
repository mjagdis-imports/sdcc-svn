/*-------------------------------------------------------------------------
  gen.c - code generator for Padauk.

  Copyright (C) 2018, Philipp Klaus Krause pkk@spth.de

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

#include "ralloc.h"
#include "gen.h"

/* Use the D macro for basic (unobtrusive) debugging messages */
#define D(x) do if (options.verboseAsm) { x; } while (0)

static struct asmop asmop_a, asmop_zero, asmop_one;
static struct asmop *const ASMOP_A = &asmop_a;
static struct asmop *const ASMOP_ZERO = &asmop_zero;
static struct asmop *const ASMOP_ONE = &asmop_one;

void
pdk_init_asmops (void)
{
  asmop_a.type = AOP_REG;
  asmop_a.size = 1;
  asmop_a.aopu.bytes[0].in_reg = TRUE;
  asmop_a.aopu.bytes[0].byteu.reg = pdk_regs + A_IDX;

  asmop_zero.type = AOP_LIT;
  asmop_zero.size = 1;
  asmop_zero.aopu.aop_lit = constVal ("0");

  asmop_one.type = AOP_LIT;
  asmop_one.size = 1;
  asmop_one.aopu.aop_lit = constVal ("1");
}

static void
emit2 (const char *inst, const char *fmt, ...)
{
  //if (!regalloc_dry_run)
    {
      va_list ap;

      va_start (ap, fmt);
      va_emitcode (inst, fmt, ap);
      va_end (ap);
    }
}

static void
cost(unsigned int words, unsigned int cycles)
{
}

static void
emitJP(const symbol *target, float probability)
{
  //if (!regalloc_dry_run)
    emit2 ("goto", "%05d$", labelKey2num (target->key));
  cost (1, 2 * probability);
}

static bool
regDead (int idx, const iCode *ic)
{
  wassert (idx == A_IDX);

  return (!bitVectBitValue (ic->rSurv, idx));
}

/*-----------------------------------------------------------------*/
/* aopInReg - asmop from offset in the register                    */
/*-----------------------------------------------------------------*/
static bool
aopInReg (const asmop *aop, int offset, short rIdx)
{
  if (aop->type != AOP_REG)
    return (false);

  return (aop->aopu.bytes[offset].in_reg && aop->aopu.bytes[offset].byteu.reg->rIdx == rIdx);
}

/*-----------------------------------------------------------------*/
/* aopSame - are two asmops in the same location?                  */
/*-----------------------------------------------------------------*/
static bool
aopSame (const asmop *aop1, int offset1, const asmop *aop2, int offset2, int size)
{
  for(; size; size--, offset1++, offset2++)
    {
      if (aop1->type == AOP_LIT && aop2->type == AOP_LIT &&
        byteOfVal (aop1->aopu.aop_lit, offset1) == byteOfVal (aop2->aopu.aop_lit, offset2))
        continue;

      if (aop1->type == AOP_DIR && aop2->type == AOP_DIR &&
        offset1 == offset2 && !strcmp(aop1->aopu.aop_dir, aop2->aopu.aop_dir))
        return (true);

      return (false);
    }

  return (true);
}

/*-----------------------------------------------------------------*/
/* aopIsLitVal - asmop from offset is val                          */
/*-----------------------------------------------------------------*/
static bool
aopIsLitVal (const asmop *aop, int offset, int size, unsigned long long int val)
{
  wassert_bt (size <= sizeof (unsigned long long int)); // Make sure we are not testing outside of argument val.

  for(; size; size--, offset++)
    {
      unsigned char b = val & 0xff;
      val >>= 8;

      // Leading zeroes
      if (aop->size <= offset && !b)
        continue;

      if (aop->type != AOP_LIT)
        return (false);

      if (byteOfVal (aop->aopu.aop_lit, offset) != b)
        return (false);
    }

  return (true);
}

static const char *
aopGet(const asmop *aop, int offset)
{
  static char buffer[256];

  if (offset >= aop->size)
    return ("#0x00");

  if (aop->type == AOP_LIT)
    {
      SNPRINTF (buffer, sizeof(buffer), "#0x%02x", byteOfVal (aop->aopu.aop_lit, offset));
      return (buffer);
    }

  if (aop->type == AOP_IMMD)
    {
      wassertl_bt (offset < (2 + (options.model == MODEL_LARGE)), "Immediate operand out of range");
      if (offset == 0)
        SNPRINTF (buffer, sizeof(buffer), "#<(%s + %d)", aop->aopu.immd, aop->aopu.immd_off);
      else
        SNPRINTF (buffer, sizeof(buffer), "#((%s + %d) >> %d)", aop->aopu.immd, aop->aopu.immd_off, offset * 8);
      return (buffer);
    }

  if (aop->type == AOP_DIR)
    {
      SNPRINTF (buffer, sizeof(buffer), "%s+%d", aop->aopu.aop_dir, aop->size - 1 - offset);
      return (buffer);
    }

  wassert_bt (0);
  return ("dummy");
}

/*-----------------------------------------------------------------*/
/* newAsmop - creates a new asmOp                                  */
/*-----------------------------------------------------------------*/
static asmop *
newAsmop (short type)
{
  asmop *aop;

  aop = Safe_calloc (1, sizeof (asmop));
  aop->type = type;

  return (aop);
}

/*-----------------------------------------------------------------*/
/* freeAsmop - free up the asmop given to an operand               */
/*----------------------------------------------------------------*/
static void
freeAsmop (operand *op)
{
  asmop *aop;

  wassert_bt (op);

  aop = op->aop;

  if (!aop)
    return;

  Safe_free (aop);

  op->aop = 0;
  if (IS_SYMOP (op) && SPIL_LOC (op))
    SPIL_LOC (op)->aop = 0;
}

/*-----------------------------------------------------------------*/
/* aopForSym - for a true symbol                                   */
/*-----------------------------------------------------------------*/
static asmop *
aopForSym (const iCode *ic, symbol *sym)
{
  asmop *aop;

  wassert_bt (ic);
  wassert_bt (sym);
  wassert_bt (sym->etype);

  // Unlike some other backends we really free asmops; to avoid a double-free, we need to support multiple asmops for the same symbol.

  if (IS_FUNC (sym->type))
    {
      aop = newAsmop (AOP_IMMD);
      aop->aopu.immd = sym->rname;
      aop->aopu.immd_off = 0;
      aop->size = getSize (sym->type);
    }
  /* Assign depending on the storage class */
  else if (sym->onStack || sym->iaccess)
    {
      wassertl (0, "Unimplemented support for on-stack operand");
    }
  else
    {
      aop = newAsmop (AOP_DIR);
      aop->aopu.aop_dir = sym->rname;
      aop->size = getSize (sym->type);
    }

  return (aop);
}

/*-----------------------------------------------------------------*/
/* aopOp - allocates an asmop for an operand  :                    */
/*-----------------------------------------------------------------*/
static void
aopOp (operand *op, const iCode *ic)
{
  wassert_bt (op);

  /* if already has an asmop */
  if (op->aop)
    return;

  /* if this a literal */
  if (IS_OP_LITERAL (op))
    {
      asmop *aop = newAsmop (AOP_LIT);
      aop->aopu.aop_lit = OP_VALUE (op);
      aop->size = getSize (operandType (op));
      op->aop = aop;
      return;
    }

  symbol *sym = OP_SYMBOL (op);

  /* if this is a true symbol */
  if (IS_TRUE_SYMOP (op))
    {
      op->aop = aopForSym (ic, sym);
      return;
    }

  /* Rematerialize symbols where all bytes are spilt. */
  if (sym->remat && sym->isspilt)
    wassertl (0, "Unimplemented rematerialized operand");

  /* if the type is a conditional */
  if (sym->regType == REG_CND)
    wassertl (0, "Unimplemented condition operand");

  /* TODO: SFR! */

  /* None of the above, which only leaves temporaries. */
  if (sym->isspilt || sym->nRegs == 0)
    {
      sym->aop = op->aop = aopForSym (ic, sym->usl.spillLoc);
      op->aop->size = getSize (sym->type);
      return;
    }

  /* TODO register */

  wassertl (0, "Unimplemented aop type");
}

/*-----------------------------------------------------------------*/
/* cheapMove - Copy a byte from one asmop to another               */
/*-----------------------------------------------------------------*/
static void
cheapMove (asmop *result, int roffset, asmop *source, int soffset, bool a_dead)
{
  bool dummy = (result->type == AOP_DUMMY || source->type == AOP_DUMMY);

  if (aopSame (result, roffset, source, soffset, 1))
    return;
  else if (!dummy && result->type == AOP_DIR && aopIsLitVal (source, soffset, 1, 0))
    {
      emit2 ("clear", "%s", aopGet (result, roffset));
      cost (1, 1);
    }
  else if (aopInReg (result, roffset, A_IDX))
    {
      emit2 ("mov", "a, %s", aopGet (source, soffset));
      cost (1, 1);
    }
  else if (aopInReg (source, soffset, A_IDX))
    {
      emit2 ("mov", "%s, a", aopGet (result, roffset));
      cost (1, 1);
    }
  else
    {
      wassert (a_dead);
      cheapMove (ASMOP_A, 0, source, soffset, false);
      cheapMove (result, roffset, ASMOP_A, 0, false);
    }
}

/*-----------------------------------------------------------------*/
/* genMove_o - Copy part of one asmop to another                   */
/*-----------------------------------------------------------------*/
static void
genMove_o (asmop *result, int roffset, asmop *source, int soffset, int size, bool a_dead_global)
{
  wassert (result->type == AOP_DIR);
  wassert (source->type == AOP_LIT || source->type == AOP_DIR);

  for (unsigned int i = 0; i < size; i++)
    cheapMove (result, roffset + i, source, soffset + i, a_dead_global);
}

/*-----------------------------------------------------------------*/
/* genMove - Copy the value from one asmop to another              */
/*-----------------------------------------------------------------*/
static void
genMove (asmop *result, asmop *source, bool a_dead)
{
  genMove_o (result, 0, source, 0, result->size, a_dead);
}

/*-----------------------------------------------------------------*/
/* genSub - generates code for subtraction                         */
/*-----------------------------------------------------------------*/
static void
genSub (const iCode *ic, asmop *result_aop, asmop *left_aop, asmop *right_aop)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = result->aop->size;

  bool started = false;
  for (int i = 0; i < size; i++)
    {
      cheapMove (ASMOP_A, 0, left->aop, i, true);

      wassertl (!started || aopIsLitVal (right->aop, i, 1, 0x00), "Unimplemented subtraction of literal with multiple nonzero bytes");

      if (started || !aopIsLitVal (right->aop, i, 1, 0x00))
        {
          if (started && aopIsLitVal (right->aop, i, 1, 0x00))
            emit2 ("subc", "a");
          else
            emit2 (started ? "sub" : "subc", "a, %s", aopGet (right->aop, i));
          cost (1, 1);
          started = true;
        }

      cheapMove (result->aop, i, ASMOP_A, 0, true);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genUminus - generates code for unary minus                      */
/*-----------------------------------------------------------------*/
static void
genUminusFloat (const iCode *ic)
{
  wassertl (0, "Unimplemented float negation");
}

/*-----------------------------------------------------------------*/
/* genUminus - generates code for unary minus                      */
/*-----------------------------------------------------------------*/
static void
genUminus (const iCode *ic)
{
  operand *result;
  operand *left;

  if (IS_FLOAT (operandType (IC_LEFT (ic))))
    {
      genUminusFloat (ic);
      return;
    }

  result = IC_RESULT (ic);
  left = IC_LEFT (ic);

  D (emit2 ("; genUminus", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  genSub (ic, result->aop, ASMOP_ZERO, left->aop);

  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genCall - generates a call statement                            */
/*-----------------------------------------------------------------*/
static void
genCall (const iCode *ic)
{
  sym_link *dtype = operandType (IC_LEFT (ic));
  sym_link *etype = getSpec (dtype);

  D (emit2 ("; genCall", ""));

  wassertl (ic->op != PCALL, "Unimplemented call through function pointer");

  if (IS_LITERAL (etype))
    emit2 ("call", "0x%04X", ulFromVal (OP_VALUE (IC_LEFT (ic))));
  else
    {
      bool jump = (!ic->parmBytes && IFFUNC_ISNORETURN (OP_SYMBOL (IC_LEFT (ic))->type));
      emit2 (jump ? "goto" : "call", "%s",
             (OP_SYMBOL (IC_LEFT (ic))->rname[0] ? OP_SYMBOL (IC_LEFT (ic))->rname : OP_SYMBOL (IC_LEFT (ic))->name));
    }
  cost (1, 2);

  bool SomethingReturned = (IS_ITEMP (IC_RESULT (ic)) &&
                       (OP_SYMBOL (IC_RESULT (ic))->nRegs || OP_SYMBOL (IC_RESULT (ic))->spildir))
                       || IS_TRUE_SYMOP (IC_RESULT (ic));

  if (SomethingReturned)
    {
      aopOp (IC_RESULT (ic), ic);

      wassertl (IC_RESULT (ic)->aop->size <= 1, "Unimplemented call to function returning more than 1 byte");
      cheapMove (IC_RESULT (ic)->aop, 0, ASMOP_A, 0, true);

      freeAsmop (IC_RESULT (ic));
    }
}

/*-----------------------------------------------------------------*/
/* genFunction - generated code for function entry                 */
/*-----------------------------------------------------------------*/
static void
genFunction (iCode *ic)
{
  const symbol *sym = OP_SYMBOL_CONST (IC_LEFT (ic));
  sym_link *ftype = operandType (IC_LEFT (ic));

  /* create the function header */
  emit2 (";", "-----------------------------------------");
  emit2 (";", " function %s", sym->name);
  emit2 (";", "-----------------------------------------");

  emit2 ("", "%s:", sym->rname);
  genLine.lineCurr->isLabel = 1;

  if (IFFUNC_ISNAKED(ftype))
    {
      emit2(";", "naked function: no prologue.");
      return;
    }

  if (IFFUNC_ISISR (sym->type))
    {
      emit2 ("push", "af");
      cost (1, 1);
    }
}

/*-----------------------------------------------------------------*/
/* genEndFunction - generates epilogue for functions               */
/*-----------------------------------------------------------------*/
static void
genEndFunction (iCode *ic)
{
  symbol *sym = OP_SYMBOL (IC_LEFT (ic));

  D (emit2 ("; genEndFunction", ""));

  if (IFFUNC_ISNAKED(sym->type))
  {
      D (emit2 (";", "naked function: no epilogue."));
      if (options.debug && currFunc /*&& !regalloc_dry_run*/)
        debugFile->writeEndFunction (currFunc, ic, 0);
      return;
  }

  /* if debug then send end of function */
  if (options.debug && currFunc /*&& !regalloc_dry_run*/)
    debugFile->writeEndFunction (currFunc, ic, 1);

  if (IFFUNC_ISISR (sym->type))
    {
      emit2 ("pop", "af");
      emit2 ("reti", "");
      cost (2, 2);
    }
  else
    {
      emit2 ("ret", "");
      cost (1, 1);
    }
}

/*-----------------------------------------------------------------*/
/* genReturn - generate code for return statement                  */
/*-----------------------------------------------------------------*/
static void
genReturn (const iCode *ic)
{
  operand *left = IC_LEFT (ic);

  D (emit2 ("; genReturn", ""));

  /* if we have no return value then
     just generate the "ret" */
  if (!left)
    goto jumpret;

  /* we have something to return then
     move the return value into place */
  aopOp (left, ic);

  wassertl (left->aop->size == 1, "return of value wider than 1 byte not yet implemented");
  cheapMove (ASMOP_A, 0, left->aop, 0, true);

  freeAsmop (left);  

jumpret:
  /* generate a jump to the return label
     if the next is not the return statement */
  if (!(ic->next && ic->next->op == LABEL && IC_LABEL (ic->next) == returnLabel))
    emitJP(returnLabel, 1.0f);
}

/*-----------------------------------------------------------------*/
/* genLabel - generates a label                                    */
/*-----------------------------------------------------------------*/
static void
genLabel (const iCode *ic)
{
  D (emit2 ("; genLabel", ""));

  /* special case never generate */
  if (IC_LABEL (ic) == entryLabel)
    return;

  if (options.debug /*&& !regalloc_dry_run*/)
    debugFile->writeLabel (IC_LABEL (ic), ic);

  emitLabel (IC_LABEL (ic));
}

/*-----------------------------------------------------------------*/
/* genGoto - generates a jump                                      */
/*-----------------------------------------------------------------*/
static void
genGoto (const iCode *ic)
{
  D (emit2 ("; genGoto", ""));

  emitJP (IC_LABEL (ic), 1.0f);
}

/*-----------------------------------------------------------------*/
/* genPlus - generates code for addition                           */
/*-----------------------------------------------------------------*/
static void
genPlus (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genPlus", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = result->aop->size;

  /* Swap if left is literal or right is in A. */
  if (left->aop->type == AOP_LIT || aopInReg (right->aop, 0, A_IDX))
    {
      operand *t = right;
      right = left;
      left = t;
    }

  bool started = false;
  for (int i = 0; i < size; i++)
    {
      cheapMove (ASMOP_A, 0, left->aop, i, true);

      wassertl (!started || aopIsLitVal (right->aop, i, 1, 0x00), "Unimplemented addition with multiple nozero bytes in literal");

      if (started || !aopIsLitVal (right->aop, i, 1, 0x00))
        {
          if (started && aopIsLitVal (right->aop, i, 1, 0x00))
            emit2 ("addc", "a");
          else
            emit2 (started ? "addc" : "add", "a, %s", aopGet (right->aop, i));
          cost (1, 1);
          started = true;
        }

      cheapMove (result->aop, i, ASMOP_A, 0, true);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genMinus - generates code for minus                             */
/*-----------------------------------------------------------------*/
static void
genMinus (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genMinus", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  genSub (ic, result->aop, left->aop, right->aop);

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genCmpEQorNE - equal or not equal comparison                    */
/*-----------------------------------------------------------------*/
static void
genCmpEQorNE (const iCode *ic, iCode *ifx)
{
  operand *left, *right, *result;

  D (emit2 ("; genCmpEQorNE", ""));

  result = IC_RESULT (ic);
  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  symbol *tlbl_ne = newiTempLabel (NULL);
  symbol *tlbl = newiTempLabel (NULL);

  int size = max (left->aop->size, right->aop->size);

  for (int i = 0; i < size; i++)
    {
      /* Prefer literal operand on right */
      if (left->aop->type == AOP_LIT || aopInReg (right->aop, i, A_IDX))
        {
          operand *temp = left;
          left = right;
          right = temp;
        }

      cheapMove (ASMOP_A, 0, left->aop, i, true);
      emit2 ("ceqsn", "a, %s", aopGet (right->aop, i));
      cost (1, 1);
      emitJP(tlbl_ne, 0.0f);
    }

  cheapMove (result->aop, 0, ic->op == EQ_OP ? ASMOP_ONE : ASMOP_ZERO, 0, true);
  emitJP(tlbl, 0.0f);
  emitLabel (tlbl_ne);
  cheapMove (result->aop, 0, ic->op == NE_OP ? ASMOP_ONE : ASMOP_ZERO, 0, true);
  emitLabel (tlbl);

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genAssign - generate code for assignment                        */
/*-----------------------------------------------------------------*/
static void
genAssign (const iCode *ic)
{
  operand *result, *right;

  D (emit2 ("; genAssign", ""));

  result = IC_RESULT (ic);
  right = IC_RIGHT (ic);

  aopOp (right, ic);
  aopOp (result, ic);

  wassert (result->aop->type != AOP_DUMMY || right->aop->type != AOP_DUMMY);

  if (right->aop->type == AOP_DUMMY)
    {
      wassert (0);
#if 0
      int i;
      D (emit2 ("; Dummy write", ""));
      for (i = 0; i < result->aop->size; i++)
        cheapMove (result->aop, i, ASMOP_A, 0, TRUE);
#endif
    }
  else if (result->aop->type == AOP_DUMMY)
    {
      wassert (0);
#if 0
      int i;
      D (emit2 ("; Dummy read", ""));

      if (!regDead(A_IDX, ic) && right->aop->type == AOP_DIR)
        for (i = 0; i < right->aop->size; i++)
          emit3_o (A_TNZ, right->aop, i, 0, 0);
      else
        {
          if (!regDead(A_IDX, ic))
            push (ASMOP_A, 0, 1);
          for (i = 0; i < right->aop->size; i++)
            cheapMove (ASMOP_A, 0, right->aop, i, FALSE);
          if (!regDead(A_IDX, ic))
            pop (ASMOP_A, 0, 1);
        }
#endif
    }
  else
    genMove(result->aop, right->aop, regDead (A_IDX, ic));

  wassert (result->aop != right->aop);

  freeAsmop (right);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genIfx - generate code for Ifx statement                        */
/*-----------------------------------------------------------------*/
static void
genIfx (const iCode *ic)
{
  operand *const cond = IC_COND (ic);
  symbol *tlbl = 0;
  aopOp (cond, ic);

  if (IC_FALSE (ic) && cond->aop->size > 1)
    tlbl = newiTempLabel (0);

  for (int i = 0; i < cond->aop->size; i++)
    {
      cheapMove (ASMOP_A, 0, cond->aop, i, true);

      if (IC_FALSE (ic) && i + 1 >= cond->aop->size)
        {
          emit2 ("ceqsn", "a, #0x00");
          emit2 ("goto", "!tlabel", labelKey2num (tlbl->key));
          cost (1, 1);
        }
      else if (IC_FALSE (ic))
        {
          emit2 ("cneqsn", "a, #0x00");
          cost (1, 1);
          emitJP (IC_FALSE (ic), 0.0f);
        }
      else
        {
          emit2 ("ceqsn", "a, #0x00");
          cost (1, 1);
          emitJP (IC_TRUE (ic), 0.0f);
        }
    }

  emitLabel (tlbl);

  freeAsmop (cond);
}

/*-----------------------------------------------------------------*/
/* genCast - generate code for cast                                */
/*-----------------------------------------------------------------*/
static void
genCast (const iCode *ic)
{
  operand *result, *right;
  int offset;
  sym_link *resulttype, *righttype;

  D (emit2 ("; genCast", ""));

  result = IC_RESULT (ic);
  right = IC_RIGHT (ic);
  resulttype = operandType (result);
  righttype = operandType (right);

  if ((getSize (resulttype) <= getSize (righttype) || !IS_SPEC (righttype) || (SPEC_USIGN (righttype) || IS_BOOL (righttype))) &&
    (!IS_BOOL (resulttype) || IS_BOOL (righttype)))
    {
      genAssign (ic);
      return;
    }

  aopOp (right, ic);
  aopOp (result, ic);

  if (IS_BOOL (resulttype))
    {
      int size = right->aop->size;

      cheapMove (ASMOP_A, 0, right->aop, 0, true);

      for(offset = 1; offset < size; offset++)
        {
          emit2 ("or", "a, %s", aopGet (right->aop, offset));
          cost (1, 1);
        }

      emit2 ("ceqsn", "a, #0x00");
      emit2 ("mov", "a, #0x01");
      cost (2, 2);

      cheapMove (result->aop, 0, ASMOP_A, 0, true);
    }
  else // Cast to signed type
    {
      genMove_o (result->aop, 0, right->aop, 0, right->aop->size, regDead (A_IDX, ic));

      int size = result->aop->size - right->aop->size;
      offset = right->aop->size;

      cheapMove (ASMOP_A, 0, result->aop, right->aop->size - 1, true);
      emit2 ("sl", "a");
      emit2 ("mov", "a, #0x00");
      emit2 ("subc", "a");
      cost (3, 3);

      while (size--)
        cheapMove (result->aop, offset++, ASMOP_A, 0, true);
    }

  freeAsmop (right);
  freeAsmop (result);
}

/*---------------------------------------------------------------------*/
/* genSTM8Code - generate code for STM8 for a single iCode instruction */
/*---------------------------------------------------------------------*/
static void
genPdkiCode (iCode *ic)
{
  genLine.lineElement.ic = ic;

#if 0
  if (resultRemat (ic))
    {
      if (!regalloc_dry_run)
        D (emit2 ("; skipping iCode since result will be rematerialized", ""));
      return;
    }
#endif

  if (ic->generated)
    {
      //if (!regalloc_dry_run)
        D (emit2 ("; skipping generated iCode", ""));
      return;
    }

  switch (ic->op)
    {
    case '!':
      wassertl (0, "Unimplemented iCode: Negation");
      break;

    case '~':
      wassertl (0, "Unimplemented iCode: Bitwise complement");
      break;

    case UNARYMINUS:
      genUminus (ic);
      break;

    case IPUSH:
      wassertl (0, "Unimplemented iCode");
      break;

    case IPOP:
      wassertl (0, "Unimplemented iCode");
      break;

    case CALL:
    case PCALL:
      genCall (ic);
      break;

    case FUNCTION:
      genFunction (ic);
      break;

    case ENDFUNCTION:
      genEndFunction (ic);
      break;

   case RETURN:
      genReturn (ic);
      break;

    case LABEL:
      genLabel (ic);
      break;

    case GOTO:
      genGoto (ic);
      break;

    case '+':
      genPlus (ic);
      break;

    case '-':
      genMinus (ic);
      break;

    case '*':
      wassertl (0, "Unimplemented iCode");
      break;

    case '/':
    case '%':
      wassertl (0, "Unimplemented iCode");
      break;

    case '>':
    case '<':
    case LE_OP:
    case GE_OP:
      wassertl (0, "Unimplemented iCode: Comparison");
      break;

    case NE_OP:
    case EQ_OP:
      genCmpEQorNE (ic, ifxForOp (IC_RESULT (ic), ic));
      break;

    case AND_OP:
    case OR_OP:
      wassertl (0, "Unimplemented iCode");
      break;

    case '^':
      wassertl (0, "Unimplemented iCode: Xor");
      break;

    case '|':
      wassertl (0, "Unimplemented iCode: Bitwise or");
      break;

    case BITWISEAND:
      wassertl (0, "Unimplemented iCode: Bitwise and");
      break;

    case INLINEASM:
      genInline (ic);
      break;

    case RRC:
    case RLC:
      wassertl (0, "Unimplemented iCode");
      break;

    case GETABIT:
      wassertl (0, "Unimplemented iCode");
      break;

    case LEFT_OP:
      wassertl (0, "Unimplemented iCode: Left shift");
      break;

    case RIGHT_OP:
      wassertl (0, "Unimplemented iCode: Right shift");
      break;

    case GET_VALUE_AT_ADDRESS:
      wassertl (0, "Unimplemented iCode: Read via pointer");
      break;

    case SET_VALUE_AT_ADDRESS:
      wassertl (0, "Unimplemented iCode: Write via pointer");
      break;

    case '=':
      wassert (!POINTER_SET (ic));
      genAssign (ic);
      break;

    case IFX:
      genIfx (ic);
      break;

    case ADDRESS_OF:
      wassertl (0, "Unimplemented iCode: Adress of");
      break;

    case JUMPTABLE:
      wassertl (0, "Unimplemented iCode");
      break;

    case CAST:
      genCast (ic);
      break;

    case RECEIVE:
    case SEND:
      wassertl (0, "Unimplemented iCode");
      break;

    case DUMMY_READ_VOLATILE:
      wassertl (0, "Unimplemented iCode: Dummy volatile read");
      break;

    case CRITICAL:
      wassertl (0, "Unimplemented iCode: Critical section");
      break;

    case ENDCRITICAL:
      wassertl (0, "Unimplemented iCode: Critical section");
      break;

    default:
      wassertl (0, "Unknown iCode");
    }
}

/*---------------------------------------------------------------------*/
/* genPdkCode - generate code for Padauk for a block of intructions    */
/*---------------------------------------------------------------------*/
void
genPdkCode (iCode *lic)
{
  int clevel = 0;
  int cblock = 0;  
  int cln = 0;

  for (iCode *ic = lic; ic; ic = ic->next)
    {
      initGenLineElement ();

      genLine.lineElement.ic = ic;

      if (ic->level != clevel || ic->block != cblock)
        {
          if (options.debug)
            debugFile->writeScope (ic);
          clevel = ic->level;
          cblock = ic->block;
        }

      if (ic->lineno && cln != ic->lineno)
        {
          if (options.debug)
            debugFile->writeCLine (ic);

          if (!options.noCcodeInAsm)
            emit2 (";", "%s: %d: %s", ic->filename, ic->lineno, printCLine (ic->filename, ic->lineno));
          cln = ic->lineno;
        }

      if (options.iCodeInAsm)
        {
          const char *iLine = printILine (ic);
          emit2 ("; ic:", "%d: %s", ic->key, iLine);
          dbuf_free (iLine);
        }

      genPdkiCode(ic);
    }

  if (options.debug)
    debugFile->writeFrameAddress (NULL, NULL, 0); /* have no idea where frame is now */

#if 0
  /* now we are ready to call the
     peephole optimizer */
  if (!options.nopeep)
    peepHole (&genLine.lineHead);
#endif

  /* now do the actual printing */
  printLine (genLine.lineHead, codeOutBuf);

  /* destroy the line list */
  destroy_line_list ();
}

