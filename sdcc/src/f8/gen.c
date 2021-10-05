/*-------------------------------------------------------------------------
  gen.c - code generator for F8.

  Copyright (C) 2021, Philipp Klaus Krause krauseph@informatik.uni-freiburg.de)

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
static unsigned int regalloc_dry_run_cost_bytes;
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
}
G;

void
f8_init_asmops (void)
{
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

/*--------------------------------------------------------------------------*/
/* updateCFA - update the debugger information to reflect the current       */
/*             connonical frame address relative to the stack pointer       */
/*--------------------------------------------------------------------------*/
static void
updateCFA (void)
{
  /* there is no frame unless there is a function */
  if (!currFunc)
    return;

  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, &f8_regs[SP_IDX], 1 + G.stack.param_offset + G.stack.pushed);
}

/*---------------------------------------------------------------------*/
/* f8_emitDebuggerSymbol - associate the current code location         */
/*   with a debugger symbol                                            */
/*---------------------------------------------------------------------*/
void
f8_emitDebuggerSymbol (const char *debugSym)
{
  G.debugLine = 1;
  emit2 ("", "%s ==.", debugSym);
  G.debugLine = 0;
}

static void
cost(unsigned int bytes, unsigned int cycles)
{
  regalloc_dry_run_cost_bytes += bytes;
  regalloc_dry_run_cost_cycles += cycles * regalloc_dry_run_cycle_scale;
}

// Get asmop for registers containing the return type of function
// Returns 0 if the function does not have a return value or it is not returned in registers.
static asmop *
aopRet (sym_link *ftype)
{
  wassert (IS_FUNC (ftype));

  return 0;
}

// Get asmop for registers containing a parameter
// Returns 0 if the parameter is passed on the stack
static asmop *
aopArg (sym_link *ftype, int i)
{
  wassert (IS_FUNC (ftype));
    
  return 0;
}

// Return true, iff ftype cleans up stack parameters.
static bool
isFuncCalleeStackCleanup (sym_link *ftype)
{
  wassert (IS_FUNC (ftype));

  return false;
}

/*-----------------------------------------------------------------*/
/* resultRemat - result is to be rematerialized                    */
/*-----------------------------------------------------------------*/
static bool
resultRemat (const iCode *ic)
{
  if (SKIP_IC (ic) || ic->op == IFX)
    return 0;

  if (IC_RESULT (ic) && IS_ITEMP (IC_RESULT (ic)))
    {
      const symbol *sym = OP_SYMBOL_CONST (IC_RESULT (ic));

      if (!sym->remat)
        return(false);

      bool completely_spilt = true;
      for (unsigned int i = 0; i < getSize (sym->type); i++)
        if (sym->regs[i])
          completely_spilt = false;

      if (completely_spilt)
        return(true);
    }

  return (false);
}

/*--------------------------------------------------------------------------*/
/* adjustStack - Adjust the stack pointer by n bytes.                       */
/*--------------------------------------------------------------------------*/
static void
adjustStack (int n)
{
  while (n)
    {
    	int m = n;
    	if (m < -128)
    	  n = -128;
    	else if (m > 127)
    	  m = 127;
    	emit2 ("add", "sp, #%d", m);
    	cost (1, 1);
    	n -= m;
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
  bool bigreturn;

  G.stack.pushed = 0;
  G.stack.param_offset = 0;

  /* create the function header */
  emit2 (";", "-----------------------------------------");
  emit2 (";", " function %s", sym->name);
  emit2 (";", "-----------------------------------------");

  D (emit2 (";", f8_assignment_optimal ? "Register assignment is optimal." : "Register assignment might be sub-optimal."));
  D (emit2 (";", "Stack space usage: %d bytes.", sym->stack));

  emit2 ("", "%s:", sym->rname);
  genLine.lineCurr->isLabel = 1;

  if (IFFUNC_ISNAKED(ftype))
  {
      updateCFA(); //ensure function has at least 1 CFA record
      emit2(";", "naked function: no prologue.");
      return;
  }

  if (IFFUNC_ISCRITICAL (ftype))
      wassert (0);

  
  if (f8_extend_stack) // Setup for extended stack access.
    wassert (!f8_extend_stack);

  bigreturn = (getSize (ftype->next) > 4);
  G.stack.param_offset += bigreturn * 2;

  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, &f8_regs[SP_IDX], 1);

  /* adjust the stack for the function */
  {
    int fadjust = -sym->stack;
    adjustStack (fadjust);
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

  wassert (!regalloc_dry_run);

  if (IFFUNC_ISNAKED(sym->type))
  {
      D (emit2 (";", "naked function: no epilogue."));
      if (options.debug && currFunc && !regalloc_dry_run)
        debugFile->writeEndFunction (currFunc, ic, 0);
      return;
  }

  const bool bigreturn = (getSize (sym->type->next) > 4) || IS_STRUCT (sym->type->next);
  int stackparmbytes = bigreturn * 2;
  for (value *arg = FUNC_ARGS(sym->type); arg; arg = arg->next)
    {
      wassert (arg->sym);
      if (!SPEC_REGPARM (arg->etype))
        stackparmbytes += getSize (arg->sym->type);
    }

  int poststackadjust = isFuncCalleeStackCleanup (sym->type) ? stackparmbytes : 0;

  //bool a_free = !aopRet (sym->type) || aopRet (sym->type)->regs[A_IDX] < 0;
  //bool x_free = !aopRet (sym->type) || (aopRet (sym->type)->regs[XL_IDX] < 0 && aopRet (sym->type)->regs[XH_IDX] < 0);
  //bool y_free = !aopRet (sym->type) || (aopRet (sym->type)->regs[YL_IDX] < 0 && aopRet (sym->type)->regs[YH_IDX] < 0);

  if (sym->stack)
    adjustStack (sym->stack);

  wassertl (!G.stack.pushed, "Unbalanced stack.");

  if (poststackadjust)
    wassert (0);

  if (IFFUNC_ISCRITICAL (sym->type))
      wassert (0);

  if (IFFUNC_ISISR (sym->type))
    wassert (0);
  else
    {
      /* if debug then send end of function */
      if (options.debug && currFunc && !regalloc_dry_run)
        debugFile->writeEndFunction (currFunc, ic, 1);

      emit2 ("ret", "");
      cost (1, 1);
    }
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

  if (options.debug && !regalloc_dry_run)
    debugFile->writeLabel (IC_LABEL (ic), ic);

  emitLabel (IC_LABEL (ic));
}

/*---------------------------------------------------------------------*/
/* genF8Code - generate code for F8 for a single iCode instruction     */
/*---------------------------------------------------------------------*/
static void
genF8iCode (iCode *ic)
{
  genLine.lineElement.ic = ic;

#if 0
  if (!regalloc_dry_run)
    printf ("ic %d op %d stack pushed %d\n", ic->key, ic->op, G.stack.pushed);
#endif

  if (resultRemat (ic))
    {
      if (!regalloc_dry_run)
        D (emit2 ("; skipping iCode since result will be rematerialized", ""));
      return;
    }

  if (ic->generated)
    {
      if (!regalloc_dry_run)
        D (emit2 ("; skipping generated iCode", ""));
      return;
    }

  switch (ic->op)
    {
    case FUNCTION:
      genFunction (ic);
      break;

    case ENDFUNCTION:
      genEndFunction (ic);
      break;

    case LABEL:
      genLabel (ic);
      break;

    default:
      wassertl (0, "Unknown iCode");
    }
}

float
dryF8iCode (iCode *ic)
{
  regalloc_dry_run = TRUE;
  regalloc_dry_run_cost_bytes = 0;
  regalloc_dry_run_cost_cycles = 0;

  initGenLineElement ();

  genF8iCode (ic);

  destroy_line_list ();

  wassert (regalloc_dry_run);

  const unsigned int byte_cost_weight = 2 << (optimize.codeSize * 3 + !optimize.codeSpeed * 3);

  return ((float)regalloc_dry_run_cost_bytes * byte_cost_weight + (float)regalloc_dry_run_cost_cycles * ic->count);
}

/*---------------------------------------------------------------------*/
/* genF8Code - generate code for F8 for a block of intructions         */
/*---------------------------------------------------------------------*/
void
genF8Code (iCode *lic)
{
  iCode *ic;
  int clevel = 0;
  int cblock = 0;  
  int cln = 0;
  regalloc_dry_run = FALSE;

  /* if debug information required */
  if (options.debug && currFunc && !regalloc_dry_run)
    debugFile->writeFunction (currFunc, lic);

  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, NULL, 0); /* have no idea where frame is now */

  //memset(stm8_regs_used_as_parms_in_calls_from_current_function, 0, sizeof(bool) * (YH_IDX + 1));
  //memset(stm8_regs_used_as_parms_in_pcalls_from_current_function, 0, sizeof(bool) * (YH_IDX + 1));

  for (ic = lic; ic; ic = ic->next)
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

      regalloc_dry_run_cost_bytes = 0;
      regalloc_dry_run_cost_cycles = 0;

      if (options.iCodeInAsm)
        {
          const char *iLine = printILine (ic);
          emit2 ("; ic:", "%d: %s", ic->key, iLine);
          dbuf_free (iLine);
        }
#if 0
      emit2 (";", "count: %f", ic->count);
#endif
      genF8iCode(ic);

#if 0
      D (emit2 (";", "Cost for generated ic %d : (%d, %d)", ic->key, regalloc_dry_run_cost_bytes, regalloc_dry_run_cost_cycles));
#endif
    }

  if (options.debug)
    debugFile->writeFrameAddress (NULL, NULL, 0); /* have no idea where frame is now */

  /* now we are ready to call the
     peephole optimizer */
  if (!options.nopeep)
    peepHole (&genLine.lineHead);

  /* now do the actual printing */
  printLine (genLine.lineHead, codeOutBuf);

  /* destroy the line list */
  destroy_line_list ();
}

bool
f8IsReturned(const char *what)
{
  if (!strcmp(what, "x"))
    return (f8IsReturned ("xl") || f8IsReturned ("xh"));
  else if (!strcmp(what, "y"))
    return (f8IsReturned ("yl") || f8IsReturned ("yh"));
  else if (!strcmp(what, "z"))
    return (f8IsReturned ("zl") || f8IsReturned ("zh"));

  const asmop *retaop = aopRet (currFunc->type);

  if (!retaop)
    return false;
  for (int i = 0; i < retaop->size; i++)
    if (!strcmp(retaop->aopu.bytes[i].byteu.reg->name, what))
      return true;
  return false;
}

// Check if what is part of the ith argument (counting from 1) to a function of type ftype.
// If what is 0, just check if hte ith argument is in registers.
bool
f8IsRegArg(struct sym_link *ftype, int i, const char *what)
{
  if (what && !strcmp(what, "x"))
    return (f8IsRegArg (ftype, i, "xl") || f8IsRegArg (ftype, i, "xh"));
  else if (what && !strcmp(what, "y"))
    return (f8IsRegArg (ftype, i, "yl") || f8IsRegArg (ftype, i, "yh"));
  else if (what && !strcmp(what, "z"))
    return (f8IsRegArg (ftype, i, "zl") || f8IsRegArg (ftype, i, "zh"));
 
  const asmop *argaop = aopArg (ftype, i);

  if (!argaop)
    return false;
    
  if (!what)
    return true;
    
  for (int i = 0; i < argaop->size; i++)
    if (!strcmp(argaop->aopu.bytes[i].byteu.reg->name, what))
      return true;

  return false; 
}

bool
f8IsParmInCall(sym_link *ftype, const char *what)
{
  const value *args;
  int i;

  for (i = 1, args = FUNC_ARGS (ftype); args; args = args->next, i++)
    if (f8IsRegArg(ftype, i, what))
      return true;
  return false;
}

