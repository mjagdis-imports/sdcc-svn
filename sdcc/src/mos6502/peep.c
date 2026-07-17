/*-------------------------------------------------------------------------
  peep.c - source file for peephole optimizer helper functions

  Copyright (C) 2011-2025, Philipp Klaus Krause pkk@spth.de, philipp@informatik.uni-frankfurt.de, philipp@colecovision.eu
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

  In other words, you are welcome to use, share and improve this program.
  You are forbidden to forbid anyone else to use, share and improve
  what you give them.   Help stamp out software-hoarding!
  -------------------------------------------------------------------------*/

#include "common.h"
#include "SDCCgen.h"

#include "ralloc.h"
#include "peep.h"
extern const m6502opcodedata m6502opcodeDataTable[];

#define NOTUSEDERROR() do {werror(E_INTERNAL_ERROR, __FILE__, __LINE__, "error in notUsed()");} while(0)

#if 0
#define D(_s) { printf _s; fflush(stdout); }
#else
#define D(_s)
#endif

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

/* Forward declarations (used before definitions below). */
static bool mos6502UncondJump (const lineNode *pl);
static bool mos6502CondJump (const lineNode *pl);
static bool mos6502SurelyReturns (const lineNode *pl);

/*-----------------------------------------------------------------*/
/* univisitLines - clear "visited" flag in all lines               */
/*-----------------------------------------------------------------*/
static void
unvisitLines (lineNode *pl)
{
  for (; pl; pl = pl->next)
    pl->visited = false;
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
    return false;
  entry->jmpToCount++;
  return true;
}

static const char *
mos6502SkipToOperands (const char *s)
{
  /* `lineIsInst()` already matched the mnemonic; here we just find the operand
     portion for simple syntactic checks (indexing, accumulator form). */
  if (!s)
    return NULL;

  while (*s && isspace ((unsigned char)*s))
    s++;

  /* Skip mnemonic */
  while (*s && !isspace ((unsigned char)*s))
    s++;

  while (*s && isspace ((unsigned char)*s))
    s++;

  return s;
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
  const char *p;
  lineNode *cpl;
  static char tlabel[32];
  int i;
  int label_len = 0;

  p = mos6502SkipToOperands (pl->line);

#if 0
  /* 1. extract label in opcode */

  /* In each jump the label is at the end */
  p = strlen (pl->line) - 1 + pl->line;

  /* Skip trailing whitespace */
  while(isspace(*p))
    p--;

  /* scan backward until space or ',' */
  for (; p > pl->line; p--)
    if (isspace(*p) || *p == ',')
      break;

  /* sanity check */
  if (p == pl->line)
    {
      NOTUSEDERROR();
      return NULL;
    }
#endif

  /* find jump target */
  while (*(p+label_len) && !isspace ((unsigned char)*(p+label_len)))
    label_len++;

  for(i=0; i<label_len; i++)
    tlabel[i]=p[i];

  tlabel[i]=0;

  D(("search for label: %s\n", tlabel));

  /* 2. increment "label jump-to count" */
  if (!incLabelJmpToCount (tlabel))
    return NULL;

  /* 3. search lineNode with label definition and return it */
  for (cpl = _G.head; cpl; cpl = cpl->next)
    {
      if(cpl->isLabel)
	D(("examine label: %s\n", cpl->line));

      if (cpl->isLabel &&
	  strncmp (p, cpl->line, label_len) == 0 &&
	  cpl->line[label_len] == ':')
        {
	  D(("MATCH!\n"));
	  return cpl;
        }
    }
  return NULL;
}

static bool
mos6502MightReadFlag(const lineNode *pl, const char *what)
{
  if (lineIsInst (pl, "php"))
    return true;

  if (lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
      lineIsInst (pl, "ror") || lineIsInst (pl, "rol") )
    return (!strcmp(what, "c"));

  if (lineIsInst (pl, "bcc") || lineIsInst (pl, "bcs"))
    return (!strcmp(what, "c"));

  if (lineIsInst (pl, "beq") || lineIsInst (pl, "bne"))
    return (!strcmp(what, "z"));

  if (lineIsInst (pl, "bmi") || lineIsInst (pl, "bpl"))
    return (!strcmp(what, "n"));

  if (lineIsInst (pl, "bvc") || lineIsInst (pl, "bvs"))
    return (!strcmp(what, "v"));

  return false;
}

static bool
mos6502OperandUsesIndexReg (const lineNode *pl, char reg)
{
  const char *p;
  char r = (char)tolower ((unsigned char)reg);

  if (!pl || !pl->line)
    return false;

  p = mos6502SkipToOperands (pl->line);
  if (!p || !*p)
    return false;

  /* Conservative: if the operand text contains ",x" / ",y" (optionally with
     spaces), assume it reads the index register for address calculation. */
  for (; *p; p++)
    {
      if (*p != ',')
        continue;

      p++;
      while (*p && isspace ((unsigned char)*p))
        p++;

      if (tolower ((unsigned char)*p) == (unsigned char)r)
        return true;
    }

  return false;
}

static bool
mos6502IsAccumulatorForm (const lineNode *pl)
{
  const char *p;

  if (!pl || !pl->line)
    return false;

  p = mos6502SkipToOperands (pl->line);
  if (!p || !*p)
    return true; /* e.g. "asl" => accumulator */

  /* Explicit "a" operand */
  //  while (*p && isspace ((unsigned char)*p))
  //    p++;

  if (tolower ((unsigned char)*p) == (unsigned char)'a')
    {
      p++;
      /* Accept "a" followed by whitespace or end; anything else is not the
         accumulator pseudo-operand. */
      return (!*p || isspace ((unsigned char)*p));
    }

  return false;
}

static bool
mos6502MightReadReg (const lineNode *pl, const char *what)
{
  /* This is used by `notUsed()`; it's OK (and safer) to be conservative.
     The goal is to avoid returning "might read" for registers that clearly
     aren't used by the instruction stream. */
  if (!strcmp (what, "a"))
    {
      /* A is read by ALU/test ops and stores/pushes/transfers from A. */
      if (lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
          lineIsInst (pl, "and") || lineIsInst (pl, "ora") ||
          lineIsInst (pl, "eor") || lineIsInst (pl, "cmp") ||
          lineIsInst (pl, "sta") || lineIsInst (pl, "pha") ||
          lineIsInst (pl, "tax") || lineIsInst (pl, "tay") ||
          lineIsInst (pl, "bit") || lineIsInst (pl, "tsb") ||
          lineIsInst (pl, "trb"))
        return true;

      /* Shifts/rotates read A only in accumulator form (no operand or "a"). */
      /* INC/DEC read A only in accumulator form (e.g. "inc a"). */
      if ((lineIsInst (pl, "asl") || lineIsInst (pl, "lsr") ||
           lineIsInst (pl, "rol") || lineIsInst (pl, "ror") || 
           lineIsInst (pl, "inc") || lineIsInst (pl, "dec") ) &&
          mos6502IsAccumulatorForm (pl))
        return true;
    }

  if (!strcmp (what, "x"))
    {
      /* X is read by stores/compares/transfers/stack ops and by indexed
         addressing modes that include ",x". */
      if (lineIsInst (pl, "stx") || lineIsInst (pl, "cpx") ||
          lineIsInst (pl, "inx") || lineIsInst (pl, "dex") ||
          lineIsInst (pl, "txa") || lineIsInst (pl, "phx") ||
          lineIsInst (pl, "txs"))
        return true;

      if (mos6502OperandUsesIndexReg (pl, 'x'))
        return true;
    }

  if (!strcmp (what, "y"))
    {
      /* Y is read by stores/compares/transfers/stack ops and by indexed
         addressing modes that include ",y". */
      if (lineIsInst (pl, "sty") || lineIsInst (pl, "cpy") ||
          lineIsInst (pl, "iny") || lineIsInst (pl, "dey") ||
          lineIsInst (pl, "tya") || lineIsInst (pl, "phy"))
        return true;

      if (mos6502OperandUsesIndexReg (pl, 'y'))
        return true;
    }

  return false;
}

static bool
mos6502MightRead(const lineNode *pl, const char *what)
{
  /* Be conservative across control transfers: the jump/call target may read
     any register/flag. This keeps `notUsed()` safe. */

  // The SDCC ABI does not pass any value in Y or flags
  if (lineIsInst (pl, "jsr") && (!strcmp (what, "a") || !strcmp (what, "x") ) )
    return true;

  // The SDCC ABI does not return any value in Y or flags
  if (lineIsInst (pl, "rts") && (!strcmp (what, "a") || !strcmp (what, "x") ) )
    return true;

  if(lineIsInst (pl, "rti"))
    return true;

  if (!strcmp (what, "n") || !strcmp (what, "z") || !strcmp (what, "c") || !strcmp (what, "v"))
    return (mos6502MightReadFlag (pl, what));

  if (!strcmp (what, "a") || !strcmp (what, "x") || !strcmp (what, "y"))
    return mos6502MightReadReg (pl, what);

  return false;
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

static bool
mos6502SurelyWritesFlag(const lineNode *pl, const char *what)
{
#if 1
  int idx = 0;
  int ret = 0;
  int i;

  for(i=0; m6502opcodeDataTable[i].name[0]!='z'; i++)
    {
      if(lineIsInst (pl, m6502opcodeDataTable[i].name ))
        {
          idx=i;
          break;
        }
    }

  if(idx==0)
    return false;

  if(m6502opcodeDataTable[idx].flags&0x01)
    ret |= !strcmp(what, "c");

  if(m6502opcodeDataTable[idx].flags&0x02)
    ret |= !strcmp(what, "z");

  if(m6502opcodeDataTable[idx].flags&0x40)
    ret |= !strcmp(what, "v");

  if(m6502opcodeDataTable[idx].flags&0x80)
    ret |= !strcmp(what, "n");

  return ret;
#else

  if (lineIsInst (pl, "plp"))
    return true;

  if (!strcmp (what, "n") || !strcmp (what, "z"))
    {
      if (lineIsInst (pl, "lda") || lineIsInst (pl, "pla") ||
          lineIsInst (pl, "ldx") || lineIsInst (pl, "plx") ||
          lineIsInst (pl, "ldy") || lineIsInst (pl, "ply") ||
          lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
          lineIsInst (pl, "and") || lineIsInst (pl, "ora") ||
          lineIsInst (pl, "eor") || lineIsInst (pl, "cmp") ||
          lineIsInst (pl, "cpx") || lineIsInst (pl, "cpy") ||
          lineIsInst (pl, "inx") || lineIsInst (pl, "iny") ||
          lineIsInst (pl, "dex") || lineIsInst (pl, "dey") ||
          lineIsInst (pl, "inc") || lineIsInst (pl, "dec") ||
          lineIsInst (pl, "tax") || lineIsInst (pl, "txa") ||
          lineIsInst (pl, "tay") || lineIsInst (pl, "tya") ||
          lineIsInst (pl, "rol") || lineIsInst (pl, "ror") ||
          lineIsInst (pl, "asl") || lineIsInst (pl, "lsr") ||
          lineIsInst (pl, "bit") || lineIsInst (pl, "tsx") )
        return true;
    }

  if (!strcmp (what, "c"))
    {
      if (lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
          lineIsInst (pl, "cpx") || lineIsInst (pl, "cpy") ||
          lineIsInst (pl, "rol") || lineIsInst (pl, "ror") ||
          lineIsInst (pl, "asl") || lineIsInst (pl, "lsr") ||
          lineIsInst (pl, "clc") || lineIsInst (pl, "sec") ||
          lineIsInst (pl, "cmp") )
        return true;
    }

  if (!strcmp (what, "v"))
    {
      if (lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
          lineIsInst (pl, "clv") || lineIsInst (pl, "bit") )
        return true;
    }

  return false;
#endif
}

static bool
mos6502SurelyWritesReg(const lineNode *pl, const char *what)
{

  if(!strcmp(what, "a"))
    {
      if (lineIsInst (pl, "lda") || lineIsInst (pl, "pla") ||
          lineIsInst (pl, "txa") || lineIsInst (pl, "tya") )
        return true;

    }

  if(!strcmp(what, "x"))
    {
      if (lineIsInst (pl, "ldx") || lineIsInst (pl, "plx") ||
          lineIsInst (pl, "tax") || lineIsInst (pl, "tsx") )
        return true;

    }

  if(!strcmp(what, "y"))
    {
      if (lineIsInst (pl, "ldy") || lineIsInst (pl, "ply") ||
          lineIsInst (pl, "tay") )
        return true;

    }

  return false;
}

static bool
mos6502SurelyWrites(const lineNode *pl, const char *what)
{
  if (!strcmp (what, "n") || !strcmp (what, "z") || !strcmp (what, "c") || !strcmp (what, "v"))
    return (mos6502SurelyWritesFlag(pl, what));
  if (!strcmp (what, "a") || !strcmp (what, "x") || !strcmp (what, "y"))
    return (mos6502SurelyWritesReg(pl, what));
  return false;
}


static bool
mos6502UncondJump (const lineNode *pl)
{
  // FIXME: should jsr be here as well?
  return (lineIsInst (pl, "jmp") || lineIsInst (pl, "bra"));
}

static bool
mos6502CondJump (const lineNode *pl)
{
  return (lineIsInst (pl, "bpl") || lineIsInst (pl, "bmi") ||
	  lineIsInst (pl, "bvc") || lineIsInst (pl, "bvs") ||
	  lineIsInst (pl, "bcc") || lineIsInst (pl, "bcs") ||
	  lineIsInst (pl, "bne") || lineIsInst (pl, "beq"));
}

static bool
mos6502SurelyReturns (const lineNode *pl)
{
  return (lineIsInst (pl, "rts") || lineIsInst (pl, "rti") );
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

      (*pl)->visited = true;

      if (mos6502MightRead (*pl, what))
        {
          D(("S4O_RD_OP\n"));
          return S4O_RD_OP;
        }

      // Check writes before conditional jumps
      if (mos6502SurelyWrites (*pl, what))
        {
          D(("S4O_WR_OP\n"));
          return S4O_WR_OP;
        }

      if (mos6502UncondJump (*pl))
        {
	  D(("JMP: %s for %s\n", (*pl)->line, what));
          *pl = findLabel (*pl);
	  if (!*pl)
	    {
	      D(("S4O_ABORT at unconditional jump\n"));
	      return S4O_ABORT;
	    }
        }

      if (mos6502CondJump (*pl))
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

      /* Don't need to check for de, hl since pdkMightRead() does that */
      if (mos6502SurelyReturns (*pl))
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

bool
mos6502notUsed (const char *what, lineNode *endPl, lineNode *head)
{
  lineNode *pl;

  _G.head = head;

  unvisitLines (_G.head);

  D(("Notused start: %s for %s\n", endPl->line, what));
  pl = endPl->next;
  return (doTermScan (&pl, what));
}

bool
mos6502notUsedFrom (const char *what, const char *label, lineNode *head)
{
  lineNode *cpl;

  for (cpl = head; cpl; cpl = cpl->next)
    if (cpl->isLabel && !strncmp (label, cpl->line, strlen(label)))
      return (mos6502notUsed (what, cpl, head));

  return false;
}

