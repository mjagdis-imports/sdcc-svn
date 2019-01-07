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

static bool regalloc_dry_run;
static unsigned int regalloc_dry_run_cost_words;
static unsigned int regalloc_dry_run_cost_cycles;
static unsigned int regalloc_dry_run_cycle_scale = 1;

static struct
{
  short debugLine;
  struct
    {
      int pushed;
      int size;
      int param_offset;
    } stack;
  bool saved;

  /* Track content of p */
  struct
   {
     AOP_TYPE type;
     const char *base;
     int offset;
   } p;
}
G;

static struct asmop asmop_a, asmop_p, asmop_zero, asmop_one, asmop_sp;
static struct asmop *const ASMOP_A = &asmop_a;
static struct asmop *const ASMOP_P = &asmop_p;
static struct asmop *const ASMOP_ZERO = &asmop_zero;
static struct asmop *const ASMOP_ONE = &asmop_one;
static struct asmop *const ASMOP_SP = &asmop_sp;

void
pdk_init_asmops (void)
{
  asmop_a.type = AOP_REG;
  asmop_a.size = 1;
  asmop_a.aopu.bytes[0].in_reg = TRUE;
  asmop_a.aopu.bytes[0].byteu.reg = pdk_regs + A_IDX;

  asmop_p.type = AOP_REG;
  asmop_p.size = 1;
  asmop_p.aopu.bytes[0].in_reg = TRUE;
  asmop_p.aopu.bytes[0].byteu.reg = pdk_regs + P_IDX;

  asmop_zero.type = AOP_LIT;
  asmop_zero.size = 1;
  asmop_zero.aopu.aop_lit = constVal ("0");

  asmop_one.type = AOP_LIT;
  asmop_one.size = 1;
  asmop_one.aopu.aop_lit = constVal ("1");

  asmop_sp.type = AOP_SFR;
  asmop_sp.aopu.aop_dir = "sp";
  asmop_sp.size = 1;
}

static void
emit2 (const char *inst, const char *fmt, ...)
{
  if (!regalloc_dry_run)
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
  regalloc_dry_run_cost_words += words;
  regalloc_dry_run_cost_cycles += cycles * regalloc_dry_run_cycle_scale;
}

static void
emitJP(const symbol *target, float probability)
{
  if (!regalloc_dry_run)
    emit2 ("goto", "%05d$", labelKey2num (target->key));
  cost (1, 2 * probability);
}

static bool
regDead (int idx, const iCode *ic)
{
  wassert (idx == A_IDX || idx == P_IDX);

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
      if (aop1->type == AOP_REG && aop2->type == AOP_REG && // Same register
        aop1->aopu.bytes[offset1].in_reg && aop2->aopu.bytes[offset2].in_reg &&
        aop1->aopu.bytes[offset1].byteu.reg == aop2->aopu.bytes[offset2].byteu.reg)
        continue;

      if (aop1->type == AOP_LIT && aop2->type == AOP_LIT &&
        byteOfVal (aop1->aopu.aop_lit, offset1) == byteOfVal (aop2->aopu.aop_lit, offset2))
        continue;

      if (aop1->type == AOP_DIR && aop2->type == AOP_DIR && offset1 == offset2 &&
        (!regalloc_dry_run || aop1->aopu.aop_dir && aop2->aopu.aop_dir && !strstr (aop1->aopu.aop_dir, "sloc")) &&
        !strcmp(aop1->aopu.aop_dir, aop2->aopu.aop_dir))
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

  if (aop->type == AOP_REG)
    return (aop->aopu.bytes[offset].byteu.reg->name);

  if (aop->type == AOP_IMMD)
    {
      wassertl_bt (offset < (2 + (options.model == MODEL_LARGE)), "Immediate operand out of range");
      if (offset == 0)
        SNPRINTF (buffer, sizeof(buffer), "#<(%s + %d)", aop->aopu.immd, aop->aopu.immd_off);
      else
        SNPRINTF (buffer, sizeof(buffer), "#((%s + %d) >> %d)", aop->aopu.immd, aop->aopu.immd_off, offset * 8);
      return (buffer);
    }

  if (aop->type == AOP_DIR || aop->type == AOP_SFR)
    {
      wassert (aop->type != AOP_SFR || aop->size == 1);
      SNPRINTF (buffer, sizeof(buffer), "%s+%d", aop->aopu.aop_dir, offset);
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
  wassert_bt (regalloc_dry_run || sym);
  wassert_bt (regalloc_dry_run || sym->etype);

  // Unlike some other backends we really free asmops; to avoid a double-free, we need to support multiple asmops for the same symbol.

  if (sym && IS_FUNC (sym->type))
    {
      aop = newAsmop (AOP_IMMD);
      aop->aopu.immd = sym->rname;
      aop->aopu.immd_off = 0;
      aop->size = getSize (sym->type);
    }
  /* Assign depending on the storage class */
  else if (sym && sym->onStack || sym && sym->iaccess)
    {
      aop = newAsmop (AOP_STK);
      aop->size = getSize (sym->type);
      int base = sym->stack + (sym->stack > 0 ? G.stack.param_offset : 0);

      for (int offset = 0; offset < aop->size; offset++)
        aop->aopu.bytes[offset].byteu.stk = base + offset;
    }
  /* sfr */
  else if (sym && IN_REGSP (SPEC_OCLS (sym->etype)))
    {
      wassertl (getSize (sym->type) <= 2, "Unimplemented support for wide (> 16 bit) I/O register");

      aop = newAsmop (AOP_SFR);
      aop->aopu.aop_dir = sym->rname;
      aop->size = getSize (sym->type);
    }
  else
    {
      aop = newAsmop (AOP_DIR);
      if (sym)
        {
          aop->aopu.aop_dir = sym->rname;
          aop->size = getSize (sym->type);
        }
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
    {
      asmop *aop = newAsmop (AOP_CND);
      op->aop = aop;
      sym->aop = sym->aop;
      return;
    }

  /* None of the above, which only leaves temporaries. */
  if (sym->isspilt || sym->nRegs == 0)
    {
      sym->aop = op->aop = aopForSym (ic, sym->usl.spillLoc);
      op->aop->size = getSize (sym->type);
      return;
    }

  /* None of the above, which only leaves temporaries. */
  { 
    bool completely_in_regs = true;
    bool completely_spilt = true;
    asmop *aop = newAsmop (AOP_REGDIR);

    aop->size = getSize (operandType (op));
    op->aop = aop;

    for (int i = 0; i < aop->size; i++)
      {
        aop->aopu.bytes[i].in_reg = !!sym->regs[i];
        if (sym->regs[i])
          {
            completely_spilt = false;
            aop->aopu.bytes[i].byteu.reg = sym->regs[i];
            //aop->regs[sym->regs[i]->rIdx] = i;
          }
        else if (sym->isspilt && sym->usl.spillLoc || sym->nRegs && regalloc_dry_run)
          {
            completely_in_regs = false;

            if (!regalloc_dry_run)
              {
                /*aop->aopu.bytes[i].byteu.stk = (long int)(sym->usl.spillLoc->stack) + aop->size - i;

                if (sym->usl.spillLoc->stack + aop->size - (int)(i) <= -G.stack.pushed)
                  {
                    fprintf (stderr, "%s %d %d %d %d at ic %d\n", sym->name, (int)(sym->usl.spillLoc->stack), (int)(aop->size), (int)(i), (int)(G.stack.pushed), ic->key);
                    wassertl_bt (0, "Invalid stack offset.");
                  }*/
              }
            else
              {
                static long int old_base = -10;
                static const symbol *old_sym = 0;
                if (sym != old_sym)
                  {
                    old_base -= aop->size;
                    if (old_base < -100)
                      old_base = -10;
                    old_sym = sym;
                  }

                // aop->aopu.bytes[i].byteu.stk = old_base + aop->size - i;
              }
          }
        else // Dummy iTemp.
          {
            aop->type = AOP_DUMMY;
            return;
          }

        if (!completely_in_regs && (!currFunc || GcurMemmap == statsg))
          {
            if (!regalloc_dry_run)
              wassertl_bt (0, "Stack asmop outside of function.");
            cost (180, 180);
          }
      }

    if (completely_in_regs)
      aop->type = AOP_REG;
    else if (completely_spilt)
      {
        sym->aop = op->aop = aopForSym (ic, sym->usl.spillLoc);
        op->aop->size = getSize (sym->type);
      }
    else
      wassertl (0, "Unsupported partially spilt aop");
  }
}

static void
cheapMove (const asmop *result, int roffset, const asmop *source, int soffset, bool a_dead, bool f_dead);

/*-----------------------------------------------------------------*/
/* pushAF - push af, adjusting stack tracking                      */
/*-----------------------------------------------------------------*/
static void pushAF (void)
{
  emit2 ("push", "af");
  cost (1, 1);
  G.stack.pushed += 2;
}

/*-----------------------------------------------------------------*/
/* popAF - pop af, adjusting stack tracking                      */
/*-----------------------------------------------------------------*/
static void popAF (void)
{
  emit2 ("pop", "af");
  cost (1, 1);
  G.stack.pushed -= 2;
}

/*-----------------------------------------------------------------*/
/* pointPStack - Make pseudo-register p point to stack location    */
/*-----------------------------------------------------------------*/
static void pointPStack (int s, bool a_dead, bool f_dead)
{
  // Try to adjust p when doing so is cheaper.
  if (G.p.type == AOP_STK && abs(G.p.offset - s) <= 3 + (!a_dead && f_dead) * 2)
    {
      if (G.p.offset == s)
        return;

      if (!f_dead)
        pushAF();

      while (G.p.offset < s)
        {
          emit2 ("inc", "p");
          cost (1, 1);
          G.p.offset++;
        }

      while (G.p.offset > s)
        {
          emit2 ("dec", "p");
          cost (1, 1);
          G.p.offset--;
        }

     if (!f_dead)
        popAF();

      return;
    }

  if (!(a_dead && f_dead))
    pushAF();

  int soffset = s - G.stack.pushed;
  cheapMove (ASMOP_A, 0, ASMOP_SP, 0, true, true);
  emit2 ("add", "a, #0x%02x", soffset & 0xff);
  cost (1, 1);
  cheapMove (ASMOP_P, 0, ASMOP_A, 0, true, true);
  G.p.type = AOP_STK;
  G.p.offset = s;

  if (!(a_dead && f_dead))
    popAF();
}

/*-----------------------------------------------------------------*/
/* cheapMove - Copy a byte from one asmop to another               */
/*-----------------------------------------------------------------*/
static void
cheapMove (const asmop *result, int roffset, const asmop *source, int soffset, bool a_dead, bool f_dead)
{
  bool dummy = (result->type == AOP_DUMMY || source->type == AOP_DUMMY);

  if (aopSame (result, roffset, source, soffset, 1))
    return;
  else if (!dummy && (result->type == AOP_DIR || aopInReg (result, roffset, P_IDX)) && aopIsLitVal (source, soffset, 1, 0))
    {
      emit2 ("clear", "%s", aopGet (result, roffset));
      cost (1, 1);
    }
  else if (source->type == AOP_STK && aopInReg (result, roffset, A_IDX))
    {
      pointPStack(source->aopu.bytes[soffset].byteu.stk, true, f_dead);
      emit2 ("idxm", "a, p");
      cost (1, 1);
    }
  else if (result->type == AOP_STK && aopInReg (source, soffset, A_IDX))
    {
      pointPStack(result->aopu.bytes[roffset].byteu.stk, false, f_dead);
      emit2 ("idxm", "p, a");
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
      if (!a_dead)
        pushAF();
      cheapMove (ASMOP_A, 0, source, soffset, true, f_dead || !a_dead);
      cheapMove (result, roffset, ASMOP_A, 0, true, f_dead || !a_dead);
      if (!a_dead)
        popAF();
    }

  if (result == ASMOP_P)
    G.p.type = AOP_INVALID;
}

/*--------------------------------------------------------------------------*/
/* adjustStack - Adjust the stack pointer by n bytes.                       */
/*--------------------------------------------------------------------------*/
static void
adjustStack (int n, bool a_free, bool p_free)
{
  wassertl_bt (!(n % 2), "Unsupported odd stack adjustment"); // The datasheets seem to require the stack pointer to be aligned to a 2-byte boundary. 

  if (n >= 0 && (!a_free || n <= 4))
    for (int i = 0; i < n; i += 2)
      {
        emit2 ("push", "af");
        cost (1, 1);
      }
  else if (!a_free && p_free && n < 0)
    {
      pushAF();

      for (int i = 0; i < 2; i++)
        {
          pointPStack (G.stack.pushed - 2 + i, true, true);
          emit2 ("idxm", "a, p");
          cost (1, 1);
          pointPStack (G.stack.pushed - 2 + n + i, false, true);
          emit2 ("idxm", "p, a");
          cost (1, 1);
        }
      
      emit2 ("mov", "a, sp");
      emit2 ("add", "a, #%d", n);
      emit2 ("mov", "sp, a");
      cost (3, 3);

      popAF();
    }
  else if (!a_free && !p_free && n < 0)
    {
      pushAF();
      cheapMove (ASMOP_A, 0, ASMOP_P, 0, true, true);
      pushAF();

      for (int i = 0; i < 4; i++)
        {
          pointPStack (G.stack.pushed - 4 + i, true, true);
          emit2 ("idxm", "a, p");
          cost (1, 1);
          pointPStack (G.stack.pushed - 4 + n + i, false, true);
          emit2 ("idxm", "p, a");
          cost (1, 1);
        }
      
      emit2 ("mov", "a, sp");
      emit2 ("add", "a, #%d", n);
      emit2 ("mov", "sp, a");
      cost (3, 3);

      popAF();
      cheapMove (ASMOP_P, 0, ASMOP_A, 0, true, true);
      popAF();
    }
  else // Can't use pop af, since it might affect reserved flag bits.
    {
      emit2 ("mov", "a, sp");
      emit2 ("add", "a, #%d", n);
      emit2 ("mov", "sp, a");
      cost (3, 3);
    }

  G.stack.pushed += n;
}

static void
push (const asmop *op, int offset, int size)
{
  wassertl (!(size % 2) && (op->type == AOP_DIR || op->type == AOP_LIT || op->type == AOP_IMMD || op->type == AOP_STK), "Unimplemented push operand");

  // Save old stack pointer
  emit2 ("mov", "a, sp");
  emit2 ("mov", "p, a");
  cost (2, 2);

  adjustStack (size, true, false);

  // Write value onto stack
  for (int i = offset; i < offset + size; i++)
    {
      cheapMove (ASMOP_A, 0, op, i, true, true);
      emit2 ("idxm", "p, a");
      cost (1, 2);
      if (i + 1 < offset + size)
        {
          emit2 ("inc", "p");
          cost (1, 1);
        }
    }
}

/*-----------------------------------------------------------------*/
/* genMove_o - Copy part of one asmop to another                   */
/*-----------------------------------------------------------------*/
static void
genMove_o (asmop *result, int roffset, asmop *source, int soffset, int size, bool a_dead_global)
{
  // Handle I/O first.
  wassert_bt ((result->type == AOP_SFR) + (source->type == AOP_SFR) <= 1);
  if (result->type == AOP_SFR || source->type == AOP_SFR)
    switch (size)
      {
      case 1:
        cheapMove (result, roffset, source, soffset, a_dead_global, true);
        return;
      case 2:
        if (result->type == AOP_SFR && source->type == AOP_DIR)
          {
            emit2 ("stt16", "%s", aopGet (source, soffset));
            cost (1, 1); // TODO: Really just 1 cycle? Other 16-bit-transfer instructions use 2.
          }
        else if (result->type == AOP_DIR && source->type == AOP_SFR)
          {
            emit2 ("ldt16", "%s", aopGet (result, roffset));
            cost (1, 1); // TODO: Really just 1 cycle? Other 16-bit-transfer instructions use 2.
          }
        else if (regalloc_dry_run)
          cost (1000, 1000);
        else
          wassertl (0, "Unimplemenetd operand in __sfr16 access");
        return;
      default:
        wassertl (0, "Unknown __sfr size");
      }

  wassert_bt (result->type == AOP_DIR || result->type == AOP_REG || result->type == AOP_STK);
  wassert_bt (source->type == AOP_LIT || source->type == AOP_IMMD || source->type == AOP_DIR || source->type == AOP_REG || source->type == AOP_STK);

  if (size == 2 &&
    (aopInReg (source, soffset, A_IDX) && aopInReg (source, soffset + 1, P_IDX) && aopInReg (result, roffset, P_IDX) && aopInReg (result, roffset + 1, A_IDX) ||
    aopInReg (source, soffset, P_IDX) && aopInReg (source, soffset + 1, A_IDX) && aopInReg (result, roffset, A_IDX) && aopInReg (result, roffset + 1, P_IDX)))
    {
      emit2 ("xch", "a, p");
      cost (1, 1);
      return;
    }

  for (unsigned int i = 0; i < size; i++)
    cheapMove (result, roffset + i, source, soffset + i, a_dead_global, true);
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
/* genNot - generates code for !                                   */
/*-----------------------------------------------------------------*/
static void
genNot (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);

  aopOp (left, ic);
  aopOp (result, ic);

  cheapMove (ASMOP_A, 0, left->aop, 0, true, true);
  for (int i = 1; i < left->aop->size; i++)
    {
      emit2 ("or", aopGet (left->aop, i));
      cost (1, 1);
    }
  emit2 ("sub", "a, #0x01");
  emit2 ("mov", "a, #0x00");
  emit2 ("src", "a");

  cheapMove (result->aop, 0, ASMOP_A, 0, true, true);
  genMove_o (result->aop, 1, ASMOP_ZERO, 0, result->aop->size - 1, true);

  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genCpl - generate code for complement                           */
/*-----------------------------------------------------------------*/
static void
genCpl (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);

  aopOp (left, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  genMove (result->aop, left->aop, true);

  for(int i = 0; i < size; i++)
    {
      emit2("not", "%s", aopGet (result->aop, i));
      cost (1, 1);
    }

  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genSub - generates code for subtraction                         */
/*-----------------------------------------------------------------*/
static void
genSub (const iCode *ic, asmop *result_aop, asmop *left_aop, asmop *right_aop)
{
  int size = result_aop->size;

  bool started = false;
  for (int i = 0; i < size; i++)
    {
      if (!started && right_aop->type == AOP_LIT && aopIsLitVal (right_aop, i, 1, 0x00))
        {
          cheapMove (result_aop, i, left_aop, i, true, true);
          continue;
        }
      else if (!started && aopIsLitVal (left_aop, i, 1, 0x00))
        {
          cheapMove (ASMOP_A, 0, right_aop, i, true, true);
          emit2 ("neg", "a");
          cost (1, 1);
          started = true;
        }
      else if (!started && aopIsLitVal (right_aop, i, 1, 0x01) && aopSame (left_aop, i, result_aop, i, 1))
        {
          emit2 ("dec", "%s", aopGet (left_aop, i));
          cost (1, 1);
          started = true;
          continue;
        }
      else if (!started && aopIsLitVal (right_aop, i, 1, 0xff) && aopSame (left_aop, i, result_aop, i, 1))
        {
          emit2 ("inc", "%s", aopGet (left_aop, i));
          cost (1, 1);
          started = true;
          continue;
        }
      else if (right_aop->type == AOP_STK)
        {
          cheapMove (ASMOP_A, 0, left_aop, i, true, !started);
          cheapMove (ASMOP_P, 0, right_aop, i, false, !started);
          emit2 (started ? "sub" : "subc", "a, p");
          cost (1, 1);
          started = true;
        }
      else if (started && right_aop->type == AOP_LIT && !aopIsLitVal (right_aop, i, 1, 0x00))
        {
          cheapMove (ASMOP_P, 0, right_aop, i, !aopInReg (left_aop, i, A_IDX), false);
          cheapMove (ASMOP_A, 0, left_aop, i, true, false);
          emit2 ("subc", "a, p");
          cost (1, 1);
        }
      else
        {
          cheapMove (ASMOP_A, 0, left_aop, i, true, !started);
          if (started || !aopIsLitVal (right_aop, i, 1, 0x00))
            {
              if (started && aopIsLitVal (right_aop, i, 1, 0x00))
                emit2 ("subc", "a");
              else
                emit2 (started ? "sub" : "subc", "a, %s", aopGet (right_aop, i));
              cost (1, 1);
              started = true;
            }
        }
      if (i + 1 < size && aopInReg (result_aop, i, P_IDX) && (left_aop->type == AOP_STK || right_aop->type == AOP_STK))
        if (regalloc_dry_run)
          cost (1000, 1000);
        else
          wassertl (0, "Unimplemented p result in subtraction with stack operand");
      cheapMove (result_aop, i, ASMOP_A, 0, true, i + 1 == size);
    }
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
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);

  aopOp (left, ic);
  aopOp (result, ic);

  if (IS_FLOAT (operandType (left)))
    {
      genUminusFloat (ic);
      return;
    }

  D (emit2 ("; genUminus", ""));

  genSub (ic, result->aop, ASMOP_ZERO, left->aop);

  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genIpush - generate code for pushing this gets a little complex */
/*-----------------------------------------------------------------*/
static void
genIpush (const iCode *ic)
{
  operand *left = IC_LEFT (ic);

  aopOp (left, ic);

  D (emit2 ("; genIPush", ""));

  if (!ic->parmPush)
    wassertl (0, "Encountered an unsupported spill push.");

  wassertl (left->aop->size == 1 || !(left->aop->size % 2), "Unimplemented operand size for parameter push");

  if (left->aop->size == 1)
    {
      cheapMove (ASMOP_A, 0, left->aop, 0, true, true);
      pushAF();
    }
  else
    push (left->aop, 0, left->aop->size);

  freeAsmop (IC_LEFT (ic));
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

  if (ic->op == PCALL)
    {
      operand *left = IC_LEFT (ic);

      aopOp (left, ic);

      // Push return address
      symbol *tlbl = (regalloc_dry_run ? 0 : newiTempLabel (NULL));

      if (!regalloc_dry_run)
        {
          emit2 ("ld", "a, sp");
          emit2 ("ld", "p, a");
          emit2 ("push", "af");
          emit2 ("ld", "a, #(!tlabel)", labelKey2num (tlbl->key));
          emit2 ("idxm", "p, a");
          emit2 ("inc", "p");
          emit2 ("ld", "a, #(!tlabel >> 8)", labelKey2num (tlbl->key));
          emit2 ("idxm", "p, a");
          G.p.type = AOP_INVALID;
        }
      cost (8, 10);

      // Jump to function
      push (left->aop, 0, 2);
      emit2 ("ret", "");
      G.stack.pushed -= 2;
      cost (2, 1);

      emitLabel (tlbl);

      freeAsmop (left);
    }
  else
    {
      if (IS_LITERAL (etype))
        emit2 ("call", "0x%04X", ulFromVal (OP_VALUE (IC_LEFT (ic))));
      else
        {
          bool jump = (!ic->parmBytes && IFFUNC_ISNORETURN (OP_SYMBOL (IC_LEFT (ic))->type));
          emit2 (jump ? "goto" : "call", "%s",
                 (OP_SYMBOL (IC_LEFT (ic))->rname[0] ? OP_SYMBOL (IC_LEFT (ic))->rname : OP_SYMBOL (IC_LEFT (ic))->name));
        }
      cost (1, 2);
    }

  bool SomethingReturned = (IS_ITEMP (IC_RESULT (ic)) &&
                       (OP_SYMBOL (IC_RESULT (ic))->nRegs || OP_SYMBOL (IC_RESULT (ic))->spildir))
                       || IS_TRUE_SYMOP (IC_RESULT (ic));

  if (!SomethingReturned)
    adjustStack (-ic->parmBytes, true, true);
  else
    {
      aopOp (IC_RESULT (ic), ic);

      wassertl (IC_RESULT (ic)->aop->size <= 2, "Unimplemented call to function returning more than 2 bytes");
      cheapMove (IC_RESULT (ic)->aop, 0, ASMOP_A, 0, true, true);
      if (IC_RESULT (ic)->aop->size > 1)
        cheapMove (IC_RESULT (ic)->aop, 1, ASMOP_P, 0, true, true);

      adjustStack (-ic->parmBytes, !(aopInReg (IC_RESULT (ic)->aop, 0, A_IDX) || aopInReg (IC_RESULT (ic)->aop, 1, A_IDX)), !(aopInReg (IC_RESULT (ic)->aop, 0, P_IDX) || aopInReg (IC_RESULT (ic)->aop, 1, P_IDX)));

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

  G.stack.pushed = 0;
  G.stack.param_offset = 0;

  /* create the function header */
  emit2 (";", "-----------------------------------------");
  emit2 (";", " function %s", sym->name);
  emit2 (";", "-----------------------------------------");

  D (emit2 (";", pdk_assignment_optimal ? "Register assignment is optimal." : "Register assignment might be sub-optimal."));

  emit2 ("", "%s:", sym->rname);
  if (!regalloc_dry_run)
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

  adjustStack (sym->stack, true, true);
}

/*-----------------------------------------------------------------*/
/* genEndFunction - generates epilogue for functions               */
/*-----------------------------------------------------------------*/
static void
genEndFunction (iCode *ic)
{
  symbol *sym = OP_SYMBOL (IC_LEFT (ic));
  int retsize = getSize (sym->type->next);

  D (emit2 ("; genEndFunction", ""));

  if (IFFUNC_ISNAKED(sym->type))
  {
      D (emit2 (";", "naked function: no epilogue."));
      if (options.debug && currFunc && !regalloc_dry_run)
        debugFile->writeEndFunction (currFunc, ic, 0);
      return;
  }

  /* adjust the stack for the function */
  if (sym->stack)
    adjustStack (-sym->stack, retsize == 0 || retsize > 2, retsize != 1);

  wassertl (!G.stack.pushed, "Unbalanced stack.");

  /* if debug then send end of function */
  if (options.debug && currFunc && !regalloc_dry_run)
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

  wassertl (left->aop->size <= 2, "return of value wider than 2 bytes not yet implemented");
  wassertl (currFunc, "return iCode outside of function");

  if (left->aop->size == 2 && aopInReg (left->aop, 0, P_IDX) && aopInReg (left->aop, 1, A_IDX))
    {
      emit2 ("xch", "a, p");
      cost(1, 1);
      goto end;
    }
  else if (left->aop->size == 2 && left->aop->type == AOP_STK)
    {
      cheapMove (ASMOP_A, 0, left->aop, 0, true, true);
      cheapMove (ASMOP_P, 0, left->aop, 1, false, true);
      goto end;
    }

  if (left->aop->size > 1)
    cheapMove (ASMOP_P, 0, left->aop, 1, true, true);
  if ((left->aop->type == AOP_LIT || left->aop->type == AOP_IMMD) && !currFunc->stack)
    {
      emit2 ("ret", "%s", aopGet (left->aop, 0));
      cost (1, 2);
      freeAsmop (left);
      return;
    }
  cheapMove (ASMOP_A, 0, left->aop, 0, true, true);

end:
  freeAsmop (left);  

jumpret:
  /* generate a jump to the return label
     if the next is not the return statement */
  if (!(ic->next && ic->next->op == LABEL && IC_LABEL (ic->next) == returnLabel))
    if (!currFunc->stack)
    {
      emit2 ("ret", "");
      cost (2, 1);
    }
    else
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

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  /* Swap if left is literal or right is in A. */
  if (left->aop->type == AOP_LIT || aopInReg (right->aop, 0, A_IDX) || right->aop->type == AOP_STK && !aopInReg (left->aop, 0, A_IDX))
    {
      operand *t = right;
      right = left;
      left = t;
    }

  bool started = false;
  for (int i = 0; i < size; i++)
    {
      if (!started && right->aop->type == AOP_LIT && aopIsLitVal (right->aop, i, 1, 0x00))
        {
          cheapMove (result->aop, i, left->aop, i, true, true);
          continue;
        }
      else if (right->aop->type == AOP_STK)
        {
          cheapMove (ASMOP_A, 0, left->aop, i, true, !started);
          cheapMove (ASMOP_P, 0, right->aop, i, false, !started);
          emit2 (started ? "addc" : "add", "a, p");
          cost (1, 1);
          started = true;
        }
      else if (started && right->aop->type == AOP_LIT && !aopIsLitVal (right->aop, i, 1, 0x00))
        {
          cheapMove (ASMOP_P, 0, right->aop, i, !aopInReg (left->aop, i, A_IDX), false);
          cheapMove (ASMOP_A, 0, left->aop, i, true, false);
          emit2 ("addc", "a, p");
          cost (1, 1);
        }
       else if (!started && (right->aop->type == AOP_DIR || right->aop->type == AOP_REGDIR || right->aop->type == AOP_REG) && aopIsLitVal (right->aop, i, 1, 0x01) && aopSame (left->aop, i, result->aop, i, 1))
        {
          emit2 ("inc", "%s", aopGet (left->aop, i));
          cost (1, 1);
          started = true;
          continue;
        }
       else if (!started && (right->aop->type == AOP_DIR || right->aop->type == AOP_REGDIR || right->aop->type == AOP_REG) && aopIsLitVal (right->aop, i, 1, 0xff) && aopSame (left->aop, i, result->aop, i, 1))
        {
          emit2 ("dec", "%s", aopGet (left->aop, i));
          cost (1, 1);
          started = true;
          continue;
        }
      else
        {
          cheapMove (ASMOP_A, 0, left->aop, i, true, !started);
          if (started || !aopIsLitVal (right->aop, i, 1, 0x00))
            {
              if (started && aopIsLitVal (right->aop, i, 1, 0x00))
                emit2 ("addc", "a");
              else
                emit2 (started ? "addc" : "add", "a, %s", aopGet (right->aop, i));
              cost (1, 1);
              started = true;
            }
        }
      if (i + 1 < size && aopInReg (result->aop, i, P_IDX) && (left->aop->type == AOP_STK || right->aop->type == AOP_STK))
        if (regalloc_dry_run)
          cost (1000, 1000);
        else
          wassertl (0, "Unimplemented p result in subtraction with stack operand");
      cheapMove (result->aop, i, ASMOP_A, 0, true, i + 1 == size);
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

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  genSub (ic, result->aop, left->aop, right->aop);

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genCmp :- greater or less than comparison                       */
/*-----------------------------------------------------------------*/
static void
genCmp (const iCode *ic, iCode *ifx)
{
  operand *left, *right, *result;

  D (emit2 ("; genCmp", ""));

  result = IC_RESULT (ic);
  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = max (left->aop->size, right->aop->size);
  bool sign = false;
  if (IS_SPEC (operandType (left)) && IS_SPEC (operandType (right)))
    sign = !(SPEC_USIGN (operandType (left)) | SPEC_USIGN (operandType (right)));

  // Non-destructive 1-byte unsigned comparison.
  if (!sign && size == 1 && ifx && aopInReg (left->aop, 0, A_IDX))
    {
      if (ic->op == '>' && right->aop->type == AOP_LIT)
        {
          wassert (!aopIsLitVal (right->aop, 0, 1, 0x00));
          emit2 ("ceqsn", "a, #0x%02x", byteOfVal (right->aop->aopu.aop_lit, 0) - 1);
          if (IC_TRUE (ifx))
            {
              emit2 ("nop", "");
              emit2 ("t1sn", "f.c");
              cost (3, 3.5);
            }
          else
            {
              emit2 ("t0sn", "f.c");
              cost (2, 2.5);
            }
          emitJP (IC_FALSE (ifx) ? IC_FALSE (ifx) : IC_TRUE (ifx), 0.5f);
          goto release;
        }
      else if (ic->op == '<' && aopInReg (left->aop, 0, A_IDX) && (right->aop->type == AOP_LIT || right->aop->type == AOP_DIR) && (IC_TRUE (ifx) || !regDead (A_IDX, ic)))
        {
          emit2 ("ceqsn", "a, %s", aopGet (right->aop, 0));
          if (IC_TRUE (ifx))
            {
              emit2 ("t1sn", "f.c");
              cost (2, 2.5);
            }
          else
            {
              emit2 ("nop", "");
              emit2 ("t0sn", "f.c");
              cost (3, 3.5);
            }
          emitJP (IC_FALSE (ifx) ? IC_FALSE (ifx) : IC_TRUE (ifx), 0.5f);
          goto release;
        }
    }

  if (ic->op == '>')
    {
      operand *t = right;
      right = left;
      left = t;  
    }

  for (int i = 0; i < size; i++)
    {
      if (!i && aopIsLitVal (left->aop, i, 1, 0x00) && aopInReg (right->aop, i, A_IDX))
        {
          emit2 ("neg", "a");
          cost (1, 1);
        }
      else if (i && right->aop->type == AOP_LIT && !aopIsLitVal (right->aop, i, 1, 0x00)) // Work around lack of subc a, #nn.
        {
          cheapMove (ASMOP_P, 0, right->aop, i, true, !i);
          cheapMove (ASMOP_A, 0, left->aop, i, true, !i);
          emit2 ("subc", "a, p");
          cost (1, 1);
        }
      else if (right->aop->type == AOP_STK)
        {
          cheapMove (ASMOP_A, 0, left->aop, i, true, !i);
          cheapMove (ASMOP_P, 0, right->aop, i, false, !i);
          emit2 ("subc", "a, p");
          cost (1, 1);
        }
      else
        {
          cheapMove (ASMOP_A, 0, left->aop, i, true, !i);
          emit2 (i ? "subc" : "sub", "a, %s", aopGet (right->aop, i));
          cost (1, 1);
        }
    }

  if (sign)
    {
      emit2 ("t0sn", "f.ov");
      emit2 ("xor", "a, #0x80");
      emit2 ("sl", "a");
      cost (3, 3);
    }

  if (ifx)
    {
      emit2 ((ic->op == '<') ^ (bool)(IC_FALSE(ifx)) ? "t1sn" : "t0sn", "f.c");
      cost (1, 1.5);
      emitJP (IC_FALSE (ifx) ? IC_FALSE (ifx) : IC_TRUE (ifx), 0.5f);
    }
  else
    {
      emit2 ("mov", "a, #0x00");
      emit2 ("slc", "a");
      cost (2, 2);
      cheapMove (result->aop, 0, ASMOP_A, 0, true, true);
    }

release:
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

  symbol *lbl_ne = 0;
  symbol *endlbl = 0;

  if (ifx)
    {
      if ((ic->op == EQ_OP) ^ (bool)(IC_FALSE (ifx)))
        lbl_ne = regalloc_dry_run ? 0 : newiTempLabel (NULL);
      else
        lbl_ne = IC_FALSE (ifx) ? IC_FALSE (ifx) : IC_TRUE (ifx);
    }
  else if (!regalloc_dry_run)
    {
      lbl_ne = newiTempLabel (NULL);
      endlbl = newiTempLabel (NULL);
    }

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

      cheapMove (ASMOP_A, 0, left->aop, i, true, true);

      if (ifx && i + 1 == size && ((ic->op == EQ_OP) ^ (bool)(IC_FALSE(ifx))))
        {
          emit2 ("cneqsn", "a, %s", aopGet (right->aop, i));
          cost (1, 1);
          emitJP (IC_FALSE (ifx) ? IC_FALSE (ifx) : IC_TRUE (ifx), 0.0f);
        }
      else
        {
          emit2 ("ceqsn", "a, %s", aopGet (right->aop, i));
          cost (1, 1);
          emitJP(lbl_ne, 0.0f);
        }
    }

  if (ifx) // Jump condition only.
    {
      if ((ic->op == EQ_OP) ^ (bool)(IC_FALSE(ifx)))
        emitLabel (lbl_ne);
    }
  else // Needs result
    {
      cheapMove (result->aop, 0, ic->op == EQ_OP ? ASMOP_ONE : ASMOP_ZERO, 0, true, true);
      emitJP(endlbl, 0.0f);
      emitLabel (lbl_ne);
      cheapMove (result->aop, 0, ic->op == NE_OP ? ASMOP_ONE : ASMOP_ZERO, 0, true, true);
      emitLabel (endlbl);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genOr - code for or                                             */
/*-----------------------------------------------------------------*/
static void
genXor (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genXor", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  /* Swap if left is literal or right is in A. */
  if (left->aop->type == AOP_LIT || aopInReg (right->aop, 0, A_IDX) || right->aop->type == AOP_STK)
    {
      operand *t = right;
      right = left;
      left = t;
    }

  for (int i = 0; i < size; i++)
    {
      cheapMove (ASMOP_A, 0, left->aop, i, true, !i);

      emit2 ("xor", "a, %s", aopGet (right->aop, i));

      cheapMove (result->aop, i, ASMOP_A, 0, true, i + 1 == size);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genOr - code for or                                             */
/*-----------------------------------------------------------------*/
static void
genOr (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genOr", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  /* Swap if left is literal or right is in A. */
  if (left->aop->type == AOP_LIT || aopInReg (right->aop, 0, A_IDX)  || right->aop->type == AOP_STK)
    {
      operand *t = right;
      right = left;
      left = t;
    }

  for (int i = 0; i < size; i++)
    {
      cheapMove (ASMOP_A, 0, left->aop, i, true, !i);

      emit2 ("or", "a, %s", aopGet (right->aop, i));

      cheapMove (result->aop, i, ASMOP_A, 0, true, i + 1 == size);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genAnd - code for and                                           */
/*-----------------------------------------------------------------*/
static void
genAnd (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genAnd", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  /* Swap if left is literal or right is in A. */
  if (left->aop->type == AOP_LIT || aopInReg (right->aop, 0, A_IDX) || right->aop->type == AOP_STK)
    {
      operand *t = right;
      right = left;
      left = t;
    }

  for (int i = 0; i < size; i++)
    {
      cheapMove (ASMOP_A, 0, left->aop, i, true, !i);

      emit2 ("and", "a, %s", aopGet (right->aop, i));

      cheapMove (result->aop, i, ASMOP_A, 0, true, i + 1 == size);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genLeftShift - generates code for right shifting               */
/*-----------------------------------------------------------------*/
static void
genLeftShift (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genLeftShift", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  genMove (result->aop, left->aop, !aopInReg (right->aop, 0, A_IDX));

  if (right->aop->type == AOP_LIT)
    {
      int shCount = ulFromVal (right->aop->aopu.aop_lit);

      bool loop = (shCount > 2 + (size == 1) * 2 + optimize.codeSpeed) && !(size == 1 && aopInReg (result->aop, 0, A_IDX));
      symbol *tlbl = (!loop || regalloc_dry_run) ? 0 : newiTempLabel (0);

      if (loop)
        {
          cheapMove (ASMOP_A, 0, right->aop, 0, true, true);
          emitLabel (tlbl);
          regalloc_dry_run_cycle_scale = shCount;
          shCount = 1;
        }

      while (shCount)
        {
          if (shCount >= 4 && size == 1 && aopInReg (result->aop, 0, A_IDX))
            {
              emit2 ("swap", "a");
              emit2 ("and", "a, 0xf0");
              cost (2, 2);
              shCount -= 4;
              continue;
            }
          
          for (int i = 0; i < size; i++)
            {
              emit2(i ? "slc" : "sl", "%s", aopGet (result->aop, i));
              cost (1, 1);
            }
          shCount--;
        }

      if (loop)
        {
          emit2 ("dzsn", "a");
          if (!regalloc_dry_run)
            emit2 ("goto", "!tlabel", labelKey2num (tlbl->key));
          cost (2, 2);
        }

      regalloc_dry_run_cycle_scale = 0;
    }
  else
    {
      symbol *tlbl1 = regalloc_dry_run ? 0 : newiTempLabel (0);
      symbol *tlbl2 = regalloc_dry_run ? 0 : newiTempLabel (0);
    
      cheapMove (ASMOP_A, 0, right->aop, 0, true, true);
      emit2 ("inc", "a");
      cost (1, 1);
      emitLabel (tlbl1);
      emit2 ("dzsn", "a");
      if (!regalloc_dry_run)
        emit2 ("goto", "!tlabel", labelKey2num (tlbl2->key));
      cost (2, 2);
    
      for(int i = 0; i < size; i++)
        {
          emit2(i ? "slc" : "sl", "%s", aopGet (result->aop, i));
          cost (1, 1);
        }
    
      if (!regalloc_dry_run)
        emit2 ("goto", "!tlabel", labelKey2num (tlbl1->key));
      cost (1, 1);

      emitLabel (tlbl2);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genRightShift - generates code for right shifting               */
/*-----------------------------------------------------------------*/
static void
genRightShift (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genRightShift", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  int size = result->aop->size;

  genMove (result->aop, left->aop, !aopInReg (right->aop, 0, A_IDX));

  if (right->aop->type == AOP_LIT)
    {
      int shCount = ulFromVal (right->aop->aopu.aop_lit);

      bool loop = (shCount > 2 + (size == 1) * 2 + optimize.codeSpeed) && !(size == 1 && aopInReg (result->aop, 0, A_IDX));
      symbol *tlbl = (!loop || regalloc_dry_run) ? 0 : newiTempLabel (0);

      if (loop)
        {
          cheapMove (ASMOP_A, 0, right->aop, 0, true, true);
          emitLabel (tlbl);
          regalloc_dry_run_cycle_scale = shCount;
          shCount = 1;
        }

      while (shCount)
        {
          if (SPEC_USIGN (getSpec (operandType (left))) && shCount >= 4 && size == 1 && aopInReg (result->aop, 0, A_IDX))
            {
              emit2 ("swap", "a");
              emit2 ("and", "a, 0x0f");
              cost (2, 2);
              shCount -= 4;
              continue;
            }
              
          // Padauk has no arithmetic right shift instruction.
          // So we need this 4-instruction sequence here.
          if (!SPEC_USIGN (getSpec (operandType (left))))
            {
              emit2 ("sl", aopGet (result->aop, size - 1));
              emit2 ("addc", aopGet (result->aop, size - 1));
              emit2 ("src", aopGet (result->aop, size - 1));
              emit2 ("src", aopGet (result->aop, size - 1));
              cost (4, 4);
            }
          else
            {
              emit2("sr", aopGet (result->aop, size - 1));
              cost (1, 1);
            }
        
          for(int i = size - 2; i >= 0; i--)
            {
              emit2 ("src", "%s", aopGet (result->aop, i));
              cost (1, 1);
            }
          shCount--;
        }

      if (loop)
        {
          emit2 ("dzsn", "a");
          if (!regalloc_dry_run)
            emit2 ("goto", "!tlabel", labelKey2num (tlbl->key));
          cost (2, 2);
        }

      regalloc_dry_run_cycle_scale = 0;
    }
  else
    {
      symbol *tlbl1 = regalloc_dry_run ? 0 : newiTempLabel (0);
      symbol *tlbl2 = regalloc_dry_run ? 0 : newiTempLabel (0);
    
      cheapMove (ASMOP_A, 0, right->aop, 0, true, true);
      emit2 ("inc", "a");
      cost (1, 1);
      emitLabel (tlbl1);
      emit2 ("dzsn", "a");
      if (!regalloc_dry_run)
        emit2 ("goto", "!tlabel", labelKey2num (tlbl2->key));
      cost (2, 2);
        
      // Padauk has no arithmetic right shift instruction.
      // So we need this 4-instruction sequence here.
      if (!SPEC_USIGN (getSpec (operandType (left))))
        {
          emit2 ("sl", aopGet (result->aop, size - 1));
          emit2 ("addc", aopGet (result->aop, size - 1));
          emit2 ("src", aopGet (result->aop, size - 1));
          emit2 ("src", aopGet (result->aop, size - 1));
          cost (4, 4);
        }
      else
        {
          emit2("sr", aopGet (result->aop, size - 1));
          cost (1, 1);
        }
    
      for(int i = size - 2; i >= 0; i--)
        {
          emit2 ("src", "%s", aopGet (result->aop, i));
          cost (1, 1);
        }
    
      if (!regalloc_dry_run)
        emit2 ("goto", "!tlabel", labelKey2num (tlbl1->key));
      cost (1, 1);

      emitLabel (tlbl2);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genPointerGet - generate code for pointer get                   */
/*-----------------------------------------------------------------*/
static void
genPointerGet (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genPointerGet", ""));

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  wassertl (right, "GET_VALUE_AT_ADDRESS without right operand");
  wassertl (IS_OP_LITERAL (right), "GET_VALUE_AT_ADDRESS with non-literal right operand");

  bool bit_field = IS_BITVAR (getSpec (operandType (result)));
  int size = result->aop->size;

  wassertl (!bit_field, "Unimplemented read of bit-field");
  wassertl (aopIsLitVal (right->aop, 0, 2, 0x0000), "Unimplemented nonzero right operand in pointer read");

  if (left->aop->type == AOP_DIR || left->aop->type == AOP_LIT || left->aop->type == AOP_IMMD)
    {
      for (int i = 0; i < size; i++)
        {
          emit2 ("idxm", "a, %s", aopGet (left->aop, 0));
          cost (1, 2);
          cheapMove (result->aop, i, ASMOP_A, 0, true, true);
          if (i + 1 != size)
            {
              emit2 ("inc", "%s", aopGet (left->aop, 0));
              cost (1, 1);
            }
        }
      for (int i = 1; i < size; i++)
        {
          emit2 ("dec", "%s", aopGet (left->aop, 0));
          cost (1, 1);
        }
    }
  else
    {
      if (!regDead (P_IDX,ic))
        {
          wassert (regalloc_dry_run);
          cost (1000, 1000);
        }
      cheapMove (ASMOP_P, 0, left->aop, 0, true, true);
      G.p.type = AOP_INVALID;
      for (int i = 0; i < size; i++)
        {
          emit2 ("idxm", "a, p");
          cost (1, 2);
          cheapMove (result->aop, i, ASMOP_A, 0, true, true);
          if (i + 1 != size)
            {
              emit2 ("inc", "p");
              cost (1, 1);
            }
        }
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genPointerSet - stores the value into a pointer location        */
/*-----------------------------------------------------------------*/
static void
genPointerSet (iCode *ic)
{
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genPointerSet", ""));

  aopOp (left, ic);
  aopOp (right, ic);

  bool bit_field = IS_BITVAR (getSpec (operandType (right))) || IS_BITVAR (getSpec (operandType (left)));
  int size = right->aop->size;

  wassertl (!bit_field, "Unimplemented write of bit-field");

  if (left->aop->type == AOP_DIR || left->aop->type == AOP_LIT || left->aop->type == AOP_IMMD)
    {
      for (int i = 0; i < size; i++)
        {
          cheapMove (ASMOP_A, 0, right->aop, i, true, true);
          emit2 ("idxm", "%s, a", aopGet (left->aop, 0));
          cost (1, 2);
          if (i + 1 != size)
            {
              emit2 ("inc", "%s", aopGet (left->aop, 0));
              cost (1, 1);
            }
        }
      for (int i = 1; i < size; i++)
        {
          emit2 ("dec", "%s", aopGet (left->aop, 0));
          cost (1, 1);
        }
    }
  else if (right->aop->type == AOP_STK)
    {
      if (!regDead (P_IDX, ic))
        {
          wassert (regalloc_dry_run);
          cost (1000, 1000);
        }
      for (int i = 0; i < size; i++)
        {
          cheapMove (ASMOP_A, 0, right->aop, i, true, true);
          cheapMove (ASMOP_P, 0, left->aop, 0, false, true);
          for (int j = 0; j < i; j++)
            {
              emit2 ("inc", "p");
              cost (1, 1);
            }
          emit2 ("idxm", "p, a");
          cost (1, 2);
        }
    }
  else
    {
      if (!regDead (P_IDX, ic))
        {
          wassert (regalloc_dry_run);
          cost (1000, 1000);
        }
      cheapMove (ASMOP_P, 0, left->aop, 0, true, true);
      G.p.type = AOP_INVALID;
      for (int i = 0; i < size; i++)
        {
          cheapMove (ASMOP_A, 0, right->aop, i, true, true);
          emit2 ("idxm", "p, a");
          cost (1, 2);
          if (i + 1 != size)
            {
              emit2 ("inc", "p");
              cost (1, 1);
            }
        }
    }

  freeAsmop (right);
  freeAsmop (left);
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

  D (emit2 ("; genIfx", ""));

  aopOp (cond, ic);

  cheapMove (ASMOP_A, 0, cond->aop, 0, true, true);

  for (int i = 1; i < cond->aop->size; i++)
    {
      if (cond->aop->type == AOP_STK)
        {
          pushAF ();
          cheapMove (ASMOP_P, 0, cond->aop, i, true, true);
          popAF ();
          emit2 ("or", "a, p");
          cost (1, 1);
        }
      else
        {
          emit2 ("or", "a, %s", aopGet (cond->aop, i));
          cost (1, 1);
        }
    }
 
  emit2 (IC_FALSE (ic) ? "cneqsn" : "ceqsn", "a, #0x00");
  cost (1, 1); 
  emitJP (IC_FALSE (ic) ? IC_FALSE (ic) : IC_TRUE (ic), 0.0f);

  freeAsmop (cond);
}

/*-----------------------------------------------------------------*/
/* genAddrOf - generates code for address of                       */
/*-----------------------------------------------------------------*/
static void
genAddrOf (const iCode *ic)
{
  operand *result, *left, *right;

  D (emit2 ("; genAddrOf", ""));

  result = IC_RESULT (ic);
  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);

  wassert (result);
  wassert (left);
  wassert (IS_TRUE_SYMOP (left));
  wassert (right && IS_OP_LITERAL (IC_RIGHT (ic)));

  const symbol *sym = OP_SYMBOL_CONST (left);
  wassert (sym);

  aopOp (result, ic);

  int size = result->aop->size;

  wassert (size == 2);

  if (sym->onStack)
    {
      int s = sym->stack + (sym->stack > 0 ? G.stack.param_offset : 0) + operandLitValue (right);

      if (G.p.type == AOP_STK && s == G.p.offset)
        cheapMove (result->aop, 0, ASMOP_P, 0, true, true);
      else
        {
          cheapMove (ASMOP_A, 0, ASMOP_SP, 0, true, true);
          emit2 ("add", "a, #0x%02x", (s - G.stack.pushed) & 0xff);
          cost (1, 1);
          cheapMove (result->aop, 0, ASMOP_A, 0, true, true);
        }

      cheapMove (result->aop, 1, ASMOP_ZERO, 0, true, true);
    }
  else
    {
      wassertl (!operandLitValue (right), "Unimplemented offset for &");
  

      emit2 ("mov", "a, #<(%s + %d)", sym->rname, 0);
      cost (1, 1);
      cheapMove (result->aop, 0, ASMOP_A, 0, true, true);
      emit2 ("mov", "a, #>(%s + %d)", sym->rname, 0);
      cheapMove (result->aop, 1, ASMOP_A, 0, true, true);
    }

  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genJumpTab - generate code for jump table                       */
/*-----------------------------------------------------------------*/
static void
genJumpTab (const iCode *ic)
{
  operand *cond;

  D (emit2 ("; genJumpTab", ""));

  cond = IC_JTCOND (ic);

  aopOp (cond, ic);

  wassertl (cond->aop->size == 1, "Jump table not implemented for operands wider than 1 byte.");

  cheapMove (ASMOP_A, 0, cond->aop, 0, true, true);

  emit2 ("add", "a, #0x02");
  emit2 ("pcadd", "a");
  cost (2, 3);

  for (symbol *jtab = setFirstItem (IC_JTLABELS (ic)); jtab; jtab = setNextItem (IC_JTLABELS (ic)))
    {
      if (!regalloc_dry_run)
        emit2 ("goto", "#!tlabel", labelKey2num (jtab->key));
      cost (1, 0);
    }

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

      cheapMove (ASMOP_A, 0, right->aop, 0, true, true);

      for(offset = 1; offset < size; offset++)
        {
          emit2 ("or", "a, %s", aopGet (right->aop, offset));
          cost (1, 1);
        }

      emit2 ("ceqsn", "a, #0x00");
      emit2 ("mov", "a, #0x01");
      cost (2, 2);

      cheapMove (result->aop, 0, ASMOP_A, 0, true, true);
    }
  else // Cast to signed type
    {
      genMove_o (result->aop, 0, right->aop, 0, right->aop->size, regDead (A_IDX, ic));

      int size = result->aop->size - right->aop->size;
      offset = right->aop->size;

      cheapMove (ASMOP_A, 0, result->aop, right->aop->size - 1, true, true);
      emit2 ("sl", "a");
      emit2 ("mov", "a, #0x00");
      emit2 ("subc", "a");
      cost (3, 3);

      while (size--)
        cheapMove (result->aop, offset++, ASMOP_A, 0, true, true);
    }

  freeAsmop (right);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genDummyRead - generate code for dummy read of volatiles        */
/*-----------------------------------------------------------------*/
static void
genDummyRead (const iCode *ic)
{
  operand *op;

  if ((op = IC_LEFT (ic)) && IS_SYMOP (op))
    ;
  else if ((op = IC_RIGHT (ic)) && IS_SYMOP (op))
    ;
  else
    return;

  aopOp (op, ic);

  if (!regDead(A_IDX, ic) && op->aop->type == AOP_DIR && op->aop->size <= 2)
    for (int i = 0; i < op->aop->size; i++)
      {
         emit2 ("ceqsn", "a, %s", aopGet (op->aop, i));
         emit2 ("nop", "");
         cost (2, 2);
      }
  else
    {
      if (!regDead(A_IDX, ic))
        {
          emit2 ("push", "af");
          cost (1, 1);
        }

      for (int i = 0; i < op->aop->size; i++)
        cheapMove (ASMOP_A, 0, op->aop, i, true, true);

      if (!regDead(A_IDX, ic))
        {
          emit2 ("pop", "af");
          cost (1, 1);
        }  
    }

  freeAsmop (op);
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
      genNot (ic);
      break;

    case '~':
      genCpl (ic);
      break;

    case UNARYMINUS:
      genUminus (ic);
      break;

    case IPUSH:
      genIpush (ic);
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
      genCmp (ic, ifxForOp (IC_RESULT (ic), ic));
      break;

    case LE_OP:
    case GE_OP:
      wassertl (0, "Unimplemented iCode");
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
      genXor (ic);
      break;

    case '|':
      genOr (ic);
      break;

    case BITWISEAND:
      genAnd (ic);
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
      genLeftShift (ic);
      break;

    case RIGHT_OP:
      genRightShift (ic);
      break;

    case GET_VALUE_AT_ADDRESS:
      genPointerGet (ic);
      break;

    case SET_VALUE_AT_ADDRESS:
      genPointerSet (ic);
      break;

    case '=':
      wassert (!POINTER_SET (ic));
      genAssign (ic);
      break;

    case IFX:
      genIfx (ic);
      break;

    case ADDRESS_OF:
      genAddrOf (ic);
      break;

    case JUMPTABLE:
      genJumpTab (ic);
      break;

    case CAST:
      genCast (ic);
      break;

    case RECEIVE:
    case SEND:
      wassertl (0, "Unimplemented iCode");
      break;

    case DUMMY_READ_VOLATILE:
      genDummyRead (ic);
      break;

    case CRITICAL:
      wassertl (0, "Unimplemented iCode: Critical section");
      break;

    case ENDCRITICAL:
      wassertl (0, "Unimplemented iCode: Critical section");
      break;

    default:
      fprintf (stderr, "iCode op %d:\n", ic->op);
      wassertl (0, "Unknown iCode");
    }
}

float
dryPdkiCode (iCode *ic)
{
  regalloc_dry_run = true;
  regalloc_dry_run_cost_words = 0;
  regalloc_dry_run_cost_cycles = 0;

  initGenLineElement ();

  genPdkiCode (ic);

  G.p.type = AOP_INVALID;

  destroy_line_list ();

  wassert (regalloc_dry_run);

  const unsigned int word_cost_weight = 2 << (optimize.codeSize * 3 + !optimize.codeSpeed * 3);

  return ((float)regalloc_dry_run_cost_words * word_cost_weight + (float)regalloc_dry_run_cost_cycles * ic->count);
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
  regalloc_dry_run = false;

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

  G.p.type = AOP_INVALID;

  /* destroy the line list */
  destroy_line_list ();
}

