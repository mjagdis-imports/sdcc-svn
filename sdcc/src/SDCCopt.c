/*-------------------------------------------------------------------------
  SDCCopt.c - calls all the optimizations routines and does some of the
              hackier transformations, these include translating iCodes
              to function calls and replacing local variables with their
              register equivalents etc. Also contains the driver routine
              for dead code elimination

             Written By -  Sandeep Dutta . sandeep.dutta@usa.net (1998)

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

#include <math.h>
#include "common.h"
#include "dbuf_string.h"

/*-----------------------------------------------------------------*/
/* global variables */
int cseDefNum = 0;

char flowChanged = 0;


/*-----------------------------------------------------------------*/
/* printSymName - prints the symbol names                          */
/*-----------------------------------------------------------------*/
int
printSymName (void *vsym)
{
  symbol *sym = vsym;
  fprintf (stdout, " %s ", sym->name);
  return 0;
}

/*-----------------------------------------------------------------*/
/* cnvToFcall - does the actual conversion to function call        */
/*-----------------------------------------------------------------*/
static void
cnvToFcall (iCode * ic, eBBlock * ebp)
{
  iCode *ip;
  iCode *newic;
  operand *left;
  operand *right;
  symbol *func = NULL;
  char *filename = ic->filename;
  int lineno = ic->lineno;
  int bytesPushed=0;

  ip = ic->next;                /* insertion point */
  /* remove it from the iCode */
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);

  if (IS_FLOAT (operandType (right)))
    {
      switch (ic->op)
        {
        case '+':
          func = fsadd;
          break;
        case '-':
          func = fssub;
          break;
        case '/':
          func = fsdiv;
          break;
        case '*':
          func = fsmul;
          break;
        case EQ_OP:
          func = fseq;
          break;
        case NE_OP:
          func = fsneq;
          break;
        case '<':
          func = fslt;
          break;
        case '>':
          {
            operand *tmp = right;
            right = left;
            left = tmp;
            func = fslt;
            break;
          }
        }
    }
  else if (IS_FIXED16X16 (operandType (right)))
    {
      switch (ic->op)
        {
        case '+':
          func = fps16x16_add;
          break;
        case '-':
          func = fps16x16_sub;
          break;
        case '/':
          func = fps16x16_div;
          break;
        case '*':
          func = fps16x16_mul;
          break;
        case EQ_OP:
          func = fps16x16_eq;
          break;
        case NE_OP:
          func = fps16x16_neq;
          break;
        case '<':
          func = fps16x16_lt;
          break;
        case '>':
          func = fps16x16_gt;
          break;
        case LE_OP:
          func = fps16x16_lteq;
          break;
        case GE_OP:
          func = fps16x16_gteq;
          break;
        }
    }

  /* if float support routines NOT compiled as reentrant */
  if (!options.float_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, left, NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, left);
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }

      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (left))
          OP_USES (left) = bitVectSetBit (OP_USES (left), newic->key);

      /* second one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->next->etype))
        {
          newic = newiCode (SEND, right, NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->next->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, right);
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type)->next, false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (right))
          OP_USES (right) = bitVectSetBit (OP_USES (right), newic->key);
    }
  else
    {
      /* push right */
      if (IS_REGPARM (FUNC_ARGS(func->type)->next->etype))
        {
          newic = newiCode (SEND, right, NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->next->etype);
        }
      else
        {
          newic = newiCode (IPUSH, right, NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(right)) + (getSize(operandType(right)) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }

      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (right))
          OP_USES (right) = bitVectSetBit (OP_USES (right), newic->key);

      /* insert push left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, left, NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, left, NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(left)) + (getSize(operandType(left)) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (left))
        OP_USES (left) = bitVectSetBit (OP_USES (left), newic->key);
    }
  /* insert the call */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
  OP_DEFS (IC_RESULT (newic)) = bitVectSetBit (OP_DEFS (IC_RESULT (newic)), newic->key);
  newic->filename = filename;
  newic->lineno = lineno;
  newic->parmBytes += bytesPushed;
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to mark as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
}

/*-----------------------------------------------------------------*/
/* cnvToFloatCast - converts casts to floats to function calls     */
/*-----------------------------------------------------------------*/
static void
cnvToFloatCast (iCode * ic, eBBlock * ebp)
{
  iCode *ip, *newic;
  symbol *func = NULL;
  sym_link *type = copyLinkChain (operandType (IC_RIGHT (ic)));
  SPEC_SHORT (type) = 0;
  int linenno = ic->lineno;
  int bwd, su;
  int bytesPushed=0;

  ip = ic->next;
  /* remove it from the iCode */
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  /* depending on the type */
  for (bwd = 0; bwd < 4; bwd++)
    {
      for (su = 0; su < 2; su++)
        {
          if (compareType (type, multypes[bwd][su], false) == 1)
            {
              func = conv[0][bwd][su];
              goto found;
            }
        }
    }

  if (compareType (type, fixed16x16Type, false) == 1)
    {
      func = fp16x16conv[0][4][0];
      goto found;
    }

  if (IS_BOOLEAN (type))
    {
      wassert(multypes[0][1] == UCHARTYPE);
      func = conv[0][0][1];
      goto found;
    }

  assert (0);
found:

  /* if float support routines NOT compiled as reentrant */
  if (!options.float_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_RIGHT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = linenno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }
  else
    {
      /* push the left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_RIGHT (ic), NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(IC_RIGHT(ic))) + (getSize(operandType(IC_RIGHT(ic))) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = linenno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }

  /* make the call */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  newic->parmBytes+=bytesPushed;
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to marked as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
  newic->filename = filename;
  newic->lineno = linenno;
  if (IS_SYMOP (IC_RESULT (ic)))
    OP_DEFS (IC_RESULT (ic)) = bitVectSetBit (OP_DEFS (IC_RESULT (ic)), newic->key);
}

/*----------------------------------------------------------------------*/
/* cnvToFixed16x16Cast - converts casts to fixed16x16 to function calls */
/*----------------------------------------------------------------------*/
static void
cnvToFixed16x16Cast (iCode * ic, eBBlock * ebp)
{
  iCode *ip, *newic;
  symbol *func = NULL;
  sym_link *type = copyLinkChain (operandType (IC_RIGHT (ic)));
  SPEC_SHORT (type) = 0;
  int linenno = ic->lineno;
  int bwd, su;
  int bytesPushed=0;

  ip = ic->next;
  /* remove it from the iCode */
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  /* depending on the type */
  for (bwd = 0; bwd < 4; bwd++)
    {
      for (su = 0; su < 2; su++)
        {
          if (compareType (type, multypes[bwd][su], false) == 1)
            {
              func = fp16x16conv[0][bwd][su];
              goto found;
            }
        }
    }
  assert (0);
found:

  /* if float support routines NOT compiled as reentrant */
  if (!options.float_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_RIGHT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = linenno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }
  else
    {
      /* push the left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_RIGHT (ic), NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(IC_RIGHT(ic)));
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = linenno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }

  /* make the call */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  newic->parmBytes+=bytesPushed;
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to marked as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
  newic->filename = filename;
  newic->lineno = linenno;
  if (IS_SYMOP (IC_RESULT (ic)))
    OP_DEFS (IC_RESULT (ic)) = bitVectSetBit (OP_DEFS (IC_RESULT (ic)), newic->key);
}

/*-----------------------------------------------------------------*/
/* cnvFromFloatCast - converts casts From floats to function calls */
/*-----------------------------------------------------------------*/
static void
cnvFromFloatCast (iCode * ic, eBBlock * ebp)
{
  iCode *ip, *newic;
  symbol *func = NULL;
  sym_link *type = copyLinkChain (operandType (IC_LEFT (ic)));
  SPEC_SHORT (type) = 0;
  char *filename = ic->filename;
  int lineno = ic->lineno;
  int bwd, su;
  int bytesPushed=0;

  ip = ic->next;
  /* remove it from the iCode */
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  /* depending on the type */
  for (bwd = 0; bwd < 4; bwd++)
    {
      for (su = 0; su < 2; su++)
        {
          if (compareType (type, multypes[bwd][su], false) == 1)
            {
              func = conv[1][bwd][su];
              goto found;
            }
        }
    }
  assert (0);
found:

  /* if float support routines NOT compiled as reentrant */
  if (!options.float_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_RIGHT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }
  else
    {
      /* push the left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_RIGHT (ic), NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(IC_RIGHT(ic))) + (getSize(operandType(IC_RIGHT(ic))) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }

  /* make the call */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  newic->parmBytes+=bytesPushed;
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to marked as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
  newic->filename = filename;
  newic->lineno = lineno;
  if (IS_SYMOP (IC_RESULT (ic)))
    OP_DEFS (IC_RESULT (ic)) = bitVectSetBit (OP_DEFS (IC_RESULT (ic)), newic->key);
}

/*--------------------------------------------------------------------------*/
/* cnvFromFixed16x16Cast - converts casts from fixed16x16 to function calls */
/*--------------------------------------------------------------------------*/
static void
cnvFromFixed16x16Cast (iCode * ic, eBBlock * ebp)
{
  iCode *ip, *newic;
  symbol *func = NULL;
  sym_link *type = copyLinkChain (operandType (IC_LEFT (ic)));
  SPEC_SHORT (type) = 0;
  char *filename = ic->filename;
  int lineno = ic->lineno;
  int bwd, su;
  int bytesPushed=0;

  ip = ic->next;
  /* remove it from the iCode */
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  /* depending on the type */
  for (bwd = 0; bwd < 4; bwd++)
    {
      for (su = 0; su < 2; su++)
        {
          if (compareType (type, multypes[bwd][su], false) == 1)
            {
              func = fp16x16conv[1][bwd][su];
              goto found;
            }
        }
    }

  if (compareType (type, floatType, false) == 1)
    {
      func = fp16x16conv[1][4][0];
      goto found;
    }

  assert (0);
found:

  /* if float support routines NOT compiled as reentrant */
  if (!options.float_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_RIGHT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }
  else
    {
      /* push the left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_RIGHT (ic), NULL);
          newic->parmPush = 1;
          bytesPushed += getSize(operandType(IC_RIGHT(ic)));
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (IC_RIGHT (ic)))
          OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
    }

  /* make the call */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  newic->parmBytes+=bytesPushed;
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to marked as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
  newic->filename = filename;
  newic->lineno = lineno;
  if (IS_SYMOP (IC_RESULT (ic)))
    OP_DEFS (IC_RESULT (ic)) = bitVectSetBit (OP_DEFS (IC_RESULT (ic)), newic->key);
}

extern operand *geniCodeRValue (operand *, bool);

/* Insert a cast of operand op of ic to type type */
void
prependCast (iCode *ic, operand *op, sym_link *type, eBBlock *ebb)
{
  if (IS_OP_LITERAL (op))
    {
      operand *newop = operandFromValue (valCastLiteral (type, operandLitValue (op), operandLitValue (op)), false);
      if (isOperandEqual (op, IC_LEFT (ic)))
        IC_LEFT (ic) = newop;
      if (isOperandEqual (op, IC_RIGHT (ic)))
        IC_RIGHT (ic) = newop;
      return;
    }
            
  iCode *newic = newiCode (CAST, operandFromLink (type), op);
  hTabAddItem (&iCodehTab, newic->key, newic);

  IC_RESULT (newic) = newiTempOperand (type, 0);
  bitVectSetBit (OP_USES (op), newic->key);
  OP_DEFS (IC_RESULT (newic)) = bitVectSetBit (OP_DEFS (IC_RESULT (newic)), newic->key);
  bitVectUnSetBit (OP_USES (op), ic->key);
  OP_USES (IC_RESULT (newic)) = bitVectSetBit (OP_USES (IC_RESULT (newic)), ic->key);
  newic->filename = ic->filename;
  newic->lineno = ic->lineno;

  addiCodeToeBBlock (ebb, newic, ic);

  if (isOperandEqual (op, IC_LEFT (ic)))
    IC_LEFT (ic) = IC_RESULT (newic);

  if (isOperandEqual (op, IC_RIGHT (ic)))
    IC_RIGHT (ic) = IC_RESULT (newic);
}

/* Insert a cast of result of ic from type type */
void
appendCast (iCode *ic, sym_link *type, eBBlock *ebb)
{
  iCode *newic = newiCode (CAST, operandFromLink (operandType (IC_RESULT (ic))), 0);
  hTabAddItem (&iCodehTab, newic->key, newic);

  IC_RESULT (newic) = IC_RESULT (ic);
  bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
  bitVectSetBit (OP_DEFS (IC_RESULT (ic)), newic->key);
  IC_RESULT (ic) = newiTempOperand (type, 0);
  IC_RIGHT (newic) = operandFromOperand (IC_RESULT (ic));
  bitVectSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
  bitVectSetBit (OP_USES (IC_RESULT (ic)), newic->key);
  newic->filename = ic->filename;
  newic->lineno = ic->lineno;
  addiCodeToeBBlock (ebb, newic, ic->next);
}

/*-----------------------------------------------------------------*/
/* convilong - converts int or long mults or divs to fcalls        */
/*-----------------------------------------------------------------*/
static void
convilong (iCode *ic, eBBlock *ebp)
{
  int op = ic->op;

  // Use basic type multiplication function for _BitInt
  if ((op == '*' || op == '/' || op == '%' || op == LEFT_OP || op == RIGHT_OP) &&
    (IS_BITINT (operandType (IC_LEFT (ic))) || IS_BITINT (operandType (IC_RIGHT (ic)))))
    {
      // Try 16x16->16
      if (IS_BITINT (operandType (IC_LEFT (ic))) && SPEC_BITINTWIDTH (operandType (IC_LEFT (ic))) <= 16 &&
        IS_BITINT (operandType (IC_RIGHT (ic))) && SPEC_BITINTWIDTH (operandType (IC_RIGHT (ic))) <= 16)
        {
          prependCast (ic, IC_LEFT (ic), newIntLink(), ebp);
          if (op != LEFT_OP && op != RIGHT_OP)
            prependCast (ic, IC_RIGHT (ic), newIntLink(), ebp);
          appendCast (ic, newIntLink(), ebp);
        }
      // Try 32x32->32
      else if (IS_BITINT (operandType (IC_LEFT (ic))) && SPEC_BITINTWIDTH (operandType (IC_LEFT (ic))) <= 32 &&
        IS_BITINT (operandType (IC_RIGHT (ic))) && SPEC_BITINTWIDTH (operandType (IC_RIGHT (ic))) <= 32)
        {
          prependCast (ic, IC_LEFT (ic), newLongLink(), ebp);
          if (op != LEFT_OP && op != RIGHT_OP)
            prependCast (ic, IC_RIGHT (ic), newLongLink(), ebp);
          appendCast (ic, newLongLink(), ebp);
        }
      else // Fall back to 64x64->64.
        {
          prependCast (ic, IC_LEFT (ic), newLongLongLink(), ebp);
          if (op != LEFT_OP && op != RIGHT_OP)
            prependCast (ic, IC_RIGHT (ic), newLongLongLink(), ebp);
          appendCast (ic, newLongLongLink(), ebp);
        }
      if ((op == '*' || op == '/' || op == '%') && port->hasNativeMulFor && port->hasNativeMulFor (ic, operandType (IC_LEFT (ic)), operandType (IC_RIGHT (ic)))) // Avoid introducing calls to non-existing support functions.
        return;
    }
  
  symbol *func = NULL;
  iCode *ip = ic->next;
  iCode *newic;
  char *filename = ic->filename;
  int lineno = ic->lineno;
  int bwd;
  int su;
  int bytesPushed=0;
  operand *left;
  operand *right;
  sym_link *leftType = copyLinkChain (operandType (IC_LEFT (ic)));
  sym_link *rightType = copyLinkChain (operandType (IC_RIGHT (ic)));
  SPEC_SHORT (leftType) = 0;
  SPEC_SHORT (rightType) = 0;

  left = ic->left;
  right = ic->right;

  // Special case: 16x16->32 multiplication.
  if (op == '*' && (muls16tos32[0] || muls16tos32[1] || port->hasNativeMulFor) &&
    (IS_SYMOP (left) && bitVectnBitsOn (OP_DEFS (left)) == 1 && bitVectnBitsOn (OP_USES (left)) == 1 || IS_OP_LITERAL (left) && operandLitValue (left) < 32768 && operandLitValue (left) >= -32768) &&
    (IS_SYMOP (right) && bitVectnBitsOn (OP_DEFS (right)) == 1 && bitVectnBitsOn (OP_USES (right)) == 1 || IS_OP_LITERAL (right) && operandLitValue (right) < 32768 && operandLitValue (right) >= -32768) &&
    getSize (leftType) == 4 && getSize (rightType) == 4)
    {
      iCode *lic = IS_SYMOP (left) ? hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (left))) : 0;
      iCode *ric = IS_SYMOP (right) ? hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (right))) : 0;

      if ((!lic || lic->op == CAST && IS_INTEGRAL (operandType (lic->right)) && getSize (operandType (lic->right)) == 2 && SPEC_USIGN (operandType (lic->right)) == SPEC_USIGN (operandType (left))) &&
        (!ric || ric->op == CAST && IS_INTEGRAL (operandType (ric->right)) && getSize (operandType (ric->right)) == 2 && SPEC_USIGN (operandType (ric->right)) == SPEC_USIGN (operandType (right))))
        {
          func = muls16tos32[SPEC_USIGN (operandType (left))];

          if (func || port->hasNativeMulFor && lic && ric && port->hasNativeMulFor (ic, operandType (lic->right), operandType (ric->right)))
            {
              if (lic)
                {
                  lic->op = '=';
                  OP_SYMBOL (left)->type = newIntLink ();
                }
              else
                ic->left = operandFromValue (valCastLiteral (newIntLink(), operandLitValue (left), operandLitValue (left)), false);

              if (ric)
                {
                  ric->op = '=';
                  OP_SYMBOL (right)->type = newIntLink ();
                }
              else
                ic->right = operandFromValue (valCastLiteral (newIntLink(), operandLitValue (right), operandLitValue (right)), false);

              if (func) // Use 16x16->32 support function
                goto found;
              else // Native
                return;
            }
        }
    }
  if (op == '*' && (mulu32u8tou64 || port->hasNativeMulFor) &&
    (IS_SYMOP (left) && bitVectnBitsOn (OP_DEFS (left)) == 1 && bitVectnBitsOn (OP_USES (left)) == 1 /*|| IS_OP_LITERAL (left) && operandLitValue (left) < 256 && operandLitValue (left) >= 0*/) &&
    (IS_SYMOP (right) && bitVectnBitsOn (OP_DEFS (right)) == 1 && bitVectnBitsOn (OP_USES (right)) == 1 /*|| IS_OP_LITERAL (right) && operandLitValue (right) < 256 && operandLitValue (right) >= 0*/) &&
    getSize (leftType) == 8 && getSize (rightType) == 8)
    {
      iCode *lic = IS_SYMOP (left) ? hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (left))) : 0;
      iCode *ric = IS_SYMOP (right) ? hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (right))) : 0;

      if ((lic && lic->op == CAST && IS_INTEGRAL (operandType (lic->right)) && getSize (operandType (lic->right)) <= 4 && SPEC_USIGN (operandType (lic->right))) && // Todo: Allow !lic / !ric for literal operands?
        (ric && ric->op == CAST && IS_INTEGRAL (operandType (ric->right)) && getSize (operandType (ric->right)) <= 1 && SPEC_USIGN (operandType (ric->right))))
        {
          func = mulu32u8tou64;

          if (func || port->hasNativeMulFor && lic && ric && port->hasNativeMulFor (ic, operandType (lic->right), operandType (ric->right)))
            {
              if (lic)
                {
                  if (getSize (operandType (IC_RIGHT (lic))) == 4)
                    lic->op = '=';
                  OP_SYMBOL (left)->type = newLongLink ();
                  SPEC_USIGN (OP_SYMBOL (left)->type) = 1;
                }
              else
                ic->left = operandFromValue (valCastLiteral (newIntLink(), operandLitValue (left), operandLitValue (left)), false);

              if (ric)
                {
                  ric->op = '=';
                  OP_SYMBOL (right)->type = newCharLink ();
                  SPEC_USIGN (OP_SYMBOL (left)->type) = 1;
                }
              else
                ic->right = operandFromValue (valCastLiteral (newIntLink(), operandLitValue (right), operandLitValue (right)), false);

              if (func) // Use 32x8->64 support function
                goto found;
              else // Native
                return;
            }
        }
    }

  if (getSize (leftType) == 1 && getSize (rightType) == 1)
    {
      int muldivmod;

      if (op == '*')
        muldivmod = 0;
      else if (op == '/')
        muldivmod = 1;
      else if (op == '%')
        muldivmod = 2;
      else
        muldivmod = -1;

      for (su = 0; su < 4 && muldivmod >= 0; su++)
        {
          if ((compareType (leftType, multypes[0][su%2], false) == 1) &&
              (compareType (rightType, multypes[0][su/2], false) == 1))
            {
              func = muldiv[muldivmod][0][su];
              goto found;
            }
        }
    }

  /* depending on the type */
  for (bwd = 0; bwd < 4; bwd++)
    {
      for (su = 0; su < 2; su++)
        {
          if (compareType (leftType, multypes[bwd][su], false) == 1)
            {
              if ((op=='*' || op=='/' || op=='%'))
                {
                  int ret = compareType (rightType, multypes[bwd][su], false);
                  if (ret != 1 && isOperandLiteral (right) && SPEC_NOUN (multypes[bwd][su]) == V_CHAR && operandLitValue (right) >= 0 && operandLitValue (right) <= 127)
                    ;
                  else if (ret != 1)
                    {
                      printf ("leftType: "); printTypeChain (leftType, 0);
                      printf ("rightType: "); printTypeChain (rightType, 0);
                      printf ("multypes[bwd][su]: "); printTypeChain (multypes[bwd][su], 0);
                      assert(0);
                    }
                }
              if (op == '*')
                func = muldiv[0][bwd][su];
              else if (op == '/')
                func = muldiv[1][bwd][su];
              else if (op == '%')
                func = muldiv[2][bwd][su];
              else if (op == RIGHT_OP)
                {
                  func = rlrr[1][bwd][su];
                  prependCast (ic, IC_RIGHT (ic), FUNC_ARGS(func->type)->next->type, ebp);
                }
              else if (op == LEFT_OP)
                {
                  func = rlrr[0][bwd][su];
                  prependCast (ic, IC_RIGHT (ic), FUNC_ARGS(func->type)->next->type, ebp);
                }
              else
                assert (0);
              goto found;
            }
        }
    }
  werrorfl (filename, lineno, E_INVALID_OP, "mul/div/shift");
  fprintf (stderr, "ic %d op %d leftType: ", ic->key, op); printTypeChain (leftType, stderr); fprintf (stderr, "\n");
  return;
found:
  // Update left and right - they might have changed due to inserted casts.
  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);
  unsetDefsAndUses (ic);
  remiCodeFromeBBlock (ebp, ic);

  /* if int & long support routines NOT compiled as reentrant */
  if (!options.intlong_rent)
    {
      /* first one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_LEFT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_LEFT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type), false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (left))
        OP_USES (left) = bitVectSetBit (OP_USES (left), newic->key);

      /* second one */
      if (IS_REGPARM (FUNC_ARGS(func->type)->next->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->next->etype);
        }
      else
        {
          newic = newiCode ('=', NULL, IC_RIGHT (ic));
          IC_RESULT (newic) = operandFromValue (FUNC_ARGS(func->type)->next, false);
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (right))
        OP_USES (right) = bitVectSetBit (OP_USES (right), newic->key);
    }
  else
    {
      /* compiled as reentrant then push */
      /* push right */
      if (IS_REGPARM (FUNC_ARGS(func->type)->next->etype))
        {
          newic = newiCode (SEND, IC_RIGHT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->next->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_RIGHT (ic), NULL);
          newic->parmPush = 1;

          bytesPushed += getSize(operandType(IC_RIGHT(ic))) + (getSize(operandType(IC_RIGHT(ic))) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (right))
        OP_USES (right) = bitVectSetBit (OP_USES (right), newic->key);

      /* insert push left */
      if (IS_REGPARM (FUNC_ARGS(func->type)->etype))
        {
          newic = newiCode (SEND, IC_LEFT (ic), NULL);
          newic->argreg = SPEC_ARGREG(FUNC_ARGS(func->type)->etype);
        }
      else
        {
          newic = newiCode (IPUSH, IC_LEFT (ic), NULL);
          newic->parmPush = 1;

          bytesPushed += getSize(operandType(IC_LEFT(ic))) + (getSize(operandType(IC_LEFT(ic))) % 2 && TARGET_PDK_LIKE); // pdk requires stack to be even-aligned
        }
      hTabAddItem (&iCodehTab, newic->key, newic);
      addiCodeToeBBlock (ebp, newic, ip);
      newic->filename = filename;
      newic->lineno = lineno;
      if (IS_SYMOP (left))
        OP_USES (left) = bitVectSetBit (OP_USES (left), newic->key);
    }

  /* for the result */
  newic = newiCode (CALL, operandFromSymbol (func, false), NULL);
  IC_RESULT (newic) = IC_RESULT (ic);
  bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
  OP_DEFS (IC_RESULT (newic)) = bitVectSetBit (OP_DEFS (IC_RESULT (newic)), newic->key);
  newic->filename = filename;
  newic->lineno = lineno;
  newic->parmBytes+=bytesPushed; // to clear the stack after the call
  ebp->hasFcall = 1;
  if (currFunc)
    FUNC_HASFCALL (currFunc->type) = 1;

  if (TARGET_PIC_LIKE)
    {
      /* normally these functions aren't marked external, so we can use their
       * _extern field to marked as already added to symbol table */

      if (!SPEC_EXTR(func->etype))
        {
          memmap *seg = SPEC_OCLS(OP_SYMBOL(IC_LEFT(newic))->etype);

          SPEC_EXTR(func->etype) = 1;
          seg = SPEC_OCLS( func->etype );
          addSet(&seg->syms, func);
        }
    }

  hTabAddItem (&iCodehTab, newic->key, newic);
  addiCodeToeBBlock (ebp, newic, ip);
}

/*-----------------------------------------------------------------*/
/* convertbuiltin - maybe convert some builtins back               */
/*-----------------------------------------------------------------*/
static void
convbuiltin (iCode *const ic, eBBlock *ebp)
{
  sym_link *ftype;
  symbol *bif;
  int stack;

  iCode *icc = ic, *icp = ic->prev, *ico = NULL;
  iCode *lastparam = ic;
  while (icc->op != CALL)
    {
      if (icc->op != SEND || !icc->builtinSEND)
        return;
      lastparam = icc;
      icc = icc->next;
    }

  if (!IS_SYMOP (IC_LEFT(icc)))
    return;

  ftype = operandType (IC_LEFT(icc));
  if (!IFFUNC_ISBUILTIN (ftype))
    return;

  bif = OP_SYMBOL (IC_LEFT (icc));

  /* Now we can be sure to have found a builtin function. */

  if ((!strcmp (bif->name, "__builtin_memcpy") || !strcmp (bif->name, "__builtin_strncpy") || !strcmp (bif->name, "__builtin_memset")) &&
    IS_OP_LITERAL (IC_LEFT (lastparam)) && !operandLitValue (IC_LEFT (lastparam)))
    {
      /* We have a builtin that does nothing. */
      /* TODO: Eliminate it, convert any SEND of volatile into DUMMY_READ_VOLATILE. */
      /* For now just convert back to call to make sure any volatiles are read. */

      strcpy(OP_SYMBOL (IC_LEFT (icc))->rname, !strcmp (bif->name, "__builtin_memcpy") ? "___memcpy" : (!strcmp (bif->name, "__builtin_strncpy") ? "_strncpy" : "_memset"));
      goto convert;
    }

  if ((TARGET_IS_Z80 || TARGET_IS_Z180 || TARGET_RABBIT_LIKE || TARGET_IS_EZ80 || TARGET_IS_Z80N || TARGET_IS_R800) && (!strcmp (bif->name, "__builtin_memcpy") || !strcmp (bif->name, "__builtin_strncpy") || !strcmp (bif->name, "__builtin_memset")))
    {
      /* Replace iff return value is used or last parameter is not an integer constant (except for memcpy, where non-integers can be handled). */
      if (bitVectIsZero (OP_USES (IC_RESULT (icc))) && (IS_OP_LITERAL (IC_LEFT (lastparam)) || !strcmp (bif->name, "__builtin_memcpy")))
        return;

      strcpy(OP_SYMBOL (IC_LEFT (icc))->rname, !strcmp (bif->name, "__builtin_memcpy") ? "___memcpy" : (!strcmp (bif->name, "__builtin_strncpy") ? "_strncpy" : "_memset"));
      goto convert;
    }

  return;

convert:
  /* Convert parameter passings from SEND to PUSH. */
  stack = 0;
  struct value *args;
  for (icc = ic, args = FUNC_ARGS (nonbuiltin_memcpy->type); icc->op != CALL; icc = icc->next, args = args->next)
    {
      wassert (args);
      icc->builtinSEND = 0;
      if (IS_REGPARM (args->etype))
        {
          icc->argreg = SPEC_ARGREG (args->etype);
        }
      else
        {
          icc->op = IPUSH;
          icc->parmPush = 1;
          stack += getSize (operandType (IC_LEFT (icc)));
        }
    }
  icc->parmBytes = stack;

  /* Reverse parameters. */
  for (icc = ic; icc->op != CALL; icc = icc->next)
    {
      if (icc->next->op != CALL)
        icc->prev = icc->next;
      else
        icc->prev = icp;
    }
  if(icc != ic)
    {
      if(icp)
        icp->next = icc->prev;
      icc->prev = ic;
    }
  for(; icc != icp; ico = icc, icc = icc->prev)
    {
      if (icc->op != CALL)
        icc->next = ico;
    }
}

/*-----------------------------------------------------------------*/
/* convconvention - handle calling convention                      */
/*-----------------------------------------------------------------*/
static void
convconvention (iCode *ic, eBBlock *ebp)
{
  iCode *icc, *icp, *ico = NULL;

  assert (ic->op == CALL || ic->op == PCALL);

  sym_link *ftype = operandType (IC_LEFT (ic));
  if (ic->op == PCALL)
    ftype =ftype->next;

  // Small-C passed stack parameters left-to-right.
  if (FUNC_ISSMALLC (ftype))
    {
      for (icc = ic->prev; icc && icc->op == IPUSH; icc = icc->prev)
        ic = icc;
      icp = icc;

      /* Reverse parameters. */
      for (icc = ic; icc->op != CALL && icc->op != PCALL; icc = icc->next)
        {
          if (icc->next->op != CALL && icc->next->op != PCALL)
            icc->prev = icc->next;
          else
            icc->prev = icp;
        }
      if (icc != ic)
        {
          if (icp)
            icp->next = icc->prev;
          else
            ebp->sch = icc->prev;
          icc->prev = ic;
        }
      for (; icc != icp; ico = icc, icc = icc->prev)
        {
          if (icc->op != CALL && icc->op != PCALL)
            icc->next = ico;
        }
    }
  else if (FUNC_ISRAISONANCE (ftype) || FUNC_ISIAR (ftype) || FUNC_ISCOSMIC (ftype) || FUNC_ISZ88DK_FASTCALL (ftype))
    ;
  else // SDCC calling convention
    {
      // Use default ABI version if no ABI version is explicitly requested.
      if (FUNC_SDCCCALL (ftype) < 0)
        FUNC_SDCCCALL (ftype) = options.sdcccall;
    }
}

/*-----------------------------------------------------------------*/
/* convertToFcall - converts some operations to fcalls             */
/*-----------------------------------------------------------------*/
static void
convertToFcall (eBBlock ** ebbs, int count)
{
  int i;

  /* for all blocks do */
  for (i = 0; i < count; i++)
    {
      iCode *ic;

      /* for all instructions in the block do */
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          /* floating point operations are
             converted to function calls */
          if ((IS_CONDITIONAL (ic) || IS_ARITHMETIC_OP (ic)) &&
              (IS_FLOAT (operandType (IC_RIGHT (ic))) || IS_FIXED( operandType (IC_RIGHT (ic)))))
            {
              cnvToFcall (ic, ebbs[i]);
            }

          /* casting is a little different */
          if (ic->op == CAST)
            {
              if (IS_FLOAT (operandType (IC_RIGHT (ic))))
                cnvFromFloatCast (ic, ebbs[i]);
              else if (IS_FLOAT (operandType (IC_LEFT (ic))))
                cnvToFloatCast (ic, ebbs[i]);
              if (IS_FIXED16X16 (operandType (IC_RIGHT (ic))))
                cnvFromFixed16x16Cast (ic, ebbs[i]);
              else if (IS_FIXED16X16 (operandType (IC_LEFT (ic))))
                cnvToFixed16x16Cast (ic, ebbs[i]);
            }

          // Easy special case which avoids function call: modulo by a literal power
          // of two can be replaced by a bitwise AND.
          if (ic->op == '%' && isOperandLiteral (IC_RIGHT(ic)))
            {
              bool us = IS_UNSIGNED (operandType (IC_LEFT(ic)));
              bool upcast = FALSE;
              iCode *dic = NULL;

              // Chek if left really is just an upcasted unsigned value.
              if (!us && IS_SYMOP (IC_LEFT(ic)) && bitVectnBitsOn (OP_DEFS (IC_LEFT (ic))) == 1)
                {
                  dic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (IC_LEFT (ic))));

                  if (dic && dic->op == CAST && IS_UNSIGNED (operandType (IC_RIGHT (dic))) && getSize (operandType (IC_RIGHT (dic))) < getSize (operandType (IC_RESULT (dic))))
                    us = upcast = true;
                }

              if (us)
                {
                  unsigned long litVal = double2ul (operandLitValue (IC_RIGHT (ic)));

                  /* modulo by 1: no remainder */
                  if (litVal == 1)
                    {
                      detachiCodeOperand (&ic->left, ic);
                      ic->op = '=';
                      IC_RIGHT (ic) = operandFromLit (0);
                      continue;
                    }
                  // See if literal value is a power of 2.
                  while (litVal && !(litVal & 1))
                    {
                      litVal >>= 1;
                    }
                  if (litVal)
                    {
                      // discard lowest set bit.
                      litVal >>= 1;
                    }

                  if (!litVal)
                    {
                      ic->op = BITWISEAND;
                      IC_RIGHT(ic) = operandFromLit (operandLitValue (IC_RIGHT (ic)) - 1);
                      if (upcast && IS_CHAR (operandType (IC_RIGHT (dic)))
                          && bitVectnBitsOn (OP_USES (IC_LEFT (ic))) == 1)
                        {
                          // Use precasted value
                          attachiCodeOperand (IC_RIGHT (dic), &IC_LEFT (ic), ic);
                          // Change cast to assignmnent to self to avoid
                          // reading IC_RIGHT (dic) twice in case it
                          // was volatile
                          attachiCodeOperand (IC_RESULT (dic), &IC_RIGHT (dic), dic);
                          dic->op = '=';
                          // If upcast from char, maybe there's a
                          // corresponding downcast to char that could
                          // be eliminated too
                          if (bitVectnBitsOn (OP_USES (IC_RESULT (ic))) == 1)
                            {
                              iCode *uic;
                              uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (IC_RESULT (ic))));
                              if (uic->op == CAST && IS_CHAR (operandType (IC_RESULT (uic))))\
                                {
                                  attachiCodeOperand (IC_RESULT (uic), &IC_RESULT (ic), ic);
                                  attachiCodeOperand (IC_RESULT (uic), &IC_RIGHT (uic), uic);
                                  uic->op = '=';
                                }
                            }
                        }
                      continue;

                    }
                }
            }

          /* if long / int mult or divide or mod */
          if (ic->op == '*' || ic->op == '/' || ic->op == '%')
            {
              sym_link *leftType = operandType (IC_LEFT (ic));

              if (IS_INTEGRAL (leftType))
                {
                  sym_link *rightType = operandType (IC_RIGHT (ic));

                  if (port->hasNativeMulFor != NULL &&
                      port->hasNativeMulFor (ic, leftType, rightType))
                    {
                      /* Leave as native */
                    }
                  else
                    {
                      convilong (ic, ebbs[i]);
                    }
                }
            }

          if (ic->op == ROT || ic->op == LEFT_OP || ic->op == RIGHT_OP)
            {
              sym_link *type = operandType (IC_LEFT (ic));

              if (IS_INTEGRAL (type) && getSize (type) > (unsigned)port->support.shift && port->support.shift >= 0)
                {
                  convilong (ic, ebbs[i]);
                }
            }
          if (ic->op == SEND && ic->builtinSEND)
            {
              convbuiltin (ic, ebbs[i]);
            }
          if (ic->op == CALL || ic->op == PCALL)
            {
              convconvention (ic, ebbs[i]);
            }
        }
    }
}

/*-----------------------------------------------------------------*/
/* isPowerOf2 - test if val is power of 2                          */
/*-----------------------------------------------------------------*/
bool
isPowerOf2 (unsigned long val)
{
  while (val && !(val & 1))
    {
      val >>= 1;
    }
  return val == 1;
}

/*-----------------------------------------------------------------*/
/* miscOpt - miscellaneous optimizations                           */
/*-----------------------------------------------------------------*/
static void
miscOpt (eBBlock ** ebbs, int count)
{
/* Borut: disabled optimization of comparison unsigned with 2^n literal
 * since it is broken; see bug #2165 Broken comparison */
#if 0
  int i;

  /* for all blocks do */
  for (i = 0; i < count; ++i)
    {
      iCode *ic;

      /* for all instructions in the block do */
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          /* patch ID: 2702889 - Summary of all uncommitted changes I applied on "my" SDCC */
          /* MB: This seems rather incomplete.
             MB: What if using <= or >= ?
             Borut: Currently <= and >= are transformed to > and < on all targets.
               Transformation depends on lt_nge, gt_nle, bool le_ngt,
               ge_nlt, ne_neq and eq_nne members of PORT structure.
             MB: Why do we need IFX in the first case and not in the second ?
             Borutr: Because the result of comparison is logically negated,
               so in case of IFX the jump logic is inverted for '<' and '<='.
               TODO: The logical negation of the result should be implemented
               for '<' and '<=' in case when the following instruction is not IFX.
             Philipp: Added the test for ifx in the second case, too:
             We want 0 or 1 as a result, the bitwise and won't do, unless we add a cast to bool.
          */
          switch (ic->op)
            {
            case '<':
            case LE_OP:
            case '>':
            case GE_OP:
              /* Only if the the right operand is literal and left operand is unsigned */
              if (isOperandLiteral (IC_RIGHT (ic)) && IS_UNSIGNED (operandType (IC_LEFT (ic))))
                {
                  unsigned litVal = ulFromVal (OP_VALUE (IC_RIGHT (ic)));

                  /* Only if the literal value is greater than 255 and a power of 2 */
                  if (litVal >= 255 &&
                    (isPowerOf2 (litVal) && (ic->op == '<' || ic->op == GE_OP) ||
                    isPowerOf2 (litVal + 1) && (ic->op == '>' || ic->op == LE_OP)))
                    {
                      iCode *ic_nxt = ic->next;

                      switch (ic->op)
                        {
                        case LE_OP:
                          ++litVal;
                          /* fall through */
                        case '<':
                          /* Only if the next instruction is IFX */
                          if (ic_nxt && (ic_nxt->op == IFX) && (ic->eBBlockNum == ic_nxt->eBBlockNum))
                            {
                              int AndMaskVal = 0 - litVal;
                              symbol *TrueLabel;

                              /* set op to bitwise and */
                              ic->op = BITWISEAND;
                              IC_RIGHT (ic) = operandFromLit (AndMaskVal);

                              /* invert jump logic */
                              TrueLabel = IC_TRUE (ic_nxt);
                              IC_TRUE (ic_nxt) = IC_FALSE (ic_nxt);
                              IC_FALSE (ic_nxt) = TrueLabel;
                            }
                          break;

                        case '>':
                          ++litVal;
                          /* fall through */
                        case GE_OP:
                            if (ic_nxt && (ic_nxt->op == IFX) && (ic->eBBlockNum == ic_nxt->eBBlockNum))
                            {
                              int AndMaskVal = 0 - litVal;

                              ic->op = BITWISEAND;
                              IC_RIGHT (ic) = operandFromLit (AndMaskVal);
                            }
                          break;
                        } /* switch */
                    } /* if */
                } /* if */
            } /* switch */
        } /* for */
    } /* for */
#endif
}

/*-----------------------------------------------------------------*/
/* separateAddressSpaces - enforce restrictions on bank switching  */
/* Operands of a single iCode must be in at most one               */
/* named address space. Use temporaries and additional assignments */
/* to enforce the rule.                                            */
/*-----------------------------------------------------------------*/
static void
separateAddressSpaces (eBBlock **ebbs, int count)
{
  int i;

  /* for all blocks do */
  for (i = 0; i < count; ++i)
    {
      iCode *ic;
      symbol *source;

      /* Skip this block if not reachable; other routines may have */
      /* also skipped it, so these iCodes may be undercooked. */
      if (ebbs[i]->noPath)
        continue;

      /* for all instructions in the block do */
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          iCode *iic = 0, *newic = 0;
          operand *left, *right, *result;
          const symbol *leftaddrspace = 0, *rightaddrspace = 0, *resultaddrspace = 0;

          /* JUMPTABLE and IFX do not have left/right/result operands. */
          /* However, they only have a single operand so they cannot   */
          /* have more than one address space to worry about. */
          if (ic->op == JUMPTABLE || ic->op == IFX)
            continue;

          left = IC_LEFT (ic);
          right = IC_RIGHT (ic);
          result = IC_RESULT (ic);

          /*printf ("Looking at ic %d, op %d\n", ic->key, (int)(ic->op));*/

          if (left && ic->op != ADDRESS_OF && IS_SYMOP (left))
            {
              if (POINTER_GET (ic))
                {
                  assert (!(IS_DECL (OP_SYMBOL (left)->type) && DCL_PTR_ADDRSPACE (OP_SYMBOL (left)->type)));
                  leftaddrspace = getAddrspace (OP_SYMBOL (left)->type->next);
                }
              else
                leftaddrspace = getAddrspace (OP_SYMBOL (left)->type);
            }
          if (right && IS_SYMOP (right))
            rightaddrspace = getAddrspace (OP_SYMBOL (right)->type);
          if (result && IS_SYMOP (result))
            { 
              if (POINTER_SET (ic))
                {
                  assert (!(IS_DECL (OP_SYMBOL (result)->type) && DCL_PTR_ADDRSPACE (OP_SYMBOL (result)->type)));
                  resultaddrspace = getAddrspace (OP_SYMBOL (result)->type->next);
                }
              else
                resultaddrspace = getAddrspace (OP_SYMBOL (result)->type);
            }

#if 0
          if (leftaddrspace)
            printf("ic %d (dcl? %d) leftaddrspace %s\n", ic->key, (int)(IS_DECL  (OP_SYMBOL (left)->type)), leftaddrspace->name);
          if (rightaddrspace)
            printf("ic %d (dcl? %d) rightaddrspace %s\n", ic->key, (int)(IS_DECL  (OP_SYMBOL (right)->type)), rightaddrspace->name);
          if (resultaddrspace)
            printf("ic %d (dcl? %d) resultaddrspace %s\n", ic->key, (int)(IS_DECL  (OP_SYMBOL (result)->type)), resultaddrspace->name);
#endif

          if (ic->op == IPUSH && leftaddrspace)
            {
              operand *newop;

              source = OP_SYMBOL (left);
              newic = newiCode ('=', 0, left);
              IC_RESULT (newic) = newop = newiTempOperand (source->type, 0);
              IC_LEFT (ic) = newop;
              leftaddrspace = 0;
              for (iic = ic; iic->prev && iic->prev->op == IPUSH; iic = iic->prev);
            }
          else if (leftaddrspace && rightaddrspace && leftaddrspace != rightaddrspace ||
            resultaddrspace && rightaddrspace && resultaddrspace != rightaddrspace ||
            resultaddrspace && leftaddrspace && resultaddrspace != leftaddrspace)
            {
              operand *newop;

              if (rightaddrspace == resultaddrspace)
                source = OP_SYMBOL (left);
              else
                source = OP_SYMBOL (right);
              newic = newiCode ('=', 0, rightaddrspace == resultaddrspace ? left : right);
              IC_RESULT (newic) = newop = newiTempOperand (source->type, 0);
              if (rightaddrspace == resultaddrspace)
                {
                  IC_LEFT (ic) = newop;
                  leftaddrspace = 0;
                }
              else
                {
                  IC_RIGHT (ic) = newop;
                  rightaddrspace = 0;
                }
              iic = ic;
            }

          if (newic)
            {
              newic->filename = ic->filename;
              newic->lineno = ic->lineno;
              hTabAddItem (&iCodehTab, newic->key, newic);
              addiCodeToeBBlock (ebbs[i], newic, iic);
            }

          assert (!leftaddrspace || !resultaddrspace || leftaddrspace == resultaddrspace);
          assert (!rightaddrspace || !resultaddrspace || rightaddrspace == resultaddrspace);
        }
    }
}

const symbol *
getAddrspaceiCode (const iCode *ic)
{
  operand *left, *right, *result;
  const symbol *leftaddrspace = 0, *rightaddrspace = 0, *resultaddrspace = 0;
  const symbol *addrspace;

  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);
  result = IC_RESULT (ic);

  /* Previous transformations in separateAddressSpaces() should
     ensure that at most one addressspace occurs in each iCode. */
  if (left && ic->op != ADDRESS_OF && IS_SYMOP (left))
    {
      if (POINTER_GET (ic))
        {
          assert (!(IS_DECL (OP_SYMBOL (left)->type) && DCL_PTR_ADDRSPACE (OP_SYMBOL (left)->type)));
          leftaddrspace = getAddrspace (OP_SYMBOL (left)->type->next);
        }
      else
        leftaddrspace = getAddrspace (OP_SYMBOL (left)->type);
    }
  if (right && IS_SYMOP (right))
    rightaddrspace = getAddrspace (OP_SYMBOL (right)->type);
  if (result && IS_SYMOP (result))
    {
      if (POINTER_SET (ic))
        {
          assert (!(IS_DECL (OP_SYMBOL (result)->type) && DCL_PTR_ADDRSPACE (OP_SYMBOL (result)->type)));
          resultaddrspace = getAddrspace (OP_SYMBOL (result)->type->next);
        }
      else
        resultaddrspace = getAddrspace (OP_SYMBOL (result)->type);
    }

  addrspace = leftaddrspace;
  if (rightaddrspace)
    {
      wassertl (!addrspace || addrspace == rightaddrspace, "Multiple named address spaces in icode.");
      addrspace = rightaddrspace;
    }
  if (resultaddrspace)
    {
      wassertl (!addrspace || addrspace == resultaddrspace, "Multiple named address spaces in icode.");
      addrspace = resultaddrspace;
    }

  return (addrspace);
}

/*-----------------------------------------------------------------*/
/* switchAddressSpaceAt - insert a bank selection instruction      */
/*-----------------------------------------------------------------*/
void
switchAddressSpaceAt (iCode *ic, const symbol *const addrspace)
{
  iCode *newic;
  const symbol *const laddrspace = getAddrspaceiCode (ic);
  wassertl(!laddrspace || laddrspace == addrspace, "Switching to invalid address space.");

  newic = newiCode (CALL, operandFromSymbol (addrspace->addressmod[0], false), 0);

  IC_RESULT (newic) = newiTempOperand (newVoidLink (), 1);
  newic->filename = ic->filename;
  newic->lineno = ic->lineno;

  newic->next = ic;
  newic->prev = ic->prev;
  if (ic->prev)
    ic->prev->next = newic;
  ic->prev = newic;
}

/*-----------------------------------------------------------------*/
/* switchAddressSpaces - insert instructions for bank switching    */
/* This is just a fallback, in case the optimal approach fails -   */
/* improbable, but possible depending on sdcc options and code.    */
/*-----------------------------------------------------------------*/
static void
switchAddressSpaces (iCode *ic)
{
  const symbol *oldaddrspace = 0;

  for (; ic; ic = ic->next)
    {
      const symbol *const addrspace = getAddrspaceiCode (ic);

      if (addrspace && addrspace != oldaddrspace)
        {
          switchAddressSpaceAt (ic, addrspace);

          oldaddrspace = addrspace;
        }

      /* Address space might not be preserved over these. */
      if (ic->op == LABEL || ic->op == CALL || ic->op == PCALL)
        oldaddrspace = 0;
    }
}

/*-----------------------------------------------------------------*/
/* isLocalWithoutDef - return 1 if sym might be used without a     */
/*                     defining iCode                              */
/*-----------------------------------------------------------------*/
static int
isLocalWithoutDef (symbol * sym)
{
  if (!IS_AUTO (sym))
    return 0;

  if (IS_VOLATILE (sym->type))
    return 0;

  if (sym->_isparm)
    return 0;

  if (IS_AGGREGATE (sym->type))
    return 0;

  if (sym->addrtaken)
    return 0;

  return !sym->defs;
}

static void
replaceRegEqvOperand (iCode * ic, operand ** opp, int force_isaddr, int new_isaddr)
{
  operand * op = *opp;
  symbol * sym = OP_SYMBOL (op);

  if (isLocalWithoutDef (sym))
    {
      werrorfl (ic->filename, ic->lineno, W_LOCAL_NOINIT, sym->name);
      OP_REQV (op) = NULL;
      sym->allocreq = 1;
    }
  else if (OP_REQV (op))
    {
      operand * nop;

      nop = operandFromOperand (OP_REQV (op));

      /* Copy def/use info from true symbol to register equivalent */
      /* but only if this hasn't been done already. */
      if (!OP_DEFS (nop))
        OP_DEFS (nop) = bitVectCopy (OP_DEFS (op));
      if (!OP_USES (nop))
        OP_USES (nop) = bitVectCopy (OP_USES (op));

      if (force_isaddr)
        nop->isaddr = new_isaddr;

      *opp = nop; /* Replace true sym operand with reg equiv */
    }
}

/*-----------------------------------------------------------------*/
/* replaceRegEqv - replace all local variables with their reqv     */
/*-----------------------------------------------------------------*/
static void
replaceRegEqv (ebbIndex *ebbi)
{
  eBBlock ** ebbs = ebbi->bbOrder;
  int count = ebbi->count;
  int i;

  /* Reset all the def/use info (Otherwise there may be stale def/use */
  /* info if a variable is also used in a previous functions) */
  for (i = 0; i < count; i++)
    {
      iCode *ic;

      if (ebbs[i]->noPath)
        continue;

      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          // Convert struct / union to pointer.
          if(ic->op != ADDRESS_OF && ic->op != RETURN && ic->op != CALL && ic->op != PCALL)
            {
              if (ic->left && IS_AGGREGATE (operandType (ic->left)))
                setOperandType (ic->left, aggrToPtr (operandType (ic->left), false));
              if (ic->right && IS_AGGREGATE (operandType (ic->right)))
                setOperandType (ic->right, aggrToPtr (operandType (ic->right), false));
              if (ic->result && IS_AGGREGATE (operandType (ic->result)))
                setOperandType (ic->result, aggrToPtr (operandType (ic->result), false));
            }
          
          if (SKIP_IC2 (ic))
            continue;

          if (IS_TRUE_SYMOP (IC_RESULT (ic)))
            {
              OP_DEFS (IC_RESULT (ic)) = NULL;
              OP_USES (IC_RESULT (ic)) = NULL;
            }

          if (IS_TRUE_SYMOP (IC_RIGHT (ic)))
            {
              OP_DEFS (IC_RIGHT (ic)) = NULL;
              OP_USES (IC_RIGHT (ic)) = NULL;
            }

          if (IS_TRUE_SYMOP (IC_LEFT (ic)))
            {
              OP_DEFS (IC_LEFT (ic)) = NULL;
              OP_USES (IC_LEFT (ic)) = NULL;
            }
        }
    }

  /* Update the symbols' def bitvector so we know if there is   */
  /* a defining iCode or not. Only replace a local variable     */
  /* with its register equivalent if there is a defining iCode; */
  /* otherwise, the port's register allocater may choke.        */
  cseAllBlocks (ebbi, TRUE);

  for (i = 0; i < count; i++)
    {
      iCode *ic;

      if (ebbs[i]->noPath)
        continue;

      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if (SKIP_IC2 (ic))
            continue;

          if (ic->op == RECEIVE)
            {
              if (OP_SYMBOL (IC_RESULT (ic))->addrtaken)
                OP_SYMBOL (IC_RESULT (ic))->isspilt = 1;
            }

          /* general case */
          if (IS_TRUE_SYMOP (IC_RESULT (ic)))
            {
              if (POINTER_SET (ic))
                replaceRegEqvOperand (ic, &IC_RESULT (ic), 1, 1);
              else
                replaceRegEqvOperand (ic, &IC_RESULT (ic), 0, 0);
            }
          if (IS_TRUE_SYMOP (IC_RIGHT (ic)))
            replaceRegEqvOperand (ic, &IC_RIGHT (ic), 1, 0);
          if (IS_TRUE_SYMOP (IC_LEFT (ic)))
            replaceRegEqvOperand (ic, &IC_LEFT (ic), 1, 0);
        }
    }
}

/*-----------------------------------------------------------------*/
/* findReqv - search for a register equivalent                     */
/*-----------------------------------------------------------------*/
operand *
findReqv (symbol * prereqv, eBBlock ** ebbs, int count)
{
  int i;
  iCode * ic;

  /* for all blocks do */
  for (i=0; i<count; i++)
    {
      /* for all instructions in the block do */
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if (IS_ITEMP (IC_LEFT (ic))
              && OP_SYMBOL (IC_LEFT (ic))->prereqv == prereqv)
            return IC_LEFT (ic);
          if (IS_ITEMP (IC_RIGHT (ic))
              && OP_SYMBOL (IC_RIGHT (ic))->prereqv == prereqv)
            return IC_RIGHT (ic);
          if (IS_ITEMP (IC_RESULT (ic))
              && OP_SYMBOL (IC_RESULT (ic))->prereqv == prereqv)
            return IC_RESULT (ic);
        }
    }

  return NULL;
}

/*-----------------------------------------------------------------*/
/* killDeadCode - eliminates dead assignments                      */
/*-----------------------------------------------------------------*/
int
killDeadCode (ebbIndex * ebbi)
{
  eBBlock ** ebbs = ebbi->dfOrder;
  int count = ebbi->count;
  int change = 1;
  int gchange = 0;
  int i = 0;

  /* basic algorithm :-                                          */
  /* first the exclusion rules :-                                */
  /*  1. if result is a global or volatile then skip             */
  /*  2. if assignment and result is a temp & isaddr  then skip  */
  /*     since this means array & pointer access, will be taken  */
  /*     care of by alias analysis.                              */
  /*  3. if the result is used in the remainder of the block skip */
  /*  4. if this definition does not reach the end of the block  */
  /*     i.e. the result is not present in the outExprs then KILL */
  /*  5. if it reaches the end of block & is used by some success */
  /*     or then skip                                            */
  /*     else            KILL                                    */
  /* this whole process is carried on iteratively till no change */
  do
    {
      change = 0;
      /* for all blocks do */
      for (i = 0; i < count; i++)
        {
          iCode *ic;

          /* for all instructions in the block do */
          for (ic = ebbs[i]->sch; ic; ic = ic->next)
            {
              int kill, j;
              kill = 0;

              if (SKIP_IC (ic) && ic->op != RECEIVE ||
                  ic->op == IFX ||
                  ic->op == RETURN ||
                  ic->op == DUMMY_READ_VOLATILE ||
                  ic->op == CRITICAL ||
                  ic->op == ENDCRITICAL)
                continue;

              /* Since both IFX & JUMPTABLE (in SKIP_IC) have been tested for */
              /* it is now safe to assume IC_LEFT, IC_RIGHT, & IC_RESULT are  */
              /* valid. */

              /* if the result is volatile then continue */
              if (IC_RESULT (ic) && isOperandVolatile (IC_RESULT (ic), FALSE))
                continue;

              /* if the result is a temp & isaddr then skip */
              if (IC_RESULT (ic) && POINTER_SET (ic))
                continue;

              /* if the results address has been taken then skip */
              if (IS_SYMOP (IC_RESULT (ic)) && OP_SYMBOL (IC_RESULT (ic))->addrtaken)
                continue;

              if (POINTER_GET (ic) && IS_VOLATILE (operandType (IC_LEFT (ic))->next)
                  && !SPIL_LOC (IC_RESULT (ic)))
                continue;

              /* if the result is used in the remainder of the */
              /* block then skip */
              if (usedInRemaining (IC_RESULT (ic), ic->next))
                continue;

              /* does this definition reach the end of the block
                 or the usage is zero then we can kill */
              if (!bitVectBitValue (ebbs[i]->outDefs, ic->key))
                kill = 1;       /* if not we can kill it */
              else
                {
                  /* if this is a global variable or function parameter */
                  /* we cannot kill anyway             */
                  if (isOperandGlobal (IC_RESULT (ic)) ||
                      (OP_SYMBOL (IC_RESULT (ic))->_isparm &&
                       !OP_SYMBOL (IC_RESULT (ic))->ismyparm))
                    continue;

                  /* if we are sure there are no usages */
                  if (bitVectIsZero (OP_USES (IC_RESULT (ic))))
                    {
                      kill = 1;
                      goto kill;
                    }

                  /* reset visited flag */
                  for (j = 0; j < count; ebbs[j++]->visited = 0);

                  /* find out if this definition is alive */
                  if (applyToSet (ebbs[i]->succList, isDefAlive, ic))
                    continue;

                  kill = 1;
                }

            kill:
              /* kill this one if required */
              if (kill)
                {
                  bool volLeft = IS_SYMOP (IC_LEFT (ic))
                                 && isOperandVolatile (IC_LEFT (ic), FALSE);
                  bool volRight = IS_SYMOP (IC_RIGHT (ic))
                                  && isOperandVolatile (IC_RIGHT (ic), FALSE);

                  /* a dead address-of operation should die, even if volatile */
                  if (ic->op == ADDRESS_OF)
                    volLeft = FALSE;

                  if (ic->next && ic->seqPoint == ic->next->seqPoint
                      && (ic->next->op == '+' || ic->next->op == '-'))
                    {
                      if (isOperandEqual (IC_LEFT(ic), IC_LEFT(ic->next))
                          || isOperandEqual (IC_LEFT(ic), IC_RIGHT(ic->next)))
                        volLeft = FALSE;
                      if (isOperandEqual (IC_RIGHT(ic), IC_LEFT(ic->next))
                          || isOperandEqual (IC_RIGHT(ic), IC_RIGHT(ic->next)))
                        volRight = FALSE;
                    }

                  if (POINTER_GET (ic) && IS_VOLATILE (operandType (IC_LEFT (ic))->next))
                    {
                      if (SPIL_LOC (IC_RESULT (ic)))
                        {
                          bitVectUnSetBit (OP_DEFS (ic->result), ic->key);
                          IC_RESULT (ic) = newiTempFromOp (IC_RESULT (ic));
                          SPIL_LOC (IC_RESULT (ic)) = NULL;
                        }
                      continue;
                    }

                  change = 1;
                  gchange++;

                  /* now delete from defUseSet */
                  deleteItemIf (&ebbs[i]->outExprs, ifDiCodeIsX, ic);
                  bitVectUnSetBit (ebbs[i]->outDefs, ic->key);

                  /* and defset of the block */
                  bitVectUnSetBit (ebbs[i]->defSet, ic->key);

                  /* If this is the last of a register equivalent, */
                  /* look for a successor register equivalent. */
                  bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
                  if (IS_ITEMP (IC_RESULT (ic))
                      && OP_SYMBOL (IC_RESULT (ic))->isreqv
                      && bitVectIsZero (OP_DEFS (IC_RESULT (ic))))
                    {
                      symbol * resultsym = OP_SYMBOL (IC_RESULT (ic));
                      symbol * prereqv = resultsym->prereqv;

                      if (prereqv && prereqv->reqv && (OP_SYMBOL (prereqv->reqv) == resultsym))
                        {
                          operand * newreqv;

                          IC_RESULT (ic) = NULL;
                          newreqv = findReqv (prereqv, ebbs, count);
                          if (newreqv)
                            {
                              prereqv->reqv = newreqv;
                            }
                        }
                    }

                  /* delete the result */
                  if (IC_RESULT (ic))
                    bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
                  IC_RESULT (ic) = NULL;

                  if (volLeft || volRight)
                    {
                      /* something is volatile, so keep the iCode */
                      /* and change the operator instead */
                      ic->op = DUMMY_READ_VOLATILE;

                      /* keep only the volatile operands */
                      if (!volLeft)
                        IC_LEFT (ic) = NULL;
                      if (!volRight)
                        IC_RIGHT (ic) = NULL;
                    }
                  else
                    {
                      /* nothing is volatile, eliminate the iCode */
                      unsetDefsAndUses (ic);
                      remiCodeFromeBBlock (ebbs[i], ic);

                      /* for the left & right remove the usage */
                      if (IS_SYMOP (ic->left) && OP_SYMBOL (ic->left)->isstrlit)
                        freeStringSymbol (OP_SYMBOL (ic->left));
                      if (IS_SYMOP (ic->right) && OP_SYMBOL (ic->right)->isstrlit)
                        freeStringSymbol (OP_SYMBOL (ic->right));
                    }
                }
            }                   /* end of all instructions */

          if (!ebbs[i]->sch && !ebbs[i]->noPath)
            disconBBlock (ebbs[i], ebbi);
        }                       /* end of for all blocks */
    }                           /* end of do */
  while (change);

  return gchange;
}

/*-----------------------------------------------------------------*/
/* printCyclomatic - prints the cyclomatic information             */
/*-----------------------------------------------------------------*/
static void
printCyclomatic (eBBlock ** ebbs, int count)
{
  int nEdges = elementsInSet (graphEdges);
  int i, nNodes = 0;

  for (i = 0; i < count; i++)
    nNodes += (!ebbs[i]->noPath);

  /* print the information */
  werror (I_CYCLOMATIC, currFunc->name, nEdges, nNodes, nEdges - nNodes + 2);
}

/*-----------------------------------------------------------------*/
/* discardDeadParamReceives - remove any RECEIVE opcodes which     */
/* refer to dead variables.                                        */
/*-----------------------------------------------------------------*/
static void
discardDeadParamReceives (eBBlock ** ebbs, int count)
{
  int i;
  iCode *ic;
  iCode dummyIcode;

  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if (ic->op == RECEIVE)
            {
              if (IC_RESULT (ic) && OP_SYMBOL (IC_RESULT (ic))
                  && !OP_SYMBOL (IC_RESULT (ic))->used)
                {
#if 0
                  fprintf (stderr, "discarding dead receive for %s\n",
                           OP_SYMBOL (IC_RESULT (ic))->name);
#endif
                  dummyIcode.next = ic->next;
                  remiCodeFromeBBlock (ebbs[i], ic);
                  ic = &dummyIcode;
                }
            }
        }
    }
}

/*-----------------------------------------------------------------*/
/* optimizeOpWidth - reduce operation width.                       */
/* Wide arithmetic operations where the result is cast to narrow   */
/* type can be optimized by doing the casts on the operands        */
/*-----------------------------------------------------------------*/
static int
optimizeOpWidth (eBBlock ** ebbs, int count)
{
  int i;
  int change = 0;
  iCode *ic, *newic;
  iCode *uic, *skipuic;
  sym_link *nextresulttype;
  symbol *sym;
  int resultsize, nextresultsize;

  // Wide loop counter
  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          sym_link *newcountertype, *oldcountertype;
          const symbol *label;
          const iCode *ifx, *inc = 0, *obstacle = 0;
          iCode *mul = 0;
          bool found = false;

          if (ic->op != LABEL || !ic->next)
            continue;

          label = IC_LABEL (ic);
          ic = ic->next;

          if (ic->op != '<' || !IS_ITEMP (IC_LEFT (ic)) || bitVectnBitsOn (OP_DEFS (IC_LEFT (ic))) != 2)
            continue;

          oldcountertype = operandType (IC_LEFT (ic));
          if (IS_VOLATILE (oldcountertype))
            continue;

          // Only try to narrow wide counters.
          // TODO: Handle signed types as well, maybe even transform int to unsigned int?
          if (!IS_INTEGRAL(oldcountertype) || bitsForType (oldcountertype) <= 8 || TARGET_IS_DS390 || TARGET_IS_DS400 || (!SPEC_USIGN (oldcountertype)))
            continue;

          ifx = ifxForOp (IC_RESULT (ic), ic);

          if (!ifx || IC_TRUE (ifx) || i + 1 >= count)
            continue;

          /* For now we handle only loops that have no complex control flow inside them and where
             the loop is entered and left through ifx only */
          for (uic = ebbs[i + 1]->sch; uic; uic = uic->next)
            {
              if (uic->op == GOTO && IC_LABEL (uic) == label)
                break;

              if (!obstacle &&
                  (uic->op == CALL || uic->op == PCALL || uic->op == IFX || uic->op == LABEL ||
                   uic->op == GOTO && IC_LABEL (uic) != label || uic->op == INLINEASM))
                {
                  obstacle = uic;
                  break;
                }
            }

          // TODO: Proceed despite obstacle, but only consider array accesses before obstacle.
          if (obstacle || !uic || uic->op != GOTO || IC_LABEL (uic) != label)
            continue;

          const bitVect *uses;
          int bit;

          uses = bitVectCopy (OP_USES (IC_LEFT (ic)));
          for (bit = bitVectFirstBit (uses); bitVectnBitsOn (uses); bitVectUnSetBit (uses, bit), bit = bitVectFirstBit (uses))
            {
              operand *prevresult = IC_LEFT(ic);
              operand *mulotherop = 0;
              iCode *mul_candidate = 0;
              uic = hTabItemWithKey (iCodehTab, bit);

              if (!uic)
                {
                  /* This iCode has been deleted but is still        */
                  /* referenced. This shouldn't happen if everything */
                  /* else is managing OP_USES properly, but better   */
                  /* to ignore the problem than crash. */
                  //printf ("%s used in iCode %d, but iCode missing\n",  OP_SYMBOL (IC_LEFT (ic))->name, bit);
                  continue;
                }

              if (uic->op == '+' && IS_OP_LITERAL (IC_RIGHT (uic)) && operandLitValue (IC_RIGHT (uic)) == 1 && isOperandEqual (IC_LEFT (uic), IC_LEFT (ic)))
                {
                  inc = uic;
                  continue;
                }

              if (uic->op != CAST && uic->op != '=' && uic->op != '+' && uic->op != '*' && uic->op != '-' && uic->op != LEFT_OP && uic->op != RIGHT_OP && uic->op != '<')
                {
                  found = false;
                  break;
                }

              if (uic && uic->op == '*')
                {
                  mulotherop = isOperandEqual (IC_LEFT (ic), IC_LEFT (uic)) ? IC_RIGHT (uic) : IC_LEFT (uic);
                  if (isOperandEqual (IC_RIGHT (ic), mulotherop))
                    {
                      mul_candidate = uic;
                      uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (IC_RESULT (uic))));
                    }
                }

              for (int i = 0;
                   i < 8 && uic &&
                   (uic->op == CAST && bitsForType (operandType (IC_RESULT (uic))) >= 16 ||
                    uic->op == '=' || uic->op == '+' || uic->op == LEFT_OP ||
                    uic->op == '*' && IS_OP_LITERAL (IC_RIGHT (uic)) && operandLitValue (IC_RIGHT (uic)) >= 1);
                   i++)
                {
                  prevresult = IC_RESULT (uic);
                  uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (IC_RESULT (uic))));
                }

              if (!uic)
                continue;

              // Use as array index?
              if (uic->op == GET_VALUE_AT_ADDRESS || POINTER_SET(uic) && isOperandEqual (IC_RESULT (uic), prevresult))
                {
                  found = true;
                  if (mul_candidate)
                    mul = mul_candidate;
                }
            }

          if (!found || !inc)
            continue;

          /* All backends (except ds390 / ds400) have an array size limit smaller than 2^16. Thus if the loop counter ever goes outside
             the range of a 16-bit type, the array access would result in undefined behaviour. We can thus replace the loop
             counter by a 16-bit type. If we found a squaring multiplication, we can even use an 8-bit type*/
          if (bitsForType (oldcountertype) <= 16 && !mul)
            continue;

          newcountertype = mul ? newCharLink () : newIntLink ();
          SPEC_USIGN (newcountertype) = 1;
          OP_SYMBOL (IC_LEFT (ic))->type = newcountertype;
          OP_SYMBOL (IC_RESULT (inc))->type = newcountertype;

          uses = bitVectCopy (OP_USES (IC_LEFT (ic)));
          for (bit = bitVectFirstBit (uses); bitVectnBitsOn (uses); bitVectUnSetBit (uses, bit), bit = bitVectFirstBit (uses))
            {
              uic = hTabItemWithKey (iCodehTab, bit);

              if (uic == inc || uic == mul)
                continue;
              if (uic->op == CAST)
                continue;
              if (uic->key == ic->key)
                continue;
              if (uic->op == '=')
                {
                  uic->op = CAST;
                  continue;
                }

              // Need to insert cast.
              prependCast (uic, IC_LEFT (ic), oldcountertype, ebbs[i + 1]);
            }

          // Insert cast for comparison.
          prependCast (ic, IC_RIGHT (ic), newcountertype, ebbs[i]);

          // Bonus: Can narrow a multiplication in the loop.
          if (mul)
            {
              prependCast (mul, IC_LEFT (mul), newcountertype, ebbs[i + 1]);
              prependCast (mul, IC_RIGHT (mul), newcountertype, ebbs[i + 1]);
              nextresulttype = newIntLink();
              SPEC_USIGN (nextresulttype) = 1;
              appendCast(mul, nextresulttype, ebbs[i + 1]);
            }
        }
    }

  // Operation followed by cast
  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if ( (ic->op == CAST || ic->op == '+' || ic->op == '-' || ic->op == UNARYMINUS || ic->op == '*' ||
                ic->op == LEFT_OP || ic->op == RIGHT_OP || ic->op == BITWISEAND || ic->op == '|' || ic->op == '^') &&
               IC_RESULT (ic) && IS_ITEMP (IC_RESULT (ic)) )
            {
              sym_link *resulttype = operandType (IC_RESULT (ic));

              if (!IS_INTEGRAL (resulttype) ||
                  ic->op != CAST && !(IS_SYMOP (IC_LEFT (ic)) ||
                  IS_OP_LITERAL (IC_LEFT (ic))) ||
                  !(IS_SYMOP (IC_RIGHT (ic)) || IS_OP_LITERAL (IC_RIGHT (ic)) || ic->op == UNARYMINUS))
                {
                  continue;
                }

              resultsize = bitsForType (resulttype);

              /* There must be only one use of this first result */
              if (bitVectnBitsOn (OP_DEFS (IC_RESULT (ic))) != 1 || bitVectnBitsOn (OP_USES (IC_RESULT (ic))) != 1)
                continue;

              uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (IC_RESULT (ic))));

              if (!uic)
                continue;

              /* Skip over assignment */
              skipuic = NULL;
              if (uic->op == '=' && IS_ITEMP (IC_RESULT (uic)) &&
                  bitVectnBitsOn (OP_DEFS (IC_RESULT (uic))) == 1 && bitVectnBitsOn (OP_USES (IC_RESULT (ic))) == 1 && bitVectnBitsOn (OP_USES (IC_RESULT (uic))) == 1 &&
                  compareType (operandType (IC_RESULT (ic)), operandType (IC_RESULT (uic)), false) == 1)
                {
                  skipuic = uic;
                  uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (IC_RESULT (uic))));
                  if (!uic)
                    continue;
                }

              /* Try to handle a few cases where the result has multiple uses */
              else if (ic->op == '*' && bitsForType (operandType (IC_RESULT (ic))) > 16 && uic->op == '=' &&
                       bitVectnBitsOn (OP_DEFS (IC_RESULT (uic))) == 1 && bitVectnBitsOn (OP_USES (IC_RESULT (ic))) == 1 && bitVectnBitsOn (OP_USES (IC_RESULT (uic))) > 1 &&
                       compareType (operandType (IC_RESULT (ic)), operandType (IC_RESULT (uic)), false) == 1)
                {
                  bool ok = true;
                  const bitVect *uses;
                  int bit;

                  uses = bitVectCopy (OP_USES (IC_RESULT (uic)));
                  for (bit = bitVectFirstBit(uses); bitVectnBitsOn (uses); bitVectUnSetBit(uses, bit), bit = bitVectFirstBit(uses))
                    {
                      iCode *uuic = hTabItemWithKey (iCodehTab, bit);
                      if (uuic->op != CAST || bitsForType (operandType (IC_RESULT (uuic))) > 16 || IS_BOOLEAN (operandType (IC_RESULT (uuic))))
                        {
                          ok = false;
                          break;
                        }
                    }

                  if (!ok)
                    continue;

                  nextresulttype = newIntLink ();
                  SPEC_USIGN (nextresulttype) = 1;
                  sym = OP_SYMBOL (IC_RESULT (uic));
                  sym->type = nextresulttype;

                  nextresulttype = newIntLink ();
                  SPEC_USIGN (nextresulttype) = 1;
                  goto optimize;
                }

              if (uic->op != CAST && uic->op != '+' && uic->op != LEFT_OP && uic->op != RIGHT_OP)
                continue;

              /* Special handling since we might need more bits in the operand than in the result */
              if (ic->op == RIGHT_OP)
                {
                  int shiftbits, resultbits;

                  if (!IS_OP_LITERAL (IC_RIGHT (ic)))
                    continue;

                  shiftbits = (int) operandLitValue (IC_RIGHT (ic));
                  resultbits = bitsForType (operandType (IC_RESULT (uic)));

                  if (resultbits + shiftbits > 16)
                    continue;
                  else if (resultbits + shiftbits > 8)
                    nextresulttype = newIntLink ();
                  else
                    nextresulttype = newCharLink ();
                  SPEC_USIGN (nextresulttype) = 1;
                }
              /* It must be a cast to another integer type that */
              /* has fewer bits */
              else if (uic->op == LEFT_OP || uic->op == RIGHT_OP)
                {
                   /* Since shifting by the width of an operand or more is undefined behaviour, and no type is wider than 256 bits,
                      we can optimize when the result is used as right operand to a shift. */
                   if (!isOperandEqual (IC_RESULT (ic), IC_RIGHT (uic)) || isOperandEqual (IC_RESULT (ic), IC_LEFT (uic)))
                     continue;

                   nextresulttype = newCharLink ();
                }
              else
                {
                  nextresulttype = operandType (IC_RESULT (uic));
                  if (!IS_INTEGRAL (nextresulttype) && !(IS_PTR (nextresulttype) && NEARPTRSIZE == 2))
                     continue;

                  if (IS_PTR (nextresulttype))
                    {
                      nextresulttype = newIntLink ();
                      SPEC_USIGN (nextresulttype) = 1;
                    }
                  else
                    {
                      nextresulttype = copyLinkChain (nextresulttype);
                    }
                }

              nextresultsize = bitsForType (nextresulttype);
              if (nextresultsize >= resultsize)
                 continue;
              /* Cast to bool and bool-like types must be preserved to ensure that all nonzero values are correctly cast to true */
              if (uic->op == CAST && IS_BOOLEAN (nextresulttype))
                 continue;

optimize:
              /* Make op result narrower */
              sym = OP_SYMBOL (IC_RESULT (ic));
              sym->type = nextresulttype;

              /* Insert casts on operands */
              if (ic->op != CAST)
                {
                  sym_link *clefttype = copyLinkChain (nextresulttype);
                  SPEC_VOLATILE (clefttype) = IS_OP_VOLATILE (IC_LEFT (ic));
                  sym_link *crighttype = copyLinkChain (nextresulttype);
                  SPEC_VOLATILE (crighttype) = IS_OP_VOLATILE (IC_RIGHT (ic));

                  if (IS_SYMOP (IC_LEFT (ic)))
                    {
                      newic = newiCode (CAST, operandFromLink (clefttype), IC_LEFT (ic));
                      hTabAddItem (&iCodehTab, newic->key, newic);
                      bitVectSetBit (OP_USES (IC_LEFT (ic)), newic->key);
                      IC_RESULT (newic) = newiTempOperand (clefttype, 0);
                      OP_DEFS (IC_RESULT (newic)) = bitVectSetBit (OP_DEFS (IC_RESULT (newic)), newic->key);
                      bitVectUnSetBit (OP_USES (IC_LEFT (ic)), ic->key);
                      IC_LEFT (ic) = operandFromOperand (IC_RESULT (newic));
                      OP_USES (IC_LEFT (ic)) = bitVectSetBit (OP_USES (IC_LEFT (ic)), ic->key);
                      newic->filename = ic->filename;
                      newic->lineno = ic->lineno;
                      addiCodeToeBBlock (ebbs[i], newic, ic);
                    }
                  else
                    {
                      wassert (IS_OP_LITERAL (IC_LEFT (ic)));
                      IC_LEFT (ic) = operandFromValue (valCastLiteral (clefttype, operandLitValue (IC_LEFT (ic)), operandLitValue (IC_LEFT (ic))), false);
                    }
                  if (ic->op != LEFT_OP && IS_SYMOP (IC_RIGHT (ic)))
                    {
                      newic = newiCode (CAST, operandFromLink (crighttype), IC_RIGHT (ic));
                      hTabAddItem (&iCodehTab, newic->key, newic);
                      bitVectSetBit (OP_USES (IC_RIGHT (ic)), newic->key);
                      IC_RESULT (newic) = newiTempOperand (crighttype, 0);
                      OP_DEFS (IC_RESULT (newic)) = bitVectSetBit (OP_DEFS (IC_RESULT (newic)), newic->key);
                      bitVectUnSetBit (OP_USES (IC_RIGHT (ic)), ic->key);
                      IC_RIGHT (ic) = operandFromOperand (IC_RESULT (newic));
                      OP_USES (IC_RIGHT (ic)) = bitVectSetBit (OP_USES (IC_RIGHT (ic)), ic->key);
                      newic->filename = ic->filename;
                      newic->lineno = ic->lineno;
                      addiCodeToeBBlock (ebbs[i], newic, ic);
                    }
                  else if (ic->op != LEFT_OP && ic->op != UNARYMINUS)
                    {
                      wassert (IS_OP_LITERAL (IC_RIGHT (ic)));
                      IC_RIGHT (ic) = operandFromValue (valCastLiteral (crighttype, operandLitValue (IC_RIGHT (ic)), operandLitValue (IC_RIGHT (ic))), false);
                    }
                }
              if (uic->op == CAST && ic->op != RIGHT_OP)
                {
                  uic->op = '=';
                  if (skipuic)
                    attachiCodeOperand (skipuic->right, &uic->right, uic);
                }
              if (skipuic && skipuic->op == '=' &&
                  compareType (operandType (IC_RESULT (skipuic)), operandType (IC_RIGHT (skipuic)), false) != 1)
                {
                  /* Because of the type change, this assignment  */
                  /* is now really a cast, so make it official.   */
                  /* Later optimizeCastCast() will decide if this */
                  /* is safe to remove completely. */
                  skipuic->op = CAST;
                  IC_LEFT (skipuic) = operandFromLink (copyLinkChain (operandType (IC_RESULT (skipuic))));
                }
              change++;
            }
        }
    }

  return change;
}

/*-----------------------------------------------------------------*/
/* Go back a chain of assignments / casts to try to find a string   */
/* literal symbol that op really is.                               */
/*-----------------------------------------------------------------*/
static symbol *findStrLitDef (operand *op, iCode **def)
{
  for(;;)
    {
      if (!IS_ITEMP (op))
        return (0);

      if (bitVectnBitsOn (OP_DEFS (op)) != 1)
        return (0);

      iCode *dic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (op)));

      wassert (dic);

      if (dic->op == ADDRESS_OF)
        {
          if (def)
            *def = dic;
          symbol *sym = OP_SYMBOL (IC_LEFT (dic));
          return (sym->isstrlit ? sym : 0);
        }

      if (dic->op != '=' && dic->op != CAST)
        return (0);

      op = IC_RIGHT (dic);
    }
}

/*-----------------------------------------------------------------*/
/* optimizeStdLibCall - optimize calls to standard library.        */
/* for now we just merge adjacent calls to puts()                  */
/*-----------------------------------------------------------------*/
static void
optimizeStdLibCall (eBBlock ** ebbs, int count)
{
  iCode *ic, *nic, *ndic;
  symbol *strsym, *nstrsym, *cstrsym;
  sym_link *strlink, *nstrlink;
  size_t replacecost;

  for (int i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          // Look for call to puts().
          if (ic->op != CALL || !ic->prev || ic->prev->op != IPUSH && ic->prev->op != SEND)
            continue;
          if (!IS_SYMOP (ic->left) || strcmp (OP_SYMBOL (ic->left)->rname, "_puts"))
            continue;

          // Look for following call to puts().
          for (nic = ic->next; nic; nic = nic->next)
            {
              if (nic->op == '=' && !POINTER_SET (ic) || nic->op == CAST)
                {
                  if (!IS_ITEMP (IC_RESULT (nic)))
                    break;
                  if (IS_OP_VOLATILE (IC_RIGHT (nic)))
                    break;
                }
              else if (nic->op == ADDRESS_OF)
                {
                  if (!IS_ITEMP (IC_RESULT (nic)))
                    break;
                }
              else if (nic->op == IPUSH || nic->op == SEND)
                {
                  if (IS_OP_VOLATILE (IC_LEFT (nic)))
                    break;
                }
              else // Todo: Handle more to make the optimization more general.
                break;
            }
          if (!nic || nic->op != CALL || nic->prev->op != IPUSH && nic->prev->op != SEND)
            continue;
          if (!IS_SYMOP (nic->left) || strcmp (OP_SYMBOL (nic->left)->rname, "_puts"))
            continue;

          // Check that the return values are unused
          if (IC_RESULT (ic) && (!IS_ITEMP (IC_RESULT (ic)) || bitVectnBitsOn (OP_USES (IC_RESULT (ic)))))
            continue;
          if (IC_RESULT (nic) && (!IS_ITEMP (IC_RESULT (nic)) || bitVectnBitsOn (OP_USES (IC_RESULT (nic)))))
            continue;

          // Check that their parameters are string literals
          strsym = findStrLitDef (IC_LEFT (ic->prev), 0);
          nstrsym = findStrLitDef (IC_LEFT (nic->prev), &ndic);
          if (!strsym || !nstrsym)
            continue;
          strlink = strsym->etype;
          nstrlink = nstrsym->etype;

          // Calculate the cost of doing the replacement in bytes of string literal
          replacecost = 1; // For '\n'
          if (strsym->isstrlit > 1)
            replacecost += strlen (SPEC_CVAL (strlink).v_char);
          if (nstrsym->isstrlit > 1)
            replacecost += strlen (SPEC_CVAL (nstrlink).v_char);

          // Doing the replacement saves at least 6 bytes of call overhead (assuming pointers are 16 bits).
          if (replacecost > 7 - optimize.codeSize + 4 * optimize.codeSpeed)
            continue;

          // Combine strings
          struct dbuf_s dbuf;
          dbuf_init (&dbuf, 3);
          dbuf_append_str(&dbuf, SPEC_CVAL (strlink).v_char);
          dbuf_append_str(&dbuf, "\n");
          dbuf_append_str(&dbuf, SPEC_CVAL (nstrlink).v_char);
          cstrsym = stringToSymbol (rawStrVal (dbuf_c_str (&dbuf), dbuf_get_length (&dbuf) + 1))->sym;
          freeStringSymbol (nstrsym);
          dbuf_destroy (&dbuf);

          // Make second call print the combined string (which allows further optimization with subsequent calls)
          IC_LEFT (ndic)->key = cstrsym->key = ++operandKey;
          IC_LEFT (ndic)->svt.symOperand = cstrsym;

          // Change unused call to assignments to self to mark it for dead-code elimination.
          bitVectSetBit (OP_USES (IC_LEFT (ic->prev)), ic->key);
          bitVectSetBit (OP_DEFS (IC_LEFT (ic->prev)), ic->prev->key);
          ic->op = '=';
          IC_RESULT (ic) = IC_LEFT (ic->prev);
          IC_RIGHT (ic) = IC_LEFT (ic->prev);
          IC_LEFT (ic) = 0;
          ic->prev->op = '=';
          IC_RESULT (ic->prev) = IC_LEFT (ic->prev);
          IC_RIGHT (ic->prev) = IC_LEFT (ic->prev);
          IC_LEFT (ic->prev) = 0;
        }
    }
}

/*-----------------------------------------------------------------*/
/* optimizeCastCast - remove unneeded intermediate casts.          */
/* Integer promotion may cast (un)signed char to int and then      */
/* recast the int to (un)signed long. If the signedness of the     */
/* char and long are the same, the cast can be safely performed in */
/* a single step.                                                  */
/*-----------------------------------------------------------------*/
static void
optimizeCastCast (eBBlock **ebbs, int count)
{
  for (int i = 0; i < count; i++)
    {
      for (iCode *ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if ((ic->op == CAST || ic->op == '=' || ic->op == '+' || ic->op == ADDRESS_OF) && ic->result && IS_ITEMP (ic->result))
            {
              sym_link *type1 = operandType ((ic->op == CAST || ic->op == '=')? ic->right : ic->left);
              sym_link *type2 = operandType (ic->result);

              /* There must be only one use of this first result */
              if (bitVectnBitsOn (OP_USES (ic->result)) != 1 ||
                bitVectnBitsOn (OP_DEFS (ic->result)) != 1)
                continue;

              iCode *uic = hTabItemWithKey (iCodehTab,
                        bitVectFirstBit (OP_USES (ic->result)));
              if(!uic || !uic->result)
                continue;

              sym_link *type3 = operandType (uic->result);
              if (ic->op == ADDRESS_OF && uic->op == CAST &&
                IS_PTR (type2) && IS_PTR (type3) && getAddrspace (type2) == getAddrspace (type3) && sclsFromPtr (type2) == sclsFromPtr (type3) &&
                ic->next == uic)
                {
                  bitVectUnSetBit (OP_DEFS (ic->result), ic->key);
                  IC_RESULT (ic) = IC_RESULT (uic);
                  bitVectSetBit (OP_DEFS (ic->result), ic->key);
                  unsetDefsAndUses (uic);
                  remiCodeFromeBBlock (ebbs[i], uic);
                  continue;
                }
              else if (ic->op == '=' && !POINTER_SET(ic) && uic->op == CAST && ic->next == uic)
                 {
                  if (IS_PTR (type1) && IS_PTR (type2))
                    ;
                  else if (IS_INTEGRAL (type1) && IS_INTEGRAL (type2) && SPEC_USIGN (type1) == SPEC_USIGN (type2))
                    ;
                  else // Sometimes we encounter weird assignments that change sign. E.g. signed char -> unsigned char -> int would break if we try to optimize.
                    continue;
                  bitVectUnSetBit (OP_USES (uic->right), uic->key);
                  uic->right = ic->right;
                  if (IS_SYMOP (uic->right))
                    bitVectSetBit (OP_USES (uic->right), uic->key);
                  unsetDefsAndUses (ic);
                  remiCodeFromeBBlock (ebbs[i], ic);
                  continue;
                }
              else if (ic->op == CAST && IS_INTEGRAL (type1) && IS_INTEGRAL (type2) && IS_INTEGRAL (type3))
                {
                  int size1 = bitsForType (type1);
                  int size2 = bitsForType (type2);
                  if (size2 < size1)
                    continue;
                  /* If they are the same size, they must have the same signedness */
                  if (size2 == size1 && SPEC_USIGN (type2) != SPEC_USIGN (type1))
                    continue;

                  /* Cast to bool must be preserved to ensure that all nonzero values are correctly cast to true */
                  if (SPEC_NOUN (type2) == V_BOOL && SPEC_NOUN(type3) != V_BOOL)
                     continue;
                  /* Similarly for signed->unsigned->signed widening casts to ensure that negative values end up as nonnegative ones in the end. */
                  if (!SPEC_USIGN (type1) && SPEC_USIGN (type2) && !SPEC_USIGN (type3) && bitsForType (type3) > bitsForType (type2))
                    continue;

                  /* Special case: Second use is a bit test */
                  if (uic->op == BITWISEAND && IS_OP_LITERAL (uic->right) && ifxForOp (uic->result, uic))
                    {
                      unsigned long long mask = operandLitValueUll (IC_RIGHT (uic));

                      /* Signed cast might set bits above the width of type1 */
                      if (!SPEC_USIGN (type1) && (mask >> (bitsForType (type1))))
                        continue;

                      IC_RIGHT (uic) = operandFromValue (valCastLiteral (type1, operandLitValue (uic->right), operandLitValue (uic->right)), false);
                    }
                  else if (uic->op == CAST) /* Otherwise this use must be a second cast */
                    {
                      /* It must be a cast to another integer type that */
                      /* has no loss of bits */
                      if (!IS_INTEGRAL (type3))
                        continue;
                      int size3 = bitsForType (type3);
                      if (size3 < size1)
                         continue;
                      /* If they are the same size, they must have the same signedness */
                      if (size3 == size2 && SPEC_USIGN (type3) != SPEC_USIGN (type2))
                        continue;

                      /* The signedness between the first and last types must match */
                      if (SPEC_USIGN (type3) != SPEC_USIGN (type1))
                        continue;
                    }
                  else
                    continue;
                }
              else if (IS_PTR (type1) && IS_PTR (type2))
                {
                  if (ic->op == CAST && uic->op == CAST)
                    ;
                  else if(uic->op == '+' && IS_PTR(type3) &&
                     getAddrspace (type1) == getAddrspace (type3) && sclsFromPtr (type1) == sclsFromPtr (type3) &&
                    (ic->op == CAST || ic->op == '+' && IS_OP_LITERAL (ic->right) && IS_OP_LITERAL (uic->right)))
                    {
                      if (ic->next == uic && isOperandEqual (ic->result, uic->left)) // Eliminate ic competely.
                        {
                          bitVectUnSetBit (OP_USES (uic->left), uic->key);
                          uic->left = ic->op == CAST ? ic->right : ic->left;
                          bitVectSetBit (OP_USES (uic->left), uic->key);
                          if (ic->op == '+')
                            uic->right = operandFromValue (valPlus (valPlus (constIntVal ("0ll"), OP_VALUE (ic->right), false), OP_VALUE (uic->right), true), false);
                          unsetDefsAndUses (ic);
                          remiCodeFromeBBlock (ebbs[i], ic);
                          continue;
                        }
                      else if (ic->op == CAST)
                        ;
                      else
                        continue;
                    }
                  else if(ic->op == '+' && uic->op == CAST && IS_PTR(type3) &&
                    getAddrspace (type1) == getAddrspace (type3) && sclsFromPtr (type1) == sclsFromPtr (type3))
                    {
                      // Change cast to assignment, change pointer type at addition.
                      uic->op = '=';
                      uic->left = NULL;
                      symbol *sym = OP_SYMBOL (ic->result);
                      sym->type = copyLinkChain (type3);
                      sym->etype = getSpec (sym->type);
                      continue;
                    }
                  else
                    continue;
                }
              else
                continue;


              /* Change the first cast to a simple assignment and */
              /* let the second cast do all the work */
              ic->op = '=';
              ic->left= NULL;

              symbol *sym = OP_SYMBOL (ic->result);
              sym->type = copyLinkChain (type1);
              sym->etype = getSpec (sym->type);
            }
        }
    }
}

/*-----------------------------------------------------------------*/
/* optimizeFinalCast - remove unneeded intermediate casts.         */
/* Backends can handle wrong types onmany pointers in poiner write */
/* and read. Exploit this to optimize out some casts. Typically    */
/* done late (i.e. just before register allocation).               */
/*-----------------------------------------------------------------*/
static void
optimizeFinalCast (ebbIndex *ebbi)
{
  // Apparently this triggers a bug in the mcs51 and ds390 backends.
  // Some regression tests fail, including gcc-torture-execute-pr38236;
  // looking into that one for -mmcs51 --model-small , register allocation
  // puts the result of a 16-bit read from e generic pointer into dptr,
  // which codegen can't handle (it genrated code where dpl is overwritten by
  // the lower byte of the result, then used as pointer once more).
  // This also triggers a pic16 bug resulting in invalid asm code being generated.
  if (TARGET_MCS51_LIKE || TARGET_IS_PIC16)
    return;

  for (int i = 0; i < ebbi->count; i++)
    {
      eBBlock **ebbs = ebbi->bbOrder;
      eBBlock *ebp = ebbs[i];

      for (iCode *ic = ebp->sch; ic; ic = ic->next)
        {
          if (ic->op != CAST || !IS_ITEMP (ic->result))
            continue;

          if (bitVectnBitsOn (OP_USES (ic->result)) != 1 || bitVectnBitsOn (OP_DEFS (ic->result)) != 1)
            continue;

          iCode *uic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_USES (ic->result)));
          if(!uic ||
            uic->op != GET_VALUE_AT_ADDRESS && !(POINTER_SET(uic) && !isOperandEqual (ic->result, uic->left) && !isOperandEqual (ic->result, uic->right) && (IS_ITEMP (ic->right) || IS_OP_LITERAL (ic->right))))
            continue;

          // For now only handle a use that follows immediately or nearly immediately.
          if (ic->next && ic->next->op != LABEL && !POINTER_SET(ic->next) &&
            (!IS_ITEMP (ic->right) || !bitVectBitValue (OP_DEFS (ic->right), ic->next->key)) &&
            uic == ic->next->next &&
            (!ic->next->result || IS_ITEMP (ic->next->result)))
            ;
          else if (uic != ic->next)
            continue;

          // Not all backends can handle multiple global operands in all operations well.
          if (IS_OP_GLOBAL (uic->result) && IS_OP_GLOBAL (ic->right) && 
            !TARGET_Z80_LIKE && !TARGET_IS_STM8 && !TARGET_F8_LIKE)
            continue;

          if (ic->op == CAST)
            {    
              sym_link *type1 = operandType (ic->right);
              sym_link *type2 = operandType (ic->left);

              if (IS_INTEGRAL (type1) && IS_INTEGRAL (type2) &&
                SPEC_NOUN (type1) == SPEC_NOUN (type2) && bitsForType (type1) == bitsForType (type2))
                ;
              else if (IS_PTR (type1) && IS_PTR (type2) &&
                sclsFromPtr (type1) == sclsFromPtr (type2) &&
                getAddrspace (type1) == getAddrspace (type2))
                ;
             else 
                continue;

             if (!IS_PTR (type1) || IS_BITFIELD(type1->next) || !IS_PTR (type2) || IS_BITFIELD(type2->next))
               continue;
            }

          // Introducinvg the duplicate smbol in the commented-out parts below would fix the type.
          // But the current backends won't propagate the rematerialization flag in their register allocators correctly if there are multiple symbols for the same iTemp.
          if (isOperandEqual (ic->result, uic->left))
            {
              operand *op = operandFromOperand (ic->right);
              /*if (IS_SYMOP (op))
                {
                  symbol *sym = copySymbol (OP_SYMBOL(op));
                  sym->type = copyLinkChain (operandType (uic->left));
                  sym->etype = getSpec (sym->type);
                  OP_SYMBOL (op) = sym;
                }
              else
                setOperandType (op, operandType (uic->left));*/
              bitVectUnSetBit (OP_USES (uic->left), uic->key);
              uic->left = op;
              bitVectSetBit (OP_USES (uic->left), uic->key);
            }
          if (isOperandEqual (ic->result, uic->right))
            {
              operand *op = operandFromOperand (ic->right);
              /*if (IS_SYMOP (op))
                {
                  symbol *sym = copySymbol (OP_SYMBOL(op));
                  sym->type = copyLinkChain (operandType (uic->right));
                  sym->etype = getSpec (sym->type);
                  OP_SYMBOL (op) = sym;
                }
              else
                setOperandType (op, operandType (uic->right));*/
              bitVectUnSetBit (OP_USES (uic->right), uic->key);
              uic->right = op;
              bitVectSetBit (OP_USES (uic->right), uic->key);
            }
          if (POINTER_SET (uic) && isOperandEqual (ic->result, uic->result))
            {
              operand *op = operandFromOperand (ic->right);
              /*if (IS_SYMOP (op))
                {
                  symbol *sym = copySymbol (OP_SYMBOL(op));
                  sym->type = copyLinkChain (operandType (uic->result));
                  sym->etype = getSpec (sym->type);
                  OP_SYMBOL (op) = sym;
                }
              else
                setOperandType (op, operandType (uic->result));*/
              op->isaddr = uic->result->isaddr;
              bitVectUnSetBit (OP_USES (uic->result), uic->key);
              uic->result = op;
              bitVectSetBit (OP_USES (uic->result), uic->key);
            }

          unsetDefsAndUses (ic);
          remiCodeFromeBBlock (ebp, ic);
      }
    }
}

/*-----------------------------------------------------------------*/
/* optimizeNegation - remove unneeded intermediate negation        */
/*-----------------------------------------------------------------*/
static void
optimizeNegation (eBBlock **ebbs, int count)
{
  int i;
  iCode *ic;
  iCode *uic;

  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if ((ic->op == '!' || (ic->op == EQ_OP || ic->op == NE_OP) && IS_OP_LITERAL (ic->right) && isEqualVal (OP_VALUE (ic->right), 0)) &&
            IC_RESULT (ic) && IS_ITEMP (IC_RESULT (ic)))
            {
              /* There must be only one use of this first result */
              if (bitVectnBitsOn (OP_USES (IC_RESULT (ic))) != 1)
                continue;

              /* This use must be an ifx */
              uic = hTabItemWithKey (iCodehTab,
                        bitVectFirstBit (OP_USES (IC_RESULT (ic))));
              if (!uic)
                continue;
              /* Todo: Optimize case where use is another negation */
              else if(uic->op == IFX) /* Remove negation by inverting jump targets */
                {
                  bool invert = ic->op == '!' || ic->op == EQ_OP;
                  bitVectUnSetBit (OP_USES (uic->left), uic->key);
                  uic->left = ic->left;
                  bitVectSetBit (OP_USES (uic->left), uic->key);
                  
                  // Make ic assignment to self, which will be optimized out.
                  bitVectUnSetBit (OP_USES (ic->left), ic->key);
                  ic->left = 0;
                  ic->right = IC_RESULT (ic);
                  bitVectSetBit (OP_USES (ic->right), ic->key);
                  ic->op = '=';

                  if (invert)
                    {
                      if (IC_TRUE (uic))
                        {
                          IC_FALSE (uic) = IC_TRUE (uic);
                          IC_TRUE (uic) = 0;
                        }
                      else
                        {
                          IC_TRUE (uic) = IC_FALSE (uic);
                          IC_FALSE (uic) = 0;
                        }
                    }
                }
            }
        }
    }
}

/* Fold pointer addition into offset of ADDRESS_OF.                  */
static void
offsetFoldGet (eBBlock **ebbs, int count)
{
  int i;
  iCode *ic;
  iCode *uic;

  if (!TARGET_Z80_LIKE && !TARGET_IS_STM8 && !TARGET_F8_LIKE)
    return;

  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if (ic->op == ADDRESS_OF && IC_RESULT (ic) && IS_ITEMP (IC_RESULT (ic)))
            {
              /* There must be only one use of the result */
              if (bitVectnBitsOn (OP_USES (IC_RESULT (ic))) != 1)
                continue;

              /* This use must be an addition / subtraction */
              uic = hTabItemWithKey (iCodehTab,
                        bitVectFirstBit (OP_USES (IC_RESULT (ic))));

              if (uic->op != '+' && uic->op != '-' || !IS_OP_LITERAL (IC_RIGHT (uic)))
                continue;

              /* Historically ADDRESS_OF didn't have a right operand */
              wassertl (IC_RIGHT (ic), "ADDRESS_OF without right operand");
              wassertl (IS_OP_LITERAL (IC_RIGHT (ic)), "ADDRESS_OF with non-literal right operand");

              bitVectUnSetBit (OP_SYMBOL (IC_RESULT (ic))->uses, uic->key);

              if (uic->op == '+')
                IC_RIGHT (uic) = operandFromLit (operandLitValue (IC_RIGHT (ic)) + operandLitValue (IC_RIGHT (uic)));
              else
                IC_RIGHT (uic) = operandFromLit (operandLitValue (IC_RIGHT (ic)) - operandLitValue (IC_RIGHT (uic)));
              IC_LEFT (uic) = operandFromOperand (IC_LEFT(ic));
              uic->op = ADDRESS_OF;
              IC_LEFT (uic)->isaddr = 1;

              ic->op = '=';
              IC_RIGHT (ic) = IC_RESULT (ic);
              IC_LEFT (ic) = 0;
              SET_ISADDR (IC_RESULT (ic), 0);
            }
        }
    }
}

/* Fold pointer addition into offset of GET_VALUE_AT_ADDRESS.                  */
/* The hc08-related ports do a similar thing in hc08/ralloc.c, packPointerOp() */
static void
offsetFoldUse (eBBlock **ebbs, int count)
{
  int i;
  iCode *ic;
  iCode *uic;

  if (!(TARGET_Z80_LIKE && !TARGET_IS_SM83) && !TARGET_IS_STM8 && !TARGET_F8_LIKE) // All z80-related targets except sm83 support non-zero right operand. stm8 also supports it.
    return;

  for (i = 0; i < count; i++)
    {
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        {
          if ((ic->op == '+' || ic->op == '-') && IC_RESULT (ic) && IS_ITEMP (IC_RESULT (ic)))
            {
              if (!IS_OP_LITERAL (IC_RIGHT (ic)))
                continue;

              /* There must be only one use of the result */
              if (bitVectnBitsOn (OP_USES (IC_RESULT (ic))) != 1)
                continue;

              /* This use must be a GET_VALUE_AT_ADDRESS */
              uic = hTabItemWithKey (iCodehTab,
                        bitVectFirstBit (OP_USES (IC_RESULT (ic))));

              if (!POINTER_GET (uic))
                continue;

              /* Historically GET_VALUE_AT_ADDRESS didn't have a right operand */
              wassertl (IC_RIGHT (uic), "GET_VALUE_AT_ADDRESS without right operand");
              wassertl (IS_OP_LITERAL (IC_RIGHT (uic)), "GET_VALUE_AT_ADDRESS with non-literal right operand");

              if (ic->op == '+')
                IC_RIGHT (uic) = operandFromLit (operandLitValue (IC_RIGHT (uic)) + operandLitValue (IC_RIGHT (ic)));
              else
                IC_RIGHT (uic) = operandFromLit (operandLitValue (IC_RIGHT (uic)) - operandLitValue (IC_RIGHT (ic)));

              ic->op = '=';
              IC_RIGHT (ic) = IC_LEFT (ic);
              IC_LEFT (ic) = 0;
              SET_ISADDR (IC_RESULT (ic), 0);
            }
        }
    }
}

/*-----------------------------------------------------------------*/
/* guessCounts - Guess execution counts for iCodes                 */
/* Needs ic->seq assigned (typically done by computeLiveRanges()   */
/*-----------------------------------------------------------------*/
void guessCounts (iCode *start_ic, ebbIndex *ebbi)
{
  iCode *ic;
  int i;
  bool needprop;

  for (ic = start_ic; ic; ic = ic->next)
    ic->count = 0;
  start_ic->pcount = 1.0f;
  needprop = TRUE;

  for(i = 0; needprop && i < 24; i++) // 24 is an arbitrary limit to reduce runtime at the cost of accuracy.
    {
      needprop = FALSE;
      for (ic = start_ic; ic; ic = ic->next)
        {
          if(ic->pcount <= 0.01) // 0.01 is an arbitrary limit to reduce runtime at the cost of accuracy.
            continue;

          ic->count += ic->pcount;

          if (ic->op == GOTO)
            {
              iCode *target = hTabItemWithKey (labelDef, IC_LABEL (ic)->key);
              target->pcount += ic->pcount;
              needprop = TRUE;
            }
          else if(ic->op == IFX) // Use a classic, simple branch prediction. Works well for typical loops.
            {
              iCode *target = hTabItemWithKey (labelDef, (IC_TRUE (ic) ? IC_TRUE (ic) : IC_FALSE (ic))->key);
              if(target->seq >= ic->seq)
                {
                  target->pcount += ic->pcount / 4;
                  if(ic->next)
                    ic->next->pcount += ic->pcount * 3 / 4;
                }
              else
                {
                  target->pcount += ic->pcount * 3 / 4;
                  if(ic->next)
                    ic->next->pcount += ic->pcount / 4;
                }
              needprop = TRUE;
            }
          else if(ic->op == JUMPTABLE)
            {
              symbol *label;
              int n = elementsInSet (IC_JTLABELS (ic));

              for (label = setFirstItem (IC_JTLABELS (ic)); label; label = setNextItem (IC_JTLABELS (ic)))
                {
                  iCode *target = hTabItemWithKey (labelDef, label->key);
                  target->pcount += ic->pcount / n;
                }
              needprop = TRUE;
            }
            else if(ic->op == CALL && IS_SYMOP (IC_LEFT (ic)) && IFFUNC_ISNORETURN (OP_SYMBOL (IC_LEFT (ic))->type))
              ;
          else if (ic->next)
            ic->next->pcount += ic->pcount;
          ic->pcount = 0.0f;
        }
    }
}

/*-----------------------------------------------------------------*/
/* narrowRead() - Will read fewer bytes by eliminating a downcast. */
/*-----------------------------------------------------------------*/
static int
narrowRead (iCode *ic, operand **opp, eBBlock *ebp)
{
  iCode *dic;
  operand *op = *opp;

  if (ic->op != CAST || !IS_ITEMP (op))
    return 0;

  if (bitVectnBitsOn (OP_USES (op)) != 1 || bitVectnBitsOn (OP_DEFS (op)) != 1)
    return 0;

  // get the definition
  if (!(dic = hTabItemWithKey (iCodehTab, bitVectFirstBit (OP_DEFS (op)))))
    return 0;

  // found the definition now check if it is local
  if (dic->seq < ebp->fSeq || dic->seq > ebp->lSeq)
    return 0;

  // for now handle pointer reads only
  if (dic->op != GET_VALUE_AT_ADDRESS || IS_VOLATILE (operandType (IC_LEFT (dic))->next))
    return 0;

  sym_link *resulttype = operandType (IC_RESULT (ic));
  sym_link *righttype = operandType (IC_RIGHT (ic));

  if (IS_BOOL (resulttype) || getSize (resulttype) >= getSize (righttype))
    return 0;

  // Narrow read
  if (!port->little_endian)
    {
      int offset = getSize (righttype) - getSize (resulttype);
      IC_RIGHT (dic) = operandFromLit (operandLitValue (IC_RIGHT (dic)) + offset);
    }
  OP_SYMBOL (IC_RESULT (dic))->type = resulttype;
  ic->op = '=';

  return 1;
}

/*-----------------------------------------------------------------*/
/* narrowReads() - Will read fewer bytes by eliminating downcasts. */
/*-----------------------------------------------------------------*/
static void
narrowReads(ebbIndex *ebbi)
{
  for (int i = 0; i < ebbi->count; i++)
    {
      eBBlock **ebbs = ebbi->bbOrder;
      eBBlock *ebp = ebbs[i];

      for (iCode *ic = ebp->sch; ic; ic = ic->next)
        if (ic->op == CAST)
          narrowRead (ic, &(IC_RIGHT (ic)), ebp);
    }
}

/*-----------------------------------------------------------------*/
/* Remove redundant temporaries (leftover from other opts)         */
/*-----------------------------------------------------------------*/
int
removeRedundantTemps (iCode *sic)
{
  int change = 0;
  for (iCode *ic = sic; ic; ic = ic->next)
    {
      iCode *pic = ic->prev;
      if (!pic)
        continue;
      if (ic->op != '=' || POINTER_SET (ic) || !IS_ITEMP (IC_RESULT (ic)) || !IS_ITEMP (IC_RIGHT (ic)))
        continue;
      if (POINTER_SET (pic) || !isOperandEqual (IC_RESULT (pic), IC_RIGHT (ic)))
        continue;
      if (bitVectnBitsOn (OP_DEFS (IC_RIGHT (ic))) != 1)
        continue;
      if (bitVectnBitsOn (OP_USES (IC_RIGHT (ic))) != 1)
        continue;
      if (compareType (operandType (IC_RESULT (ic)), operandType (IC_RIGHT (ic)), false) != 1)
        continue;
      if (IS_BITFIELD (operandType (IC_RESULT (pic))))
        continue;

#if 0
      printf ("removeRedundantTemps: in %s optimized out ic %d: %s = %s\n", (currFunc ? currFunc->name : "NO_FUNCTION"), ic->key, OP_SYMBOL (IC_RESULT (ic))->name, OP_SYMBOL (IC_RIGHT (ic))->name);
#endif

      bitVectUnSetBit (OP_DEFS (IC_RESULT (pic)), pic->key);
      IC_RESULT (pic) = IC_RESULT (ic);
      bitVectSetBit (OP_DEFS (IC_RESULT (pic)), pic->key);

      // Assignment to self will get optimized out later
      bitVectUnSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);
      IC_RESULT (ic) = IC_RIGHT (ic);
      bitVectSetBit (OP_DEFS (IC_RESULT (ic)), ic->key);

      change++;
    }
  return (change);
}

/*-----------------------------------------------------------------*/
/* checkRestartAtomic - try to prove that no restartable           */
/*                      atomics implementation is used from here.  */
/*-----------------------------------------------------------------*/
void
checkRestartAtomic (ebbIndex *ebbi)
{
  if (!currFunc)
    return;

  currFunc->funcRestartAtomicSupport = false;
//  if (!options.std_c11)
//    return;

  for (int i = 0; i < ebbi->count; i++)
    {
      eBBlock **ebbs = ebbi->bbOrder;
      const eBBlock *ebp = ebbs[i];

      for (const iCode *ic = ebp->sch; ic; ic = ic->next)
        if (ic->op == CALL && !IS_OP_LITERAL (ic->left))
          currFunc->funcRestartAtomicSupport |= OP_SYMBOL(ic->left)->funcRestartAtomicSupport;
        else if (ic->op == CALL || ic->op == PCALL || ic->op == INLINEASM)
          currFunc->funcRestartAtomicSupport = true;
    }
}

/*-----------------------------------------------------------------*/
/* eBBlockFromiCode - creates extended basic blocks from iCode     */
/*                    will return an array of eBBlock pointers     */
/*-----------------------------------------------------------------*/
eBBlock **
eBBlockFromiCode (iCode *ic)
{
  ebbIndex *ebbi = NULL;
  int change = 1;
  int lchange = 0;
  int kchange = 0;
  hTab *loops;

  /* if nothing passed then return nothing */
  if (!ic)
    return NULL;

  eBBNum = 0;

  /* optimize the chain for labels & gotos
     this will eliminate redundant labels and
     will change jump to jumps by jumps */
  ic = iCodeLabelOptimize (ic);

  /* break it down into basic blocks */
  ebbi = iCodeBreakDown (ic);
  /* hash the iCode keys so that we can quickly index */
  /* them in the rest of the optimization steps */
  setToNull ((void *) &iCodehTab);
  iCodehTab = newHashTable (iCodeKey);
  hashiCodeKeys (ebbi->bbOrder, ebbi->count);

  /* compute the control flow */
  computeControlFlow (ebbi);

  /* dumpraw if asked for */
  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_RAW0, ebbi);

  /* replace the local variables with their
     register equivalents : the liveRange computation
     along with the register allocation will determine
     if it finally stays in the registers */
  replaceRegEqv (ebbi);

  /* create loop regions */
  loops = createLoopRegions (ebbi);

  /* dumpraw if asked for */
  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_RAW1, ebbi);

  if (!optimize.noStdLibCall)
    optimizeStdLibCall (ebbi->bbOrder, ebbi->count);

  optimizeCastCast (ebbi->bbOrder, ebbi->count);
  while (optimizeOpWidth (ebbi->bbOrder, ebbi->count))
    optimizeCastCast (ebbi->bbOrder, ebbi->count);
  optimizeNegation (ebbi->bbOrder, ebbi->count);

  /* Burn the corpses, so the dead may rest in peace,
     safe from cse necromancy */
  computeDataFlow (ebbi);
  killDeadCode (ebbi);

  /* do common subexpression elimination for each block */
  change = cseAllBlocks (ebbi, FALSE);

  /* dumpraw if asked for */
  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_CSE, ebbi);

  // optimizeCastCast (ebbi->bbOrder, ebbi->count); TODO: Enable after fixing GCSE issue. GCSE messes up some pointer types, triggered by this, resulting in the bitfields.c test failing for some targets.

  /* compute the data flow */
  computeDataFlow (ebbi);

  /* dumpraw if asked for */
  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_DFLOW, ebbi);

  /* global common subexpression elimination  */
  if (optimize.global_cse)
    {
      change += cseAllBlocks (ebbi, FALSE);
      if (options.dump_i_code)
        dumpEbbsToFileExt (DUMP_GCSE, ebbi);
    }
  else
    {
      // compute the dataflow only
      assert(cseAllBlocks (ebbi, TRUE)==0);
    }

  /* kill dead code */
  kchange = killDeadCode (ebbi);

  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_DEADCODE, ebbi);

  // We want to eliminate some unnecessary short-lived temporaries now -
  // Before they will get lifted out of loops, and have their life-ranges
  // extended across multiple blocks.
  optimizeCastCast (ebbi->bbOrder, ebbi->count);

  /* do loop optimizations */
  change += (lchange = loopOptimizations (loops, ebbi));
  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_LOOP, ebbi);

  /* recompute the data flow and apply global cse again
     if loops optimizations or dead code caused a change:
     loops will brings out of the loop which then may be
     available for use in the later blocks: dead code
     elimination could potentially disconnect some blocks
     conditional flow may be efected so we need to apply
     subexpression once more */
  if (lchange || kchange)
    {
      computeDataFlow (ebbi);
      change += cseAllBlocks (ebbi, FALSE);
      if (options.dump_i_code)
        dumpEbbsToFileExt (DUMP_LOOPG, ebbi);

      /* if loop optimizations caused a change then do
         dead code elimination once more : this will
         get rid of the extra assignments to the induction
         variables created during loop optimizations */
      killDeadCode (ebbi);

      if (options.dump_i_code)
        dumpEbbsToFileExt (DUMP_LOOPD, ebbi);
    }

  offsetFoldGet (ebbi->bbOrder, ebbi->count);
  optimizeCastCast (ebbi->bbOrder, ebbi->count);
  computeControlFlow (ebbi);
  loops = createLoopRegions (ebbi);
  computeDataFlow (ebbi);
  computeLiveRanges (ebbi->bbOrder, ebbi->count, true);

  // Generalized constant propagation - do it here a first time before the first call to computeLiveRanges to ensure uninitalized variables are still recognized as such.
  if (optimize.genconstprop)
    {
      ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
      recomputeValinfos (ic, ebbi, "_0");
      optimizeValinfo (ic);
      freeeBBlockData (ebbi);
      ebbi = iCodeBreakDown (ic);
      computeControlFlow (ebbi);
      loops = createLoopRegions (ebbi);
      computeDataFlow (ebbi);
    }

  // lospre
  recomputeLiveRanges (ebbi->bbOrder, ebbi->count, false);
  while (optimizeOpWidth (ebbi->bbOrder, ebbi->count))
    optimizeCastCast (ebbi->bbOrder, ebbi->count);
  recomputeLiveRanges (ebbi->bbOrder, ebbi->count, false); // Recompute again before killing dead code, since dead code elimination needs updated ic->seq - the old ones might have been invalidated in optimizeOpWidth above.
  killDeadCode (ebbi); // Ensure lospre doesn't resurrect dead code.
  adjustIChain (ebbi->bbOrder, ebbi->count);
  ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
  shortenLiveRanges (ic, ebbi);
  guessCounts (ic, ebbi);
  if (optimize.lospre && (TARGET_Z80_LIKE || TARGET_HC08_LIKE || TARGET_IS_STM8 || TARGET_F8_LIKE)) /* For mcs51, we get a code size regression with lospre enabled, since the backend can't deal well with the added temporaries */
    {
      lospre (ic, ebbi);
      if (options.dump_i_code)
        dumpEbbsToFileExt (DUMP_LOSPRE, ebbi);

      /* GCSE, lospre and maybe other optimizations sometimes create temporaries that have non-connected live ranges, which is bad (e.g. for offsetFoldUse and register allocation). Split them. */
      freeeBBlockData (ebbi);
      ebbi = iCodeBreakDown (ic);
      computeControlFlow (ebbi);
      loops = createLoopRegions (ebbi);
      computeDataFlow (ebbi);
      recomputeLiveRanges (ebbi->bbOrder, ebbi->count, false);
      adjustIChain (ebbi->bbOrder, ebbi->count);
      ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
      separateLiveRanges (ic, ebbi);
    }

  removeRedundantTemps (ic); // Remove some now-redundant leftovers iTemps that can confuse later optimizations.

  /* Break down again and redo some steps to not confuse live range analysis later. */
  freeeBBlockData (ebbi);
  ebbi = iCodeBreakDown (ic);
  computeControlFlow (ebbi);
  loops = createLoopRegions (ebbi);
  computeDataFlow (ebbi);
  killDeadCode (ebbi);
  offsetFoldUse (ebbi->bbOrder, ebbi->count);
  killDeadCode (ebbi);

  /* sort it back by block number */
  //qsort (ebbs, saveCount, sizeof (eBBlock *), bbNumCompare);

  /* enforce restrictions on acesses to named address spaces */
  separateAddressSpaces (ebbi->bbOrder, ebbi->count);

  /* insert bank switching instructions. Do it here, before the
     other support routines, since we can assume that there is no
     bank switching happening in those other support routines
     (but assume that it can happen in other functions) */
  adjustIChain (ebbi->bbOrder, ebbi->count);
  ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
  if (!currFunc || switchAddressSpacesOptimally (ic, ebbi))
    switchAddressSpaces (ic); /* Fallback. Very unlikely to be triggered, unless --max-allocs-per-node is set to very small values or very weird control-flow graphs */

  /* Break down again and redo some steps to not confuse live range analysis. */
  freeeBBlockData (ebbi);
  ebbi = iCodeBreakDown (ic);
  computeControlFlow (ebbi);
  loops = createLoopRegions (ebbi);
  computeDataFlow (ebbi);

  if (!options.lessPedantic)
    {
      // this is a good place to check missing return values
      if (currFunc)
        {
          // the user is on his own with naked functions...
          if (!IS_VOID(currFunc->etype) && !FUNC_ISNAKED(currFunc->type))
            {
              eBBlock *bp;
              // make sure all predecessors of the last block end in a return
              for (bp=setFirstItem(ebbi->bbOrder[ebbi->count-1]->predList);
                   bp;
                   bp=setNextItem(ebbi->bbOrder[ebbi->count-1]->predList))
                {
                  if (bp->ech->op != RETURN)
                    {
                      werrorfl (bp->ech->filename, bp->ech->lineno, W_VOID_FUNC, currFunc->name);
                    }
                }
            }
        }
    }

  /* if cyclomatic info requested then print it */
  if (options.cyclomatic)
    printCyclomatic (ebbi->bbOrder, ebbi->count);

  /* convert operations with support routines
     written in C to function calls : I am doing
     this at this point since I want all the
     operations to be as they are for optimizations */
  convertToFcall (ebbi->bbOrder, ebbi->count);
  adjustIChain (ebbi->bbOrder, ebbi->count); // Fix ebb->sch pointers potentially messed up by convertToFcall.

  /* miscellaneous optimizations */
  miscOpt (ebbi->bbOrder, ebbi->count);

  // Generalized constant propagation - second time.
  if (optimize.genconstprop)
    {
      computeControlFlow (ebbi);
      loops = createLoopRegions (ebbi);
      computeDataFlow (ebbi);
      recomputeLiveRanges (ebbi->bbOrder, ebbi->count, false);
      ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
      recomputeValinfos (ic, ebbi, "_1");
      optimizeValinfo (ic);
      freeeBBlockData (ebbi);
      ebbi = iCodeBreakDown (ic);
      computeControlFlow (ebbi);
      loops = createLoopRegions (ebbi);
      computeDataFlow (ebbi);
      killDeadCode (ebbi);
      if (options.dump_i_code)
        dumpEbbsToFileExt (DUMP_GENCONSTPROP, ebbi);
    }

  optimizeFinalCast (ebbi);

  /* Split any live-ranges that became non-connected in dead code elimination. */
  change = 0;
  do
  {
    adjustIChain (ebbi->bbOrder, ebbi->count);
    recomputeLiveRanges (ebbi->bbOrder, ebbi->count, false);
    ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbi->bbOrder, ebbi->count));
    change = separateLiveRanges (ic, ebbi);
    removeRedundantTemps (ic); // Remove some now-redundant leftovers iTemps that can confuse later optimizations.
    freeeBBlockData (ebbi);
    ebbi = iCodeBreakDown (ic);
    computeControlFlow (ebbi);
    loops = createLoopRegions (ebbi);
    computeDataFlow (ebbi);
    killDeadCode (ebbi); /* iCodeLabelOptimize() above might result in dead code, when both branches of an ifx go to the same destination. */
  }
  while (change);

  /* compute the live ranges */
  recomputeLiveRanges (ebbi->bbOrder, ebbi->count, true);

  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_RANGE, ebbi);

  /* Now that we have the live ranges, discard parameter
   * receives for unused parameters.
   */
  discardDeadParamReceives (ebbi->bbOrder, ebbi->count);

  narrowReads (ebbi);

  checkRestartAtomic (ebbi);

  /* allocate registers & generate code */
  if (!options.syntax_only)
    port->assignRegisters (ebbi);

  /* throw away blocks */
  setToNull ((void *) &graphEdges);
  freeeBBlockData (ebbi);

  return NULL;
}

