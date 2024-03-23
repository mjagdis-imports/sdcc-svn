#include "common.h"
#include "SDCCicode.h"
#include "SDCCglobl.h"
#include "SDCCgen.h"

#include "peep.h"
#include "gen.h"

#define NOTUSEDERROR() do {werror(E_INTERNAL_ERROR, __FILE__, __LINE__, "error in notUsed()");} while(0)

// #define D(_s) { printf _s; fflush(stdout); }
#define D(_s)

#define ISINST(l, i) (!STRNCASECMP((l), (i), sizeof(i) - 1) && (!(l)[sizeof(i) - 1] || isspace((unsigned char)((l)[sizeof(i) - 1]))))
#define STARTSINST(l, i) (!STRNCASECMP((l), (i), sizeof(i) - 1))

typedef enum
{
  S4O_CONDJMP,
  S4O_WR_OP,
  S4O_RD_OP,
  S4O_TERM,
  S4O_VISITED,
  S4O_ABORT,
  S4O_CONTINUE
} S4O_RET;

static struct
{
  lineNode *head;
} _G;

//extern bool stm8_regs_used_as_parms_in_calls_from_current_function[YH_IDX + 1];
//extern bool stm8_regs_used_as_parms_in_pcalls_from_current_function[YH_IDX + 1];

int
f8instructionSize(lineNode *pl)
{
  return 4; // Max instruction size is 4 (including one prefix byte).}
}

/*-----------------------------------------------------------------*/
/* incLabelJmpToCount - increment counter "jmpToCount" in entry    */
/* of the list labelHash                                           */
/*-----------------------------------------------------------------*/
static bool
incLabelJmpToCount (const char *label)
{
  labelHashEntry *entry;

  entry = getLabelRef (label, _G.head);
  if (!entry)
    return FALSE;
  entry->jmpToCount++;
  return TRUE;
}

/*-----------------------------------------------------------------*/
/* findLabel -                                                     */
/* 1. extracts label in the opcode pl                              */
/* 2. increment "label jump-to count" in labelHash                 */
/* 3. search lineNode with label definition and return it          */
/*-----------------------------------------------------------------*/
static lineNode *
findLabel (const lineNode *pl)
{
  char *p;
  lineNode *cpl;

  /* 1. extract label in opcode */

  /* In each jump the label is at the end */
  p = strlen (pl->line) - 1 + pl->line;

  /* Skip trailing whitespace */
  while(isspace(*p))
    p--;

  /* scan backward until space or ',' */
  for (; p > pl->line; p--)
    if (isspace(*p) || *p == ',' || *p == '#')
      break;

  /* sanity check */
  if (p == pl->line)
    {
      NOTUSEDERROR();
      return NULL;
    }

  /* skip ',' resp. '\t' */
  ++p;

  /* 2. increment "label jump-to count" */
  if (!incLabelJmpToCount (p))
    return NULL;

  /* 3. search lineNode with label definition and return it */
  for (cpl = _G.head; cpl; cpl = cpl->next)
    if (cpl->isLabel &&
      strncmp (p, cpl->line, strlen(p)) == 0 &&
      cpl->line[strlen(p)] == ':')
        return cpl;

  return NULL;
}

static bool
f8MightReadFlag (const lineNode *pl, const char *what)
{
  if (ISINST (pl->line, "adc") || ISINST (pl->line, "sbc"))
    return !strcmp (what, "cf");
  if (ISINST (pl->line, "adcw") || ISINST (pl->line, "sbcw"))
    return strcmp (what, "cf");
  if (ISINST (pl->line, "add") || ISINST (pl->line, "and") || ISINST (pl->line, "cp") || ISINST (pl->line, "or") || ISINST (pl->line, "sub") || ISINST (pl->line, "xor"))
    return false;
  if (ISINST (pl->line, "sll") || ISINST (pl->line, "srl") || ISINST (pl->line, "inc") || ISINST (pl->line, "dec") || ISINST (pl->line, "clr") || ISINST (pl->line, "push") || ISINST (pl->line, "tst"))
    return false;
  if (ISINST (pl->line, "rlc") || ISINST (pl->line, "rrc"))
    return strcmp (what, "cf");
  if (ISINST (pl->line, "addw") || ISINST (pl->line, "orw") || ISINST (pl->line, "subw"))
    return false;
  if (ISINST (pl->line, "adcw") || ISINST (pl->line, "sbcw"))
    return strcmp (what, "cf");
  if (ISINST (pl->line, "clrw") || ISINST (pl->line, "pushw") || ISINST (pl->line, "tstw"))
    return false;
  if (ISINST (pl->line, "ld") || ISINST (pl->line, "ldw"))
    return false;
  if (ISINST (pl->line, "bool") || ISINST (pl->line, "cax") || ISINST (pl->line, "mad") || ISINST (pl->line, "msk") || ISINST (pl->line, "pop") || ISINST (pl->line, "rot") || ISINST (pl->line, "sra") || ISINST (pl->line, "thrd"))
    return false;
  if (ISINST (pl->line, "daa"))
    return (strcmp (what, "cf") || strcmp (what, "hf"));
  if (ISINST (pl->line, "xch"))
    return true; // Todo: make this more accurate.
  if (ISINST (pl->line, "boolw") || ISINST (pl->line, "caxw") || ISINST (pl->line, "cpw") || ISINST (pl->line, "decw") || ISINST (pl->line, "incnw") || ISINST (pl->line, "mul") || ISINST (pl->line, "negw") || ISINST (pl->line, "popw") || ISINST (pl->line, "sex") || ISINST (pl->line, "xchw") || ISINST (pl->line, "zex")) // Todo: wide shifts / rotations.
    return false;
  if (ISINST (pl->line, "xchb"))
    return false;
  if (ISINST (pl->line, "dnjnz") || ISINST (pl->line, "jr") || ISINST (pl->line, "jp"))
    return false;
  if (ISINST (pl->line, "jrc") || ISINST (pl->line, "jrnc"))
    return strcmp (what, "cf");
  if (ISINST (pl->line, "jrn") || ISINST (pl->line, "jrnn"))
    return strcmp (what, "nf");
  if (ISINST (pl->line, "jro") || ISINST (pl->line, "jrno"))
    return strcmp (what, "of");
  if (ISINST (pl->line, "jrz") || ISINST (pl->line, "jrnz"))
    return strcmp (what, "zf");
  // Todo: remaining conditional jumps.
  if (ISINST (pl->line, "ret"))
    return false;
  if (ISINST (pl->line, "nop"))
    return false;

  // Fail-safe fallback.
  return true;
}

/* Check if reading arg implies reading what. */
static bool argCont(const char *arg, char what)
{
  if (!arg || strlen (arg) == 0 || !(what == 'x' || what == 'y' || what == 'z'))
    return false;

  while (isblank ((unsigned char)(arg[0])))
    arg++;

  if (arg[0] == ',')
    arg++;

  while (isblank ((unsigned char)(arg[0])))
    arg++;

  if (arg[0] == '#')
    return false;

  if (arg[0] == '(') 
    arg++;
  if (arg[0] == '0' && (tolower(arg[1])) == 'x') 
    arg += 2; // Skip hex prefix to avoid false x positive.

  if (strlen(arg) == 0)
    return false;

  return (strchr(arg, what));
}

static bool
f8MightRead (const lineNode *pl, const char *what)
{
  char extra = 0;

  if (!strcmp (what, "xl") || !strcmp (what, "xh"))
    extra = 'x';
  else if (!strcmp (what, "yl") || !strcmp (what, "yh"))
    extra = 'y';
  else if (!strcmp (what, "zl") || !strcmp (what, "zh"))
    extra = 'z';
  else
    return f8MightReadFlag(pl, what);

  if (ISINST (pl->line, "jp") || ISINST (pl->line, "jr"))
    return false;

  if (ISINST (pl->line, "ldw"))
    return argCont (strchr (pl->line, ','), extra);

  // Fail-safe fallback.
  return true;
}

static bool
f8UncondJump (const lineNode *pl)
{
  return (ISINST (pl->line, "jp") || ISINST (pl->line, "jr"));
}

static bool
f8CondJump (const lineNode *pl)
{
  return (!f8UncondJump (pl) && STARTSINST (pl->line, "jr") ||
    ISINST (pl->line, "dnjz"));
}

static bool
f8SurelyWritesFlag (const lineNode *pl, const char *what)
{
  // 8-bit 2-op inst.
  if (ISINST (pl->line, "adc") || ISINST (pl->line, "add") || ISINST (pl->line, "cp") || ISINST (pl->line, "sbc") || ISINST (pl->line, "sub"))
    return true;
  if (ISINST (pl->line, "or") || ISINST (pl->line, "and") || ISINST (pl->line, "xor"))
    return (!strcmp (what, "zf") || !strcmp (what, "nf"));
  // 8-bit 1-op inst.
  if (ISINST (pl->line, "dec") || ISINST (pl->line, "inc"))
    return true;
  if (ISINST (pl->line, "clr") || ISINST (pl->line, "push"))
    return false;
  if (ISINST (pl->line, "srl") || ISINST (pl->line, "sll") || ISINST (pl->line, "rrc") || ISINST (pl->line, "rlc"))
    return (!strcmp (what, "zf") || !strcmp (what, "cf"));
  if (ISINST (pl->line, "tst"))
    return strcmp (what, "hf");
  // 16-bit 1/2-op inst.
  if (ISINST (pl->line, "adcw") || /*ISINST (pl->line, "addw") || - enable when addw sp, #d can be handles correctly */ ISINST (pl->line, "sbcw") || ISINST (pl->line, "subw"))
    return strcmp (what, "hf");
  if (ISINST (pl->line, "clrw") || ISINST (pl->line, "pushw"))
    return false;
  if (ISINST (pl->line, "orw"))
    return (!strcmp (what, "of") || !strcmp (what, "zf") || !strcmp (what, "nf"));
  if (ISINST (pl->line, "tstw"))
    return strcmp (what, "hf");
  // ld
  // todo
  // ldw
  // todo
  // 8-bit 0-op inst.
  if (ISINST (pl->line, "bool") || ISINST (pl->line, "cax"))
    return !strcmp (what, "zf");
  if (ISINST (pl->line, "daa") || ISINST (pl->line, "sra"))
    return (!strcmp (what, "zf") || !strcmp (what, "cf"));
  if (ISINST (pl->line, "mad"))
    return (!strcmp (what, "zf") || !strcmp (what, "nf"));
  if (ISINST (pl->line, "msk") || ISINST (pl->line, "pop"))
     return false;
  // todo: rot
  if (ISINST (pl->line, "xch"))
    return false; // todo: improve accuracy
  // 16-bit 0-op inst.
  if (ISINST (pl->line, "boolw") || ISINST (pl->line, "zex"))
    return !strcmp (what, "zf");
  if (ISINST (pl->line, "decw"))
    return true;
  if (ISINST (pl->line, "incnw") || ISINST (pl->line, "popw") || ISINST (pl->line, "xchw"))
    return false;
  if (ISINST (pl->line, "mul") || ISINST (pl->line, "srlw") || ISINST (pl->line, "rlcw") || ISINST (pl->line, "rrcw"))
    return (!strcmp (what, "zf") || !strcmp (what, "nf") || !strcmp (what, "cf"));
  if (ISINST (pl->line, "sex"))
    return (!strcmp (what, "zf") || !strcmp (what, "nf"));
  // todo: sllw
  if (ISINST (pl->line, "xchb"))
    return !strcmp (what, "zf");
  // jumps
  // todo
  if (ISINST (pl->line, "ret"))
    return true;
  if (ISINST (pl->line, "nop"))
    return false;

  // Fail-safe fallback.
  return false;
}

static bool
f8SurelyWrites (const lineNode *pl, const char *what)
{
  char extra = 0;

  if (!strcmp (what, "xl") || !strcmp (what, "xh"))
    extra = 'x';
  else if (!strcmp (what, "yl") || !strcmp (what, "yh"))
    extra = 'y';
  else if (!strcmp (what, "zl") || !strcmp (what, "zh"))
    extra = 'z';
  else
    return (f8SurelyWritesFlag (pl, what));

  if (ISINST (pl->line, "ldw"))
    return (pl->line[4] == extra);

  // Fail-safe fallback.
  return false;
}

static bool
f8SurelyReturns (const lineNode *pl)
{
  return (false);
}

/*-----------------------------------------------------------------*/
/* scan4op - "executes" and examines the assembler opcodes,        */
/* follows conditional and un-conditional jumps.                   */
/* Moreover it registers all passed labels.                        */
/*                                                                 */
/* Parameter:                                                      */
/*    lineNode **pl                                                */
/*       scanning starts from pl;                                  */
/*       pl also returns the last scanned line                     */
/*    const char *pReg                                             */
/*       points to a register (e.g. "ar0"). scan4op() tests for    */
/*       read or write operations with this register               */
/*    const char *untilOp                                          */
/*       points to NULL or a opcode (e.g. "push").                 */
/*       scan4op() returns if it hits this opcode.                 */
/*    lineNode **plCond                                            */
/*       If a conditional branch is met plCond points to the       */
/*       lineNode of the conditional branch                        */
/*                                                                 */
/* Returns:                                                        */
/*    S4O_ABORT                                                    */
/*       on error                                                  */
/*    S4O_VISITED                                                  */
/*       hit lineNode with "visited" flag set: scan4op() already   */
/*       scanned this opcode.                                      */
/*    S4O_FOUNDOPCODE                                              */
/*       found opcode and operand, to which untilOp and pReg are   */
/*       pointing to.                                              */
/*    S4O_RD_OP, S4O_WR_OP                                         */
/*       hit an opcode reading or writing from pReg                */
/*    S4O_CONDJMP                                                  */
/*       hit a conditional jump opcode. pl and plCond return the   */
/*       two possible branches.                                    */
/*    S4O_TERM                                                     */
/*       acall, lcall, ret and reti "terminate" a scan.            */
/*-----------------------------------------------------------------*/
static S4O_RET
scan4op (lineNode **pl, const char *what, const char *untilOp,
         lineNode **plCond)
{
  for (; *pl; *pl = (*pl)->next)
    {
      if (!(*pl)->line || (*pl)->isDebug || (*pl)->isComment || (*pl)->isLabel)
        continue;
      D(("Scanning %s for %s\n", (*pl)->line, what));
      /* don't optimize across inline assembler,
         e.g. isLabel doesn't work there */
      if ((*pl)->isInline)
        {
          D(("S4O_ABORT at inline asm\n"));
          return S4O_ABORT;
        }

      if ((*pl)->visited)
        {
          D(("S4O_VISITED\n"));
          return S4O_VISITED;
        }

      (*pl)->visited = TRUE;

      if(f8MightRead(*pl, what))
        {
          D(("S4O_RD_OP\n"));
          return S4O_RD_OP;
        }

      // Check writes before conditional jumps, some jumps (btjf, btjt) write 'c'
      if(f8SurelyWrites(*pl, what))
        {
          D(("S4O_WR_OP\n"));
          return S4O_WR_OP;
        }

      if(f8UncondJump(*pl))
        {
          *pl = findLabel (*pl);
            if (!*pl)
              {
                D(("S4O_ABORT at unconditional jump\n"));
                return S4O_ABORT;
              }
        }
      if(f8CondJump(*pl))
        {
          *plCond = findLabel (*pl);
          if (!*plCond)
            {
              D(("S4O_ABORT at conditional jump\n"));
              return S4O_ABORT;
            }
          D(("S4O_CONDJMP\n"));
          return S4O_CONDJMP;
        }

      if(f8SurelyReturns(*pl))
        {
          D(("S4O_TERM\n"));
          return S4O_TERM;
        }
    }
  D(("S4O_ABORT\n"));
  return S4O_ABORT;
}

/*-----------------------------------------------------------------*/
/* doTermScan - scan through area 2. This small wrapper handles:   */
/* - action required on different return values                    */
/* - recursion in case of conditional branches                     */
/*-----------------------------------------------------------------*/
static bool
doTermScan (lineNode **pl, const char *what)
{
  lineNode *plConditional;
  for (;; *pl = (*pl)->next)
    {
      switch (scan4op (pl, what, NULL, &plConditional))
        {
          case S4O_TERM:
          case S4O_VISITED:
          case S4O_WR_OP:
            /* all these are terminating conditions */
            return true;
          case S4O_CONDJMP:
            /* two possible destinations: recurse */
              {
                lineNode *pl2 = plConditional;
                D(("CONDJMP trying other branch first\n"));
                if (!doTermScan (&pl2, what))
                  return false;
                D(("Other branch OK.\n"));
              }
            continue;
          case S4O_RD_OP:
          default:
            /* no go */
            return false;
        }
    }
}

/*-----------------------------------------------------------------*/
/* univisitLines - clear "visited" flag in all lines               */
/*-----------------------------------------------------------------*/
static void
unvisitLines (lineNode *pl)
{
  for (; pl; pl = pl->next)
    pl->visited = false;
}

bool
f8notUsed (const char *what, lineNode *endPl, lineNode *head)
{
  lineNode *pl;
  if(strcmp(what, "x") == 0)
    return(f8notUsed("xl", endPl, head) && f8notUsed("xh", endPl, head));
  else if(strcmp(what, "y") == 0)
    return(f8notUsed("yl", endPl, head) && f8notUsed("yh", endPl, head));
  else if(strcmp(what, "z") == 0)
    return(f8notUsed("zl", endPl, head) && f8notUsed("zh", endPl, head));

  _G.head = head;

  unvisitLines (_G.head);

  pl = endPl->next;
  return (doTermScan (&pl, what));
}

bool
f8notUsedFrom (const char *what, const char *label, lineNode *head)
{
  lineNode *cpl;

  for (cpl = head; cpl; cpl = cpl->next)
    if (cpl->isLabel && !strncmp (label, cpl->line, strlen(label)))
      return (f8notUsed (what, cpl, head));

  return false;
}

/* can be directly assigned with ld */
bool
f8canAssign (const char *op1, const char *op2, const char *exotic)
{
  return false;
}

