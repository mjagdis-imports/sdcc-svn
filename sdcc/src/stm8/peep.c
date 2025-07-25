#include "common.h"
#include "SDCCicode.h"
#include "SDCCglobl.h"
#include "SDCCgen.h"

#include "peep.h"
#include "gen.h"

#define NOTUSEDERROR() do {werror(E_INTERNAL_ERROR, __FILE__, __LINE__, "error in notUsed()");} while(0)

// #define D(_s) { printf _s; fflush(stdout); }
#define D(_s)

#define EQUALS(l, i) (!STRCASECMP((l), (i)))
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

extern bool stm8_regs_used_as_parms_in_calls_from_current_function[YH_IDX + 1];
extern bool stm8_regs_used_as_parms_in_pcalls_from_current_function[YH_IDX + 1];

/*----------------------------------------------------------------------------*/
/* strNextCharBlock - Returns the next block of chars (after spaces, comma)   */
/* Leading spaces and Current block are skipped and search stops at next block*/
/* Valid block separators are: ' ' and ','                                    */
/* If no block is found (EOS or ';'), returns NULL                            */
/*----------------------------------------------------------------------------*/
static char *
strNextCharBlock(const char *str)
{
  if (!str || !str[0])
    return 0;

  while (isblank ((unsigned char)(str[0])))
    str++; // skip leading blanks

  while (str[0] && !isblank ((unsigned char)(str[0])) && str[0] != ';')
    {
      if (str[0] == ',')
        {
          str++; // current block is finished with ','
          break;
        }
      str++; // next char of current block
    }

  while (isblank ((unsigned char)(str[0])))
    str++; // skip trailing blanks 
    
  if (str[0] && str[0] != ';')
    return (char *)str;
  return 0;
}

static bool
isInt(const char *str)
{
  int ret;
  while(str[0] == '#' || str[0] == '(' || str[0] == '[' || isspace ((unsigned char)str[0]))
    str++;
  if(sscanf(str, "0x%x", &ret))
    return(ret);
  if(!sscanf(str, "%d", &ret))
    return(false);
  return(true);
}

static int
readint(const char *str)
{
  int ret;
  while(str[0] == '#' || str[0] == '(' || str[0] == '[' || isspace ((unsigned char)str[0]))
    str++;
  if(sscanf(str, "0x%x", &ret))
    return(ret);
  if(!sscanf(str, "%d", &ret))
    {
      wassertl (0, "readint() got non-integer argument:");
      fprintf (stderr, "%s\n", str);
      ret = -1;
    }
  return(ret);
}

static int
isReg(const char *what)
{
  if(what[0] == '(')
    what++;
  if(what[0] == 'a' || what[0] == 'x' || what[0] == 'y')
    return(true);
  if(!strcmp(what, "sp"))
    return(true);
  return(false);
}

static char *
nextToken(char *p)
{
  /* strtok replacement */
  static char *str, *ret, *end;
  // Use an internal buffer to prevent *p from being modified
  static char buf[128];
  if(p) {
    strncpy(buf, p, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
    str = buf;
    end = buf + strlen(p);
  }
  if(str >= end)
    return(NULL);
  ret = str;
  // Strip separators
  while(*str == ',' || isspace(*str))
    str++;
  if(*str == '(')
  {
    // Take an expression in brackets
    while(*str && *str != ')')
      str++;
    str++;
  } else {
    // Take until EOL or separator
    while(*str && *str != ',' && !isspace(*str))
      str++;
  }
  *str = '\0';
  str++;
  return(ret);
}

static bool
isRelativeAddr(const char *what, const char *mode)
{
  char buf[4];
  strcpy(buf, mode);
  strcat(buf, ")");
  return(what[0] == '(' && strstr(what, buf));
}

static bool
isLabel(const char *what)
{
  const char *end;

  end = strchr(what, '+');
  if(!end)
    end = what + strlen(what);
  if(what[0] == '(' && !strchr(what, ','))
    what++;
  if(what[0] == '#')
    return (what[1] == '_' || what[1] == '<' || what[1] == '>');
  return(what[0] == '_' || *(end-1) == '$');
}

static bool
isImmediate(const char *what)
{
  return(what[0] == '#');
}

static bool
isShortoff(const char *what, const char *mode)
{
  return(isRelativeAddr(what, mode) && isInt(what) && readint(what) <= 0xff);
}

static bool
isLongoff(const char *what, const char *mode)
{
  return(isRelativeAddr(what, mode) && (!isInt(what) || readint(what) > 0xff));
}

static bool
isPtr(const char *what)
{
  return(what[0] == '[' || what[0] == '(' && (what[1] == '[' || what[1] == '('));
}

static bool
isSpIndexed(const char *what)
{
  return isRelativeAddr(what, "sp");
}

/*-----------------------------------------------------------------*/
/* stm8InstIsRegToReg - Checks if 'line' is a reg to reg move      */
/* isword == FALSE : Look for registers a, xl, xh, yl & yh         */
/* isword == TRUE  : Look for registers x, y & sp                  */
/*-----------------------------------------------------------------*/
static bool
stm8InstIsRegToReg(const char *line, bool isword)
{
  int regNumber = 0;

  if ((line = strNextCharBlock(line)))
    {
      while(line[0])
        {
          bool regFound = false;
          char chrLow = tolower ((unsigned char)line[0]);

          // Check for register names
          if (isword)
            {
              if (chrLow == 'x' || chrLow == 'y')
                regFound = true;

              if (chrLow == 's')
                {
                  line++;
                  if (line[0] == 'p')
                    regFound = true;
                  else
                    return false;
                }
            }
          else
            {
              if (chrLow == 'a')
                regFound = true;

              if (chrLow == 'x' || chrLow == 'y')
                {
                  line++;
                  chrLow = tolower ((unsigned char)line[0]);
                  if (chrLow == 'h' || chrLow == 'l')
                    regFound = true;
                  else
                    return false;
                }
            }

          // If register, process next character
          if (regFound)
            line++;

          // Continue only if valid separator or end
          if (!line[0] || line[0] == ',' || isblank (line[0]))
            {
              if (regFound)
                regNumber++;
            }
          else
            {
              return false;
            }

          // Next char if not eos
          if(line[0])
            line++;
        }
    }
  return (regNumber == 2);
}

int
stm8instructionSize(lineNode *pl)
{ // this function is quite rough, it makes all indirect addressing cases to the longest.
  char *operand;
  char *op1start;
  char *op2start;
  
  operand = nextToken(pl->line);
  op1start = nextToken(NULL);
  op2start = nextToken(NULL);

  while(op1start && isspace((unsigned char)op1start[0])) op1start++;
  while(op2start && isspace((unsigned char)op2start[0])) op2start++;
  //printf("line=%s operand=%s op1start=%s op2start=%s\n", pl->line, operand, op1start, op2start);

  /* Operations that always costs 1 byte */
  if (lineIsInst (pl, "ccf")
    || lineIsInst (pl, "divw")
    || lineIsInst (pl, "exgw")
    || lineIsInst (pl, "iret")
    || lineIsInst (pl, "nop")
    || lineIsInst (pl, "rcf")
    || lineIsInst (pl, "ret")
    || lineIsInst (pl, "retf")
    || lineIsInst (pl, "rvf")
    || lineIsInst (pl, "break")
    || lineIsInst (pl, "halt")
    || lineIsInst (pl, "rim")
    || lineIsInst (pl, "trap")
    || lineIsInst (pl, "wfi")
    || lineIsInst (pl, "sim")
    || lineIsInst (pl, "scf"))
      return 1;

  /* Operations that always costs 3 byte */
  if(lineIsInst (pl, "jrh")
    || lineIsInst (pl, "jrnh")
    || lineIsInst (pl, "jril")
    || lineIsInst (pl, "jrih")
    || lineIsInst (pl, "jrm")
    || lineIsInst (pl, "jrnm"))
      return 3;

  /* Operations that always costs 2 byte */
  if(STARTSINST(operand, "jr")
    || lineIsInst (pl, "callr")
    || lineIsInst (pl, "wfe"))
      return 2;

  /* Operations that always costs 4 byte */
  if(lineIsInst (pl, "bccm")
    || lineIsInst (pl, "bcpl")
    || lineIsInst (pl, "bres")
    || lineIsInst (pl, "bset")
    || lineIsInst (pl, "callf")
    || lineIsInst (pl, "int")
    || lineIsInst (pl, "jpf"))
      return 4;

  /* Operations that always costs 5 byte */
  if(lineIsInst (pl, "btjf")
    || lineIsInst (pl, "btjt"))
      return 5;

  if (EQUALS(operand, "push")
    || EQUALS(operand, "pop"))
  {
    wassert (op1start);
    if (!strcmp(op1start, "a"))
      return 1;
    if (!strcmp(op1start, "cc"))
      return 1;
    if (isImmediate(op1start)) // immediate
      return 2;
    else // longmem
      return 3;
  }

  /* arity=1 */
  if(EQUALS(operand, "clr")
                || EQUALS(operand, "dec")
                || EQUALS(operand, "inc")
                || EQUALS(operand, "swap")
                || EQUALS(operand, "jp")
                || EQUALS(operand, "call")
                || EQUALS(operand, "cpl")
                || EQUALS(operand, "neg")
                || EQUALS(operand, "sll")
                || EQUALS(operand, "sla")
                || EQUALS(operand, "srl")
                || EQUALS(operand, "sra")
                || EQUALS(operand, "rlc")
                || EQUALS(operand, "rrc")
                || EQUALS(operand, "tnz"))
  {
    int i = 0;

    wassert (op1start);
    if(!strcmp(op1start, "a") || !strcmp(op1start, "(x)"))
      return(1);
    if(!strcmp(op1start, "(y)"))
      return(2);
    if(op1start[0] == '('|| op1start[0] == '[')
      op1start++;
    if(strstr(op1start, ",y)"))
      i++; // costs extra byte for operating with y
    if ((lineIsInst (pl, "jp") || lineIsInst (pl, "call")) && *op1start != '(' && *op1start != '[') // jp and call are 3 bytes for direct long addressing mode.
      return(3);
    if(isLabel(op1start))
      return(4);
    if(readint(op1start) <= 0xFF)
      return(2+i);
    /* op1 > 0xFF */
    if((lineIsInst (pl, "jp") || lineIsInst (pl, "call")) && !strchr(op1start, 'y'))
      return(3);
    return(4);
  }

  if(EQUALS(operand, "exg"))
  {
    assert (!strcmp(op1start, "a") && op2start != NULL);
    if(isReg(op2start))
      return(1);
    else
      return(3);
  }

  if(EQUALS(operand, "addw") || EQUALS(operand, "subw"))
  {
    assert (op1start != NULL);
    if(!strcmp(op1start, "sp"))
      return(2);
    if(isImmediate(op2start) && op1start[0] == 'y')
      return(4);
    if(isImmediate(op2start) && op1start[0] == 'x')
      return(3);
    if(isSpIndexed(op2start))
      return(3);
    return(4);
  }

  if(lineIsInst (pl, "cplw"))
  {
    assert (op1start != NULL);
    if(op1start[0] == 'y')
      return(2);
    else
      return(1);
  }

  if(lineIsInst (pl, "ldf"))
  {
    assert (op1start != NULL);
    if(isRelativeAddr(op1start, "y") || isRelativeAddr(op2start, "y"))
      return(5);
    else
      return(4);
  }

  /* Operations that costs 2 or 3 bytes for immediate */
  if(STARTSINST(operand, "ld")
                || !strncmp(operand, "cp", 2)
                || EQUALS(operand, "adc")
                || EQUALS(operand, "add")
                || EQUALS(operand, "and")
                || EQUALS(operand, "bcp")
                || EQUALS(operand, "or")
                || EQUALS(operand, "sbc")
                || EQUALS(operand, "sub")
                || EQUALS(operand, "xor"))
  {
    int i = 0;
    char suffix;
    wassert (op1start && op2start);
    suffix = operand[strlen(operand)-1];
    if(suffix == 'w' && isImmediate(op2start))
      {
        i++; // costs extra byte
        if(!strcmp(op1start, "y"))
          i++;
      }
    if(isImmediate(op2start))
      return(2+i); // ld reg, #immd
    if(isSpIndexed(op1start) || isSpIndexed(op2start))
      return(2);
    if(!strcmp(op1start, "(x)") || !strcmp(op2start, "(x)"))
      return(1);
    if(!strcmp(op1start, "(y)") || !strcmp(op2start, "(y)"))
      return(2);
    if(isShortoff(op1start, "x") || isShortoff(op2start, "x"))
      return(2);
    if(isShortoff(op1start, "y") || isShortoff(op2start, "y"))
      return(3);
    if(isLongoff(op1start, "x") || isLongoff(op2start, "x"))
      return(3);
    if(isLongoff(op1start, "y") || isLongoff(op2start, "y"))
      return(4);
    if(isPtr(op1start) || isPtr(op2start))
      return(4);
    if(strchr(op1start, 'y') || strchr(op2start, 'y'))
      i++; // costs extra byte for operating with y
    if(isLabel(op1start) || isLabel(op2start))
      return(3+i);
    if(isReg(op1start) && isReg(op2start))
      {
        if (!strncmp(op1start, "x", 1) && (!strncmp(op2start, "y", 1) || !strncmp(op2start, "sp", 2))
          || !strncmp(op1start, "sp", 2) && !strncmp(op2start, "x", 1))
          return(1);
        return(1+i);
      }
    if(!strcmp(op2start, "a"))
      return(3);
    if(readint(op2start) <= 0xFF)
      return(2+i);
    else
      return(3+i);
    return 4;
  }

  /* mov costs 3, 4 or 5 bytes depending on its addressing mode */
  if(lineIsInst (pl, "mov")) {
    assert (op1start != NULL && op2start != NULL);
    if(isImmediate(op2start))
      return(4);
    if(isLabel(op2start))
      return(5);
    if(readint(op2start) <= 0xFF)
      return(3);
    if(readint(op2start) > 0xFF)
      return(5);
  }

  /* Operations that costs 2 or 1 bytes depending on 
     is the Y or X register used */
  if(EQUALS(operand, "clrw")
                || EQUALS(operand, "decw")
                || EQUALS(operand, "div")
                || EQUALS(operand, "incw")
                || EQUALS(operand, "mul")
                || EQUALS(operand, "negw")
                || EQUALS(operand, "popw")
                || EQUALS(operand, "pushw")
                || EQUALS(operand, "rlcw")
                || EQUALS(operand, "rlwa")
                || EQUALS(operand, "rrcw")
                || EQUALS(operand, "rrwa")
                || EQUALS(operand, "sllw")
                || EQUALS(operand, "slaw")
                || EQUALS(operand, "sraw")
                || EQUALS(operand, "srlw")
                || EQUALS(operand, "swapw")
                || EQUALS(operand, "tnzw"))
  {
    assert (op1start != NULL);
    if((op1start && !strcmp(op1start, "y")) || (op2start && !strcmp(op2start, "y")))
      return(2);
    else
      return(1);
  }

  if(lineIsInst (pl, ".db") || lineIsInst (pl, ".byte"))
    {
      int i, j;
      for(i = 1, j = 0; pl->line[j]; i += pl->line[j] == ',', j++);
      return(i);
    }

  if(lineIsInst (pl, ".dw") || lineIsInst (pl, ".word"))
    {
      int i, j;
      for(i = 1, j = 0; pl->line[j]; i += pl->line[j] == ',', j++);
      return(i * 2);
    }

  return(5); // Maximum instruction size, e.g. btjt.
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
    if (isspace(*p) || *p == ',')
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

/* Check if reading arg implies reading what. */
static bool argCont(const char *arg, char what)
{
  if (arg == NULL || strlen (arg) == 0 || !(what == 'a' || what == 'x' || what == 'y'))
    return FALSE;

  while (isblank ((unsigned char)(arg[0])))
    arg++;

  if (arg[0] == ',')
    arg++;

  while (isblank ((unsigned char)(arg[0])))
    arg++;

  if (arg[0] == '#')
    return FALSE;

  if (arg[0] == '(') 
    arg++;
  if (arg[0] == '0' && (tolower(arg[1])) == 'x') 
    arg += 2; // Skip hex prefix to avoid false x positive.

  if (strlen(arg) == 0)
    return false;

  if (arg[0] == '_')
    {
      if (what == 'a') // The STM8 has no a-relative addressing modes.
        return false;
      arg = strchr(arg, ','); // Skip over user-defined variable names.
      if (!arg)
        return false;
    }

  return (strchr(arg, what) != NULL);
}

static bool
stm8MightReadFlag(const lineNode *pl, const char *what)
{
  if (strcmp (what, "v") && strcmp (what, "c") && strcmp (what, "n") && strcmp (what, "z"))
    return true;

  if (lineIsInst (pl, "push"))
     return (pl->line[5] == 'c');

  if (!strcmp (what, "v"))
    return (lineIsInst (pl, "jrnv") || lineIsInst (pl, "jrsge") || lineIsInst (pl, "jrsgt") || lineIsInst (pl, "jrsle") || lineIsInst (pl, "jrslt") || lineIsInst (pl, "jrv"));

  if (!strcmp (what, "n"))
    return (lineIsInst (pl, "jrmi") || lineIsInst (pl, "jrpl") || lineIsInst (pl, "jrsge") || lineIsInst (pl, "jrsgt") || lineIsInst (pl, "jrsle") || lineIsInst (pl, "jrslt"));

  if (!strcmp (what, "z"))
    return (lineIsInst (pl, "jreq") || lineIsInst (pl, "jrne") || lineIsInst (pl, "jrsgt") || lineIsInst (pl, "jrsle"));

  if (!strcmp (what, "c"))
    return (lineIsInst (pl, "jrc") || lineIsInst (pl, "jrnc") || lineIsInst (pl, "jruge") || lineIsInst (pl, "jrugt") || lineIsInst (pl, "jrule") || lineIsInst (pl, "jrult") ||
      lineIsInst (pl, "adc") || lineIsInst (pl, "sbc") ||
      lineIsInst (pl, "ccf") || lineIsInst (pl, "rlc") || lineIsInst (pl, "rlcw") || lineIsInst (pl, "rrc") || lineIsInst (pl, "rrcw"));

  return true;
}

static bool
stm8MightBeParmInCallFromCurrentFunction(const char *what)
{
  if (!strcmp(what, "a"))
    return stm8_regs_used_as_parms_in_calls_from_current_function[A_IDX];
    
  if ((!strcmp(what, "x") || !strcmp(what, "xl")) && stm8_regs_used_as_parms_in_calls_from_current_function[XL_IDX])
    return true;
  if ((!strcmp(what, "x") || !strcmp(what, "xh")) && stm8_regs_used_as_parms_in_calls_from_current_function[XH_IDX])
    return true;

  if ((!strcmp(what, "y") || !strcmp(what, "yl")) && stm8_regs_used_as_parms_in_calls_from_current_function[YL_IDX])
    return true;
  if ((!strcmp(what, "yx") || !strcmp(what, "yh")) && stm8_regs_used_as_parms_in_calls_from_current_function[YH_IDX])
    return true;

  return false;
}

static bool
stm8MightBeParmInPCallFromCurrentFunction(const char *what)
{
  if (!strcmp(what, "a"))
    return stm8_regs_used_as_parms_in_pcalls_from_current_function[A_IDX];

  if ((!strcmp(what, "x") || !strcmp(what, "xl")) && stm8_regs_used_as_parms_in_pcalls_from_current_function[XL_IDX])
    return true;
  if ((!strcmp(what, "x") || !strcmp(what, "xh")) && stm8_regs_used_as_parms_in_pcalls_from_current_function[XH_IDX])
    return true;

  if ((!strcmp(what, "y") || !strcmp(what, "yl")) && stm8_regs_used_as_parms_in_pcalls_from_current_function[YL_IDX])
    return true;
  if ((!strcmp(what, "yx") || !strcmp(what, "yh")) && stm8_regs_used_as_parms_in_pcalls_from_current_function[YH_IDX])
    return true;

  return false;
}

static bool
stm8MightRead(const lineNode *pl, const char *what)
{
  char extra = 0;

  if (!strcmp (what, "xl") || !strcmp (what, "xh"))
    extra = 'x';
  else if (!strcmp (what, "yl") || !strcmp (what, "yh"))
    extra = 'y';
  else if (strcmp (what, "a") != 0)
    return stm8MightReadFlag(pl, what);

  if (!extra)
    {
      if (lineIsInst (pl, "adc")
        || lineIsInst (pl, "and")
        || lineIsInst (pl, "bcp")
        || lineIsInst (pl, "cp")
        || lineIsInst (pl, "div")
        || lineIsInst (pl, "mul")
        || lineIsInst (pl, "or")
        || lineIsInst (pl, "rlwa")
        || lineIsInst (pl, "rrwa")
        || lineIsInst (pl, "sbc")
        || lineIsInst (pl, "trap")
        || lineIsInst (pl, "xor"))
          return TRUE;

      if ((lineIsInst (pl, "add")
        || lineIsInst (pl, "cpl")
        || lineIsInst (pl, "dec")
        || lineIsInst (pl, "exg")
        || lineIsInst (pl, "inc")
        || lineIsInst (pl, "neg")
        || lineIsInst (pl, "rlc")
        || lineIsInst (pl, "rrc")
        || lineIsInst (pl, "sll")
        || lineIsInst (pl, "sla")
        || lineIsInst (pl, "sra")
        || lineIsInst (pl, "srl")
        || lineIsInst (pl, "sub")
        || lineIsInst (pl, "tnz")) &&
        pl->line[4] == 'a')
          return TRUE;

      if ((lineIsInst (pl, "push")
        || lineIsInst (pl, "swap")) &&
        pl->line[5] == 'a')
          return TRUE;

      if ((lineIsInst (pl, "ld") || lineIsInst (pl, "ldf")) && argCont (strchr (pl->line, ','), 'a'))
          return TRUE;
    }
  else
    {
      if (lineIsInst (pl, "divw") || lineIsInst (pl, "exgw") || lineIsInst (pl, "trap"))
        return TRUE;
 
      if (lineIsInst (pl, "exg") && strstr (strchr(pl->line, ','), what))
        return true;

      if ((lineIsInst (pl, "div") || lineIsInst (pl, "mul")) && pl->line[4] == extra)
        return true;

      if ((lineIsInst (pl, "addw")
        || lineIsInst (pl, "cplw")
        || lineIsInst (pl, "decw")
        || lineIsInst (pl, "incw")
        || lineIsInst (pl, "negw")
        || lineIsInst (pl, "rlcw")
        || lineIsInst (pl, "rlwa")
        || lineIsInst (pl, "rrcw")
        || lineIsInst (pl, "rrwa")
        || lineIsInst (pl, "sllw")
        || lineIsInst (pl, "slaw")
        || lineIsInst (pl, "sraw")
        || lineIsInst (pl, "srlw")
        || lineIsInst (pl, "subw")
        || lineIsInst (pl, "tnzw")) &&
        pl->line[5] == extra)
          return TRUE;

      if ((lineIsInst (pl, "pushw")
        || lineIsInst (pl, "swapw")) && pl->line[6] == extra)
          return TRUE;

      if (lineIsInst (pl, "cpw") && pl->line[4] == extra)
        return TRUE;

      if ((strchr (pl->line, ',') ? argCont (strchr (pl->line, ','), extra) : argCont (strchr (pl->line, '('), extra)) &&
        (lineIsInst (pl, "adc")
        || lineIsInst (pl, "add")
        || lineIsInst (pl, "and")
        || lineIsInst (pl, "bcp")
        || lineIsInst (pl, "call")
        || lineIsInst (pl, "clr")
        || lineIsInst (pl, "cp")
        || lineIsInst (pl, "cpl")
        || lineIsInst (pl, "dec")
        || lineIsInst (pl, "inc")
        || lineIsInst (pl, "jp")
        || lineIsInst (pl, "neg")
        || lineIsInst (pl, "or")
        || lineIsInst (pl, "rlc")
        || lineIsInst (pl, "rrc")
        || lineIsInst (pl, "sbc")
        || lineIsInst (pl, "sll")
        || lineIsInst (pl, "sla")
        || lineIsInst (pl, "sra")
        || lineIsInst (pl, "srl")
        || lineIsInst (pl, "sub")
        || lineIsInst (pl, "swap")
        || lineIsInst (pl, "tnz")
        || lineIsInst (pl, "cpw")
        || lineIsInst (pl, "ldf")
        || lineIsInst (pl, "ldw")
        || lineIsInst (pl, "ld")
        || lineIsInst (pl, "xor")))
          return TRUE;

      if (lineIsInst (pl, "ld") || lineIsInst (pl, "ldw"))
        {
          char buf[128], *p;
          if (strlen (pl->line) >= 128) // Avoid buffer overflow, err on safe side.
            return TRUE;
          strcpy (buf, pl->line);
          if (!!(p = strstr (buf, "0x")) || !!(p = strstr (buf, "0X")))
            p[0] = p[1] = ' ';
          if (!!(p = strchr (buf, '(')) && !!strchr (p, extra))
            return TRUE;
        }
    }

  if (lineIsInst (pl, "call") || lineIsInst (pl, "callr") || lineIsInst (pl, "callf"))
    {
      const symbol *f = findSym (SymbolTab, 0, pl->line + (lineIsInst (pl, "call") ? 6 : 7));
      if (f && IS_FUNC (f->type))
        return stm8IsParmInCall(f->type, what);
      else // Fallback needed for calls through function pointers and for calls to literal addresses.
        return (stm8MightBeParmInCallFromCurrentFunction(what) || stm8MightBeParmInPCallFromCurrentFunction(what));
    }

  if(lineIsInst (pl, "ret")) // IAR calling convention uses ret for some calls via pointers
    return(stm8IsReturned(what) || stm8MightBeParmInPCallFromCurrentFunction(what));

  if(lineIsInst (pl, "retf")) // Large model uses retf for calls via function pointers
    return(stm8IsReturned(what) || stm8MightBeParmInPCallFromCurrentFunction(what));

  return FALSE;
}

static bool
stm8UncondJump(const lineNode *pl)
{
  return (lineIsInst (pl, "jp") || lineIsInst (pl, "jra") || lineIsInst (pl, "jrt") || lineIsInst (pl, "jpf"));
}

static bool
stm8CondJump(const lineNode *pl)
{
  return (!stm8UncondJump(pl) && STARTSINST(pl->line, "jr") ||
    lineIsInst (pl, "btjt") || lineIsInst (pl, "btjf"));
}

static bool
stm8SurelyWritesFlag(const lineNode *pl, const char *what)
{
  // according to calling convention caller has to save flags
  if(lineIsInst (pl, "ret") || lineIsInst (pl, "retf") ||
     lineIsInst (pl, "call") || lineIsInst (pl, "callf") ||
     lineIsInst (pl, "jp") && findSym (SymbolTab, 0, pl->line + 4) ||
     lineIsInst (pl, "jpf") && findSym (SymbolTab, 0, pl->line + 5))
    return true;

  if (!strcmp (what, "v") || !strcmp (what, "c"))
    {        
      if (lineIsInst (pl, "addw") && !strcmp (pl->line + 5, "sp"))
        return false;
      if (lineIsInst (pl, "sub") && !strcmp (pl->line + 4, "sp"))
        return false;

      if (lineIsInst (pl, "adc") ||
        STARTSINST (pl->line, "add") || // add, addw
        STARTSINST (pl->line, "cp") || // cp, cpw, cpl, cplw
        STARTSINST (pl->line, "div") || // div, divw
        STARTSINST (pl->line, "neg") || // neg, negw
        lineIsInst (pl, "sbc") ||
        STARTSINST (pl->line, "sub")) // sub, subw
        return true;
    }

  if (!strcmp (what, "n") || !strcmp (what, "z"))
    {
      if (lineIsInst (pl, "addw") && !strcmp (pl->line + 5, "sp"))
        return false;
      if (lineIsInst (pl, "sub") && !strcmp (pl->line + 4, "sp"))
        return false;
      if (lineIsInst (pl, "ld"))
        return !stm8InstIsRegToReg(pl->line, false);
      if (lineIsInst (pl, "ldw"))
        return !stm8InstIsRegToReg(pl->line, true);
      if (lineIsInst (pl, "pop"))
        return (pl->line[5] == 'c');
      if (lineIsInst (pl, "bccm") || lineIsInst (pl, "bcpl") ||
        lineIsInst (pl, "break") ||
        lineIsInst (pl, "bres") || lineIsInst (pl, "bset") ||
        lineIsInst (pl, "btjf") || lineIsInst (pl, "btjt") ||
        lineIsInst (pl, "call") || lineIsInst (pl, "callf") || lineIsInst (pl, "callr") ||
        lineIsInst (pl, "ccf") ||
        lineIsInst (pl, "exg") || lineIsInst (pl, "exgw") ||
        lineIsInst (pl, "halt") || lineIsInst (pl, "int") ||
        STARTSINST (pl->line, "jp") ||
        STARTSINST (pl->line, "jr") ||
        lineIsInst (pl, "mov") || lineIsInst (pl, "mul") ||
        lineIsInst (pl, "nop") ||
        lineIsInst (pl, "popw") || lineIsInst (pl, "push") || lineIsInst (pl, "pushw") ||
        lineIsInst (pl, "rcf") ||
        lineIsInst (pl, "ret") || lineIsInst (pl, "retf") ||
        lineIsInst (pl, "rvf") || lineIsInst (pl, "scf") ||
        lineIsInst (pl, "sim") || lineIsInst (pl, "trap") || lineIsInst (pl, "wfe") || lineIsInst (pl, "wfi"))
        return false;
      return true;
    }

  if (!strcmp (what, "c"))
    {
      if (STARTSINST (pl->line, "btj") || // btjt, btjf
        lineIsInst (pl, "ccf") ||
        lineIsInst (pl, "rcf") ||
        STARTSINST (pl->line, "rlc") || // rlc, rlcw
        STARTSINST (pl->line, "rrc") || // rrc, rrcw
        lineIsInst (pl, "sbc") ||
        lineIsInst (pl, "scf") ||
        STARTSINST (pl->line, "sl") || // sll, sla, sllw, slaw
        STARTSINST (pl->line, "sr") || // sra, sraw, srl, srlw
        STARTSINST (pl->line, "sub")) // sub, subw
        return true;
    }

  return false;
}

static bool
callSurelyWrites (const lineNode *pl, const char *what)
{
  const symbol *f = 0;
  if ((lineIsInst (pl, "call") || lineIsInst (pl, "callf")) && !strchr(pl->line, ','))
    f = findSym (SymbolTab, 0, pl->line + 6 + lineIsInst (pl, "callf"));
  else if ((lineIsInst (pl, "jp") || lineIsInst (pl, "jr") || lineIsInst (pl, "jpf")) && !strchr(pl->line, ','))
    f = findSym (SymbolTab, 0, pl->line + 4 + lineIsInst (pl, "jpf"));

  const bool *preserved_regs;

  if(f)
    preserved_regs = f->type->funcAttrs.preserved_regs;
  else // Err on the safe side for jp and jr - might not be a function call, might e.g. be a jump table.
    return (false);

  if (!strcmp (what, "a"))
    return !preserved_regs[A_IDX];
  else if (!strcmp (what, "xl"))
    return !preserved_regs[XL_IDX];
  else if (!strcmp (what, "xh"))
    return !preserved_regs[XH_IDX];
  else if (!strcmp (what, "yl"))
    return !preserved_regs[YL_IDX];
  else if (!strcmp (what, "yh"))
    return !preserved_regs[YH_IDX];

  return false;
}

static bool
stm8SurelyWrites(const lineNode *pl, const char *what)
{
  char extra = 0;
  if (!strcmp (what, "xl") || !strcmp (what, "xh"))
    extra = 'x';
  else if (!strcmp (what, "yl") || !strcmp (what, "yh"))
    extra = 'y';
  else if (strcmp (what, "a"))
    return (stm8SurelyWritesFlag (pl, what));

  if (!extra)
    {
      if (lineIsInst (pl, "adc")
        || lineIsInst (pl, "and")
        || lineIsInst (pl, "div")
        || lineIsInst (pl, "iret")
        || lineIsInst (pl, "or")
        || lineIsInst (pl, "rlwa")
        || lineIsInst (pl, "rrwa")
        || lineIsInst (pl, "sbc")
        || lineIsInst (pl, "xor"))
          return TRUE;

      if ((lineIsInst (pl, "add")
        || lineIsInst (pl, "clr")
        || lineIsInst (pl, "cpl")
        || lineIsInst (pl, "dec")
        || lineIsInst (pl, "exg")
        || lineIsInst (pl, "inc")
        || lineIsInst (pl, "neg")
        || lineIsInst (pl, "pop")
        || lineIsInst (pl, "rlc")
        || lineIsInst (pl, "rrc")
        || lineIsInst (pl, "sll")
        || lineIsInst (pl, "sla")
        || lineIsInst (pl, "sra")
        || lineIsInst (pl, "srl")
        || lineIsInst (pl, "ldf")
        || lineIsInst (pl, "sub")) &&
        pl->line[4] == 'a')
          return TRUE;

      if (lineIsInst (pl, "swap") && pl->line[5] == 'a')
        return TRUE;

      if (lineIsInst (pl, "ld") && pl->line[3] == 'a')
        return TRUE;
    }
  else
    {
      if (lineIsInst (pl, "divw")
        || lineIsInst (pl, "exgw")
        || lineIsInst (pl, "iret"))
          return TRUE;

      if ((lineIsInst (pl, "div")
        || lineIsInst (pl, "ldw")
        || lineIsInst (pl, "mul"))
        && pl->line[4] == extra)
          return TRUE;

      if ((lineIsInst (pl, "addw")
        || lineIsInst (pl, "clrw")
        || lineIsInst (pl, "cplw")
        || lineIsInst (pl, "decw")
        || lineIsInst (pl, "incw")
        || lineIsInst (pl, "negw")
        || lineIsInst (pl, "popw")
        || lineIsInst (pl, "rlcw")
        || lineIsInst (pl, "rlwa")
        || lineIsInst (pl, "rrcw")
        || lineIsInst (pl, "rrwa")
        || lineIsInst (pl, "sllw")
        || lineIsInst (pl, "slaw")
        || lineIsInst (pl, "sraw")
        || lineIsInst (pl, "srlw")
        || lineIsInst (pl, "subw")) &&
        pl->line[5] == extra)
          return TRUE;

      if (lineIsInst (pl, "swapw") && pl->line[6] == extra)
        return TRUE;

      if (lineIsInst (pl, "ld")
        && strncmp (pl->line + 3, what, strlen (what)) == 0)
        return TRUE;

      if (lineIsInst (pl, "exg") && strstr (strstr (pl->line, ","), what))
        return true;
    }

  if (lineIsInst (pl, "call"))
    return (callSurelyWrites (pl, what));

  return false;
}

static bool
stm8SurelyReturns(const lineNode *pl)
{
  return(lineIsInst (pl, "ret") || lineIsInst (pl, "retf"));
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

      if(stm8MightRead(*pl, what))
        {
          D(("S4O_RD_OP\n"));
          return S4O_RD_OP;
        }

      // Check writes before conditional jumps, some jumps (btjf, btjt) write 'c'
      if(stm8SurelyWrites(*pl, what))
        {
          D(("S4O_WR_OP\n"));
          return S4O_WR_OP;
        }

      if(stm8UncondJump(*pl))
        {
          lineNode *tlbl = findLabel (*pl);
          if (!tlbl) // jp/jr could be a tail call.
            {
              const symbol *f = findSym (SymbolTab, 0, (*pl)->line + 4 + lineIsInst (*pl, "jpf"));
              if (f && stm8IsParmInCall(f->type, what))
                {
                  D (("S4O_RD_OP\n"));
                  return S4O_RD_OP;
                }
              else if(callSurelyWrites (*pl, what))
                {
                  D (("S4O_WR_OP\n"));
                  return S4O_WR_OP;
                }
            }
          *pl = tlbl;
          if (!*pl)
            {
              D(("S4O_ABORT at unconditional jump\n"));
              return S4O_ABORT;
            }
        }
      if(stm8CondJump(*pl))
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

      /* Don't need to check for de, hl since stm8MightRead() does that */
      if(stm8SurelyReturns(*pl))
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
stm8notUsed (const char *what, lineNode *endPl, lineNode *head)
{
  lineNode *pl;
  if(strcmp(what, "x") == 0)
    return(stm8notUsed("xl", endPl, head) && stm8notUsed("xh", endPl, head));
  else if(strcmp(what, "y") == 0)
    return(stm8notUsed("yl", endPl, head) && stm8notUsed("yh", endPl, head));

  _G.head = head;

  unvisitLines (_G.head);

  pl = endPl->next;
  return (doTermScan (&pl, what));
}

bool
stm8notUsedFrom (const char *what, const char *label, lineNode *head)
{
  lineNode *cpl;

  for (cpl = head; cpl; cpl = cpl->next)
    if (cpl->isLabel && !strncmp (label, cpl->line, strlen(label)))
      return (stm8notUsed (what, cpl, head));

  return FALSE;
}

/* can be directly assigned with ld */
bool
stm8canAssign (const char *op1, const char *op2, const char *exotic)
{
  //fprintf(stderr, "op1=%s op2=%s exotic=%s\n", op1, op2, exotic);
  const char *reg, *payload;
  reg = op1[0] == 'a' ? op1 : op2;
  payload = reg == op1 ? op2 : op1;
  if(isRelativeAddr(payload, "x")
                || isRelativeAddr(payload, "y")
                || isRelativeAddr(payload, "sp")
                || !strcmp(payload, "(x)")
                || !strcmp(payload, "(y)")
                || !strcmp(payload, "xl")
                || !strcmp(payload, "xh"))
    return(reg[0] == 'a');
  return(FALSE);
}
