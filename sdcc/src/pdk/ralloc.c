#include "ralloc.h"
#include "gen.h"

reg_info pdk_regs[] =
{
  {REG_GPR, A_IDX,   "a"},
  {0, SP_IDX,        "sp"},
};

/* Flags to turn on debugging code.
 */
enum
{
  D_ALLOC = 0,
};

#if 1
#define D(_a, _s)       if (_a)  { printf _s; fflush(stdout); }
#else
#define D(_a, _s)
#endif

/** Transform weird SDCC handling of writes via pointers
    into something more sensible. */
static void
transformPointerSet (eBBlock **ebbs, int count)
{
  /* for all blocks */
  for (int i = 0; i < count; i++)
    {
      iCode *ic;

      /* for all instructions do */
      for (ic = ebbs[i]->sch; ic; ic = ic->next)
        if (POINTER_SET (ic))
          {
            IC_LEFT (ic) = IC_RESULT (ic);
            IC_RESULT (ic) = 0;
            ic->op = SET_VALUE_AT_ADDRESS;
          }
    }
}

/** Register reduction for assignment.
 */
static int
packRegsForAssign (iCode *ic, eBBlock *ebp)
{
  iCode *dic, *sic;

  if (!IS_ITEMP (IC_RIGHT (ic)) || OP_SYMBOL (IC_RIGHT (ic))->isind || OP_LIVETO (IC_RIGHT (ic)) > ic->seq)
    return 0;
  
  /* Avoid having multiple named address spaces in one iCode. */
  if (IS_SYMOP (IC_RESULT (ic)) && SPEC_ADDRSPACE (OP_SYMBOL (IC_RESULT (ic))->etype))
    return 0;

  /* find the definition of iTempNN scanning backwards if we find a
     a use of the true symbol in before we find the definition then
     we cannot */
  for (dic = ic->prev; dic; dic = dic->prev)
    {
      /* PENDING: Don't pack across function calls. */
      if (dic->op == CALL || dic->op == PCALL)
        {
          dic = NULL;
          break;
        }

      if (SKIP_IC2 (dic))
        continue;

      if (dic->op == IFX)
        {
          if (IS_SYMOP (IC_COND (dic)) &&
              (IC_COND (dic)->key == IC_RESULT (ic)->key || IC_COND (dic)->key == IC_RIGHT (ic)->key))
            {
              dic = NULL;
              break;
            }
        }
      else
        {
          if (IS_TRUE_SYMOP (IC_RESULT (dic)) && IS_OP_VOLATILE (IC_RESULT (dic)))
            {
              dic = NULL;
              break;
            }

          if (IS_SYMOP (IC_RESULT (dic)) && IC_RESULT (dic)->key == IC_RIGHT (ic)->key)
            {
              break;
            }

          if (IS_SYMOP (IC_RIGHT (dic)) &&
              (IC_RIGHT (dic)->key == IC_RESULT (ic)->key || IC_RIGHT (dic)->key == IC_RIGHT (ic)->key))
            {
              dic = NULL;
              break;
            }

          if (IS_SYMOP (IC_LEFT (dic)) &&
              (IC_LEFT (dic)->key == IC_RESULT (ic)->key || IC_LEFT (dic)->key == IC_RIGHT (ic)->key))
            {
              dic = NULL;
              break;
            }

          if (IS_SYMOP (IC_RESULT (dic)) && IC_RESULT (dic)->key == IC_RESULT (ic)->key)
            {
              dic = NULL;
              break;
            }
        }
    }

  if (!dic)
    return 0;                   /* did not find */

  /* if assignment then check that right is not a bit */
  if (ic->op == '=')
    {
      sym_link *etype = operandType (IC_RESULT (dic));
      if (IS_BITFIELD (etype))
        {
          /* if result is a bit too then it's ok */
          etype = operandType (IC_RESULT (ic));
          if (!IS_BITFIELD (etype))
            {
              return 0;
            }
        }
    }

  /* if the result is on stack or iaccess then it must be
     the same as at least one of the operands */
  if (OP_SYMBOL (IC_RESULT (ic))->onStack || OP_SYMBOL (IC_RESULT (ic))->iaccess)
    {
      /* the operation has only one symbol
         operator then we can pack */
      if ((IC_LEFT (dic) && !IS_SYMOP (IC_LEFT (dic))) || (IC_RIGHT (dic) && !IS_SYMOP (IC_RIGHT (dic))))
        goto pack;

      if (!((IC_LEFT (dic) &&
             IC_RESULT (ic)->key == IC_LEFT (dic)->key) || (IC_RIGHT (dic) && IC_RESULT (ic)->key == IC_RIGHT (dic)->key)))
        return 0;
    }
pack:
  /* found the definition */
  /* replace the result with the result of */
  /* this assignment and remove this assignment */
  bitVectUnSetBit (OP_SYMBOL (IC_RESULT (dic))->defs, dic->key);
  IC_RESULT (dic) = IC_RESULT (ic);

  if (IS_ITEMP (IC_RESULT (dic)) && OP_SYMBOL (IC_RESULT (dic))->liveFrom > dic->seq)
    {
      OP_SYMBOL (IC_RESULT (dic))->liveFrom = dic->seq;
    }
  /* delete from liverange table also
     delete from all the points in between and the new
     one */
  for (sic = dic; sic != ic; sic = sic->next)
    {
      bitVectUnSetBit (sic->rlive, IC_RESULT (ic)->key);
      if (IS_ITEMP (IC_RESULT (dic)))
        bitVectSetBit (sic->rlive, IC_RESULT (dic)->key);
    }

  remiCodeFromeBBlock (ebp, ic);
  // PENDING: Check vs mcs51
  bitVectUnSetBit (OP_SYMBOL (IC_RESULT (ic))->defs, ic->key);
  hTabDeleteItem (&iCodehTab, ic->key, ic, DELETE_ITEM, NULL);
  OP_DEFS (IC_RESULT (dic)) = bitVectSetBit (OP_DEFS (IC_RESULT (dic)), dic->key);
  return 1;
}

/** Does some transformations to reduce register pressure.
 */
static void
packRegisters (eBBlock * ebp)
{
  iCode *ic;
  int change = 0;

  D (D_ALLOC, ("packRegisters: entered.\n"));

  for(;;)
    {
      change = 0;
      /* look for assignments of the form */
      /* iTempNN = TRueSym (someoperation) SomeOperand */
      /*       ....                       */
      /* TrueSym := iTempNN:1             */
      for (ic = ebp->sch; ic; ic = ic->next)
        {
          /* find assignment of the form TrueSym := iTempNN:1 */
          if (ic->op == '=')
            change += packRegsForAssign (ic, ebp);
        }
      if (!change)
        break;
    }
}

/*-----------------------------------------------------------------*/
/* assignRegisters - assigns registers to each live range as need  */
/*-----------------------------------------------------------------*/
void
pdk_assignRegisters (ebbIndex *ebbi)
{
  eBBlock **ebbs = ebbi->bbOrder;
  int count = ebbi->count;

  pdk_init_asmops();

  transformPointerSet (ebbs, count);

  /* change assignments this will remove some
     live ranges reducing some register pressure */
  for (int i = 0; i < count; i++)
    packRegisters (ebbs[i]);

  /* liveranges probably changed by register packing
     so we compute them again */
  recomputeLiveRanges (ebbs, count, FALSE);

  if (options.dump_i_code)
    dumpEbbsToFileExt (DUMP_PACK, ebbi);

  iCode *ic = iCodeLabelOptimize (iCodeFromeBBlock (ebbs, count));

  genPdkCode (ic);
}

