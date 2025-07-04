/*-------------------------------------------------------------------------
  gen.c - source file for code generation for the MOS6502

  Copyright (C) 1998, Sandeep Dutta . sandeep.dutta@usa.net
  Copyright (C) 1999, Jean-Louis VERN.jlvern@writeme.com
  Bug Fixes - Wojciech Stryjewski  wstryj1@tiger.lsu.edu (1999 v2.1.9a)
  Hacked for the HC08:
  Copyright (C) 2003, Erik Petrich
  Hacked for the MOS6502:
  Copyright (C) 2020, Steven Hugg  hugg@fasterlight.com
  Copyright (C) 2021-2025, Gabriele Gorla

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
#include "dbuf_string.h"

extern int allocInfo;

static bool regalloc_dry_run;
static unsigned int regalloc_dry_run_cost_bytes;
static float regalloc_dry_run_cost_cycles;

struct m6502_state_t _S;

extern int m6502_ptrRegReq;
extern int m6502_nRegs;
extern struct dbuf_s *codeOutBuf;

static bool operandsEqu (operand * op1, operand * op2);
static asmop *newAsmop (short type);
static void aopAdrPrepare (asmop * aop, int loffset);
static const char *aopAdrStr (asmop * aop, int loffset, bool bit16);
static void aopAdrUnprepare (asmop * aop, int loffset);
static void updateiTempRegisterUse (operand * op);
static char * aopName (asmop * aop);


static asmop *m6502_aop_pass[8];
asmop tsxaop;

const char *IMMDFMT = "#0x%02x";
const char *TEMPFMT = "*(REGTEMP+%d)";
const char *TEMPFMT_IND = "[REGTEMP+%d]";
//static char *TEMPFMT_IY = "[REGTEMP+%d],y";

const char *IDXFMT_X = "0x%x,x";
//static char *TEMPFMT_IX = "[(REGTEMP+%d),x]";
const char *DPTRFMT = "*(DPTR+%d)";
const char *DPTRFMT_IY = "[DPTR],y";
const char *INDFMT_IY = "[%s],y";

const int STACK_TOP = 0x100;


#define RESULTONSTACK(x)                        \
  (IC_RESULT(x) && IC_RESULT(x)->aop &&         \
   IC_RESULT(x)->aop->type == AOP_STK )

#define IS_AOPOFS_A(x,o) (((x)->type == AOP_REG) && ((x)->aopu.aop_reg[o]->mask == M6502MASK_A))
#define IS_AOPOFS_X(x,o) (((x)->type == AOP_REG) && ((x)->aopu.aop_reg[o]->mask == M6502MASK_X))
#define IS_AOPOFS_Y(x,o) (((x)->type == AOP_REG) && ((x)->aopu.aop_reg[o]->mask == M6502MASK_Y))


#define IS_SAME_DPTR_OP(op) (AOP(op) && _S.DPTRAttr[0].aop && _S.DPTRAttr[1].aop \
			     && sameRegs (AOP(op), _S.DPTRAttr[0].aop))

/**************************************************************************
 * returns the register containing the AOP or null if not found
 *
 * @param aop pointer to aop
 * @param offset offset of the aop
 * @return reg pointer or NULL if not found
 *************************************************************************/
static reg_info*
findRegAop (asmop * aop, int loffset)
{
  reg_info *ret=NULL;

  if (m6502_reg_a->aop && m6502_reg_a->aop->op && aop->op
      && operandsEqu (m6502_reg_a->aop->op, aop->op) && (m6502_reg_a->aopofs == loffset))
    {
      ret=m6502_reg_a;
    }
  else if (m6502_reg_x->aop && m6502_reg_x->aop->op && aop->op
	   && operandsEqu (m6502_reg_x->aop->op, aop->op) && (m6502_reg_x->aopofs == loffset))
    {
      ret=m6502_reg_x;
    }
  else if (m6502_reg_y->aop && m6502_reg_y->aop->op && aop->op
	   && operandsEqu (m6502_reg_y->aop->op, aop->op) && (m6502_reg_y->aopofs == loffset))
    {
      ret=m6502_reg_y;
    }

  return ret;
}

/**************************************************************************
 * Keeps track of last aop for the register
 *
 * @param reg pointer to the register
 * @param aop pointer to aop
 * @param offset offset of the aop
 *************************************************************************/
void
regTrackAop(reg_info *reg, asmop *aop, int offset)
{
  if(!reg)
    emitcode(";ERROR","  %s : called with NULL reg", __func__ );
  if(!aop)
    emitcode(";ERROR","  %s : called with NULL aop", __func__ );

  //  emitComment (ALWAYS /*REGOPS|VVDBG*/, "  reg %s = A %d", reg->name, offset);
  switch(reg->rIdx)
    {
    case A_IDX:
    case X_IDX:
    case Y_IDX:
      reg->aop = aop;
      reg->aop->op = aop->op;
      reg->aopofs = offset;
      break;
    case XA_IDX:
      regTrackAop(m6502_reg_a, aop, offset);
      regTrackAop(m6502_reg_x, aop, offset+1);
      break;
    default:
      emitcode ("ERROR", " %s - illegal register %s", __func__, reg->name);
      break;
    }
}

/**************************************************************************
 * Marks registers stale based on an aop
 *
 * @param reg pointer to the register to exclude. All registers if NULL.
 * @param aop pointer to aop
 * @param offset offset of the aop
 *************************************************************************/
void
dirtyRegAop(reg_info *reg, asmop *aop, int offset)
{
  emitComment (REGOPS|VVDBG, " %s - reg=%08x  asmop=%08d off=%d",
	       __func__, reg, aop, offset);

  if(reg==m6502_reg_xa)
    {
      dirtyRegAop(m6502_reg_a, aop, offset);
      dirtyRegAop(m6502_reg_x, aop, offset+1);
      return;
    }

  if(reg!=m6502_reg_a && m6502_reg_a->aop)
    {
      if(sameRegs (m6502_reg_a->aop, aop) 
         && (m6502_reg_a->aopofs == offset) )
        {
          emitComment (REGOPS|VVDBG, "  marking A stale");
          m6502_reg_a->aop = NULL;
        }
    }
  if(reg!=m6502_reg_x && m6502_reg_x->aop)
    {
      if(sameRegs (m6502_reg_x->aop, aop) 
         && (m6502_reg_x->aopofs == offset) )
        {
          emitComment (REGOPS|VVDBG, "  marking X stale");
          m6502_reg_x->aop = NULL;
        }
    }
  if(reg!=m6502_reg_y && m6502_reg_y->aop)
    {
      if(sameRegs (m6502_reg_y->aop, aop) 
         && (m6502_reg_y->aopofs == offset) )
        {
          emitComment (REGOPS|VVDBG, "  marking Y stale");
          m6502_reg_y->aop = NULL;
        }
    }

  if(_S.DPTRAttr[0].aop && sameRegs(_S.DPTRAttr[0].aop, aop) && _S.DPTRAttr[0].aopofs==offset)
    {
      _S.DPTRAttr[0].aop=NULL;
    }

  if(_S.DPTRAttr[1].aop && sameRegs(_S.DPTRAttr[1].aop, aop) && _S.DPTRAttr[1].aopofs==offset)
    {
      _S.DPTRAttr[1].aop=NULL;
    }

}


symbol *
safeNewiTempLabel(const char * a)
{
  if(regalloc_dry_run)
    return NULL;
  else
    return newiTempLabel(a);
}

/**************************************************************************
 * Returns the number for the label
 *
 * @param a  pointer to the label symbol
 * @return label number
 *************************************************************************/
void
safeEmitLabel(symbol * a)
{ 
  if(!regalloc_dry_run)
    {
      if (a)
        emitLabel(a);
      else
        emitcode(";ERROR","  %s : called with NULL symbol", __func__ );
    }
  _S.lastflag=-1;
  _S.carryValid=0;
}

/**************************************************************************
 * Returns the number for the label
 *
 * @param a  pointer to the label symbol
 * @return label number
 *************************************************************************/
int
safeLabelNum(symbol * a)
{
  if(regalloc_dry_run)
    return 0;
 
  if(a)
    return labelKey2num(a->key);

  emitcode("ERROR","  %s : called with NULL symbol", __func__ );
  return 0;
}

/**************************************************************************
 * Returns the last regtemp location
 *
 * @return last temp offset
 *************************************************************************/
int
getLastTempOfs()
{
  return _S.tempOfs-1;
}

/**************************************************************************
 * Returns the last register affecting the flag
 *
 * @return last register
 *************************************************************************/
int
getLastFlag()
{
  return _S.lastflag;
}

void
setLastFlag(int reg_idx)
{
  // should check for out of bound
  _S.lastflag=reg_idx;
}

/**************************************************************************
 * Returns the cycle count for the instruction
 *
 * @param opcode  pointer to opcode entry in the opcode table
 * @param arg string constant with the opcode argument
 * @return minimum number of cycles for the instruction
 *************************************************************************/
int
m6502_opcodeCycles(const m6502opcodedata *opcode, const char *arg)
{
  int lastpos;
  
  lastpos=(*arg)?strlen(arg)-1:0;
  
  switch (opcode->type)
    {
    case M6502OP_INH: /* Inherent addressing mode */
    case M6502OP_IDD:
    case M6502OP_IDI:
    case M6502OP_BR:  /* Branch (1 byte signed offset) */
      if(opcode->name[0]=='r'&&opcode->name[1]=='t') // rti and rts
        return 6;
      return 2;
    case M6502OP_SPH:
      return 3;
    case M6502OP_SPL:
      return 4;
    case M6502OP_BBR:  /* Branch on bit (1 byte signed offset) */
      return 3;
    case M6502OP_RMW: /* read/modify/write instructions */
      if (!strcmp(arg, "a"))  /* accumulator */
        return 2;
      if (arg[0] == '*') /* Zero page */
        return 5;
      if(lastpos>2 && arg[lastpos-1]!=',' && arg[lastpos]=='x' )
        return 7;
      return 6;  /* absolute */
    
    case M6502OP_REG: /* standard instruction */
    case M6502OP_CMP:
    case M6502OP_LD:
      if (arg[0] == '#') /* Immediate addressing mode */
        return 2;
      if (arg[0] == '*')
        { /* Zero page */
          if(arg[lastpos]=='x' || arg[lastpos]=='y')
            return 4;
          return 3;
        }
      if (arg[0] == '[')
        { /* indirect */
          if(arg[lastpos]==']')
            return 6;
          return 5;
        }
      return 4; /* Otherwise, must be absolute addressing mode */
    
    case M6502OP_ST:
      if (arg[0] == '*')
        { /* Zero page */
          if(arg[lastpos]=='x' || arg[lastpos]=='y')
            return 4;
          return 3;
        }
      if (arg[0] == '[')  /* indirect */
        return 6;
      if(arg[lastpos]=='x' || arg[lastpos]=='y')
        return 5;
      return 4;
    
    case M6502OP_JMP:
      if(opcode->name[1]=='s')
        return 6;
      if(arg[0]=='[')
        return 5;
      return 3;
    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "unknown instruction type in m6502_opcodeSize");
      return 3;
    }
}

/**************************************************************************
 * Emits comments and debug messages
 *
 * @param level  bitfield based on enum debug_messages
 * @param fmt string constant or printf style format string
 *************************************************************************/
void
emitComment (unsigned int level, const char *fmt, ...)
{
  bool print=false;
  va_list ap;

  va_start (ap, fmt);

  if ( level&DBG_MSG )
    {
      if(!(level&VVDBG))
        print=true;
      else if(DBG_MSG&VVDBG)
        print=true;
    }
  if (level==VASM && options.verboseAsm)
    print=true;
  if (level==ALWAYS)
    print=true;
  
  if(print) va_emitcode (";", fmt, ap);
  va_end (ap);
}

/**************************************************************************
 * Returns register state as a string
 *
 * @return
 *************************************************************************/
const char *
regInfoStr()
{
  static char outstr[40];
  char regstring[3][10];
  char *flagreg;

  if(m6502_reg_a->isLitConst) snprintf(regstring[0],10,"A:%c%c:#%02X ",
                                       (m6502_reg_a->isFree)?'-':'U',
                                       (m6502_reg_a->isDead)?'-':'L',
                                       m6502_reg_a->litConst&0xff );
  else if(m6502_reg_a->aop == &tsxaop) snprintf(regstring[0],10,"A:%c%c:S%+-3d",
                                                (m6502_reg_a->isFree)?'-':'U',
                                                (m6502_reg_a->isDead)?'-':'L',
                                                _S.tsxStackPushes - _S.stackPushes );
  else if(m6502_reg_a->aop) snprintf(regstring[0],10,"A:%c%c:A%+-3d",
				     (m6502_reg_a->isFree)?'-':'U',
				     (m6502_reg_a->isDead)?'-':'L',
				     m6502_reg_a->aopofs );
  else snprintf(regstring[0],10,"A:%c%c:??? ",
                (m6502_reg_a->isFree)?'-':'U',
                (m6502_reg_a->isDead)?'-':'L');
  
  if(m6502_reg_x->isLitConst) snprintf(regstring[1],10,"X:%c%c:#%02X ",
                                       (m6502_reg_x->isFree)?'-':'U',
                                       (m6502_reg_x->isDead)?'-':'L',
                                       m6502_reg_x->litConst&0xff  );
  else if(m6502_reg_x->aop == &tsxaop) snprintf(regstring[1],10,"X:%c%c:S%+-3d",
                                                (m6502_reg_x->isFree)?'-':'U',
                                                (m6502_reg_x->isDead)?'-':'L',
                                                _S.tsxStackPushes - _S.stackPushes );
  else if(m6502_reg_x->aop) snprintf(regstring[1],10,"X:%c%c:A%+-3d",
				     (m6502_reg_x->isFree)?'-':'U',
				     (m6502_reg_x->isDead)?'-':'L',
				     m6502_reg_x->aopofs );
  else snprintf(regstring[1],10,"X:%c%c:??? ",
                (m6502_reg_x->isFree)?'-':'U',
                (m6502_reg_x->isDead)?'-':'L');
  
  if(m6502_reg_y->isLitConst) snprintf(regstring[2],10,"Y:%c%c:#%02X ",
                                       (m6502_reg_y->isFree)?'-':'U',
                                       (m6502_reg_y->isDead)?'-':'L',
                                       m6502_reg_y->litConst&0xff );
  else if(m6502_reg_y->aop == &tsxaop) snprintf(regstring[2],10,"Y:%c%c:S%+-3d",
                                                (m6502_reg_y->isFree)?'-':'U',
                                                (m6502_reg_y->isDead)?'-':'L',
                                                _S.tsxStackPushes - _S.stackPushes );
  else if(m6502_reg_y->aop) snprintf(regstring[2],10,"Y:%c%c:A%+-3d",
				     (m6502_reg_y->isFree)?'-':'U',
				     (m6502_reg_y->isDead)?'-':'L',
				     m6502_reg_y->aopofs );
  else snprintf(regstring[2],10,"Y:%c%c:??? ",
                (m6502_reg_y->isFree)?'-':'U',
                (m6502_reg_y->isDead)?'-':'L');
  
  flagreg = (_S.lastflag>=0)?m6502_regWithIdx(_S.lastflag)->name:"?";
  
  char carry = (_S.carryValid)?((_S.carry)?'1':'0'):'?';
  
  snprintf(outstr, 40, "%s %s %s F:%s C:%c",
           regstring[0], regstring[1], regstring[2], flagreg, carry );

  return outstr;
}

/**************************************************************************
 * Returns operand information in the passed string
 *
 * @return
 *************************************************************************/
char *
opInfo(char str[64], operand *op)
{
  int size = 0;
  char *type = "";
  
  if(op)
    {
      if(AOP(op))
        size=AOP_SIZE(op);
      if(AOP(op))
        type=aopName(AOP(op));
    }

  if(op==0)
    {
      snprintf(str, 64, "---");
    }
  else if(IS_SYMOP(op))
    {
      if (snprintf(str, 64, "SYM:%s(%s:%d)", op->svt.symOperand->rname, type, size) >= 64)
        {
	  str[63] = 0; // ridiculous workaround to silence GCC warning ‘%s’ directive output may be truncated
	}
    }
  else if(IS_VALOP(op))
    {
      snprintf(str, 64, "VAL(%s:%d)", type, size);
    }
  else if(IS_TYPOP(op))
    {
      snprintf(str, 64, "TYP");
    }
  else
    {
      snprintf(str, 64, "???");
    }

  return str;
}

/**************************************************************************
 * Prints iCode debug information
 *
 * @return
 *************************************************************************/
void
printIC(iCode *ic)
{
  operand *left, *right, *result;
  char tmpstr[3][64];

  left = IC_LEFT (ic);
  right = IC_RIGHT (ic);
  result = IC_RESULT (ic);

  opInfo(tmpstr[0], result);
  opInfo(tmpstr[1], left);
  opInfo(tmpstr[2], right);

  emitComment (TRACEGEN|VVDBG, "  [%s] = [%s] %s [%s]", tmpstr[0], 
               tmpstr[1], getTableEntry (ic->op)->printName, tmpstr[2]);
}

/**************************************************************************
 * emit6502op - emits opcopdes, updates cost and register state
 *
 * @param inst string containing the opcode
 * @param fmt string operands or printf style format string
 *************************************************************************/
void
emit6502op (const char *inst, const char *fmt, ...)
{
  static char verboseFmt[512];
  va_list ap;
  int isize = 0;
  float cycles = 0;
  float probability=1;

  const m6502opcodedata *opcode = m6502_getOpcodeData(inst);

  if(fmt==0) werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "NULL fmt in emit6502op");


  if(opcode)
    {
      isize = m6502_opcodeSize(opcode, fmt);
      cycles = m6502_opcodeCycles(opcode, fmt);
      reg_info *dst_reg = (opcode->dest>=0)?m6502_regWithIdx(opcode->dest):NULL;

      if(opcode->flags&0x82) // zero and negative flags
        _S.lastflag=opcode->dest;

      if(opcode->flags&0x01)
        {
          // carry flag
          _S.carryValid=0;
        }

      // mark the destination register dirty as necessary
      // transfers are handled in the instruction generator
      switch (opcode->type)
        {
        case M6502OP_LD:
          if(fmt[0]!='#' || !isdigit(fmt[1]))
            m6502_dirtyReg(dst_reg);
          break;
        case M6502OP_REG: // target is accumulator
#if 1
          if(dst_reg && dst_reg->isLitConst &&
             fmt[0]=='#' && isdigit(fmt[1]))
            {
              unsigned char b=strtol(&fmt[1],NULL,0);
              if(!strcmp(inst,"and")) {
                dst_reg->litConst&=b;
                break;
              }
              if(!strcmp(inst,"ora"))
                {
                  dst_reg->litConst|=b;
                  break;
                }
              if(!strcmp(inst,"eor"))
                {
                  dst_reg->litConst^=b;
                  break;
                }
            }
#endif
          if(dst_reg)
            m6502_dirtyReg (dst_reg);
          break;
        case M6502OP_CMP:
          if(!strcmp(fmt,"#0x00"))
            {
              if(inst[2]=='p') _S.lastflag=A_IDX;
              else if(inst[2]=='x') _S.lastflag=X_IDX;
              else if(inst[2]=='y') _S.lastflag=Y_IDX;
              _S.carryValid=1;
              _S.carry=1;
            }
          break;
        case M6502OP_INH:
          if(!strcmp(inst,"clc"))
            {
              _S.carryValid=1;
              _S.carry=0;
              break;
            }
          if(!strcmp(inst,"sec"))
            {
              _S.carryValid=1;
              _S.carry=1;
              break;
            }
          if(!strcmp(inst,"tsx") || !strcmp(inst,"txs") )
            {
              m6502_dirtyReg (m6502_reg_x);
              m6502_reg_x->aop = &tsxaop;
              _S.tsxStackPushes = _S.stackPushes;
              break;
            }
          break;
        case M6502OP_RMW: // target is accumulator
          if (!strcmp(fmt, "a"))
            {
              m6502_dirtyReg (m6502_reg_a);
              _S.lastflag=A_IDX;
            }
          // FIXME: add 65c02 INC/DEC A literal
          break;
        case M6502OP_SPL: // stack pull
          _S.stackPushes--;
          // FIXME: add stack tracking
          m6502_dirtyReg(m6502_reg_a);
          break;
        case M6502OP_SPH: // stack push
          _S.stackPushes++;
          break;
        case M6502OP_IDD: // index decrement
          if(dst_reg->isLitConst)
            dst_reg->litConst--;
          else if(dst_reg->aop==&tsxaop)
            _S.tsxStackPushes++;
          else
            m6502_dirtyReg(dst_reg);
          break;
        case M6502OP_IDI: // index increment
          if(dst_reg->isLitConst)
            dst_reg->litConst++;
          else if(dst_reg->aop==&tsxaop)
            _S.tsxStackPushes--;
          else
            m6502_dirtyReg(dst_reg);
          break;
        case M6502OP_BR: // add penalty for taken branches
          // this assumes:
          // 50% not taken (2 cycles)
          // 40% taken with target in the same page (3 cycles)
          // 10% taken with target in a different page (4 cycles)
          cycles += (0.4 * 1) + (0.1 * 2);
          break;
        case M6502OP_ST:
        case M6502OP_JMP:
        case M6502OP_BBR:
          break;
        }
    }
  else
    {
      emitcode("ERROR","Unimplemented opcode %s", inst);
      isize=10;
      //werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "NULL opcode in emit6502op");
    }
  
  regalloc_dry_run_cost_bytes += isize;
  regalloc_dry_run_cost_cycles += cycles * probability;

  va_start (ap, fmt);
  if (options.verboseAsm)
    {
      char dstring[3][64];
      dstring[0][0]=0;
      dstring[1][0]=0;
      dstring[2][0]=0;
    
      if (DBG_MSG&COST)
        {
          snprintf(dstring[0], 64, " sz=%d cl=%f p=%f",
                   isize, cycles, probability);
        }
    
      if (DBG_MSG&REGALLOC)
        {
          snprintf(dstring[1], 64, " %s",
                   regInfoStr() );
        }
      if (DBG_MSG&TRACE_STACK)
        {
          snprintf(dstring[2], 64, " stkpush=%d",
                   _S.stackPushes );
        }

      // FIXME: figure out how to align the comments in the asm output
      snprintf(verboseFmt, 512, "%s \t;%s%s%s",
               fmt, dstring[0], dstring[1], dstring[2]);
      va_emitcode (inst, verboseFmt, ap);
    }
  else
    {
      va_emitcode (inst, fmt, ap);
    }
  va_end (ap);
}

/**************************************************************************
 * m6502_unimplemented
 *
 *************************************************************************/
static void
m6502_unimplemented(const char *msg)
{
  emitcode("ERROR","Unimplemented %s", msg);
#ifndef DEBUG_UNIMPLEMENTED
  regalloc_dry_run_cost_bytes  += 500;
  regalloc_dry_run_cost_cycles += 500;
#else
  regalloc_dry_run_cost_bytes  = 0;
  regalloc_dry_run_cost_cycles = 0;
#endif
}

/**************************************************************************
 * emitSignedBranch
 *
 *************************************************************************/
void
emitSignedBranch (bool gt, bool eq, symbol * tlbl)
{
  symbol *tlbl2 = safeNewiTempLabel (NULL);
  symbol *tlbl3 = safeNewiTempLabel (NULL);

  if (eq && !gt)
    emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
  if (!eq && gt)
    emit6502op ("beq", "%05d$", safeLabelNum (tlbl2));
  emit6502op (gt ? "bvs" : "bvc", "%05d$", safeLabelNum (tlbl2));
  emit6502op ("bpl", "%05d$", safeLabelNum (tlbl));
  emit6502op ("bmi", "%05d$", safeLabelNum (tlbl3));
  safeEmitLabel (tlbl2);
  emit6502op ("bmi", "%05d$", safeLabelNum (tlbl));
  safeEmitLabel (tlbl3);
}

/**************************************************************************
 * emitUnsignedBranch
 *
 *************************************************************************/
static void
emitUnsignedBranch (bool gt, bool eq, symbol * tlbl)
{
  symbol *tlbl2 = safeNewiTempLabel (NULL);
  
  if (eq && !gt)
    emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
  if (!eq && gt)
    emit6502op ("beq", "%05d$", safeLabelNum (tlbl2));
  emit6502op (gt ? "bcs" : "bcc", "%05d$", safeLabelNum (tlbl));
  safeEmitLabel (tlbl2);
}

/**************************************************************************
 * emitBranch
 *
 *************************************************************************/
void
emitBranch (char *branchop, symbol * tlbl)
{
  if (!strcmp("bls", branchop))
    {
      emitUnsignedBranch(0, 1, tlbl);
    }
  else if (!strcmp("bhi", branchop))
    {
      emitUnsignedBranch(1, 0, tlbl);
    }
  else if (!strcmp("blt", branchop))
    {
      emitSignedBranch(0, 0, tlbl);
    }
  else if (!strcmp("bgt", branchop))
    {
      emitSignedBranch(1, 0, tlbl);
    }
  else if (!strcmp("ble", branchop))
    {
      emitSignedBranch(0, 1, tlbl);
    }
  else if (!strcmp("bge", branchop))
    {
      emitSignedBranch(1, 1, tlbl);
    }
  else
    {
      if (!IS_MOS65C02 && !strcmp(branchop, "bra"))
        branchop = "jmp";
      emit6502op (branchop, "%05d$", safeLabelNum (tlbl));
    }
}

/**************************************************************************
 * emitSetCarry - emit CLC/SEC if necessary
 *
 * @param c carry value to set
 *************************************************************************/
void
emitSetCarry(int c)
{
  if(_S.carryValid && _S.carry==c)
    return;
  if(c)
    emit6502op("sec", "");
  else
    emit6502op ("clc", "");
}

/**************************************************************************
 * emitCmp - emit CMP/CPX/CPY with immediate if necessary
 *
 * @param reg pointer to register
 * @param val immediate to compare with
 * @return true if the instruction was necessary 
 *************************************************************************/
bool
emitCmp (reg_info *reg, unsigned char v)
{
  if(!reg)
    emitcode ("ERROR", "  %s - reg is NULL", __func__);

  if(v==0 && _S.lastflag==reg->rIdx)
    return false;

  switch(reg->rIdx)
    {
    case A_IDX:
      emit6502op("cmp", "#0x%02x", v);
      break;
    case X_IDX:
      emit6502op("cpx", "#0x%02x", v);
      break;
    case Y_IDX:
      emit6502op("cpy", "#0x%02x", v);
      break;
    default:
      emitcode ("ERROR", " %s - illegal %d reg_idx", __func__, reg->rIdx);
      break;
    }
  return true;
}

/**************************************************************************
 * Adjust register by n bytes if possible.
 *
 * @param reg pointer to the register to adjust
 * @param n amount of adjustment (can be positive or negative)
 * @return return true if the ajdust was performed
 *************************************************************************/
bool
smallAdjustReg (reg_info *reg, int n)
{
  emitComment (REGOPS, __func__ );

  if(n==0)
    return true;

  if( (reg!=m6502_reg_x) && (reg!=m6502_reg_y) && !IS_MOS65C02)
    return false;
  
  if (n <= -4 || n >= 4)
    {
      return false;
    }

  while (n < 0)
    {
      rmwWithReg ("dec", reg); /* 1 byte,  2 cycles */
      n++;
    }
  while (n > 0)
    {
      rmwWithReg ("inc", reg); /* 1 byte,  2 cycles */
      n--;
    }
  return true;
}

/**************************************************************************
 * Associate the current code location with a debugger symbol
 *************************************************************************/
void
m6502_emitDebuggerSymbol (const char *debugSym)
{
  genLine.lineElement.isDebug = 1;
  emitcode ("", "%s ==.", debugSym);
  genLine.lineElement.isDebug = 0;
}

/**************************************************************************
 * Transfer from register(s) sreg to register(s) dreg.
 * If freesrc is true, sreg is marked free and available for reuse.
 * sreg and dreg must be of equal size
 *
 * @param sreg pointer to the source register
 * @param dreg pointer to the destination register
 * @param freesrc free the source register if true
 *************************************************************************/
void
transferRegReg (reg_info *sreg, reg_info *dreg, bool freesrc)
{
  int srcidx;
  int dstidx;
  char error = 0;
  
  /* Nothing to do if no destination. */
  if (!dreg)
    return;
  
  /* But it's definitely an error if there's no source. */
  if (!sreg)
    {
      //     emitcode("ERROR","%s: src reg is null", __func__);
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "NULL sreg in transferRegReg");
      return;
    }

  emitComment (REGOPS, "  %s(%s,%s)", __func__, sreg->name, dreg->name);
  emitComment (REGOPS, "  %s %s", __func__, regInfoStr() );

  srcidx = sreg->rIdx;
  dstidx = dreg->rIdx;

  if (srcidx == dstidx)
    {
      emitComment (REGOPS|VVDBG, "  %s: sameregs", __func__);
      m6502_useReg (dreg);
      return;
    }

  // TODO: make sure regs are killed if clobbered
  switch (dstidx)
    {
    case A_IDX:
      switch (srcidx) {
      case Y_IDX:            /* Y to A */
        emit6502op ("tya", "");
        break;
      case X_IDX:            /* X to A */
        emit6502op ("txa", "");
        break;
      default:
        error = 1;
      }
      break;
    case X_IDX:
      switch (srcidx)
        {
        case A_IDX:            /* A to X */
          emit6502op ("tax", "");
          break;
        case Y_IDX:            /* Y to X */
          if(m6502_reg_y->isLitConst) {
            loadRegFromConst (m6502_reg_x, m6502_reg_y->litConst);
          } else if(m6502_reg_a->isFree) {
            emit6502op ("tya", "");
            emit6502op ("tax", "");
          } else {
            if (IS_MOS65C02) {
              emit6502op ("phy", "");
              emit6502op ("plx", "");
            } else {
              storeRegTemp (m6502_reg_y, false);
              loadRegTemp (m6502_reg_x);
            }
          }
          break;
        default:
          error = 1;
        }
      break;
    case Y_IDX:
      switch (srcidx)
        {
        case A_IDX:            /* A to Y */
          emit6502op ("tay", "");
          break;
        case X_IDX:            /* X to Y */
          if(m6502_reg_x->isLitConst) {
            loadRegFromConst (m6502_reg_y, m6502_reg_x->litConst);
          }
          else if(m6502_reg_a->isFree)
            {
              emit6502op ("txa", "");
              emit6502op ("tay", "");
            }
          else
            {
              if (IS_MOS65C02)
                {
                  emit6502op ("phx", "");
                  emit6502op ("ply", "");
                }
              else
                {
                  storeRegTemp (m6502_reg_x, false);
                  loadRegTemp (m6502_reg_y);
                }
            }
          break;
        default:
          error = 1;
        }
      break;
    case XA_IDX:
      switch (srcidx)
        {
        case YX_IDX:           /* YX to XA */
          transferRegReg(m6502_reg_x, m6502_reg_a, true);
          storeRegTemp (m6502_reg_y, true);
          loadRegTemp (m6502_reg_x);
          break;
        default:
          error = 1;
        }
      break;
    case YX_IDX:
      switch (srcidx)
        {
        case XA_IDX:           /* XA to YX */
          storeRegTemp (m6502_reg_x, false);
          transferRegReg(m6502_reg_a, m6502_reg_x, true);
          loadRegTemp (m6502_reg_y);
          break;
        default:
          error = 1;
        }
      break;
    default:
      error = 1;
    }
  
  if(error)
    emitcode("ERROR", "bad combo in transferRegReg 0x%02x -> 0x%02x", srcidx, dstidx);

  m6502_dirtyReg (dreg);
  m6502_useReg (dreg);

  if(sreg->isLitConst)
    {
      dreg->isLitConst = sreg->isLitConst;
      dreg->litConst = sreg->litConst;
    }
  else if(sreg->aop)
    {
      regTrackAop(dreg, sreg->aop, sreg->aopofs);
    }

  emitComment (REGOPS, "  %s %s", __func__, regInfoStr() );
  if (freesrc)
    m6502_freeReg (sreg);
}

/**************************************************************************
 * updateCFA - update the debugger information to reflect the current
 *             canonical frame address relative to the stack pointer
 *************************************************************************/
void
updateCFA (void)
{
  /* there is no frame unless there is a function */
  if (!currFunc)
    return;
  
  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, m6502_reg_sp, 1 + _S.stackOfs + _S.stackPushes);
}

/**************************************************************************
 * Return a string with debugging information about an asmop.
 *************************************************************************/
static char *
aopName (asmop * aop)
{
  static char buffer[276];
  char *buf = buffer;

  if (!aop)
    return "(asmop*)NULL";

  switch (aop->type)
    {
    case AOP_IMMD:
      sprintf (buf, "IMMD(%s)", aop->aopu.aop_immd);
      return buf;
    case AOP_LIT:
      sprintf (buf, "LIT(%s)", aopLiteral (aop->aopu.aop_lit, 0));
      return buf;
    case AOP_DIR:
      sprintf (buf, "DIR(%s)", aop->aopu.aop_dir);
      return buf;
    case AOP_EXT:
      sprintf (buf, "EXT(%s)", aop->aopu.aop_dir);
      return buf;
    case AOP_SOF:
      sprintf (buf, "SOF(%s@%d)", OP_SYMBOL (aop->op)->name, aop->aopu.aop_stk);
      return buf;
    case AOP_REG:
      sprintf (buf, "REG(%s%s%s%s)",
               aop->aopu.aop_reg[3] ? aop->aopu.aop_reg[3]->name : "",
               aop->aopu.aop_reg[2] ? aop->aopu.aop_reg[2]->name : "",
               aop->aopu.aop_reg[1] ? aop->aopu.aop_reg[1]->name : "",
               aop->aopu.aop_reg[0] ? aop->aopu.aop_reg[0]->name : "-");
      return buf;
    case AOP_STK:
      return "STK";
    default:
      sprintf (buf, "?%d", aop->type);
      return buf;
    }

  return "?";
}

// can we BIT aop ?
bool
canBitOp (const operand* aop)
{
  switch (AOP_TYPE(aop))
    {
      // bit aa, bit aaaa
    case AOP_DIR:
    case AOP_EXT:
      return true;
      // bit #aa
    case AOP_LIT:
      return IS_MOS65C02;
      // TODO: ind,x for 65c02?
    default:
      break;
    }
  return false;
}

/**************************************************************************
 * Load register reg from logical offset loffset of aop.
 * For multi-byte registers, loffset is of the lsb reg.
 *************************************************************************/
void
loadRegFromAop (reg_info * reg, asmop * aop, int loffset)
{
  int regidx = reg->rIdx;

  emitComment (REGOPS, "      loadRegFromAop (%s, %s, %d)", reg->name, aopName (aop), loffset);
  
  if (aop->stacked && aop->stk_aop[loffset])
    {
      loadRegFromAop (reg, aop->stk_aop[loffset], 0);
      return;
    }

  /* If operand is volatile, we cannot optimize. */
  if (!aop->op || isOperandVolatile (aop->op, false))
    goto forceload;

  /* If this register already has this offset of the operand
     then we need only mark it as in use. */
  if (reg->aop && reg->aop->op && aop->op && operandsEqu (reg->aop->op, aop->op) && (reg->aopofs == loffset))
    {
      m6502_useReg (reg);
      emitComment (REGOPS, "  already had correct value for %s", reg->name);
      return;
    }

  /* check to see if we can transfer from another register */
  if(reg!=m6502_reg_xa)
    {
      reg_info *srcreg=findRegAop(aop, loffset);
      if(srcreg)
	{
	  emitComment (REGOPS, "  found correct value for %s in %s", reg->name, srcreg->name);
	  transferRegReg (srcreg, reg, false);
	  m6502_useReg (reg);
	  return;
	}
    }

 forceload:
  switch (regidx) {
  case A_IDX:
  case X_IDX:
  case Y_IDX:
    if (aop->type == AOP_REG)
      {
        if (loffset < aop->size)
          transferRegReg (aop->aopu.aop_reg[loffset], reg, false);
        else
          loadRegFromConst (reg, 0); /* TODO: handle sign extension */
      }
    else if (aop->type == AOP_LIT)
      {
        loadRegFromConst (reg, byteOfVal (aop->aopu.aop_lit, loffset));
      }
    else if (aop->type == AOP_SOF && regidx != A_IDX)
      {
        // TODO: add support for ldy aaaa,x
        bool needloada = storeRegTempIfUsed(m6502_reg_a); // FIXME: maybe push?
        loadRegFromAop(m6502_reg_a, aop, loffset);
        transferRegReg(m6502_reg_a, reg, false);
        loadOrFreeRegTemp(m6502_reg_a,needloada);
      }
    else
      {
        if(aop->type == AOP_SOF)
          emitComment (TRACE_STACK|VVDBG, "      loadRegFromAop: A [%d, %d]",aop->aopu.aop_stk, loffset);
        aopAdrPrepare(aop, loffset);
        const char *l = aopAdrStr (aop, loffset, false);
        emit6502op (regidx == A_IDX ? "lda" : regidx == X_IDX ? "ldx" : "ldy", l);
        aopAdrUnprepare(aop, loffset);
        m6502_dirtyReg (reg);
        if( !isOperandVolatile (aop->op, false))
          {
	    regTrackAop(reg, aop, loffset);
          }
      }
    break;
  case XA_IDX:
    emitComment (REGOPS, "  %s - XA", __func__);
    if (IS_AOP_XA (aop))
      break;
    else if (IS_AOP_YX (aop))
      transferRegReg (m6502_reg_yx, m6502_reg_xa, false);
    else if(IS_AOP_X(aop))
      {
	transferRegReg (m6502_reg_x, m6502_reg_a, false);
	loadRegFromConst (m6502_reg_x, 0);
      }
    else if(aop->type == AOP_SOF)
      {
	bool use_y = (m6502_reg_y->isFree && m6502_reg_y->isDead);
	m6502_freeReg(m6502_reg_x);
	loadRegFromAop (m6502_reg_a, aop, loffset);
	if(aop->size >= 2)
	  {
	    if(use_y)
	      transferRegReg (m6502_reg_a, m6502_reg_y, true);
	    else
	      storeRegTemp(m6502_reg_a, true);

	    loadRegFromAop (m6502_reg_a, aop, loffset + 1);
	    transferRegReg(m6502_reg_a, m6502_reg_x, true);

	    if(use_y)
	      transferRegReg (m6502_reg_y, m6502_reg_a, true);
	    else
	      loadRegTemp(m6502_reg_a);
	  }
	if(aop->size == 1)
	  loadRegFromConst (m6502_reg_x, 0);
      }
    else
      {
	if(aop->size == 1)
	  loadRegFromConst (m6502_reg_x, 0);
	else
	  loadRegFromAop (m6502_reg_x, aop, loffset + 1);

	loadRegFromAop (m6502_reg_a, aop, loffset);
      }
    break;
  case YX_IDX:
    if (IS_AOP_YX (aop))
      break;
    else if (IS_AOP_XA (aop))
      transferRegReg (m6502_reg_xa, m6502_reg_yx, false);
    else
      {
	loadRegFromAop (m6502_reg_x, aop, loffset);
	loadRegFromAop (m6502_reg_y, aop, loffset + 1);
      }
    break;
  }

  m6502_useReg (reg);
}

/**************************************************************************
 * Find a free index register
 *
 * @return pointer to reg_info or NULL if no index register is available
 *************************************************************************/
reg_info*
getFreeIdxReg()
{
  // TODO: add reentrant and stack auto
  //if (m6502_reg_y->isFree && !m6502_reg_y->isLitConst)
  //   return m6502_reg_y;
  //  else
  if (m6502_reg_x->isFree && m6502_reg_x->aop!=&tsxaop)
    return m6502_reg_x;
  else if (m6502_reg_y->isFree)
    return m6502_reg_y;
  else if(m6502_reg_x->isFree)
    return m6502_reg_x;

  return NULL;
}

/**************************************************************************
 * Find any free 8-bit register
 *
 * @return pointer to reg_info or NULL if no register is available
 *************************************************************************/
reg_info*
getFreeByteReg()
{
  if (m6502_reg_a->isFree)
    return m6502_reg_a;
  else
    return getFreeIdxReg();
}

// TODO: move more to this one?
reg_info*
getDeadByteReg()
{
  if (m6502_reg_a->isDead)
    return m6502_reg_a;
  else if (m6502_reg_y->isDead)
    return m6502_reg_y;
  else if (m6502_reg_x->isDead)
    return m6502_reg_x;
  else
    return NULL;
}

/**************************************************************************
 * storeRegToAop - Store register reg to logical offset loffset of aop.
 *                 For multi-byte registers, loffset is of the lsb reg.
 *************************************************************************/
void
storeRegToAop (reg_info *reg, asmop * aop, int loffset)
{
  bool needloada = false;
  bool needloadx = false;
  int regidx = reg->rIdx;

  emitComment (TRACE_AOP, "      %s (%s, %s, %d), stacked=%d",
               __func__, reg->name, aopName (aop), loffset, aop->stacked);

  if (aop->type == AOP_DUMMY)
    return;

  if (aop->type == AOP_CRY)     /* This can only happen if IFX was optimized */
    return;                     /* away, so just toss the result */

  if (regidx==XA_IDX && aop->size == 1 )
    {
      storeRegToAop (m6502_reg_a, aop, loffset);
      return;
    }

  if (aop->type == AOP_REG)
    {
      // handle reg to reg
      switch (regidx)
        {
        case A_IDX:
        case X_IDX:
        case Y_IDX:
          transferRegReg (reg, aop->aopu.aop_reg[loffset], true);
          break;
        case XA_IDX:
          if (IS_AOP_YX (aop))
            {
              transferRegReg (reg, m6502_reg_yx, false);
            }
          else
            {
              if(!IS_AOP_XA(aop))
		emitcode("ERROR", "%s: unsupported reg in AOP (XA)", __func__);
            }
          break;
        case YX_IDX:
          if (IS_AOP_XA (aop))
            {
              transferRegReg (reg, m6502_reg_xa, false);
            }
          else
            {
              if(!IS_AOP_YX(aop))
		emitcode("ERROR", "%s: unsupported reg in AOP (YX)", __func__);
            }
          break;
        }
      return;
    }

  if (aop->type == AOP_DIR || aop->type == AOP_EXT)
    {
      // handle ZP and absolute addresses
      switch (regidx)
        {
        case A_IDX:
          emit6502op ("sta", aopAdrStr (aop, loffset, true));
          break;
        case X_IDX:
          emit6502op ("stx", aopAdrStr (aop, loffset, true));
          break;
        case Y_IDX:
          emit6502op ("sty", aopAdrStr (aop, loffset, true));
          break;
        case XA_IDX:
          emit6502op ("sta", aopAdrStr (aop, loffset, true));
          emit6502op ("stx", aopAdrStr (aop, loffset+1, true));
          break;
        case YX_IDX:
          emit6502op ("stx", aopAdrStr (aop, loffset, true));
          emit6502op ("sty", aopAdrStr (aop, loffset+1, true));
          break;
        }
    }
  else if (aop->type == AOP_SOF)
    {
      // handle stack
      //    int xofs = STACK_TOP + _S.stackOfs + _S.tsxStackPushes + aop->aopu.aop_stk + loffset + 1;

      switch (regidx)
        {
        case A_IDX:
          emitComment (TRACE_STACK|VVDBG, "      %s: A [%d, %d]",
                       __func__, aop->aopu.aop_stk, loffset);
          if(m6502_reg_x->aop != &tsxaop)
            {
              needloadx = storeRegTempIfUsed (m6502_reg_x);
              emitTSX ();
            }
          emit6502op ("sta", aopAdrStr (aop, loffset, false));
          m6502_freeReg (m6502_reg_a);
          loadOrFreeRegTemp (m6502_reg_x, needloadx);
          break;
        case X_IDX:
        case Y_IDX:
          // TODO: push if live
          needloada = storeRegTempIfUsed (m6502_reg_a);
          transferRegReg (reg, m6502_reg_a, false);
          storeRegToAop (m6502_reg_a, aop, loffset);
          loadOrFreeRegTemp (m6502_reg_a, needloada);
          break;
        case XA_IDX:
          emitComment (REGOPS, "      %s - XA", __func__);
          // options.stackAuto 
          //        pushReg(m6502_reg_a, true);
          needloadx = storeRegTempIfUsed (m6502_reg_x);
          storeRegTemp (m6502_reg_a, false);
          transferRegReg (m6502_reg_x, m6502_reg_a, true);
          emitTSX();
	  emit6502op ("sta", aopAdrStr (aop, loffset + 1, false));
          //        pullReg(m6502_reg_a);
          loadRegTemp(m6502_reg_a);
	  emit6502op ("sta", aopAdrStr (aop, loffset, false));
          loadOrFreeRegTemp(m6502_reg_x, needloadx);
          break;
        case YX_IDX:
          needloada = storeRegTempIfUsed (m6502_reg_a);
          needloadx = storeRegTempIfUsed (m6502_reg_x);
          transferRegReg (m6502_reg_x, m6502_reg_a, true);
          emitTSX();
          emit6502op ("sta", aopAdrStr (aop, loffset, false));
          transferRegReg (m6502_reg_y, m6502_reg_a, true);
          emit6502op ("sta", aopAdrStr (aop, loffset + 1, false));
          loadOrFreeRegTemp (m6502_reg_x, needloadx);
          loadOrFreeRegTemp (m6502_reg_a, needloada);
          break;
        default:
          emitcode("ERROR", "%s: bad reg 0x%02x", __func__, regidx);
        }
    }

  if(!reg->isLitConst)
    {
      //      emitComment (ALWAYS /*TRACE_AOP|VVDBG*/, " %s - looking for stale reg", __func__);
      //      emitComment (ALWAYS /*TRACE_AOP|VVDBG*/, " %s - reg_a->aop=%08x aop=%08x aop->op=%08x", 
      //                   __func__, m6502_reg_a->aop, aop, aop->op);
      dirtyRegAop(reg, aop, loffset);
      regTrackAop(reg, aop, loffset);
    }
}

/**************************************************************************
 * loadRegFromConst - Load register reg from constant c.
 *************************************************************************/
void
loadRegFromConst (reg_info * reg, int c)
{
  emitComment (REGOPS, __func__ );

  switch (reg->rIdx) {
  case A_IDX:
    c &= 0xff;
    if (reg->isLitConst && reg->litConst == c)
      break;

    if (m6502_reg_y->isLitConst && m6502_reg_y->litConst == c)
      transferRegReg (m6502_reg_y, reg, false);
    else if (m6502_reg_x->isLitConst && m6502_reg_x->litConst == c)
      transferRegReg (m6502_reg_x, reg, false);
    else {
      emit6502op ("lda", IMMDFMT, (unsigned int)c);
    }
    break;
  case X_IDX:
    c &= 0xff;
    if (reg->isLitConst) {
      if (reg->litConst == c)
        break;
      if (((reg->litConst + 1) & 0xff) == c)
        {
          emit6502op ("inx", "");
          break;
        }
      if (((reg->litConst - 1) & 0xff) == c)
        {
          emit6502op ("dex", "");
          break;
        }
    }

    if (m6502_reg_a->isLitConst && m6502_reg_a->litConst == c)
      transferRegReg (m6502_reg_a, reg, false);
    /*
      TODO does not work for X<->Y
      else if (m6502_reg_y->isLitConst && m6502_reg_y->litConst == c)
      transferRegReg (m6502_reg_y, reg, false);
    */
    else
      {
        emit6502op ("ldx", IMMDFMT, (unsigned int)c);
      }
    break;
  case Y_IDX:
    c &= 0xff;
    if (reg->isLitConst)
      {
        if (reg->litConst == c)
          break;
        if (((reg->litConst + 1) & 0xff) == c)
          {
            emit6502op ("iny", "");
            break;
          }
        if (((reg->litConst - 1) & 0xff) == c)
          {
            emit6502op ("dey", "");
            break;
          }
      }

    if (m6502_reg_a->isLitConst && m6502_reg_a->litConst == c)
      transferRegReg (m6502_reg_a, reg, false);
    /*
      TODO does not work for X<->Y
      else if (m6502_reg_x->isLitConst && m6502_reg_x->litConst == c)
      transferRegReg (m6502_reg_x, reg, false);
    */
    else
      {
        emit6502op ("ldy", IMMDFMT, (unsigned int)c);
      }
    break;
  case XA_IDX:
    c &= 0xffff;
    loadRegFromConst (m6502_reg_x, c >> 8);
    loadRegFromConst (m6502_reg_a, c);
    break;
  case YX_IDX:
    c &= 0xffff;
    loadRegFromConst (m6502_reg_y, c >> 8);
    loadRegFromConst (m6502_reg_x, c);
    break;
  default:
    emitcode ("ERROR", "bad reg 0x%02x in %s", reg->rIdx, __func__);
    return;
  }

  m6502_dirtyReg (reg);
  reg->isLitConst = 1;
  reg->litConst = c;

  m6502_useReg (reg);
}

/**************************************************************************
 * loadRegFromImm - Load register reg from immediate value c.
 *************************************************************************/
static void
loadRegFromImm (reg_info * reg, char * c)
{
  emitComment (REGOPS, __func__ );

  if(!c) {
    werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "loadRegFromImm called with a null arg pointer");
  }

  if (*c == '#')
    c++;
  switch (reg->rIdx) {
  case A_IDX:
    emit6502op ("lda", "#%s", c);
    break;
  case X_IDX:
    emit6502op ("ldx", "#%s", c);
    break;
  case Y_IDX:
    emit6502op ("ldy", "#%s", c);
    break;
  case XA_IDX:
    emit6502op ("ldx", "#%s >> 8", c);
    emit6502op ("lda", "#%s", c);
    break;
  case YX_IDX:
    emit6502op ("ldy", "#%s >> 8", c);
    emit6502op ("ldx", "#%s", c);
    break;
  default:
    werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "Bad rIdx in loadRegFromConst");
    return;
  }
  m6502_dirtyReg (reg);
  m6502_useReg (reg);
}

/**************************************************************************
 * storeConstToAop - Store constant c to logical offset loffset of 
 *                   asmop aop.
 *************************************************************************/
void
storeConstToAop (int c, asmop * aop, int loffset)
{
  emitComment (REGOPS, __func__ );

  if (aop->stacked && aop->stk_aop[loffset])
    {
      storeConstToAop (c, aop->stk_aop[loffset], 0);
      return;
    }

  /* If the value needed is already in A, X or Y just store it */
  if (m6502_reg_a->isLitConst && m6502_reg_a->litConst == c)
    {
      storeRegToAop (m6502_reg_a, aop, loffset);
      return;
    }
  if (m6502_reg_x->isLitConst && m6502_reg_x->litConst == c)
    {
      storeRegToAop (m6502_reg_x, aop, loffset);
      return;
    }
  if (m6502_reg_y->isLitConst && m6502_reg_y->litConst == c)
    {
      storeRegToAop (m6502_reg_y, aop, loffset);
      return;
    }

  switch (aop->type)
    {
    case AOP_REG:
      if (loffset > (aop->size - 1))
        break;
      loadRegFromConst (aop->aopu.aop_reg[loffset], c);
      break;
    case AOP_DUMMY:
      break;
    case AOP_DIR:
    case AOP_EXT:
      /* stz operates with read-modify-write cycles, so don't use if the */
      /* destination is volatile to avoid the read side-effect. */
      if (c==0 && IS_MOS65C02 && !(aop->op && isOperandVolatile (aop->op, false)))
        {
          emit6502op ("stz", "%s", aopAdrStr (aop, loffset, false));
          break;
        }
    default:
      if(aop->type != AOP_SOF)
        {
          // prefer X if literal!=0 && X does not contain tsx offset 
          if(c!=0 && m6502_reg_x->isFree && m6502_reg_x->aop != &tsxaop)
            {
              loadRegFromConst (m6502_reg_x, c);
              storeRegToAop (m6502_reg_x, aop, loffset);
              m6502_freeReg (m6502_reg_x);
              return;
            }
          else if(m6502_reg_y->isFree)
            {
              loadRegFromConst (m6502_reg_y, c);
              storeRegToAop (m6502_reg_y, aop, loffset);
              m6502_freeReg (m6502_reg_y);
              return;
            }
        }
      bool needpulla = pushRegIfUsed (m6502_reg_a);
      // bool needpulla = storeRegTempIfUsed (m6502_reg_a);
      loadRegFromConst (m6502_reg_a, c);
      storeRegToAop (m6502_reg_a, aop, loffset);
      // loadOrFreeRegTemp (m6502_reg_a, needpulla);
      pullOrFreeReg (m6502_reg_a, needpulla);
    }
}

/**************************************************************************
 * storeImmToAop - Store immediate value c to logical offset
 *                 loffset of asmop aop.
 *************************************************************************/
static void
storeImmToAop (char *c, asmop * aop, int loffset)
{
  emitComment (TRACE_AOP, __func__ );

  if (aop->stacked && aop->stk_aop[loffset])
    {
      storeImmToAop (c, aop->stk_aop[loffset], 0);
      return;
    }

  switch (aop->type) {
  case AOP_REG:
    if (loffset > (aop->size - 1))
      break;
    loadRegFromImm (aop->aopu.aop_reg[loffset], c);
    break;
  case AOP_DUMMY:
    break;
  case AOP_DIR:
    if (!strcmp (c, "#0x00") && IS_MOS65C02 )
      {
        emit6502op ("stz", "%s", aopAdrStr (aop, loffset, false));
        break;
      }
  default:
    if (m6502_reg_x->isFree)
      {
        loadRegFromImm (m6502_reg_x, c);
        storeRegToAop (m6502_reg_x, aop, loffset);
        m6502_freeReg (m6502_reg_x);
      }
    else if (m6502_reg_y->isFree)
      {
	loadRegFromImm (m6502_reg_y, c);
	storeRegToAop (m6502_reg_y, aop, loffset);
	m6502_freeReg (m6502_reg_y);
      }
    else
      {
	bool needpulla = pushRegIfUsed (m6502_reg_a);
	loadRegFromImm (m6502_reg_a, c);
	storeRegToAop (m6502_reg_a, aop, loffset);
	pullOrFreeReg (m6502_reg_a, needpulla);
      }
  }
}

void
signExtendA ()
{
  emit6502op ("asl", "a");
  loadRegFromConst (m6502_reg_a, 0);
  emit6502op ("adc", "#0xff");
  emit6502op ("eor", "#0xff");
}

/**************************************************************************
 * storeRegSignToUpperAop - If isSigned is true, the sign bit of register
 *                          reg is extended to fill logical offsets loffset
 *                          and above of asmop aop. Otherwise, logical
 *                          offsets loffset and above of asmop aop are
 *                          zeroed. reg must be an 8-bit register.
 *************************************************************************/
static void
storeRegSignToUpperAop (reg_info * reg, asmop * aop, int loffset, bool isSigned)
{
  emitComment (TRACE_AOP, __func__ );

  //  int regidx = reg->rIdx;
  int size = aop->size;

  if (loffset >= size)
    return;

  if (!isSigned)
    {
      /* Unsigned case */
      while (loffset < size)
        storeConstToAop (0, aop, loffset++);
    }
  else
    {
      /* Signed case */
      if(reg!=m6502_reg_a)
        {
          symbol *tlbl = safeNewiTempLabel (NULL);
          if(reg==m6502_reg_x)
            emit6502op("cpx","#0x80");
          else
            emit6502op("cpy","#0x80");

          loadRegFromConst (reg, 0);
          emitBranch ("bcc", tlbl);
          loadRegFromConst (reg, 0xff);
          safeEmitLabel (tlbl);
          m6502_dirtyReg (reg);
        }
      else
        {
          signExtendA ();
          m6502_useReg (m6502_reg_a);
        }
      while (loffset < size)
        storeRegToAop (reg, aop, loffset++);
      m6502_freeReg (reg);
    }
}

/**************************************************************************
 * storeRegToFullAop - Store register reg to asmop aop with appropriate
 *                     padding and/or truncation as needed. If isSigned is
 *                     true, sign extension will take place in the padding.
 *************************************************************************/
void
storeRegToFullAop (reg_info *reg, asmop *aop, bool isSigned)
{
  int regidx = reg->rIdx;
  int size = aop->size;

  emitComment (TRACE_AOP, __func__ );

  switch (regidx)
    {
    case A_IDX:
    case X_IDX:
    case Y_IDX:
#if 0
      // FIXME: this optimization reduce code size but increase runtime
      // should conditionally add it when optmizing for size
      if ( IS_AOP_XA(aop) && regidx==A_IDX )
        {
          loadRegFromConst(m6502_reg_x,0);
          if (isSigned)
            {
              symbol *tlbl = safeNewiTempLabel (NULL);
              emit6502op ("cmp","#0x80");
              emitBranch ("bcc", tlbl);
              loadRegFromConst (m6502_reg_x, 0xff);
              safeEmitLabel (tlbl);
              m6502_dirtyReg (m6502_reg_x);
            }
        }
      else
#endif
        { 
          storeRegToAop (reg, aop, 0);
          if (size > 1 && isSigned && aop->type == AOP_REG && aop->aopu.aop_reg[0]->rIdx == A_IDX)
            pushReg (m6502_reg_a, true);
          storeRegSignToUpperAop (reg, aop, 1, isSigned);
          if (size > 1 && isSigned && aop->type == AOP_REG && aop->aopu.aop_reg[0]->rIdx == A_IDX)
            pullReg (m6502_reg_a);
        }
      break;
    case XA_IDX:
      if (size == 1)
	{
	  storeRegToAop (m6502_reg_a, aop, 0);
	}
      else
	{
	  storeRegToAop (reg, aop, 0);
	  if (size>2)
	    {
	      if (aop->type!=AOP_SOF)
		{
		  storeRegSignToUpperAop (m6502_reg_x, aop, 2, isSigned);
		}
	      else
		{
		  storeRegTemp (m6502_reg_a, true);
		  loadRegFromAop (m6502_reg_a, aop, 1);
		  storeRegSignToUpperAop (m6502_reg_a, aop, 2, isSigned);
		  loadRegTemp (m6502_reg_a);
		}
	    }
	}
      break;
    case YX_IDX:
      if (size == 1)
	{
	  storeRegToAop (m6502_reg_x, aop, 0);
	}
      else
	{
	  storeRegToAop (reg, aop, 0);
	  storeRegSignToUpperAop (m6502_reg_y, aop, 2, isSigned);
	}
      break;
    default:
      emitcode("ERROR", "bad reg 0x%02x in storeRegToFullAop()", regidx);
    }
}

/**************************************************************************
 * transferAopAop - Transfer the value at logical offset srcofs of asmop
 *                  srcaop to logical offset dstofs of asmop dstaop.
 *************************************************************************/
void
transferAopAop (asmop *srcaop, int srcofs, asmop *dstaop, int dstofs)
{
  bool needpula = false;
  reg_info *reg = NULL;
  bool keepreg = false;

  emitComment (TRACE_AOP, __func__ );

  if(!srcaop || !dstaop)
    {
      if (!srcaop) emitcode("ERROR", "srcaop is null");
      if (!dstaop) emitcode("ERROR", "dstaop is null");
      return;
    }
  wassert (srcaop && dstaop);

  /* ignore transfers at the same byte, unless its volatile */
  if (srcaop->op && !isOperandVolatile (srcaop->op, false)
      && dstaop->op && !isOperandVolatile (dstaop->op, false)
      && operandsEqu (srcaop->op, dstaop->op) && srcofs == dstofs && dstaop->type == srcaop->type)
    return;

  if (srcaop->stacked && srcaop->stk_aop[srcofs])
    {
      transferAopAop (srcaop->stk_aop[srcofs], 0, dstaop, dstofs);
      return;
    }

  if (dstaop->stacked && dstaop->stk_aop[srcofs])
    {
      transferAopAop (srcaop, srcofs, dstaop->stk_aop[dstofs], 0);
      return;
    }

  emitComment (TRACE_AOP|VVDBG, "    transferAopAop from (%s, %d, 0x%x)",
               aopName (srcaop), srcofs, srcaop->regmask);
  emitComment (TRACE_AOP|VVDBG, "    transferAopAop   to (%s, %d, 0x%x)",
               aopName (dstaop), dstofs, dstaop->regmask);

  if (dstofs >= dstaop->size)
    return;

  // same registers and offset, no transfer
  if (srcaop->type == AOP_REG && dstaop->type == AOP_REG)
    {
      emitComment (TRACE_AOP|VVDBG, "  %s: regreg", __func__);
      transferRegReg(srcaop->aopu.aop_reg[srcofs], dstaop->aopu.aop_reg[dstofs], false);
      return;
    }

#if 0
  // same stack offset, no transfer
  if(srcaop->type == AOP_SOF && dstaop->type == AOP_SOF)
    if( (srcaop->aopu.aop_stk+srcofs) == (dstaop->aopu.aop_stk+dstofs) )
      {
	emitComment (TRACE_AOP|VVDBG, "    transferAopAop  AOP_SOF same offset");
	return;
      }
#endif

  if (srcaop->type == AOP_LIT)
    {
      storeConstToAop (byteOfVal (srcaop->aopu.aop_lit, srcofs), dstaop, dstofs);
      return;
    }
  if (dstaop->type == AOP_REG)
    {
      reg = dstaop->aopu.aop_reg[dstofs];
      keepreg = true;
    } 
  else if ((srcaop->type == AOP_REG) && (srcaop->aopu.aop_reg[srcofs]))
    {
      reg = srcaop->aopu.aop_reg[srcofs];
      keepreg = true;
    }

  // TODO: pick reg based on if can load op?
  if (!reg)
    {
      if(srcaop->type != AOP_SOF && dstaop->type != AOP_SOF)
        reg = getFreeByteReg();

      if (reg == NULL)
        {
          // FIXME: used vs. surv triggers failure on bug 3556 in stack-auto
          // seems to not affect the bug anymore (?)
	  //          needpula = pushRegIfUsed (m6502_reg_a);
          needpula = storeRegTempIfUsed (m6502_reg_a);
          reg = m6502_reg_a;
        }
    }

  emitComment (TRACE_AOP|VVDBG, "  %s: general case", __func__);

  loadRegFromAop (reg, srcaop, srcofs);
  storeRegToAop (reg, dstaop, dstofs);

  if (!keepreg)
    //    pullOrFreeReg (reg, needpula);
    loadOrFreeRegTemp (reg, needpula);
}

#if 0
/**************************************************************************
 * forceStackedAop - Reserve space on the stack for asmop aop; when
 *                   freeAsmop is called with aop, the stacked data will
 *                   be copied to the original aop location.
 *************************************************************************/
// TODO????
static asmop * forceStackedAop (asmop * aop, bool copyOrig)
{
  reg_info *reg = NULL;
  int offset;
  bool needpula = false;
  asmop *newaop = newAsmop (AOP_DIR);
  memcpy (newaop, aop, sizeof (*newaop));
  newaop->aopu.aop_dir = "REGTEMP";

  emitComment (TRACE_AOP|VVDBG, "  forcedStackedAop %s", aopName (aop));

  if (copyOrig)
    {
      reg = getFreeByteReg ();
      if (reg == NULL)
	{
	  reg = m6502_reg_a;
	  storeRegTemp(reg, true);
	  needpula = true;
	}
    }
  for (offset=0; offset<newaop->size; offset++)
    {
      asmop *aopsof = newAsmop (AOP_SOF);
      aopsof->size = 1;
      if (copyOrig)
	{
	  loadRegFromAop (reg, aop, offset);
	  pushReg (reg, false);
	}
      else
	{
	  pushReg (m6502_reg_a, false);
	}
      aopsof->aopu.aop_stk = -_S.stackOfs - _S.stackPushes;
      aopsof->op = aop->op;
      newaop->stk_aop[offset] = aopsof;
    }

  if (!reg && copyOrig)
    {
      for (offset = 0; offset < newaop->size; offset++)
	{
	  transferAopAop (aop, offset, newaop, offset);
	}
    }
  newaop->stacked = 1;
  loadOrFreeRegTemp(reg, needpulla);
  return newaop;
}
#endif

// TODO: fix these
/**************************************************************************
 * accopWithAop - Emit accumulator modifying instruction accop with
 *                the byte at logical offset loffset of asmop aop.
 *                Supports: adc, and, cmp, eor, ora, sbc
 *************************************************************************/
void
accopWithAop (char *accop, asmop *aop, int loffset)
{
  bool ucp = false;
  emitComment (TRACE_AOP, __func__ );

  if ( !strcmp(accop, "ucp") )
    {
      // special case for unsigned compare
      ucp=true;
      accop = "cmp";
    }

  if (aop->stacked && aop->stk_aop[loffset])
    {
      accopWithAop (accop, aop->stk_aop[loffset], 0);
      return;
    }

  if (aop->type == AOP_DUMMY)
    return;

  if (aop->type == AOP_REG)
    {
      if (loffset < aop->size)
        {
          storeRegTemp (aop->aopu.aop_reg[loffset], true);
          emitRegTempOp( accop, getLastTempOfs() );
          loadRegTemp(NULL);
        }
      else
	{
	  emit6502op (accop, "#0x00");
	}
    }
  else
    {
      aopAdrPrepare(aop, loffset);
      //    emit6502op (accop, aopAdrStr (aop, loffset, false));
      const char *arg = aopAdrStr (aop, loffset, false);
      if ( ucp && _S.lastflag==A_IDX && !strcmp(arg,"#0x00") )
	{
	  // do nothing
	} else 
	emit6502op (accop, arg);
      aopAdrUnprepare(aop, loffset);
    }
}

/**************************************************************************
 * rmwWithReg - Emit read/modify/write instruction rmwop with register reg.
 *              byte at logical offset loffset of asmop aop. Register reg
 *              must be 8-bit.
 *              Supports: com, dec, inc, lsl, lsr, neg, rol, ror
 *************************************************************************/
void
rmwWithReg (char *rmwop, reg_info * reg)
{
  if (reg->rIdx == A_IDX)
    {
      if (!strcmp(rmwop, "inc"))
        {
          if (IS_MOS65C02)
            {
              emit6502op (rmwop, "a");
            }
          else
            {
              emitSetCarry(0);
              emit6502op ("adc", "#0x01");
            }
        }
      else if (!strcmp(rmwop, "dec"))
        {
          if (IS_MOS65C02)
            {
              emit6502op (rmwop, "a");
            }
          else
            {
              emitSetCarry(1);
              emit6502op ("sbc", "#0x01");
            }
        }
      else if (!strcmp(rmwop, "com"))
        {
          emit6502op ("eor", "#0xff");
        }
      else if (!strcmp(rmwop, "neg"))
        {
          emit6502op ("eor", "#0xff");
          emitSetCarry (0);
          emit6502op ("adc", "#0x01");
        }
      else if (!strcmp(rmwop, "bit"))
        { // TODO???
	  emitcode("ERROR", "   %s : called with unsupported opcode: %s", __func__, rmwop);
        } 
      else
        {
          emit6502op (rmwop, "a");
        }
    }
  else if (reg->rIdx == X_IDX)
    {
      if (!strcmp(rmwop, "inc"))
        {
          emit6502op ("inx", "");
        }
      else if (!strcmp(rmwop, "dec"))
        {
          emit6502op ("dex", "");
        }
      else
        {
          bool needpulla = pushRegIfUsed (m6502_reg_a);
          transferRegReg (m6502_reg_x, m6502_reg_a, true);
          rmwWithReg (rmwop, m6502_reg_a);
          transferRegReg (m6502_reg_a, m6502_reg_x, true);
          pullOrFreeReg (m6502_reg_a, needpulla);
        }
    }
  else if (reg->rIdx == Y_IDX)
    {
      if (!strcmp(rmwop, "inc"))
        {
          emit6502op ("iny", "");
        }
      else if (!strcmp(rmwop, "dec"))
        {
          emit6502op ("dey", "");
        }
      else
        {
          bool needpulla = pushRegIfUsed (m6502_reg_a);
          transferRegReg (m6502_reg_y, m6502_reg_a, true);
          rmwWithReg (rmwop, m6502_reg_a);
          transferRegReg (m6502_reg_a, m6502_reg_y, true);
          pullOrFreeReg (m6502_reg_a, needpulla);
        }
    }
  else
    {
      emitcode("ERROR", "bad reg in rmwWithReg()");
    }
  // always dirty dest. register
  //  m6502_dirtyReg (reg);
}

/**************************************************************************
 * rmwWithAop - Emit read/modify/write instruction rmwop with the byte at
 *                logical offset loffset of asmop aop.
 *                Supports: bit, dec, inc, lsl, lsr, neg, rol, ror
 *************************************************************************/
void
rmwWithAop (char *rmwop, asmop * aop, int loffset)
{
  //  bool needpull = false;
  emitComment (TRACE_AOP, __func__ );

  if (aop->stacked && aop->stk_aop[loffset])
    {
      rmwWithAop (rmwop, aop->stk_aop[loffset], 0);
      return;
    }

  switch (aop->type)
    {
    case AOP_REG:
      rmwWithReg (rmwop, aop->aopu.aop_reg[loffset]);
      break;
    case AOP_DIR:
    case AOP_EXT:
      emitComment (TRACE_AOP, "  rmwWithAop DIR/EXT");
      emit6502op (rmwop, aopAdrStr(aop, loffset, false));
      dirtyRegAop(NULL, aop, loffset);
      break;
    case AOP_DUMMY:
      break;
    case AOP_SOF:
      {
        emitComment (TRACE_AOP, "  rmwWithAop AOP_SOF");
        // TODO: does anything but A make sense here?
	//        reg_info * reg = getFreeByteReg();
	//        if (!reg)
	//            reg = m6502_reg_a;
        int offset = loffset; // SEH: aop->size - 1 - loffset;
        offset += _S.stackOfs + _S.stackPushes + aop->aopu.aop_stk + 1;
#if 0
        if ((offset > 0x1ff) || (offset < 0))
	  {
	    emitComment (TRACE_AOP, "  rmwWithAop large offset");
	    /* Unfortunately, the rmw class of instructions only support a */
	    /* single byte stack pointer offset and we need two. */
	    needpull = pushRegIfUsed (reg);
	    loadRegFromAop (reg, aop, loffset);
	    rmwWithReg (rmwop, reg);
	    if (strcmp ("tst", rmwop)) //TODO: no tst
	      storeRegToAop (reg, aop, loffset);
	    pullOrFreeReg (reg, needpull);
	    break;
	  }
#endif
        /* If the offset is small enough, fall through to default case */
      }
    default:
      emitComment (TRACE_AOP, "  rmwWithAop small offset ");
      // FIXME: figure out if asr handling should be here
      // or if asr should be generated at all by the codegen
      emit6502op (rmwop, aopAdrStr (aop, loffset, false));
      dirtyRegAop(NULL, aop, loffset);
    }

}

/**************************************************************************
 * stores reg in DPTR at offset dofs
 *************************************************************************/
static void
storeRegToDPTR(reg_info *reg, int dofs)
{
  int regidx=reg->rIdx;

  if(reg->isLitConst && _S.DPTRAttr[dofs].isLiteral
     && reg->litConst == _S.DPTRAttr[dofs].literalValue )
    {
      emitComment (TRACEGEN, " %s: DPTR[%d] has same literal %02x",
                   __func__, dofs, reg->litConst);
      m6502_freeReg(reg);
      return;
    }

  if ( reg->aop && _S.DPTRAttr[dofs].aop && sameRegs (reg->aop, _S.DPTRAttr[dofs].aop) 
       && (reg->aopofs == dofs) )
    {
      emitComment (TRACEGEN, " %s: DPTR already has result", __func__);
      return;
    }

  switch(regidx)
    {
    case A_IDX:
      emit6502op ("sta", DPTRFMT, dofs);
      break;
    case X_IDX:
      emit6502op ("stx", DPTRFMT, dofs);
      break;
    case Y_IDX:
      emit6502op ("sty", DPTRFMT, dofs);
      break;
    default:
      emitcode("ERROR","  %s: illegal register index %d", __func__, regidx);
      return;
    }

  _S.DPTRAttr[dofs].isLiteral=reg->isLitConst;
  _S.DPTRAttr[dofs].literalValue=reg->litConst;
  _S.DPTRAttr[dofs].aop=reg->aop;
  _S.DPTRAttr[dofs].aopofs=reg->aopofs;

  m6502_freeReg(reg);
}

/**************************************************************************
 * sets up DPTR for a indexed operation
 * clobbers A if savea==false and clobbers Y if savea==true
 *************************************************************************/
static int
setupDPTR(operand *op, int offset, char * rematOfs, bool savea)
{
  emitComment (TRACEGEN, "  %s - off=%d remat=%s savea=%d", __func__, offset, rematOfs, savea?1:0);

  /* The rematerialized offset may have a "#" prefix; skip over it */
  if (rematOfs && rematOfs[0] == '#')
    rematOfs++;
  if (rematOfs && !rematOfs[0])
    rematOfs = NULL;

  /* force offset to signed 16-bit range */
  offset &= 0xffff;
  if (offset & 0x8000)
    offset = 0x10000 - offset;
  //    offset = offset - 0x10000;

  if(!op)
    {
      emitcode("ERROR", "    %s: op is null", __func__);
      return 0;
    }

  if (!rematOfs && offset >= 0 && offset <= 0xff)
    {
      // no remat and 8-bit offset
      reg_info *reg0=findRegAop(AOP(op), 0);
      reg_info *reg1=findRegAop(AOP(op), 1);

      if ( IS_SAME_DPTR_OP(op) )
        {
          // do nothing
	  emitComment (TRACEGEN|VVDBG, "  %s: DPTR already has correct value", __func__);
        }
      else if(AOP_TYPE(op) == AOP_REG)
	{
	  emitComment (TRACEGEN|VVDBG, "    %s: AOP_REG", __func__);
	  storeRegToDPTR(AOP(op)->aopu.aop_reg[0], 0);
	  storeRegToDPTR(AOP(op)->aopu.aop_reg[1], 1);
	}
      else
        {
          reg_info *reg = NULL;

          emitComment (TRACEGEN|VVDBG, "    %s: not AOP_REG", __func__);

          if(reg0)
            storeRegToDPTR(reg0, 0);
          if(reg1)
            storeRegToDPTR(reg1, 1);
          if(reg0&&reg1)
            return offset;

	  if(m6502_reg_x->isFree && m6502_reg_x->aop!=&tsxaop )
	    reg=m6502_reg_x;
	  else if(m6502_reg_a->isFree && !savea)
	    reg=m6502_reg_a;
	  else if(m6502_reg_y->isFree)
	    reg=m6502_reg_y;

          // FIXME: save/restore x if SOF

          if(AOP(op)->type == AOP_SOF || reg==NULL)
            reg=m6502_reg_a;

          if(savea)
	    transferRegReg(m6502_reg_a, m6502_reg_y, true);

	  if(!reg0)
	    {
	      loadRegFromAop(reg, AOP(op), 0);
	      storeRegToDPTR(reg, 0);
	    }
	  if(!reg1)
	    {
	      loadRegFromAop(reg, AOP(op), 1);
	      storeRegToDPTR(reg, 1);
            }
          if(savea)
            transferRegReg(m6502_reg_y, m6502_reg_a, true);
          else
	    m6502_freeReg(m6502_reg_a);
	}
      return offset;
    }
  else
    {
      // general case
      emitComment (TRACEGEN|VVDBG, "    %s: general case", __func__);

      if(!rematOfs)
        rematOfs="0";

      if(savea)
        transferRegReg(m6502_reg_a, m6502_reg_y, true);

      emitSetCarry(0);
      loadRegFromAop(m6502_reg_a, AOP(op), 0);
      emit6502op ("adc", "#<(%s+%d)", rematOfs, offset);
      storeRegToDPTR(m6502_reg_a, 0);
      loadRegFromAop(m6502_reg_a, AOP(op), 1);
      emit6502op ("adc", "#>(%s+%d)", rematOfs, offset);
      storeRegToDPTR(m6502_reg_a, 1);
      if(savea)
        transferRegReg(m6502_reg_y, m6502_reg_a, true);
      else
        m6502_freeReg(m6502_reg_a);
      return 0;
    }
}

/**************************************************************************
 * newAsmop - creates a new asmOp
 *************************************************************************/
static asmop *
newAsmop (short type)
{
  asmop *aop;
  // TODO: are these ever freed?
  aop = Safe_calloc (1, sizeof (asmop));
  aop->type = type;
  aop->op = NULL;
  return aop;
}

#if 0
/**************************************************************************
 * operandConflictsWithYX - true if operand in y and/or x register
 *************************************************************************/
static bool
operandConflictsWithYX (operand *op)
{
  symbol *sym;
  int i;

  if (IS_ITEMP (op))
    {
      sym = OP_SYMBOL (op);
      if (!sym->isspilt)
        {
          for(i = 0; i < sym->nRegs; i++)
            if (sym->regs[i] == m6502_reg_y || sym->regs[i] == m6502_reg_x)
              return true;
        }
    }

  return false;
}

/**************************************************************************
 * operandConflictsWithX - true if operand in x register
 *************************************************************************/
static bool
operandConflictsWithX (operand *op)
{
  symbol *sym;
  int i;

  if (IS_ITEMP (op))
    {
      sym = OP_SYMBOL (op);
      if (!sym->isspilt)
        {
          for(i = 0; i < sym->nRegs; i++)
            if (sym->regs[i] == m6502_reg_x)
              return true;
        }
    }

  return false;
}

/**************************************************************************
 * operandOnStack - returns True if operand is on the stack
 *************************************************************************/
static bool
operandOnStack(operand *op)
{
  symbol *sym;

  if (!op || !IS_SYMOP (op))
    return false;
  sym = OP_SYMBOL (op);
  if (!sym->isspilt && sym->onStack)
    return true;
  if (sym->isspilt)
    {
      sym = sym->usl.spillLoc;
      if (sym && sym->onStack)
        return true;
    }
  return false;
}

/**************************************************************************
 * tsxUseful - returns True if tsx could help at least one
 *             anticipated stack references
 *************************************************************************/
static bool
tsxUseful(const iCode *ic)
{
  operand *right  = IC_RIGHT(ic);
  operand *left   = IC_LEFT(ic);
  operand *result = IC_RESULT(ic);
  int uses = 0;

  if (ic->op == CALL)
    {
      if (result && operandSize (result) < 2 && operandOnStack (result))
        {
          uses++;
          ic = ic->next;
        }
    }

  while (ic && uses < 1)
    {
      if (ic->op == IFX)
	{
	  if (operandOnStack (IC_COND (ic)))
	    uses += operandSize(IC_COND (ic));
	  break;
	}
      else if (ic->op == JUMPTABLE)
	{
	  if (operandOnStack (IC_JTCOND (ic)))
	    uses++;
	  break;
	}
      else if (ic->op == ADDRESS_OF)
	{
	  if (operandOnStack (right))
	    break;
	}
      else if (ic->op == LABEL || ic->op == GOTO || ic->op == CALL || ic->op == PCALL)
	break;
      else if (POINTER_SET (ic) || POINTER_GET (ic))
	break;
      else
	{
	  if (operandConflictsWithYX (result))
	    break;
	  if (operandOnStack (left))
	    uses += operandSize (left);
	  if (operandOnStack (right))
	    uses += operandSize (right);
	  if (operandOnStack (result))
	    uses += operandSize (result);
	}

      ic = ic->next;
    }

  return uses >= 1;
}
#endif

void
emitTSX()
{
  emitComment (TRACE_STACK|VVDBG, "%s: stackOfs=%d tsx=%d stackpush=%d",
               __func__, _S.stackOfs, _S.tsxStackPushes, _S.stackPushes);

  // already did TSX
  if (m6502_reg_x->aop == &tsxaop)
    return;

  // put stack pointer in X
  if(!m6502_reg_x->isFree)
    emitcode("ERROR","emitTSX called with X in use");
  emit6502op ("tsx", "");
}

// TODO: make these subroutines
static void saveBasePtr()
{
#if 0
  storeRegTemp (m6502_reg_x, true); // TODO: only when used?
  // TODO: if X is free should we call emitTSX() to mark X=S?
  emitTSX();
  emit6502op ("stx", BASEPTR);
  _S.baseStackPushes = _S.stackPushes;
  loadRegTemp (m6502_reg_x);
#endif
}

static void
restoreBasePtr()
{
  // we recompute with saveBasePtr() after each jsr
}

/**************************************************************************
 * aopForSym - for a true symbol
 *************************************************************************/
static asmop * aopForSym (const iCode * ic, symbol * sym)
{
  asmop *aop;
  memmap *space;

  wassertl (ic != NULL, "Got a null iCode");
  wassertl (sym != NULL, "Got a null symbol");

  emitComment (TRACE_AOP|VVDBG, "%s", __func__);
      
  space = SPEC_OCLS (sym->etype);

  /* if already has one */
  if (sym->aop)
    {
      return sym->aop;
    }

  /* special case for a function */
  if (IS_FUNC (sym->type))
    {
      sym->aop = aop = newAsmop (AOP_IMMD);
      aop->aopu.aop_immd = Safe_calloc (1, strlen (sym->rname) + 1 + 6);
      sprintf (aop->aopu.aop_immd, "(%s)", sym->rname); // function pointer; take back one for RTS
      aop->size = FARPTRSIZE;
      return aop;
    }

  /* if it is on the stack */
  if (sym->onStack)
    {
      sym->aop = aop = newAsmop (AOP_SOF);
      aop->size = getSize (sym->type);
      aop->aopu.aop_stk = sym->stack;

      emitComment (TRACE_STACK|VVDBG, "%s: symbol %s: stack=%d size=%d", 
		   __func__, sym->name, sym->stack, aop->size);

#if 0
      if (!regalloc_dry_run && m6502_reg_x->isFree && m6502_reg_x->aop != &tsxaop) {
	if (!m6502_reg_x->isDead)
	  return aop;
	if (ic->op == IFX && operandConflictsWithX (IC_COND (ic)))
	  return aop;
	else if (ic->op == JUMPTABLE && operandConflictsWithX (IC_JTCOND (ic)))
	  return aop;
	else
          {
	    // FIXME: this is likely incorrect as YX is not a adr register in the 6502
	    /* If this is a pointer gen/set, then hx is definitely in use */
	    if (POINTER_SET (ic) || POINTER_GET (ic))
	      return aop;
	    if (ic->op == ADDRESS_OF)
	      return aop;
	    if (operandConflictsWithX (IC_LEFT (ic)))
	      return aop;
	    if (operandConflictsWithX (IC_RIGHT (ic)))
	      return aop;
	  }
	// TODO?
	/* It's safe to use tsx here. */
	if (!tsxUseful (ic))
	  return aop;
	// transfer S to X
	emitTSX();
      }
#endif
      return aop;
    }

  /* if it is in direct space */
  if (IN_DIRSPACE (space)) {
    sym->aop = aop = newAsmop (AOP_DIR);
    aop->aopu.aop_dir = sym->rname;
    aop->size = getSize (sym->type);
    return aop;
  }

  /* default to far space */
  sym->aop = aop = newAsmop (AOP_EXT);
  aop->aopu.aop_dir = sym->rname;
  aop->size = getSize (sym->type);
  return aop;
}

/**************************************************************************
 * aopForRemat - rematerializes an object
 *************************************************************************/
static asmop * aopForRemat (symbol * sym)
{
  iCode *ic = sym->rematiCode;
  asmop *aop = NULL;
  int val = 0;

  if (!ic) {
    fprintf (stderr, "Symbol %s to be rematerialized, but has no rematiCode.\n", sym->name);
    wassert (0);
  }

  for (;;) {
    if (ic->op == '+')
      val += (int) operandLitValue (IC_RIGHT (ic));
    else if (ic->op == '-')
      val -= (int) operandLitValue (IC_RIGHT (ic));
    else if (IS_CAST_ICODE (ic)) {
      ic = OP_SYMBOL (IC_RIGHT (ic))->rematiCode;
      continue;
    } else
      break;

    ic = OP_SYMBOL (IC_LEFT (ic))->rematiCode;
  }

  if (ic->op == ADDRESS_OF) {
    if (val) {
      SNPRINTF (buffer, sizeof (buffer),
                "(%s %c 0x%04x)", OP_SYMBOL (IC_LEFT (ic))->rname, val >= 0 ? '+' : '-', abs (val) & 0xffff);
    } else {
      strncpyz (buffer, OP_SYMBOL (IC_LEFT (ic))->rname, sizeof (buffer));
    }

    aop = newAsmop (AOP_IMMD);
    aop->aopu.aop_immd = Safe_strdup (buffer);
  } else if (ic->op == '=') {
    val += (int) operandLitValue (IC_RIGHT (ic));
    val &= 0xffff;
    SNPRINTF (buffer, sizeof (buffer), "0x%04x", val);
    aop = newAsmop (AOP_LIT);
    aop->aopu.aop_lit = constVal (buffer);
  } else {
    werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "unexpected rematerialization");
  }

  return aop;
}

#if 0 // No longer used?
/**************************************************************************
 * regsInCommon - two operands have some registers in common
 *************************************************************************/
static bool
regsInCommon (operand * op1, operand * op2)
{
  symbol *sym1, *sym2;
  int i;

  /* if they have registers in common */
  if (!IS_SYMOP (op1) || !IS_SYMOP (op2))
    return false;

  sym1 = OP_SYMBOL (op1);
  sym2 = OP_SYMBOL (op2);

  if (sym1->nRegs == 0 || sym2->nRegs == 0)
    return false;

  for (i = 0; i < sym1->nRegs; i++)
    {
      int j;
      if (!sym1->regs[i])
        continue;

      for (j = 0; j < sym2->nRegs; j++)
        {
          if (!sym2->regs[j])
            continue;

          if (sym2->regs[j] == sym1->regs[i])
            return true;
        }
    }

  return false;
}
#endif

/**************************************************************************
 * operandsEqu - equivalent
 *************************************************************************/
static bool
operandsEqu (operand *op1, operand *op2)
{
  symbol *sym1, *sym2;

  /* if they not symbols */
  if (!IS_SYMOP (op1) || !IS_SYMOP (op2))
    return false;

  sym1 = OP_SYMBOL (op1);
  sym2 = OP_SYMBOL (op2);

  /* if both are itemps & one is spilt
     and the other is not then false */
  if (IS_ITEMP (op1) && IS_ITEMP (op2) && sym1->isspilt != sym2->isspilt)
    return false;

  /* if they are the same */
  if (sym1 == sym2)
    return true;

  /* if they have the same rname */
  if (sym1->rname[0] && sym2->rname[0] && strcmp (sym1->rname, sym2->rname) == 0)
    return true;

  /* if left is a tmp & right is not */
  if (IS_ITEMP (op1) && !IS_ITEMP (op2) && sym1->isspilt && (sym1->usl.spillLoc == sym2))
    return true;

  if (IS_ITEMP (op2) && !IS_ITEMP (op1) && sym2->isspilt && sym1->level > 0 && (sym2->usl.spillLoc == sym1))
    return true;

  return false;
}

/**************************************************************************
 * sameRegs - two asmops have the same registers
 *************************************************************************/
bool
sameRegs (asmop * aop1, asmop * aop2)
{
  int i;

  if (aop1 == aop2)
    return true;

  //  if (aop1->size != aop2->size)
  //    return false;

  if (aop1->type == aop2->type) {
    switch (aop1->type) {
    case AOP_REG:
      for (i = 0; i < aop1->size; i++)
        if (aop1->aopu.aop_reg[i] != aop2->aopu.aop_reg[i])
          return false;
      return true;
    case AOP_SOF:
      return (aop1->aopu.aop_stk == aop2->aopu.aop_stk);
    case AOP_DIR:
      //          if (regalloc_dry_run)
      //            return false; // TODO: why?
    case AOP_EXT:
      return (!strcmp (aop1->aopu.aop_dir, aop2->aopu.aop_dir));
    default:
      break;
    }
  }

  return false;
}

/**************************************************************************
 * aopCanIncDec - asmop is EXT or DIR or X/Y
 *
 *************************************************************************/
bool
aopCanIncDec (asmop * aop)
{
  switch (aop->type)
    {
    case AOP_REG:
      if(aop->aopu.aop_reg[0]->rIdx == A_IDX) return IS_MOS65C02;
    case AOP_DIR:
    case AOP_EXT:
      return true;
    default:
      break;
    }
  return false;
}

/**************************************************************************
 * aopCanShift - asmop is EXT or DIR or A
 *
 *************************************************************************/
bool
aopCanShift (asmop * aop)
{
  switch (aop->type) {
  case AOP_REG:
    return ((aop->size == 1) && (aop->aopu.aop_reg[0]->rIdx == A_IDX));
  case AOP_DIR:
  case AOP_EXT:
    return true;
  default:
    break;
  }
  return false;
}

/**************************************************************************
 * addSign - complete with sign
 *************************************************************************/
void
addSign (operand * result, int offset, int sign)
{
  int size = (AOP_SIZE (result) - offset);
  if (size > 0) {
    if (sign) {
      signExtendA();
      while (size--)
        storeRegToAop (m6502_reg_a, AOP (result), offset++);
    } else
      while (size--)
        storeConstToAop (0, AOP (result), offset++);
  }
}

/**************************************************************************
 * aopOp - allocates an asmop for an operand
 *************************************************************************/
void
aopOp (operand *op, const iCode * ic)
{
  asmop *aop = NULL;
  symbol *sym;
  int i;

  emitComment (TRACE_AOP, __func__);

  if (!op)
    return;

  /* if already has an asmop */
  if (op->aop)
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: skip", __func__);
      if (IS_SYMOP (op) && OP_SYMBOL (op)->aop)
        {
          if(op->aop->type==AOP_SOF) {
            emitComment (VVDBG|TRACE_AOP, "    asmop symbol: %s [%d:%d] - %d",
                         OP_SYMBOL (op)->name, OP_SYMBOL (op)->stack, op->aop->size,
                         op->aop->aopu.aop_stk );
            // FIXME FIXME: the following is incorrect but fixes failures in gte991019-1
            // change the stack offset for some symbols
            // enabling the workaround breaks shifts3 and muldiv_long_*_volatile
            // Should find the source of the gte bug
            // op->aop->aopu.aop_stk = OP_SYMBOL (op)->stack;
          }
        }
      return;
    }

  // Is this a pointer set result?
  if ((op == IC_RESULT (ic)) && POINTER_SET (ic))
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: POINTER_SET", __func__);
    }

  /* if this a literal */
  if (IS_OP_LITERAL (op))
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: LITERAL = 0x%x:%d",
                   __func__, ulFromVal(OP_VALUE (op)), getSize(operandType(op)) );
      aop = newAsmop (AOP_LIT);
      aop->aopu.aop_lit = OP_VALUE (op);
      aop->size = getSize (operandType (op));
      op->aop = aop;
      aop->op = op; // asmopToBool needs the op to check the type of the literal.
      return;
    }


  //  printf("checking underlying sym\n");
  /* if the underlying symbol has a aop */
  if (IS_SYMOP (op) && OP_SYMBOL (op)->aop)
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: SYMOP", __func__);
      op->aop = aop = Safe_calloc (1, sizeof (*aop));
      memcpy (aop, OP_SYMBOL (op)->aop, sizeof (*aop));
      //op->aop = aop = OP_SYMBOL (op)->aop;
      aop->size = getSize (operandType (op));
      emitComment (VVDBG|TRACE_AOP, "    symbol: %s [%d]",
                   OP_SYMBOL (op)->name, OP_SYMBOL (op)->stack );
      //printf ("reusing underlying symbol %s\n",OP_SYMBOL (op)->name);
      //printf (" with size = %d\n", aop->size);

      aop->op = op;
      return;
    }

  //  printf("checking true sym\n");
  /* if this is a true symbol */
  if (IS_TRUE_SYMOP (op))
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: TRUE_SYMOP", __func__);
      op->aop = aop = aopForSym (ic, OP_SYMBOL (op));
      aop->op = op;
      //printf ("new symbol %s\n", OP_SYMBOL (op)->name);
      //printf (" with size = %d\n", aop->size);
      return;
    }

  /* this is a temporary : this has
     only five choices :
     a) register
     b) spillocation
     c) rematerialize
     d) conditional
     e) can be a return use only */

  emitComment (VVDBG|TRACE_AOP, "    %s: temp",
	       __func__);

  if (!IS_SYMOP (op))
    piCode (ic, NULL);
  sym = OP_SYMBOL (op);

  //  printf("checking conditional\n");
  /* if the type is a conditional */
  if (sym->regType == REG_CND)
    {
      emitComment (VVDBG|TRACE_AOP, "    %s: AOP_CRY",
		   __func__);
      sym->aop = op->aop = aop = newAsmop (AOP_CRY);
      aop->size = 0;
      aop->op = op;
      return;
    }

  //  printf("checking spilt\n");
  /* if it is spilt then two situations
     a) is rematerialize
     b) has a spill location */
  if (sym->isspilt || sym->nRegs == 0)
    {
      //      printf("checking remat\n");
      /* rematerialize it NOW */
      if (sym->remat)
        {
	  emitComment (VVDBG|TRACE_AOP, "    %s: remat",
		       __func__);
          sym->aop = op->aop = aop = aopForRemat (sym);
          aop->size = getSize (sym->type);
          aop->op = op;
          return;
        }

      wassertl (!sym->ruonly, "sym->ruonly not supported");

      if (regalloc_dry_run)
        {
          // Todo: Handle dummy iTemp correctly
          if (options.stackAuto || (currFunc && IFFUNC_ISREENT (currFunc->type)))
            {
              sym->aop = op->aop = aop = newAsmop (AOP_SOF);
              aop->aopu.aop_stk = 8; /* bogus stack offset, high enough to prevent optimization */
            }
          else
            {
              sym->aop = op->aop = aop = newAsmop (AOP_DIR);
              aop->aopu.aop_dir = sym->name; //TODO? avoids crashing in sameRegs()
            }
          aop->size = getSize (sym->type);
          aop->op = op;
          return;
        }

      /* else spill location  */
      if (sym->isspilt && sym->usl.spillLoc || regalloc_dry_run)
        {
          asmop *oldAsmOp = NULL;

	  emitComment (VVDBG|TRACE_AOP, "    %s: spill",
		       __func__);

          if (sym->usl.spillLoc->aop && sym->usl.spillLoc->aop->size != getSize (sym->type))
            {
              /* force a new aop if sizes differ */
              oldAsmOp = sym->usl.spillLoc->aop;
              sym->usl.spillLoc->aop = NULL;
              //printf ("forcing new aop\n");
            }
          sym->aop = op->aop = aop = aopForSym (ic, sym->usl.spillLoc);
          if (sym->usl.spillLoc->aop->size != getSize (sym->type))
            {
              /* Don't reuse the new aop, go with the last one */
              sym->usl.spillLoc->aop = oldAsmOp;
            }
          aop->size = getSize (sym->type);
          aop->op = op;
          //printf ("spill symbol %s\n", OP_SYMBOL (op)->name);
          //printf (" with size = %d\n", aop->size);
          return;
        }

      /* else must be a dummy iTemp */
      sym->aop = op->aop = aop = newAsmop (AOP_DUMMY);
      aop->size = getSize (sym->type);
      aop->op = op;
      return;
    }

  //  printf("assuming register\n");
  /* must be in a register */
  emitComment (VVDBG|TRACE_AOP, "    %s: nregs %d",
	       __func__, sym->nRegs );
  wassert (sym->nRegs);
  sym->aop = op->aop = aop = newAsmop (AOP_REG);
  aop->size = sym->nRegs;
  for (i = 0; i < sym->nRegs; i++)
    {
      wassert (sym->regs[i] >= regsm6502 && sym->regs[i] < regsm6502 + 3);
      wassertl (sym->regs[i], "Symbol in register, but no register assigned.");
      aop->aopu.aop_reg[i] = sym->regs[i];
      aop->regmask |= sym->regs[i]->mask;
    }
  //  if ((sym->nRegs > 1) && (sym->regs[0]->mask > sym->regs[1]->mask))
  //    aop->regmask |= M6502MASK_REV;
  aop->op = op;
}

/**************************************************************************
 * freeAsmop - free up the asmop given to an operand
 *************************************************************************/
void
freeAsmop (operand * op, asmop * aaop)
{
  asmop *aop;

  if (!op)
    aop = aaop;
  else
    aop = op->aop;

  if (!aop)
    return;

  if (aop->freed)
    goto dealloc;

  aop->freed = 1;

  if (aop->stacked) {
    int stackAdjust;
    int loffset;

    emitComment (TRACE_AOP, "  freeAsmop restoring stacked %s", aopName (aop));
    aop->stacked = 0;
    stackAdjust = 0;
    for (loffset = 0; loffset < aop->size; loffset++)
      if (aop->stk_aop[loffset])
        {
	  transferAopAop (aop->stk_aop[loffset], 0, aop, loffset);
	  stackAdjust++;
	}
    pullNull (stackAdjust);
  }

 dealloc:
  /* all other cases just dealloc */
  if (op)
    {
      op->aop = NULL;
      if (IS_SYMOP (op))
	{
	  OP_SYMBOL (op)->aop = NULL;
	  /* if the symbol has a spill */
	  if (SPIL_LOC (op))
	    SPIL_LOC (op)->aop = NULL;
	}
    }
}


/**************************************************************************
 * aopDerefAop - treating the aop parameter as a pointer, return an asmop
 *               for the object it references
 *************************************************************************/
static asmop * aopDerefAop (asmop * aop, int offset)
{
  int adr;
  asmop *newaop = NULL;
  sym_link *type, *etype;
  int p_type;
  struct dbuf_s dbuf;

  emitComment (TRACE_AOP, "      aopDerefAop(%s)", aopName (aop));
  if (aop->op) {
    type = operandType (aop->op);
    etype = getSpec (type);
    /* if op is of type of pointer then it is simple */
    if (IS_PTR (type) && !IS_FUNC (type->next))
      p_type = DCL_TYPE (type);
    else
      {
        /* we have to go by the storage class */
        p_type = PTR_TYPE (SPEC_OCLS (etype));
      }
  }
  else
    p_type = UPOINTER;

  switch (aop->type) {
  case AOP_IMMD:
    if (p_type == POINTER)
      newaop = newAsmop (AOP_DIR);
    else
      newaop = newAsmop (AOP_EXT);
    if (!offset)
      newaop->aopu.aop_dir = aop->aopu.aop_immd;
    else {
      dbuf_init (&dbuf, 64);
      dbuf_printf (&dbuf, "(%s+%d)", aop->aopu.aop_immd, offset);
      newaop->aopu.aop_dir = dbuf_detach_c_str (&dbuf);
    }
    break;
  case AOP_LIT:
    adr = (int) ulFromVal (aop->aopu.aop_lit);
    if (p_type == POINTER)
      adr &= 0xff;
    adr = (adr + offset) & 0xffff;
    dbuf_init (&dbuf, 64);

    if (adr < 0x100) {
      newaop = newAsmop (AOP_DIR);
      dbuf_printf (&dbuf, "0x%02x", adr);
    } else {
      newaop = newAsmop (AOP_EXT);
      dbuf_printf (&dbuf, "0x%04x", adr);
    }
    newaop->aopu.aop_dir = dbuf_detach_c_str (&dbuf);
    break;
  default:
    werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "unsupported asmop");
    return NULL;
  }

  return newaop;
}

// is it safe to aopAdrStr?
static bool isAddrSafe(operand* op, reg_info* reg)
{
  switch (AOP(op)->type) {
  case AOP_IMMD:    // #nn
  case AOP_LIT:
  case AOP_DIR:     // aa
  case AOP_EXT:     // aaaa
    return true;
  case AOP_SOF:     // SOF,x
    if (reg == m6502_reg_a && (m6502_reg_x->isFree || m6502_reg_y->isFree))
      return true;
  default:
    break;
  }
  return false;
}

static int aopPrepareStoreTemp = 0;
static int aopPreparePreserveFlags = 0;

// TODO: make sure this is called before/after aopAdrStr if indexing might be used
static void aopAdrPrepare (asmop * aop, int loffset)
{
  emitComment (TRACE_AOP, "%s", __func__ );

  aopPreparePreserveFlags = 0;
  if (loffset > (aop->size - 1))
    return;

  if (aop->type==AOP_SOF) {
#if 0
    // code for lda [BASEPTR],y
    aopPrepareStoreTemp = storeRegTemp(m6502_reg_y, false);
    // FIXME: offset is wrong
    emitComment (TRACE_AOP, "ofs=%d base=%d tsx=%d push=%d stk=%d loffset=%d", _S.stackOfs, _S.baseStackPushes, _S.tsxStackPushes, _S.stackPushes, aop->aopu.aop_stk, loffset);
    loadRegFromConst(m6502_reg_y, _S.stackOfs + _S.baseStackPushes + aop->aopu.aop_stk + loffset + 1);
    // ORIG: loadRegFromConst(m6502_reg_y, _S.stackOfs - _S.baseStackPushes + aop->aopu.aop_stk + loffset + 1);
    m6502_reg_y->aop = &tsxaop;
#else
    // can we get stack pointer?
    aopPrepareStoreTemp=0;
    if (!m6502_reg_x->isFree) {
      // FIXME: check if used/dead is ok
      // aopPrepareStoreTemp = storeRegTempIfSurv(m6502_reg_x);
      if (m6502_reg_x->aop != &tsxaop)
        {
          emitComment (TRACE_AOP, "    aopAdrPrepare: x!=tsxaop");
          storeRegTemp(m6502_reg_x, true);
          aopPrepareStoreTemp = true;
          // m6502_reg_x->isFree=true;
        }
    }

    emitTSX();
#endif
    aopPreparePreserveFlags = 1; // TODO: also need to make sure flags are needed by caller
  }
}

static void aopAdrUnprepare (asmop * aop, int loffset)
{
  if (loffset > (aop->size - 1))
    return;

  if (aop->type==AOP_SOF) {
    if (aopPrepareStoreTemp) {
      if (aopPreparePreserveFlags)
        loadRegTempNoFlags(m6502_reg_x, true);
      else
        loadRegTemp(m6502_reg_x);

      aopPreparePreserveFlags = 0;
      aopPrepareStoreTemp = 0;
    }
  }
}


/**************************************************************************
 * aopAdrStr - for referencing the address of the aop
 *************************************************************************/
/* loffset seems to have a weird meaning here. It seems to be nonzero in some places where one would expect an offset to be zero */
static const char *
aopAdrStr (asmop * aop, int loffset, bool bit16)
{
  char *s = buffer;
  char *rs;
  int offset = loffset; // SEH: aop->size - 1 - loffset - (bit16 ? 1 : 0);
  int xofs;

  emitComment(VVDBG|TRACEGEN,"      %s: size=%d offs=%d",
	      __func__, aop->size, offset);

  /* offset is greater than
     size then zero */
  if (loffset > (aop->size - 1) && aop->type != AOP_LIT)
    return "#0x00";

  /* depending on type */
  switch (aop->type)
    {
    case AOP_DUMMY:
      return "#0x00";

    case AOP_IMMD:
      if (loffset)
	{
	  if (loffset > 1)
	    sprintf (s, "#(%s >> %d)", aop->aopu.aop_immd, loffset * 8);
	  else
	    sprintf (s, "#>%s", aop->aopu.aop_immd);
	}
      else
	sprintf (s, "#%s", aop->aopu.aop_immd);
      rs = Safe_calloc (1, strlen (s) + 1);
      strcpy (rs, s);
      return rs;

    case AOP_DIR:
      if (regalloc_dry_run)
	return "*dry";
      if (offset)
	sprintf (s, "*(%s + %d)", aop->aopu.aop_dir, offset);
      else
	sprintf (s, "*%s", aop->aopu.aop_dir);
      rs = Safe_calloc (1, strlen (s) + 1);
      strcpy (rs, s);
      return rs;

    case AOP_EXT:
      if (regalloc_dry_run)
	return "dry";
      if (offset)
	sprintf (s, "(%s + %d)", aop->aopu.aop_dir, offset);
      else
	sprintf (s, "%s", aop->aopu.aop_dir);
      rs = Safe_calloc (1, strlen (s) + 1);
      strcpy (rs, s);
      return rs;

    case AOP_REG:
      return aop->aopu.aop_reg[loffset]->name;

    case AOP_LIT:
      if (bit16)
	return aopLiteralLong (aop->aopu.aop_lit, loffset, 2);
      else
	return aopLiteral (aop->aopu.aop_lit, loffset);

    case AOP_SOF: // TODO?
      if (regalloc_dry_run)
	{
	  return "0x100,x"; // fake result, not needed
	}
      else
	{
	  // FIXME FIXME: force emit of TSX to avoid offset < 0x100
	  // this is a workaround for the assembler incorrectly
	  // generating ZP,x instead of ABS,x
	  if((_S.stackOfs + _S.tsxStackPushes + aop->aopu.aop_stk + offset + 1)<0)
	    {
	      m6502_dirtyReg(m6502_reg_x);
	    }
          // FIXME: this is usually redundant as it is explicitly called
          // before calling aopAdrStr
	  emitTSX();
	  // hc08's tsx returns +1, ours returns +0
	  //DD( emitcode( "", "; %d + %d + %d + %d + 1", _S.stackOfs, _S.tsxStackPushes, aop->aopu.aop_stk, offset ));
	  xofs = STACK_TOP + _S.stackOfs + _S.tsxStackPushes + aop->aopu.aop_stk + offset + 1;
	  emitComment(VVDBG|TRACE_STACK,"      op target: STACK_FRAME%+d (SP%+d [%d, %d, %d])",
		      /*_S.tsxStackPushes + */aop->aopu.aop_stk + offset + 1,
		      _S.stackOfs + aop->aopu.aop_stk + offset + 1,
		      _S.tsxStackPushes, aop->aopu.aop_stk, offset);
	  sprintf (s, IDXFMT_X, xofs);
	  rs = Safe_calloc (1, strlen (s) + 1);
	  strcpy (rs, s);
	  return rs;
#if 0
	  else if (m6502_reg_y->aop == &tsxaop) {
	    return "[__BASEPTR],y";
	  } else {
	    // FIXME: unimplemented
	    //          loadRegFromConst(m6502_reg_x, offset);
	    return "ERROR [__BASEPTR],y"; // TODO: is base ptr or Y loaded?
	  }
#endif
	}
    case AOP_IDX:
      return "ERROR - aopAdrStr: AOP_IDX ";
    default:
      break;
    }
  return "ERROR - aopAdrStr: unknown aop type";
}

/**************************************************************************
 * asmopToBool - Emit code to convert an asmop to a boolean.
 *               Result left in A (0=false, 1=true) if ResultInA,
 *               otherwise result left in Z flag (1=false, 0=true)
 *************************************************************************/
static void
asmopToBool (asmop *aop, bool resultInA)
{
  symbol *tlbl = safeNewiTempLabel (NULL);
  sym_link *type;
  int size = aop->size;
  int offset = size - 1;
  bool needloada = false;
  bool isFloat;

  emitComment (TRACE_AOP, "  %s resultinA: %s", __func__, resultInA?"yes":"no");

  wassert (aop);
  type = operandType (AOP_OP (aop));
  isFloat = IS_FLOAT (type);

  if (resultInA)
    m6502_freeReg (m6502_reg_a);

  if (IS_BOOL (type))
    {
      if (resultInA)
        {
          // result -> A
          loadRegFromAop (m6502_reg_a, aop, 0);
        }
      else
        {
          // result -> flags
          if (aop->type==AOP_REG)
            {
	      emitCmp(aop->aopu.aop_reg[0], 0);
            }
          else
            {
              reg_info* freereg = getDeadByteReg();
              if (freereg)
                {
                  loadRegFromAop (freereg, aop, 0);
		  //              if(getLastFlag()!=freereg->rIdx)
		  emitCmp(freereg, 0x00);
                }
              else
                {
                  // no choice, all regs are full
                  storeRegTemp (m6502_reg_a, true);
                  loadRegFromAop (m6502_reg_a, aop, 0);
                  loadRegTempNoFlags (m6502_reg_a, true); // TODO?
                }
            }
        }
      return;
    }

#if 0 
  if (resultInA && size == 1 && !IS_AOP_A(aop) /*_S.lastflag!=A_IDX*/)
    {
      loadRegFromAop (m6502_reg_a, aop, 0);
      emitCmp(m6502_reg_a, 0x01);
      loadRegFromConst (m6502_reg_a, 0);
      rmwWithReg ("rol", m6502_reg_a);
      return;
    }
#endif

  if(resultInA && size==1)
    {
      if(IS_AOP_A(aop))
	emitCmp(m6502_reg_a, 0);
      else
	loadRegFromAop (m6502_reg_a, aop, 0);
      goto end;
    }

  switch (aop->type)
    {
    case AOP_REG:
      if (size==1)
        {  // A, X or Y
	  emitCmp(aop->aopu.aop_reg[0], 0);
          return;
        }
      else if (IS_AOP_XA (aop))
        {
#if 0
	  // FIXME: this optimization makes the code smaller and slower
	  // should consider for size optimization

	  if(m6502_reg_a->isDead && _S.lastflag!=A_IDX && _S.lastflag!=X_IDX)
	    {
	      storeRegTemp(m6502_reg_x, false);
	      emitRegTempOp( "ora", getLastTempOfs() );
	      loadRegTemp (NULL);
	    }
	  else
#endif

	    if(_S.lastflag==X_IDX) 
	      {
		if (!(m6502_reg_x->isLitConst && m6502_reg_x->litConst==0 ) )
		  emit6502op ("bne", "%05d$", safeLabelNum (tlbl));
		emitCmp(m6502_reg_a, 0);
	      }
	    else
	      {
		emitCmp(m6502_reg_a, 0);
		if( !(m6502_reg_x->isLitConst && m6502_reg_x->litConst==0))
		  {
		    emit6502op ("bne", "%05d$", safeLabelNum (tlbl));
		    // FIXME: this optimization makes the code smaller (expected) and slower (unexpected)
		    //          if(m6502_reg_a->isDead) 
		    //            transferRegReg(m6502_reg_x, m6502_reg_a, true);
		    //          else 
		    emitCmp(m6502_reg_x, 0);
		  }
	      }
        }
      else if (IS_AOP_YX (aop))
        {
	  emitCmp(m6502_reg_x, 0);
          emit6502op ("bne", "%05d$", safeLabelNum (tlbl));
	  emitCmp(m6502_reg_y, 0); // FIXME: needed?
        }
      else
        {
          emitcode("ERROR", "Bad %02x regmask in asmopToBool", (aop)->regmask);
          return;
        }
      break;

    case AOP_LIT:
      /* Higher levels should optimize this case away but let's be safe */
      if (ulFromVal (aop->aopu.aop_lit))
        loadRegFromConst (m6502_reg_a, 1);
      else
        loadRegFromConst (m6502_reg_a, 0);
      m6502_freeReg (m6502_reg_a);
      break;

    case AOP_DIR:
    case AOP_EXT:
      emitComment (TRACE_AOP|VVDBG, "asmopToBool - AOP_DIR || AOP_EXT");

#if 1
      if (!resultInA && (size == 1) && !IS_AOP_A (aop) && !m6502_reg_a->isFree && m6502_reg_x->isFree)
        {
          loadRegFromAop (m6502_reg_x, aop, 0);
          return;
        }
#else
      if (!resultInA && (size == 1) )
        {
          reg_info *reg=getFreeByteReg();
          if(reg)
            {
              loadRegFromAop (reg, aop, 0);
              return;
            }
        }
#endif 

    default:
      if (!resultInA)
        // TODO: this could be IfSurv but A should be first instead of last.
        needloada = storeRegTempIfUsed(m6502_reg_a);

      loadRegFromAop (m6502_reg_a, aop, offset--);
      if (isFloat)
        emit6502op ("and", "#0x7F");
      else if(getLastFlag()!=A_IDX && size==1)
	emitCmp(m6502_reg_a, 0x00);

      while (--size)
        accopWithAop ("ora", aop, offset--);

      if (!resultInA)
        {
          loadRegTempNoFlags (m6502_reg_a, needloada);
          return;
        }
      else
        {
          m6502_freeReg (m6502_reg_a);
        }
      break;
    }

 end:
  if (resultInA)
    {
      symbol *endlbl = safeNewiTempLabel (NULL);

      emitBranch ("beq", endlbl);
      safeEmitLabel (tlbl);
      loadRegFromConst (m6502_reg_a, 1);
      safeEmitLabel (endlbl);
      _S.lastflag=A_IDX;
      m6502_dirtyReg (m6502_reg_a);
      m6502_useReg (m6502_reg_a);
    }
  else
    {
      if(tlbl) safeEmitLabel (tlbl);
    }
}

/**************************************************************************
 * genCopy - Copy the value from one operand to another
 *           The caller is responsible for aopOp and freeAsmop
 *************************************************************************/
void
genCopy (operand * result, operand * source)
{
  int size = AOP_SIZE (result);
  int srcsize = AOP_SIZE (source);
  int offset = 0;

  emitComment (TRACEGEN, __func__);
  emitComment (TRACEGEN|VVDBG, "      %s - size %d -> %d", __func__, srcsize, size);
  emitComment (TRACEGEN|VVDBG, "      %s - regmask %02x -> %02x",
               __func__, AOP(source)->regmask, AOP(result)->regmask );

  /* if they are the same and not volatile */
  if (operandsEqu (result, source) && !isOperandVolatile (result, false) &&
      !isOperandVolatile (source, false))
    return;

  /* if they are the same registers */
  if (sameRegs (AOP (source), AOP (result)) && srcsize == size )
    return;

  if(IS_AOP_XA (AOP (result)) )
    {
      loadRegFromAop (m6502_reg_xa, AOP(source), 0);
      return;
    }

  if (IS_AOP_XA (AOP (source)) && size <= 2  )
    {
      storeRegToAop (m6502_reg_xa, AOP (result), 0);
      return;
    }

  if(size==2 && AOP_TYPE(result) != AOP_SOF)
    {
      reg_info *reg0=findRegAop(AOP(source), 0);
      reg_info *reg1=findRegAop(AOP(source), 1);
      if(reg0&&reg1)
        {
          emitComment (TRACEGEN|VVDBG, "      %s (regtrack)", __func__);
	  storeRegToAop (reg0, AOP(result), 0);
	  storeRegToAop (reg1, AOP(result), 1);
          return;
        }
    }

#if 1
  if(AOP_TYPE (source) == AOP_SOF || AOP_TYPE(result) == AOP_SOF)
    {
      emitComment (TRACEGEN|VVDBG, "      %s (SOF)", __func__);
      bool save_a, save_x;
      save_a = storeRegTempIfSurv(m6502_reg_a);
      save_x = storeRegTempIfSurv(m6502_reg_x);
      offset=size-1;
      while (offset>=0)
	{

	  if(offset >= srcsize)
	    {
	      loadRegFromConst (m6502_reg_a, 0);
	      storeRegToAop (m6502_reg_a, AOP(result), offset);
	      m6502_freeReg(m6502_reg_a);
	    }
	  else
	    {
	      loadRegFromAop (m6502_reg_a, AOP(source), offset);
	      storeRegToAop (m6502_reg_a, AOP(result), offset);
	      m6502_freeReg(m6502_reg_a);
	    }
	  offset--;
	}
      loadOrFreeRegTemp(m6502_reg_x, save_x);
      loadOrFreeRegTemp(m6502_reg_a, save_a);

      return;
    }
#endif

  /* general case */
  emitComment (TRACEGEN|VVDBG, "      %s (general case)", __func__);

#if 0
  while (srcsize && size)
    {
      transferAopAop (AOP (source), offset, AOP (result), offset);
      offset++;
      srcsize--;
      size--;
    }
  while (size)
    {
      storeConstToAop (0, AOP (result), offset);
      offset++;
      size--;
    }
#else 
  // reverse order
  offset=size-1;

  while ( offset >= srcsize )
    {
      storeConstToAop (0, AOP (result), offset);
      offset--;
    }
  while (offset>=0)
    {
      transferAopAop (AOP (source), offset, AOP (result), offset);
      offset--;
    }
#endif
}

/**************************************************************************
 * genNot - generate code for ! operation
 *************************************************************************/
static void
genNot (iCode * ic)
{
  operand *left = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);
  bool needpulla;

  emitComment (TRACEGEN, __func__);
  printIC(ic);

  /* assign asmOps to operand & result */
  aopOp (left, ic);
  aopOp (result, ic);
  needpulla = pushRegIfSurv (m6502_reg_a);
  asmopToBool (AOP (left), true);

  emit6502op ("eor", "#0x01");
  storeRegToFullAop (m6502_reg_a, AOP (result), false);
  pullOrFreeReg (m6502_reg_a, needpulla);

  freeAsmop (result, NULL);
  freeAsmop (left, NULL);
}


/**************************************************************************
 * genCpl - generate code for complement
 *************************************************************************/
static void
genCpl (iCode * ic)
{
  operand *left = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  int offset = 0;
  int size;
  bool needpullreg;

  emitComment (TRACEGEN, __func__);
  printIC(ic);

  /* assign asmOps to operand & result */
  aopOp (left, ic);
  aopOp (result, ic);
  size = AOP_SIZE (result);

  emitComment (TRACEGEN|VVDBG, "      %s - regmask %02x -> %02x",
               __func__, AOP(left)->regmask, AOP(result)->regmask );

  if (size==2 && AOP_TYPE (result) == AOP_REG && AOP_TYPE (left) == AOP_REG)
    {
      if(IS_AOP_XA(AOP(left)) && IS_AOP_XA(AOP(result)))
	{
	  pushReg (m6502_reg_a, true);
	  transferRegReg (m6502_reg_x, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_x, true);
	  pullReg (m6502_reg_a);
	  rmwWithReg ("com", m6502_reg_a);
	}
      else if(IS_AOP_YX(AOP(left)) && IS_AOP_YX(AOP(result)))
	{
	  bool pa = pushRegIfSurv (m6502_reg_a);
	  transferRegReg (m6502_reg_y, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_y, true);
	  transferRegReg (m6502_reg_x, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_x, true);
	  pullOrFreeReg (m6502_reg_a, pa);
	}
      else if(IS_AOP_YX(AOP(left)) && IS_AOP_XA(AOP(result)))
	{
	  transferRegReg (m6502_reg_x, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  pushReg (m6502_reg_a, true);
	  transferRegReg (m6502_reg_y, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_x, true);
	  pullReg (m6502_reg_a);
	}
      else if(IS_AOP_XA(AOP(left)) && IS_AOP_YX(AOP(result)))
	{
	  pushReg (m6502_reg_a, true);
	  transferRegReg (m6502_reg_x, m6502_reg_a, true);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_y, true);
	  pullReg (m6502_reg_a);
	  rmwWithReg ("com", m6502_reg_a);
	  transferRegReg (m6502_reg_a, m6502_reg_x, true);
	}
      else
	{
	  m6502_unimplemented("unknown register pair in genCpl");
	}

      goto release;
    }

  if (AOP_TYPE (left) == AOP_REG && size==2)
    {
      rmwWithReg ("com", m6502_reg_a);
      storeRegToAop (m6502_reg_a, AOP (result), 0);
      loadRegFromAop (m6502_reg_a, AOP (left), 1);
      rmwWithReg ("com", m6502_reg_a);
      storeRegToAop (m6502_reg_a, AOP (result), 1);
      goto release;
    }

  needpullreg = pushRegIfSurv (m6502_reg_a);
  for(offset=size-1; offset>=0; offset--)
    {
      emitComment (TRACEGEN|VVDBG, "  %s: general case", __func__);
      loadRegFromAop (m6502_reg_a, AOP (left), offset);
      rmwWithReg ("com", m6502_reg_a);
      storeRegToAop (m6502_reg_a, AOP (result), offset);
    }
  pullOrFreeReg (m6502_reg_a, needpullreg);

  /* release the aops */
 release:
  freeAsmop (result, NULL);
  freeAsmop (left, NULL);
}

/**************************************************************************
 * genUminusFloat - unary minus for floating points
 *************************************************************************/
static void
genUminusFloat (operand * op, operand * result)
{
  int size, offset = 0;
  bool needpula;

  emitComment (TRACEGEN, __func__);

  /* for this we just copy and then flip the bit */

  size = AOP_SIZE (op) - 1;

  while (size--)
    {
      transferAopAop (AOP (op), offset, AOP (result), offset);
      offset++;
    }

  needpula = pushRegIfSurv (m6502_reg_a);
  loadRegFromAop (m6502_reg_a, AOP (op), offset);
  emit6502op ("eor", "#0x80");
  //  m6502_useReg (m6502_reg_a);
  storeRegToAop (m6502_reg_a, AOP (result), offset);
  pullOrFreeReg (m6502_reg_a, needpula);
}

/**************************************************************************
 * genUminus - unary minus code generation
 *************************************************************************/
static void genUminus (iCode * ic)
{
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  int offset, size;
  sym_link *optype;
  bool carry = true;
  bool needpula=false;

  emitComment (TRACEGEN, __func__);
  printIC (ic);

  /* assign asmops */
  aopOp (left, ic);
  aopOp (result, ic);

  optype = operandType (left);

  sym_link *resulttype = operandType (IC_RESULT (ic));
  unsigned topbytemask = (IS_BITINT (resulttype) && SPEC_USIGN (resulttype) && (SPEC_BITINTWIDTH (resulttype) % 8)) ?
    (0xff >> (8 - SPEC_BITINTWIDTH (resulttype) % 8)) : 0xff;
  bool maskedtopbyte = (topbytemask != 0xff);

  /* if float then do float stuff */
  if (IS_FLOAT (optype))
    {
      genUminusFloat (left, result);
      goto release;
    }

  /* otherwise subtract from zero */
  size = AOP_SIZE (left);
  offset = 0;

  if (size == 1)
    {
      needpula = pushRegIfSurv (m6502_reg_a);
      loadRegFromAop (m6502_reg_a, AOP (left), 0);
      rmwWithReg ("neg", m6502_reg_a);
      if (maskedtopbyte)
	emit6502op ("and", IMMDFMT, topbytemask);
      //      m6502_freeReg (m6502_reg_a);
      storeRegToFullAop (m6502_reg_a, AOP (result), SPEC_USIGN (operandType (left)));
      goto release;
    }

  /* If either left or result are in registers, handle this carefully to     */
  /* avoid prematurely overwriting register values. The 1 byte case was      */
  /* handled above and there aren't enough registers to handle 4 byte values */
  /* so this case only needs to deal with 2 byte values. */
  if (AOP_TYPE (result) == AOP_REG || AOP_TYPE (left) == AOP_REG)
    {
      reg_info *result0 = NULL;
      reg_info *left0 = NULL;
      reg_info *left1 = NULL;
      if (AOP_TYPE (left) == AOP_REG)
        {
          left0 = AOP (left)->aopu.aop_reg[0];
          left1 = AOP (left)->aopu.aop_reg[1];
        }
      if (AOP_TYPE (result) == AOP_REG)
        {
          result0 = AOP (result)->aopu.aop_reg[0];
        }
      needpula = pushRegIfSurv (m6502_reg_a);
      if (left1 == m6502_reg_a)
        pushReg (left1, true);

      if (left0 == m6502_reg_a) // TODO?
        rmwWithReg ("neg", m6502_reg_a);
      else {
        loadRegFromConst (m6502_reg_a, 0);
        emitSetCarry(1);
        accopWithAop ("sbc", AOP (left), 0);
      }
      if (result0 == m6502_reg_a || (result0 && result0 == left1))
        pushReg (m6502_reg_a, true);
      else
        storeRegToAop (m6502_reg_a, AOP (result), 0);
      loadRegFromConst (m6502_reg_a, 0);
      if (left1 == m6502_reg_a)
	{
	  // FIXME: unimplemented
	  m6502_unimplemented ("genUniminus with left1=A");
	  m6502_dirtyReg (m6502_reg_a);
	}
      else
	{
	  accopWithAop ("sbc", AOP (left), 1);
	}
      storeRegToAop (m6502_reg_a, AOP (result), 1);
      if (result0 == m6502_reg_a || (result0 && result0 == left1))
        pullReg (result0);
      if (left1 == m6502_reg_a)
        pullNull (1);
      goto release;
    }

  needpula = pushRegIfSurv (m6502_reg_a);
  for (offset=0; offset<size; offset++)
    {
      loadRegFromConst (m6502_reg_a, 0);
      if (carry)
	{
	  emitSetCarry(1);
	}
      accopWithAop ("sbc", AOP (left), offset);
      storeRegToAop (m6502_reg_a, AOP(result), offset);
      carry = false;
    }
  //  storeRegSignToUpperAop (m6502_reg_a, AOP(result), offset, SPEC_USIGN (operandType (left)));

 release:
  pullOrFreeReg (m6502_reg_a, needpula);
  freeAsmop (result, NULL);
  freeAsmop (left, NULL);
}

/**************************************************************************
 * saveRegisters - will look for a call and save the registers
 *************************************************************************/
static void saveRegisters (iCode *lic)
{
  int i;
  iCode *ic;

  /* look for call */
  for (ic = lic; ic; ic = ic->next)
    if (ic->op == CALL || ic->op == PCALL)
      break;

  if (!ic)
    {
      fprintf (stderr, "found parameter push with no function call\n");
      return;
    }

  /* if the registers have been saved already or don't need to be then
     do nothing */
  if (ic->regsSaved)
    return;
  if (IS_SYMOP (IC_LEFT (ic)) &&
      (IFFUNC_CALLEESAVES (OP_SYMBOL (IC_LEFT (ic))->type) || IFFUNC_ISNAKED (OP_SYM_TYPE (IC_LEFT (ic)))))
    return;

  if (!regalloc_dry_run)
    ic->regsSaved = 1;

  emitComment (REGOPS, "  saveRegisters");

  // make sure not to clobber A
  // TODO: why does isUsed not set?
  // TODO: only clobbered if m6502_reg_a->isFree

  bool clobbers_a = !IS_MOS65C02
    && (bitVectBitValue(ic->rSurv, X_IDX) || bitVectBitValue(ic->rSurv, Y_IDX))
    && !bitVectBitValue(ic->rSurv, A_IDX);

  if (clobbers_a && _S.sendSet)
    storeRegTemp (m6502_reg_a, true);

  for (i = A_IDX; i <= Y_IDX; i++)
    {
      if (bitVectBitValue (ic->rSurv, i))
	pushReg (m6502_regWithIdx (i), false);
    }
  if (clobbers_a && _S.sendSet)
    loadRegTemp (m6502_reg_a);
}

/**************************************************************************
 * unsaveRegisters - pop the pushed registers
 *************************************************************************/
static void unsaveRegisters (iCode *ic)
{
  int i;

  emitComment (REGOPS, "; unsaveRegisters");

  // TODO: only clobbered if m6502_reg_a->isFree

  bool clobbers_a = !IS_MOS65C02
    && (bitVectBitValue(ic->rSurv, X_IDX) || bitVectBitValue(ic->rSurv, Y_IDX))
    && !bitVectBitValue(ic->rSurv, A_IDX);
  if (clobbers_a)
    storeRegTemp (m6502_reg_a, true);
  for (i = Y_IDX; i >= A_IDX; i--)
    {
      if (bitVectBitValue (ic->rSurv, i))
	pullReg (m6502_regWithIdx (i));
    }
  if (clobbers_a)
    loadRegTemp (m6502_reg_a);
}

/**************************************************************************
 * pushSide
 * store oper to the RegTemp Stack
 * TODO: change function name
 * TODO: consider using DPTR
 *************************************************************************/
static void
pushSide (operand *oper, int size, iCode *ic)
{
  int offset = 0;
  //  bool xIsFree = m6502_reg_x->isFree;

  aopOp (oper, ic);

  if (AOP_TYPE (oper) == AOP_REG)
    {
      /* The operand is in registers; we can push them directly */
      storeRegTempAlways (AOP (oper)->aopu.aop_reg[0], true);
      storeRegTempAlways (AOP (oper)->aopu.aop_reg[1], true);
    }
  else
    {
      // push A if not free
      // TODO: consider other regs for 65C02
      bool needloada = pushRegIfUsed(m6502_reg_a);
      bool needloadx = false;
      if(AOP_TYPE(oper)==AOP_SOF) needloadx=pushRegIfUsed(m6502_reg_x);
      /* A is free, so piecewise load operand into a and push A */
      for (offset=0; offset<size; offset++)
	{
	  loadRegFromAop (m6502_reg_a, AOP (oper), offset);
	  storeRegTempAlways (m6502_reg_a, true);
	}
      pullOrFreeReg (m6502_reg_x, needloadx);
      pullOrFreeReg (m6502_reg_a, needloada);
    }

  freeAsmop (oper, NULL);
  //  if (xIsFree)
  //    m6502_freeReg (m6502_reg_x);
}

/**************************************************************************
 * assignResultValue
 *************************************************************************/
static void
assignResultValue (operand * oper)
{
  int size = AOP_SIZE (oper);
  int offset = 0;

  emitComment (TRACEGEN, __func__);

  if (size>8)
    {
      emitcode("ERROR","assignresultvalue return struct size: %d\n",size);
      return;
    }

  if (AOP_TYPE (oper) == AOP_REG)
    {
      if(size==1)
        {
          transferRegReg (m6502_reg_a, (AOP(oper))->aopu.aop_reg[0], true);
        }
      else
        {
          if (IS_AOP_YX(AOP(oper)))
            transferRegReg(m6502_reg_xa, m6502_reg_yx, true);
        }
      return;
    }

  if (AOP_TYPE (oper)==AOP_SOF && size>1)
    {
      int i;
      storeRegTemp (m6502_reg_x, true);
      for(i=0; i<size; i++)
	{
	  if (i==1)
	    {
	      loadRegTemp(m6502_reg_a);
	      storeRegToAop (m6502_reg_a, AOP (oper), i);
	    }
	  else
	    transferAopAop (m6502_aop_pass[i], 0, AOP (oper), i);
	}  
    }
  else
    {
      while (size--)
	{
	  transferAopAop (m6502_aop_pass[offset], 0, AOP (oper), offset);
	  if (m6502_aop_pass[offset]->type == AOP_REG)
	    m6502_freeReg (m6502_aop_pass[offset]->aopu.aop_reg[0]);
	  offset++;
	}
    }
}

/**************************************************************************
 * genIpush - generate code for pushing this gets a little complex
 *************************************************************************/
static void
genIpush (iCode * ic)
{
  operand *left   = IC_LEFT (ic);

  int size, offset = 0;

  emitComment (TRACEGEN, __func__);

  /* if this is not a parm push : ie. it is spill push
     and spill push is always done on the local stack */
  if (!ic->parmPush)
    {
      /* and the item is spilt then do nothing */
      if (OP_SYMBOL (left)->isspilt)
	return;

      aopOp (left, ic);
      size = AOP_SIZE (left);
      /* push it on the stack */
      for (offset=size-1; offset>=0; offset--)
	{
	  loadRegFromAop (m6502_reg_a, AOP (left), offset);
	  pushReg (m6502_reg_a, true);
	}
      return;
    }

  /* this is a parameter push: in this case we call
     the routine to find the call and save those
     registers that need to be saved */
  if (!regalloc_dry_run) /* Cost for saving registers is counted at CALL or PCALL */
    saveRegisters (ic);

  /* then do the push */
  aopOp (left, ic);

  // pushSide(IC_LEFT (ic), AOP_SIZE (IC_LEFT(ic)));
  size = AOP_SIZE (left);

  //  l = aopGet (AOP (left), 0, false, true);
  if (AOP_TYPE (left) == AOP_IMMD || AOP_TYPE (left) == AOP_LIT ||IS_AOP_YX (AOP (left))) {
    if ((size == 2) && m6502_reg_yx->isDead || IS_AOP_YX (AOP (left))) {
      loadRegFromAop (m6502_reg_yx, AOP (left), 0);
      pushReg (m6502_reg_yx, true);
      goto release;
    }
  }

  if (AOP_TYPE (left) == AOP_REG)
    {
      if (IS_AOP_XA (AOP (left)))
	{
	  pushReg (m6502_reg_xa, true);
	}
      else
	{
	  for (offset=size-1; offset>=0; offset--)
	    pushReg (AOP (left)->aopu.aop_reg[offset], true);
	}
      goto release;
    }

  for (offset=size-1; offset>=0; offset--) {
    //      printf ("loading %d\n", offset);
    loadRegFromAop (m6502_reg_a, AOP (left), offset);
    //      printf ("pushing \n");
    pushReg (m6502_reg_a, true);
  }

 release:
  freeAsmop (left, NULL);
}

/**************************************************************************
 * genPointerPush - generate code for pushing
 *************************************************************************/
static void
genPointerPush (iCode *ic)
{
  operand *left = IC_LEFT (ic);
  int yoff;

  emitComment (TRACEGEN, __func__);

  aopOp (left, ic);

  wassertl (IC_RIGHT (ic), "IPUSH_VALUE_AT_ADDRESS without right operand");
  wassertl (IS_OP_LITERAL (IC_RIGHT (ic)), "IPUSH_VALUE_AT_ADDRESS with non-literal right operand");
  wassertl (!operandLitValue (IC_RIGHT(ic)), "IPUSH_VALUE_AT_ADDRESS with non-zero right operand");

  yoff = setupDPTR(left, 0, NULL, false);


  int size = getSize (operandType (left)->next);
  while (size--)
    {
      loadRegFromConst (m6502_reg_y, yoff+size);
      emit6502op ("lda", INDFMT_IY, "DPTR");
      pushReg (m6502_reg_a, true);
    }

  freeAsmop (left, NULL);
}


/**************************************************************************
 * genSend - gen code for SEND
 *************************************************************************/
static void
genSend (set *sendSet)
{
  iCode *send1;
  iCode *send2;

  emitComment (TRACEGEN, __func__);

  /* case 1: single parameter in A
   * case 2: single parameter in XA
   * case 3: first parameter in A, second parameter in X
   */
  send1 = setFirstItem (sendSet);
  send2 = setNextItem (sendSet);

  if (!send2)
    {
      int size;
      /* case 1 or 2, this is fairly easy */
      aopOp (IC_LEFT (send1), send1);
      size = AOP_SIZE (IC_LEFT (send1));
      wassert (size <= 2);
      if (size == 1)
        {
          loadRegFromAop (m6502_reg_a, AOP (IC_LEFT (send1)), 0);
        }
      else if (isOperandVolatile (IC_LEFT (send1), false))
        {
          /* use lsb to msb order for volatile operands */
          loadRegFromAop (m6502_reg_a, AOP (IC_LEFT (send1)), 0);
          loadRegFromAop (m6502_reg_x, AOP (IC_LEFT (send1)), 1);
        }
      else
        {
	  loadRegFromAop (m6502_reg_xa, AOP (IC_LEFT (send1)), 0);      
        }
      freeAsmop (IC_LEFT (send1), NULL);
    }
  else
    {
      /* case 3 */
      /* make sure send1 is the first argument and swap with send2 if not */
      if (send1->argreg > send2->argreg)
        {
          iCode * sic = send1;
          send1 = send2;
          send2 = sic;
        }
      aopOp (IC_LEFT (send1), send1);
      aopOp (IC_LEFT (send2), send2);
      if (IS_AOP_A (AOP (IC_LEFT (send2))))
        {
          loadRegFromAop (m6502_reg_x, AOP (IC_LEFT (send2)), 0);
          loadRegFromAop (m6502_reg_a, AOP (IC_LEFT (send1)), 0);
        }
      else
        {
          loadRegFromAop (m6502_reg_a, AOP (IC_LEFT (send1)), 0);
          loadRegFromAop (m6502_reg_x, AOP (IC_LEFT (send2)), 0);
        }
      freeAsmop (IC_LEFT (send2), NULL);
      freeAsmop (IC_LEFT (send1), NULL);
    }
}

/**************************************************************************
 * genCall - generates a call statement
 *************************************************************************/
static void
genCall (iCode * ic)
{
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  sym_link *dtype;
  sym_link *etype;
  //  bool restoreBank = false;
  //  bool swapBanks = false;

  emitComment (TRACEGEN, __func__);
  printIC (ic);

  /* if caller saves & we have not saved then */
  if (!ic->regsSaved)
    saveRegisters (ic);

  dtype = operandType (left);
  etype = getSpec (dtype);
  /* if send set is not empty then assign */
  if (_S.sendSet && !regalloc_dry_run)
    {
      if (IFFUNC_ISREENT (dtype))
	{
	  /* need to reverse the send set */
	  //genSend (_S.sendSet);
	  genSend (reverseSet (_S.sendSet));
	} else {
	genSend (_S.sendSet);
      }
      _S.sendSet = NULL;
    }

  /* make the call */
  if (IS_LITERAL (etype))
    {
      emit6502op ("jsr", "0x%04X", ulFromVal (OP_VALUE (left)));
    }
  else
    {
      bool jump = (!ic->parmBytes && IFFUNC_ISNORETURN (OP_SYMBOL (left)->type));

      emit6502op (jump ? "jmp" : "jsr", "%s", (OP_SYMBOL (left)->rname[0] ?
					       OP_SYMBOL (left)->rname : OP_SYMBOL (left)->name));
    }

  m6502_dirtyAllRegs ();

  _S.DPTRAttr[0].isLiteral=0;
  _S.DPTRAttr[1].isLiteral=0;
  _S.DPTRAttr[0].aop=NULL;
  _S.DPTRAttr[1].aop=NULL;
  _S.lastflag=-1;
  _S.carryValid=0;


  /* do we need to recompute the base ptr? */
  if (_S.funcHasBasePtr)
    {
      saveBasePtr();
    }

  /* if we need assign a result value */
  if ((IS_ITEMP (result) &&
       (OP_SYMBOL (result)->nRegs || OP_SYMBOL (result)->spildir)) || IS_TRUE_SYMOP (result))
    {
      m6502_useReg (m6502_reg_a);
      if (operandSize (result) > 1)
	m6502_useReg (m6502_reg_x);
      aopOp (result, ic);

      assignResultValue (result);

      freeAsmop (result, NULL);
    }

  /* adjust the stack for parameters if required */
  if (ic->parmBytes) {
    pullNull (ic->parmBytes);
  }

  /* if we had saved some registers then unsave them */
  if (ic->regsSaved && !IFFUNC_CALLEESAVES (dtype))
    unsaveRegisters (ic);
}

/**************************************************************************
 * genPcall - generates a call by pointer statement
 *************************************************************************/
static void
genPcall (iCode * ic)
{
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  sym_link *dtype;
  sym_link *etype;
  iCode * sendic;

  emitComment (TRACEGEN, __func__);
  printIC(ic);

  dtype = operandType (left)->next;
  etype = getSpec (dtype);
  /* if caller saves & we have not saved then */
  if (!ic->regsSaved)
    saveRegisters (ic);

  /* Go through the send set and mark any registers used by iTemps as */
  /* in use so we don't clobber them while setting up the return address */
  for (sendic = setFirstItem (_S.sendSet); sendic; sendic = setNextItem (_S.sendSet)) {
    updateiTempRegisterUse (IC_LEFT (sendic));
  }

  // TODO: handle DIR/EXT with jmp [aa] or jmp [aaaa]

  if (!IS_LITERAL (etype)) {
    updateCFA ();
    /* compute the function address */
    pushSide (left, FARPTRSIZE, ic); // -1 is baked into initialization
  }

  /* if send set is not empty then assign */
  if (_S.sendSet && !regalloc_dry_run) {
    genSend (reverseSet (_S.sendSet));
    _S.sendSet = NULL;
  }

  /* make the call */
  if (!IS_LITERAL (etype)) {
    emit6502op("jsr","__sdcc_indirect_jsr");
    // pushSide put address in RegTemp
    // perhaps use DPTR instead?
    loadRegTemp (NULL);
    loadRegTemp (NULL);
    updateCFA ();
  }
  else
    {
      emit6502op ("jsr", "0x%04X", ulFromVal (OP_VALUE (left)));
    }

  m6502_dirtyAllRegs ();

  _S.DPTRAttr[0].isLiteral=0;
  _S.DPTRAttr[1].isLiteral=0;
  _S.DPTRAttr[0].aop=NULL;
  _S.DPTRAttr[1].aop=NULL;
  _S.lastflag=-1;
  _S.carryValid=0;

  /* do we need to recompute the base ptr? */
  if (_S.funcHasBasePtr)
    {
      saveBasePtr();
    }

  /* if we need assign a result value */
  if ((IS_ITEMP (result) &&
       (OP_SYMBOL (result)->nRegs || OP_SYMBOL (result)->spildir)) || IS_TRUE_SYMOP (result)) {
    m6502_useReg (m6502_reg_a);
    if (operandSize (result) > 1)
      m6502_useReg (m6502_reg_x);
    aopOp (result, ic);

    assignResultValue (result);

    freeAsmop (result, NULL);
  }

  /* adjust the stack for parameters if required */
  if (ic->parmBytes) {
    pullNull (ic->parmBytes);
  }

  /* if we had saved some registers then unsave them */
  if (ic->regsSaved && !IFFUNC_CALLEESAVES (dtype))
    unsaveRegisters (ic);
}

/**************************************************************************
 * resultRemat - result  is rematerializable
 *************************************************************************/
static int
resultRemat (iCode * ic)
{
  operand *result = IC_RESULT (ic);

  if (SKIP_IC (ic) || ic->op == IFX)
    return 0;

  if (result && IS_ITEMP (result)) {
    symbol *sym = OP_SYMBOL (result);
    if (sym->remat && !POINTER_SET (ic))
      return 1;
  }

  return 0;
}

/**************************************************************************
 * inExcludeList - return 1 if the string is in exclude Reg list
 *************************************************************************/
static int
regsCmp (void *p1, void *p2)
{
  return (STRCASECMP ((char *) p1, (char *) (p2)) == 0);
}

static bool inExcludeList (char *s)
{
  const char *p = setFirstItem (options.excludeRegsSet);

  if (p == NULL || STRCASECMP (p, "none") == 0)
    return false;


  return isinSetWith (options.excludeRegsSet, s, regsCmp);
}

/**************************************************************************
 * genFunction - generated code for function entry
 *************************************************************************/
static void
genFunction (iCode * ic)
{
  symbol *sym = OP_SYMBOL (IC_LEFT (ic));
  sym_link *ftype;
  iCode *ric = (ic->next && ic->next->op == RECEIVE) ? ic->next : NULL;
  int stackAdjust = sym->stack;
  //  int accIsFree = sym->recvSize == 0;

  /* create the function header */
  emitComment (ALWAYS, "-----------------------------------------");
  emitComment (ALWAYS, " function %s", sym->name);
  emitComment (ALWAYS, "-----------------------------------------");
  emitComment (ALWAYS, m6502_assignment_optimal ? "Register assignment is optimal." : "Register assignment might be sub-optimal.");
  emitComment (ALWAYS, "Stack space usage: %d bytes.", sym->stack);

  emitcode ("", "%s:", sym->rname);
  genLine.lineCurr->isLabel = 1;
  ftype = operandType (IC_LEFT (ic));

  if (sym->recvSize>0)
    {
      m6502_useReg (m6502_reg_a);
      m6502_reg_a->isDead=0;
    }
  if (sym->recvSize>1)
    {
      m6502_useReg (m6502_reg_x);
      m6502_reg_x->isDead=0;
    }

  _S.stackOfs = 0;
  _S.stackPushes = 0;
  _S.tsxStackPushes = 0;
  _S.lastflag=-1;
  _S.carryValid=0;

  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, m6502_reg_sp, 0);

  if (IFFUNC_ISNAKED (ftype))
    {
      emitComment (ALWAYS, "naked function: no prologue.");
      return;
    }

  /* if this is an interrupt service routine then
     save a, x & y  */
  if (IFFUNC_ISISR (sym->type))
    {
      if (!inExcludeList ("a"))
        pushReg (m6502_reg_a, true);
      if (!inExcludeList ("x"))
        pushReg (m6502_reg_x, true);
      if (!inExcludeList ("y"))
        pushReg (m6502_reg_y, true);
    }

  /* For some cases it is worthwhile to perform a RECEIVE iCode */
  /* before setting up the stack frame completely. */
  int numStackParams = 0;
  while (ric && ric->next && ric->next->op == RECEIVE)
    ric = ric->next;
  while (ric && IC_RESULT (ric))
    {
      symbol *rsym = OP_SYMBOL (IC_RESULT (ric));
      int rsymSize = rsym ? getSize (rsym->type) : 0;

      if (rsym->isitmp)
        {
          if (rsym && rsym->regType == REG_CND)
            rsym = NULL;
          if (rsym && (/*rsym->accuse ||*/ rsym->ruonly))
            rsym = NULL;
          if (rsym && (rsym->isspilt || rsym->nRegs == 0) && rsym->usl.spillLoc)
            rsym = rsym->usl.spillLoc;
        }

      /* If the RECEIVE operand immediately spills to the first entry on the  */
      /* stack, we can push it directly rather than use an sp relative store. */
      if (rsym && rsym->onStack && rsym->stack == -_S.stackPushes - rsymSize)
        {
          int ofs;

          genLine.lineElement.ic = ric;
          emitComment (TRACEGEN, "genReceive: size=%d", rsymSize);
          //          for (ofs = 0; ofs < rsymSize; ofs++)
	  m6502_useReg (m6502_reg_a);
          for (ofs = rsymSize-1; ofs >=0; ofs--)
            {
              reg_info *reg = m6502_aop_pass[ofs + (ric->argreg - 1)]->aopu.aop_reg[0];
              emitComment (TRACEGEN, "pushreg: ofs=%d", ofs);
              pushReg (reg, true);
              //              if (reg->rIdx == A_IDX)
              //                accIsFree = 1;
              stackAdjust--;
            }
          genLine.lineElement.ic = ic;
          ric->generated = 1;
        }
      ric = (ric->prev && ric->prev->op == RECEIVE) ? ric->prev : NULL;
    }

  /* adjust the stack for the function */
  if (stackAdjust)
    adjustStack (-stackAdjust);

  _S.stackOfs = sym->stack;
  _S.stackPushes = 0;
  _S.tsxStackPushes = 0;
  _S.funcHasBasePtr = 0;
  // TODO: how to see if needed? how to count params?
  if ( stackAdjust || sym->stack || numStackParams || IFFUNC_ISREENT(sym->type) )
    {
      saveBasePtr();
      _S.funcHasBasePtr = 1;
    }

  /* if critical function then turn interrupts off */
  if (IFFUNC_ISCRITICAL (ftype))
    {
      emit6502op ("php", "");
      emit6502op ("sei", "");
    }
}

/**************************************************************************
 * genEndFunction - generates epilogue for functions
 *************************************************************************/
static void
genEndFunction (iCode * ic)
{
  symbol *sym = OP_SYMBOL (IC_LEFT (ic));

  emitComment (TRACEGEN, __func__);
  emitComment (REGOPS, "  %s %s", __func__, regInfoStr() );

  if (IFFUNC_ISNAKED (sym->type))
    {
      emitComment (ALWAYS, "naked function: no epilogue.");
      if (options.debug && currFunc && !regalloc_dry_run)
        debugFile->writeEndFunction (currFunc, ic, 0);
      return;
    }

  if (IFFUNC_ISCRITICAL (sym->type))
    {
      emit6502op ("plp", "");
    }

  if (IFFUNC_ISREENT (sym->type) || options.stackAuto)
    {
    }

  if (_S.funcHasBasePtr)
    restoreBasePtr();

  if (_S.stackPushes)
    emitcode("ERROR","_S.stackPushes=%d in genEndFunction", _S.stackPushes);

  if (sym->stack)
    {
      //  if (sym->regsUsed && sym->regsUsed->size)
      // FIXME: need to figure out how to get the exact registers needed by the function return
      m6502_useReg(m6502_reg_a);
      m6502_useReg(m6502_reg_x);
      _S.stackPushes += sym->stack;
      adjustStack (sym->stack);
    }

  if (IFFUNC_ISISR (sym->type))
    {
      if (!inExcludeList ("y"))
        pullReg (m6502_reg_y);
      if (!inExcludeList ("x"))
        pullReg (m6502_reg_x);
      if (!inExcludeList ("a"))
        pullReg (m6502_reg_a);

      /* if debug then send end of function */
      if (options.debug && currFunc && !regalloc_dry_run)
        {
          debugFile->writeEndFunction (currFunc, ic, 1);
        }

      emit6502op ("rti", "");
    }
  else
    {
      if (IFFUNC_CALLEESAVES (sym->type))
        {
          int i;

          /* if any registers used */
          if (sym->regsUsed)
            {
              /* save the registers used */
              for (i = sym->regsUsed->size; i >= 0; i--)
                {
                  if (bitVectBitValue (sym->regsUsed, i) || (m6502_ptrRegReq && (i == YX_IDX || i == YX_IDX)))
                    {
                      // FIXME
                      emitcode ("pop", "%s", m6502_regWithIdx (i)->name); /* Todo: Cost. Can't find this instruction in manual! */
                    }
                }
            }
        }

      /* if debug then send end of function */
      if (options.debug && currFunc && !regalloc_dry_run)
        {
          debugFile->writeEndFunction (currFunc, ic, 1);
        }

      emit6502op ("rts", "");
    }
}

/**************************************************************************
 * genRet - generate code for return statement
 *************************************************************************/
static void genRet (iCode * ic)
{
  operand *left   = IC_LEFT (ic);

  int size, offset = 0;
  //  int pushed = 0;
  bool delayed_x = false;

  emitComment (TRACEGEN, __func__);
  emitComment (REGOPS, "  %s %s", __func__, regInfoStr() );


  /* if we have no return value then
     just generate the "ret" */
  if (!left)
    goto jumpret;

  /* we have something to return then
     move the return value into place */
  aopOp (left, ic);
  size = AOP_SIZE (left);
  const bool bigreturn = IS_STRUCT (operandType (left));

  if (bigreturn)
    {
      // FIXME: only up to size 8 is supported
      if(size>8)
        {
          if (!regalloc_dry_run)
            emitcode ("ERROR","  %s: return size>8 not supported", __func__);
          goto jumpret;
        }

      for(offset=size-1; offset>=2; offset--)
        transferAopAop (AOP (left), offset, m6502_aop_pass[offset], 0);

      loadRegFromAop (m6502_reg_xa, AOP (left), 0);
      goto jumpret;
    }

  if (AOP_TYPE (left) == AOP_LIT)
    {
      /* If returning a literal, we can load the bytes of the return value */
      /* in any order. By loading A and X first, any other bytes that match */
      /* can use the shorter sta and stx instructions. */
      offset = 0;
      while (size--)
        {
	  transferAopAop (AOP (left), offset, m6502_aop_pass[offset], 0);
	  offset++;
	}
    }
  else
    {
      /* Take care when swapping a and x */
      if (AOP_TYPE (left) == AOP_REG && size > 1 && AOP (left)->aopu.aop_reg[0]->rIdx == X_IDX)
        {
	  delayed_x = true;
	  storeRegTemp (m6502_reg_x, true);
	}

      offset = size - 1;
      if(size>1 && AOP_TYPE(left)==AOP_SOF)
	{
	  int i;
	  for (i=size-1; i>1; i--)
	    {
	      loadRegFromAop (m6502_reg_a, AOP(left), i);
	      storeRegToAop (m6502_reg_a, m6502_aop_pass[i], 0);
	    }
	  loadRegFromAop (m6502_reg_a, AOP(left), 1);
	  storeRegTemp (m6502_reg_a, true);
	  loadRegFromAop (m6502_reg_a, AOP(left), 0);
	  loadRegTemp (m6502_reg_x);
	}
      else
	{
	  while (size--)
	    {
	      if (!(delayed_x && !offset))
		transferAopAop (AOP (left), offset, m6502_aop_pass[offset], 0);
	      offset--;
	    }
	}

      if (delayed_x)
	loadRegTemp (m6502_reg_a);
    }

  freeAsmop (left, NULL);

 jumpret:
  /* generate a jump to the return label
     if the next is not the return statement */
  if (!(ic->next && ic->next->op == LABEL && IC_LABEL (ic->next) == returnLabel))
    {
      emit6502op ("jmp", "%05d$", safeLabelNum (returnLabel));
    }
}

/**************************************************************************
 * genLabel - generates a label
 *************************************************************************/
static void genLabel (iCode * ic)
{
  int i;

  emitComment (TRACEGEN, __func__);
  emitComment (REGOPS, "  %s %s", __func__, regInfoStr() );


  for(i=0;i<NUM_TEMP_REGS;i++)
    dirtyRegTemp (i);

  _S.DPTRAttr[0].isLiteral=0;
  _S.DPTRAttr[1].isLiteral=0;
  _S.DPTRAttr[0].aop=NULL;
  _S.DPTRAttr[1].aop=NULL;

  _S.lastflag=-1;
  _S.carryValid=0;

  /* special case never generate */
  if (IC_LABEL (ic) == entryLabel)
    return;

  /* For the high level labels we cannot depend on any */
  /* register's contents. Amnesia time.                */
  m6502_dirtyAllRegs();

  if (options.debug && !regalloc_dry_run)
    debugFile->writeLabel (IC_LABEL (ic), ic);

  safeEmitLabel (IC_LABEL (ic));
}

/**************************************************************************
 * genGoto - generates a jmp
 *************************************************************************/
static void genGoto (iCode * ic)
{
  emitComment (TRACEGEN, __func__);
  emit6502op ("jmp", "%05d$", safeLabelNum (IC_LABEL (ic)));
}



/**************************************************************************
 * genMult - generates code for multiplication
 *************************************************************************/
static void
genMult (iCode * ic)
{
  /* Shouldn't occur - all done through function calls */
  wassertl (0, "Multiplication is handled through support function calls");
}

/**************************************************************************
 * genDiv - generates code for division
 *************************************************************************/
static void
genDiv (iCode * ic)
{
  /* Shouldn't occur - all done through function calls */
  wassertl (0, "Division is handled through support function calls");
}

/**************************************************************************
 * genMod - generates code for division
 *************************************************************************/
static void
genMod (iCode * ic)
{
  /* Shouldn't occur - all done through function calls */
  wassertl (0, "Division is handled through support function calls");
}

/**************************************************************************
 * genIfxJump :- will create a jump depending on the ifx
 *************************************************************************/
void
genIfxJump (iCode * ic, char *jval)
{
  symbol *jlbl;
  symbol *tlbl = safeNewiTempLabel (NULL);
  char *inst = "ERROR";

  emitComment (TRACEGEN, "%s : %s", __func__, jval);

  /* if true label then we jump if condition
     supplied is true */
  if (IC_TRUE (ic))
    {
      jlbl = IC_TRUE (ic);
      if (!strcmp (jval, "z"))
	inst = "beq";
      else if (!strcmp (jval, "c"))
	inst = "bcc";
      else if (!strcmp (jval, "n"))
	inst = "bpl";
      else if (!strcmp (jval, "v"))
	inst = "bvc";
      else
	emitcode("ERROR", "; %s IC_TRUE: Unimplemented jval (%s)", __func__, jval );
      //      inst = "bge";
    }
  else
    {
      /* false label is present */
      jlbl = IC_FALSE (ic);
      if (!strcmp (jval, "z"))
	inst = "bne";
      else if (!strcmp (jval, "c"))
	inst = "bcs";
      else if (!strcmp (jval, "n"))
	inst = "bmi";
      else if (!strcmp (jval, "v"))
	inst = "bvs";
      else
	emitcode("ERROR", "; %s IC_FALSE: Unimplemented jval (%s)", __func__, jval );
      //      inst = "blt";
    }
  emitBranch (inst, tlbl);
  emitBranch ("jmp", jlbl);
  safeEmitLabel (tlbl);

  /* mark the icode as generated */
  ic->generated = 1;
}


/**************************************************************************
 * exchangedCmp : returns the opcode need if the two operands are
 *                exchanged in a comparison
 *************************************************************************/
static int
exchangedCmp (int opcode)
{
  switch (opcode)
    {
    case '<':
      return '>';
    case '>':
      return '<';
    case LE_OP:
      return GE_OP;
    case GE_OP:
      return LE_OP;
    case NE_OP:
      return NE_OP;
    case EQ_OP:
      return EQ_OP;
    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "opcode not a comparison");
    }
  return EQ_OP;                 /* shouldn't happen, but need to return something */
}

/**************************************************************************
 * negatedCmp : returns the equivalent opcode for when a comparison
 *              if not true
 *************************************************************************/
static int
negatedCmp (int opcode)
{
  switch (opcode)
    {
    case '<':
      return GE_OP;
    case '>':
      return LE_OP;
    case LE_OP:
      return '>';
    case GE_OP:
      return '<';
    case NE_OP:
      return EQ_OP;
    case EQ_OP:
      return NE_OP;
    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "opcode not a comparison");
    }
  return EQ_OP;                 /* shouldn't happen, but need to return something */
}

/**************************************************************************
 * nameCmp : helper function for human readable debug output
 *************************************************************************/
static char *
nameCmp (int opcode)
{
  switch (opcode)
    {
    case '<':
      return "<";
    case '>':
      return ">";
    case LE_OP:
      return "<=";
    case GE_OP:
      return ">=";
    case NE_OP:
      return "!=";
    case EQ_OP:
      return "==";
    default:
      return "invalid";
    }
}

/**************************************************************************
 * branchInstCmp : returns the conditional branch instruction that
 *                 will branch if the comparison is true
 *************************************************************************/
static char *
branchInstCmp (int opcode, int sign)
{
  switch (opcode)
    {
    case '<':
      if (sign)
	return "blt";
      else
	return "bcc";
    case '>':
      if (sign)
	return "bgt";
      else
	return "bhi";
    case LE_OP:
      if (sign)
	return "ble";
      else
	return "bls";
    case GE_OP:
      if (sign)
	return "bge";
      else
	return "bcs";
    case NE_OP:
      return "bne";
    case EQ_OP:
      return "beq";
    default:
      werror (E_INTERNAL_ERROR, __FILE__, __LINE__, "opcode not a comparison");
    }
  return "brn";
}


/**************************************************************************
 * genCmp :- greater or less than (and maybe with equal) comparison
 *************************************************************************/
static void
genCmp (iCode * ic, iCode * ifx)
{
  operand *right  = IC_RIGHT (ic);
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  sym_link *letype, *retype;
  int sign, opcode;
  int size, offset = 0;
  unsigned long long lit = 0ull;
  char *sub;
  symbol *jlbl = NULL;
  bool needloada = false;
  bool bmi = false;
  bool bit = false;

  emitComment (TRACEGEN, __func__);

  opcode = ic->op;

  // TODO: optimize for cmp regs with 0 or constant

  sign = 0;
  // TODO: don't use signed when unsigned will do
  if (IS_SPEC (operandType (left)) && IS_SPEC (operandType (right)))
    {
      letype = getSpec (operandType (left));
      retype = getSpec (operandType (right));
      sign = !(SPEC_USIGN (letype) | SPEC_USIGN (retype));
    }

  /* assign the amsops */
  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);
  printIC (ic);

  /* need register operand on left, prefer literal operand on right */
  if ((AOP_TYPE (right) == AOP_REG) || AOP_TYPE (left) == AOP_LIT)
    {
      // don't swap if left is A
      // FIXME: not sure if this is necessary
      if(!((AOP_TYPE (left) == AOP_REG) && AOP (left)->aopu.aop_reg[0]->rIdx == A_IDX))
        {
          operand *temp = left;
          left = right;
          right = temp;
          opcode = exchangedCmp (opcode);
        }
    }
  // TODO: special case for compare with 0

  if (ifx)
    {
      if (IC_TRUE (ifx))
        {
          jlbl = IC_TRUE (ifx);
          opcode = negatedCmp (opcode);
        }
      else
        {
          /* false label is present */
          jlbl = IC_FALSE (ifx);
        }
    }

  size = max (AOP_SIZE (left), AOP_SIZE (right));

  emitComment (TRACEGEN|VVDBG, "  %s (%s, size %d, sign %d)", __func__,
               nameCmp (opcode), size, sign);

  if (sign && (AOP_TYPE (right) == AOP_LIT) && opcode=='<' && ullFromVal (AOP (right)->aopu.aop_lit) ==0 && canBitOp(left) )
    {
      accopWithAop ("bit", AOP (left), size-1);
      bit=true;
      bmi=true;
    }
  else if (sign && (AOP_TYPE (right) == AOP_LIT) && opcode==GE_OP && ullFromVal (AOP (right)->aopu.aop_lit) ==0 && canBitOp(left) )
    {
      accopWithAop ("bit", AOP (left), size-1);
      bit=true;
      bmi=false;
    }
  else if (!sign && size == 1 && IS_AOP_X (AOP (left)) && isAddrSafe(right, m6502_reg_x) )
    {
      //    if (AOP_TYPE (right) == AOP_LIT && ullFromVal (AOP (right)->aopu.aop_lit) == 0)
      //     emitCmp(m6502_reg_x, 0);
      //   else
      accopWithAop ("cpx", AOP (right), offset);
    }
  else if (!sign && size == 1 && IS_AOP_Y (AOP (left)) && isAddrSafe(right, m6502_reg_y) )
    {
      //   if (AOP_TYPE (right) == AOP_LIT && ullFromVal (AOP (right)->aopu.aop_lit) == 0)
      //    emitCmp(m6502_reg_y, 0);
      //   else
      accopWithAop ("cpy", AOP (right), offset);
    }
  else if (!sign && size == 1 && IS_AOP_A (AOP (left)) && isAddrSafe(right, m6502_reg_a))
    {
      if (AOP_TYPE (right) == AOP_LIT && ullFromVal (AOP (right)->aopu.aop_lit) == 0 && opcode=='>')
        {
#if 1
          symbol *tlbl3 = safeNewiTempLabel (NULL);
	  emitCmp (m6502_reg_a, 0);
          emitBranch ("beq", tlbl3);
          loadRegFromConst (m6502_reg_a, 1);
          safeEmitLabel (tlbl3);
          goto release;
#else
          // in some corner cases the branches need carry
	  if(!emitCmp (m6502_reg_a, 0))
            emit6502op ("sec","");
#endif
        }
      else
        {
          accopWithAop ("cmp", AOP (right), offset);
        }
    }
  else
    {
      offset = 0;
      // need V flag for signed compare
      // FIXME: is this covered above?
      if (size == 1 && sign == 0)
        {
          sub = "cmp";
        }
      else
        {
          sub = "sub";

          /* These conditions depend on the Z flag bit, but Z is */
          /* only valid for the last byte of the comparison, not */
          /* the whole value. So exchange the operands to get a  */
          /* comparison that doesn't depend on Z. (This is safe  */
          /* to do here since ralloc won't assign multi-byte     */
          /* operands to registers for comparisons)              */
          if ((opcode == '>') || (opcode == LE_OP))
            {
              operand *temp = left;
              left = right;
              right = temp;
              opcode = exchangedCmp (opcode);
            }

          if ((AOP_TYPE (right) == AOP_LIT) && !isOperandVolatile (left, false))
            {
              lit = ullFromVal (AOP (right)->aopu.aop_lit);
              while ((size > 1) && (((lit >> (8 * offset)) & 0xff) == 0))
                {
                  offset++;
                  size--;
                }
            }
        }
      needloada = storeRegTempIfSurv (m6502_reg_a);
      while (size--)
        {
          emitComment (TRACEGEN|VVDBG, "   GenCmp - size counter = %d", size);

          if (AOP_TYPE (right) == AOP_REG && AOP(right)->aopu.aop_reg[offset]->rIdx == A_IDX)
            {
              emitComment (TRACEGEN|VVDBG, "   GenCmp - REG_A");
	      if(!needloada)
		storeRegTemp (m6502_reg_a, true);
              loadRegFromAop (m6502_reg_a, AOP (left), offset);
	      if (!strcmp (sub, "sub"))
		{
		  emitSetCarry (1);
		  emitRegTempOp("sbc", getLastTempOfs() );
		}
	      else
		{
		  emitRegTempOp(sub, getLastTempOfs() );
		}
	      if(!needloada) 
		loadRegTemp(NULL);
            }
          else
            {
              emitComment (TRACEGEN|VVDBG, "   GenCmp - generic");
              loadRegFromAop (m6502_reg_a, AOP (left), offset);
              if (!strcmp(sub, "sub"))
		{
		  emitSetCarry (1);
		  accopWithAop ("sbc", AOP (right), offset);
		}
	      else
		{
		  accopWithAop (sub, AOP (right), offset);
		}
            }
          m6502_freeReg (m6502_reg_a);
          offset++;
          sub = "sbc";
        }
    }

  if (ifx)
    {
      symbol *tlbl = safeNewiTempLabel (NULL);
      char *inst;



      if (!bit)
	{
	  inst = branchInstCmp (opcode, sign);
	  if(needloada)
            {
	      if (!strcmp(inst,"bcc") || !strcmp(inst,"bcs"))
		loadRegTemp(m6502_reg_a);
	      else
		loadRegTempNoFlags (m6502_reg_a, needloada);
            }
	  else
	    m6502_freeReg (m6502_reg_a);
          emitBranch (inst, tlbl);
        }
      else
        {
	  if (needloada)
	    loadRegTempNoFlags (m6502_reg_a, needloada);
	  else
	    m6502_freeReg (m6502_reg_a);

          if(bmi) emitBranch ("bmi", tlbl);
          else emitBranch ("bpl", tlbl);
        }
      emitBranch ("jmp", jlbl);
      safeEmitLabel (tlbl);

      /* mark the icode as generated */
      ifx->generated = 1;
    }
  else
    {
      symbol *true_lbl = safeNewiTempLabel (NULL);
      symbol *tlbl2 = safeNewiTempLabel (NULL);

      if (!needloada)
        needloada = storeRegTempIfSurv (m6502_reg_a);

      if(!bit)
        {
          emitBranch (branchInstCmp (opcode, sign), true_lbl);
        }
      else
        {
          if(bmi) emitBranch ("bmi", true_lbl);
          else emitBranch ("bpl", true_lbl);
        }
      loadRegFromConst (m6502_reg_a, 0);
      emitBranch ("bra", tlbl2);
      safeEmitLabel (true_lbl);
      loadRegFromConst (m6502_reg_a, 1);
      safeEmitLabel (tlbl2);
      m6502_dirtyReg (m6502_reg_a);
      storeRegToFullAop (m6502_reg_a, AOP (result), false);
      loadOrFreeRegTemp (m6502_reg_a, needloada);
    }

 release:
  freeAsmop (right, NULL);
  freeAsmop (left, NULL);
  freeAsmop (result, NULL);
}


/**************************************************************************
 * genCmpEQorNE - equal or not equal comparison
 *************************************************************************/
static void
genCmpEQorNE (iCode * ic, iCode * ifx)
{
  operand *right  = IC_RIGHT (ic);
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);
  int opcode;
  int size;
  symbol *jlbl = NULL;
  symbol *tlbl_NE = NULL;
  symbol *tlbl_EQ = NULL;
  bool needloada = false;
  int offset = 0;

  emitComment (TRACEGEN, __func__);

  opcode = ic->op;

  emitComment (TRACEGEN|VVDBG, "      genCmpEQorNE (%s)", nameCmp (opcode));

  /* assign the amsops */
  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);
  printIC (ic);

  /* need register operand on left, prefer literal operand on right */
  if ((AOP_TYPE (right) == AOP_REG) || AOP_TYPE (left) == AOP_LIT)
    {
      // don't swap if left is A
      if(!((AOP_TYPE (left) == AOP_REG) && AOP (left)->aopu.aop_reg[0]->rIdx == A_IDX))
	{
	  operand *temp = left;
	  left = right;
	  right = temp;
	  opcode = exchangedCmp (opcode);
	}
    }

  size = max (AOP_SIZE (left), AOP_SIZE (right));

  if (ifx)
    {
      if (IC_TRUE (ifx))
	{
	  jlbl = IC_TRUE (ifx);
	  opcode = negatedCmp (opcode);
	}
      else
	{
	  /* false label is present */
	  jlbl = IC_FALSE (ifx);
	}
    }

  if(AOP_TYPE (left) == AOP_REG)
    emitComment (TRACEGEN|VVDBG, "   genCmpEQorNE left is reg: %s",AOP (left)->aopu.aop_reg[offset]->name);
  else
    emitComment (TRACEGEN|VVDBG, "   genCmpEQorNE left is not not a reg");

  if(ifx && size==1 && AOP_TYPE(right)==AOP_LIT && AOP_TYPE(left)!=AOP_SOF)
    {
      bool restore_a=false;
      reg_info *reg;

      if(AOP_TYPE(left)==AOP_REG)
	{
	  reg=m6502_regWithIdx(AOP(left)->aopu.aop_reg[0]->rIdx);
	}
      else
	{
	  reg=getFreeByteReg();
	  if(!reg)
	    {
	      reg=m6502_reg_a;
	      restore_a=storeRegTempIfSurv(m6502_reg_a);
	    }
	  loadRegFromAop (reg, AOP(left), 0);
	}
      emitCmp(reg, ullFromVal (AOP(right)->aopu.aop_lit));
      if(restore_a)
	loadRegTempNoFlags(m6502_reg_a, true);
    }
  else
    // TODO: could clobber A if reg = XA?
    {
      offset = 0;
      while (size--)
	{
	  if (AOP_TYPE (left) == AOP_REG && AOP (left)->aopu.aop_reg[offset]->rIdx == X_IDX 
              && isAddrSafe(right, m6502_reg_x))
	    accopWithAop ("cpx", AOP (right), offset);
	  else if (AOP_TYPE (left) == AOP_REG && AOP (left)->aopu.aop_reg[offset]->rIdx == Y_IDX
		   && isAddrSafe(right, m6502_reg_y))
	    accopWithAop ("cpy", AOP (right), offset);
	  else 
	    {
	      emitComment (TRACEGEN|VVDBG, "      genCmpEQorNE can't cpx or cpy");

	      // TODO? why do we push when we could cpx?
	      if (!(AOP_TYPE (left) == AOP_REG && AOP (left)->aopu.aop_reg[offset]->rIdx == A_IDX))
		{
		  if(AOP_TYPE(right) == AOP_REG)
		    {
		      emitComment (TRACEGEN|VVDBG, "   genCmpEQorNE right is reg: %s",AOP (right)->aopu.aop_reg[offset]->name);
		    }

		  // FIXME: always?
		  //                  storeRegTemp (m6502_reg_a, true);
		  //                  needloada = true;
		  needloada = storeRegTempIfSurv(m6502_reg_a);
		  //loadRegFromAop (m6502_reg_a, AOP (left), offset);
		}
              if(m6502_reg_x->aop&&sameRegs(m6502_reg_x->aop, AOP(left))&&m6502_reg_x->aopofs==offset)
                {
		  accopWithAop ("cpx", AOP (right), offset);
                }
              else
                {
		  loadRegFromAop (m6502_reg_a, AOP (left), offset);
		  accopWithAop ("ucp", AOP (right), offset);
		}
	      loadRegTempNoFlags (m6502_reg_a, needloada);
	      needloada = false;
	    }
	  if (size)
	    {
	      symbol *tmp_label = safeNewiTempLabel (NULL);;
	      if (!tlbl_NE)
		tlbl_NE = safeNewiTempLabel (NULL);
	      if (!needloada && !ifx)
		needloada = storeRegTempIfSurv (m6502_reg_a);

	      emitBranch ("beq", tmp_label);
	      emitBranch ("jmp", tlbl_NE);
	      safeEmitLabel (tmp_label);

	      loadOrFreeRegTemp (m6502_reg_a, needloada);
	      needloada = false;
	    }
	  offset++;
	}
    }

  if (ifx)
    {
      if (opcode == EQ_OP)
	{
	  if (!tlbl_EQ)
	    tlbl_EQ = safeNewiTempLabel (NULL);
	  emitBranch ("beq", tlbl_EQ);
	  if (tlbl_NE)
	    safeEmitLabel (tlbl_NE);
	  emitBranch ("jmp", jlbl);
	  safeEmitLabel (tlbl_EQ);
	}
      else
	{
	  if (!tlbl_NE)
	    tlbl_NE = safeNewiTempLabel (NULL);
	  emitBranch ("bne", tlbl_NE);
	  emitBranch ("jmp", jlbl);
	  safeEmitLabel (tlbl_NE);
	}

      /* mark the icode as generated */
      ifx->generated = 1;
    }
  else
    {
      symbol *tlbl = safeNewiTempLabel (NULL);

      if (!needloada)
	needloada = storeRegTempIfSurv (m6502_reg_a);
      if (opcode == EQ_OP)
	{
	  if (!tlbl_EQ)
	    tlbl_EQ = safeNewiTempLabel (NULL);
	  emitBranch ("beq", tlbl_EQ);
	  if (tlbl_NE)
	    safeEmitLabel (tlbl_NE);
	  loadRegFromConst (m6502_reg_a, 0);
	  emitBranch ("bra", tlbl);
	  safeEmitLabel (tlbl_EQ);
	  loadRegFromConst (m6502_reg_a, 1);
	}
      else 
	{
	  if (!tlbl_NE)
	    tlbl_NE = safeNewiTempLabel (NULL);
	  emitBranch ("bne", tlbl_NE);
	  loadRegFromConst (m6502_reg_a, 0);
	  emitBranch ("bra", tlbl);
	  safeEmitLabel (tlbl_NE);
	  loadRegFromConst (m6502_reg_a, 1);
	}

      safeEmitLabel (tlbl);
      m6502_dirtyReg (m6502_reg_a);
      storeRegToFullAop (m6502_reg_a, AOP (result), false);
      loadOrFreeRegTemp (m6502_reg_a, needloada);
    }

  m6502_dirtyReg (m6502_reg_a);
  _S.DPTRAttr[0].aop=NULL;
  _S.DPTRAttr[1].aop=NULL;

  freeAsmop (right, NULL);
  freeAsmop (left, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genAndOp - for && operation
 *************************************************************************/
static void genAndOp (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  symbol *tlbl, *tlbl0;
  bool needpulla;

  emitComment (TRACEGEN, __func__);

  // TODO: optimize & 0xff as cast when signed

  /* note here that && operations that are in an
     if statement are taken away by backPatchLabels
     only those used in arthmetic operations remain */
  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);
  printIC (ic);

  tlbl = safeNewiTempLabel (NULL);
  tlbl0 = safeNewiTempLabel (NULL);

  needpulla = pushRegIfSurv (m6502_reg_a);
  asmopToBool (AOP (left), false);
  emitBranch ("beq", tlbl0);
  asmopToBool (AOP (right), false);
  emitBranch ("beq", tlbl0);
  loadRegFromConst (m6502_reg_a, 1);
  emitBranch ("bra", tlbl);
  safeEmitLabel (tlbl0);
  //  m6502_dirtyReg (m6502_reg_a);
  loadRegFromConst (m6502_reg_a, 0);
  safeEmitLabel (tlbl);
  m6502_dirtyReg (m6502_reg_a);

  //  m6502_useReg (m6502_reg_a);
  //  m6502_freeReg (m6502_reg_a);

  storeRegToFullAop (m6502_reg_a, AOP (result), false);
  pullOrFreeReg (m6502_reg_a, needpulla);

  freeAsmop (left, NULL);
  freeAsmop (right, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genOrOp - for || operation
 *************************************************************************/
static void genOrOp (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  symbol *tlbl, *tlbl0;
  bool needpulla;

  emitComment (TRACEGEN, __func__);

  /* note here that || operations that are in an
     if statement are taken away by backPatchLabels
     only those used in arthmetic operations remain */
  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);
  printIC (ic);

  tlbl = safeNewiTempLabel (NULL);
  tlbl0 = safeNewiTempLabel (NULL);

  needpulla = pushRegIfSurv (m6502_reg_a);
  asmopToBool (AOP (left), false);
  emitBranch ("bne", tlbl0);
  asmopToBool (AOP (right), false);
  emitBranch ("bne", tlbl0);
  loadRegFromConst (m6502_reg_a, 0);
  emitBranch ("bra", tlbl);
  safeEmitLabel (tlbl0);
  //  m6502_dirtyReg (m6502_reg_a);
  loadRegFromConst (m6502_reg_a, 1);
  safeEmitLabel (tlbl);
  m6502_dirtyReg (m6502_reg_a);

  //  m6502_useReg (m6502_reg_a);
  //  m6502_freeReg (m6502_reg_a);

  storeRegToFullAop (m6502_reg_a, AOP (result), false);
  pullOrFreeReg (m6502_reg_a, needpulla);

  freeAsmop (left, NULL);
  freeAsmop (right, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * isLiteralBit - test if lit == 2^n
 *************************************************************************/
int
isLiteralBit (unsigned long long lit)
{
  int idx;

  for (idx = 0; idx < 64; idx++)
    if (lit == 1ull<<idx)
      return idx + 1;
  return 0;
}

/**************************************************************************
 * litmask - return the mask based on the operand size
 *************************************************************************/
unsigned long long
litmask (int size)
{
  unsigned long long ret = 0xffffffffffffffffull;
  if (size == 1)
    ret = 0xffull;
  else if (size == 2)
    ret= 0xffffull;
  else if (size == 4)
    ret = 0xffffffffull;
  return ret;
}

static const char * expand_symbols (iCode * ic, const char *inlin)
{
  const char *begin = NULL, *p = inlin;
  bool inIdent = false;
  struct dbuf_s dbuf;

  dbuf_init (&dbuf, 128);

  while (*p) {
    if (inIdent) {
      if ('_' == *p || isalnum (*p))
        /* in the middle of identifier */
        ++p;
      else {
        /* end of identifier */
        symbol *sym, *tempsym;
        char *symname = Safe_strndup (p + 1, p - begin - 1);

        inIdent = 0;

        tempsym = newSymbol (symname, ic->level);
        tempsym->block = ic->block;
        sym = (symbol *) findSymWithLevel (SymbolTab, tempsym);
        if (!sym) {
          dbuf_append (&dbuf, begin, p - begin);
        } else {
          asmop *aop = aopForSym (ic, sym);
          const char *l = aopAdrStr (aop, aop->size - 1, true);

          if ('#' == *l)
            l++;
          sym->isref = 1;
          if (sym->level && !sym->allocreq && !sym->ismyparm) {
            werror (E_ID_UNDEF, sym->name);
            werror (W_CONTINUE,
                    "  Add 'volatile' to the variable declaration so that it\n"
                    "  can be referenced within inline assembly");
          }
          dbuf_append_str (&dbuf, l);
        }
        Safe_free (symname);
        begin = p++;
      }
    } else if ('_' == *p) {
      /* begin of identifier */
      inIdent = true;
      if (begin)
        dbuf_append (&dbuf, begin, p - begin);
      begin = p++;
    } else {
      if (!begin)
        begin = p;
      p++;
    }
  }

  if (begin)
    dbuf_append (&dbuf, begin, p - begin);

  return dbuf_detach_c_str (&dbuf);
}

/**************************************************************************
 * genInline - write the inline code out
 *************************************************************************/
static void m6502_genInline (iCode * ic)
{
  char *buf, *bp, *begin;
  const char *expanded;
  bool inComment = false;

  emitComment (TRACEGEN, __func__);

  genLine.lineElement.isInline += (!options.asmpeep);

  buf = bp = begin = Safe_strdup (IC_INLINE (ic));

  /* emit each line as a code */
  while (*bp) {
    switch (*bp) {
    case ';':
      inComment = true;
      ++bp;
      break;

    case '\x87':
    case '\n':
      inComment = false;
      *bp++ = '\0';
      expanded = expand_symbols (ic, begin);
      emitcode (expanded, NULL);
      dbuf_free (expanded);
      begin = bp;
      break;

    default:
      /* Add \n for labels, not dirs such as c:\mydir */
      if (!inComment && (*bp == ':') && (isspace ((unsigned char) bp[1]))) {
        ++bp;
        *bp = '\0';
        ++bp;
        emitcode (begin, NULL);
        begin = bp;
      }
      else
        ++bp;
      break;
    }
  }
  if (begin != bp) {
    expanded = expand_symbols (ic, begin);
    emitcode (expanded, NULL);
    dbuf_free (expanded);
  }

  Safe_free (buf);

  /* consumed; we can free it here */
  dbuf_free (IC_INLINE (ic));

  genLine.lineElement.isInline -= (!options.asmpeep);
}

/**************************************************************************
 * genGetByte - generates code to get a single byte
 *************************************************************************/
static void genGetByte (const iCode *ic)
{
  operand *left   = IC_LEFT (ic);
  operand *right  = IC_RIGHT (ic);
  operand *result = IC_RESULT (ic);
  int offset;

  emitComment (TRACEGEN, __func__);

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  offset = (int) ulFromVal (right->aop->aopu.aop_lit) / 8;
  transferAopAop(left->aop, offset, result->aop, 0);

  freeAsmop (result, NULL);
  freeAsmop (right, NULL);
  freeAsmop (left, NULL);
}

/**************************************************************************
 * genGetWord - generates code to get a 16-bit word
 *************************************************************************/
static void genGetWord (const iCode *ic)
{
  operand *left   = IC_LEFT (ic);
  operand *right  = IC_RIGHT (ic);
  operand *result = IC_RESULT (ic);
  int offset;

  emitComment (TRACEGEN, __func__);

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  offset = (int) ulFromVal (right->aop->aopu.aop_lit) / 8;
  transferAopAop(left->aop, offset, result->aop, 0);
  transferAopAop(left->aop, offset+1, result->aop, 1);

  freeAsmop (result, NULL);
  freeAsmop (right, NULL);
  freeAsmop (left, NULL);
}

/**************************************************************************
 * genRRC - rotate right with carry
 *************************************************************************/
static void genRRC (iCode * ic)
{
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  int size, offset;
  bool resultInA = false;
  char *shift;

  emitComment (TRACEGEN, __func__);

  /* rotate right with carry */
  aopOp (left, ic);
  aopOp (result, ic);
  printIC (ic);

  if(IS_AOP_WITH_A(AOP(result))) resultInA=true;
  size = AOP_SIZE (result);
  offset = size - 1;

  shift = "lsr";
  if(IS_AOP_XA(AOP(left)))
    {
      storeRegTempAlways (m6502_reg_x, true);
      emit6502op ("lsr", TEMPFMT, getLastTempOfs() );
      emit6502op ("ror", "a");
      if(IS_AOP_XA(AOP(result)) )
        {
	  storeRegTemp (m6502_reg_a, true);
	  loadRegFromConst (m6502_reg_a, 0);
	  emit6502op("ror", "a");
	  emit6502op ("ora", TEMPFMT, getLastTempOfs()-1 );
	  transferRegReg (m6502_reg_a, m6502_reg_x, true);
	  loadRegTemp (m6502_reg_a);
	}
      else
	{
	  // optimization if the result is in DIR or EXT
	  storeRegToAop (m6502_reg_a, AOP(result), 0);
	  loadRegFromConst (m6502_reg_a, 0);
	  emit6502op ("ror", "a");
	  emit6502op ("ora", TEMPFMT, getLastTempOfs() );
	  storeRegToAop(m6502_reg_a, AOP(result), 1);
        }
      loadRegTemp (NULL);
      goto release;
    }
  else if (sameRegs (AOP (left), AOP (result)))
    {
      while (size--)
        {
          rmwWithAop (shift, AOP (result), offset--);
          shift = "ror";
        }
    }
  else
    {
      while (size--)
        {
          loadRegFromAop (m6502_reg_a, AOP (left), offset);
          rmwWithReg (shift, m6502_reg_a);
          storeRegToAop (m6502_reg_a, AOP (result), offset);
          shift = "ror";
          offset--;
        }
    }

  if(resultInA) storeRegTemp(m6502_reg_a, true);

  /* now we need to put the carry into the
     highest order byte of the result */
  offset = AOP_SIZE (result) - 1;
  loadRegFromConst(m6502_reg_a, 0);
  emit6502op ("ror", "a");
  accopWithAop ("ora", AOP (result), offset);
  storeRegToAop (m6502_reg_a, AOP (result), offset);

  if(resultInA) loadRegTemp(m6502_reg_a);

 release:
  //  pullOrFreeReg (m6502_reg_a, needpula);

  freeAsmop (left, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genRLC - generate code for rotate left with carry
 *************************************************************************/
static void genRLC (iCode * ic)
{
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  int size, offset;
  char *shift;
  bool resultInA = false;
  bool needpulla = false;

  emitComment (TRACEGEN, __func__);

  /* rotate right with carry */
  aopOp (left, ic);
  aopOp (result, ic);
  printIC(ic);

  if(IS_AOP_WITH_A(AOP(result))) resultInA=true;
  size = AOP_SIZE (result);
  offset = 0;

  shift = "asl";
  if (!resultInA && sameRegs (AOP (left), AOP (result)))
    {
      while (size--)
	{
	  rmwWithAop (shift, AOP (result), offset++);
	  shift = "rol";
	}
    }
  else
    {
      while (size--)
	{
	  loadRegFromAop (m6502_reg_a, AOP (left), offset);
	  rmwWithReg (shift, m6502_reg_a);
	  if(offset==0 && resultInA) storeRegTemp (m6502_reg_a, true);
	  else storeRegToAop (m6502_reg_a, AOP (result), offset);
	  shift = "rol";
	  offset++;
	}
    }

  /* now we need to put the carry into the
     lowest order byte of the result */
  needpulla=pushRegIfSurv(m6502_reg_a);
  offset = 0;
  loadRegFromConst(m6502_reg_a, 0);
  emit6502op ("rol", "a");
  if (resultInA)
    {
      emit6502op("ora", TEMPFMT, getLastTempOfs() );
      loadRegTemp(NULL);
    }
  else
    {
      accopWithAop ("ora", AOP (result), offset);
    }
  storeRegToAop (m6502_reg_a, AOP (result), offset);

  pullOrFreeReg (m6502_reg_a, needpulla);

  freeAsmop (left, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genRot - generates code for rotation
 *************************************************************************/
static void
genRot (iCode *ic)
{
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);
  unsigned int lbits = bitsForType (operandType (left));
  if (IS_OP_LITERAL (right) && operandLitValueUll (right) % lbits == 1)
    genRLC (ic);
  else if (IS_OP_LITERAL (right) && operandLitValueUll (right) % lbits ==  lbits - 1)
    genRRC (ic);
  else
    wassertl (0, "Unsupported rotation.");
}

/**************************************************************************
 * decodePointerOffset - decode a pointer offset operand into a
 *                    literal offset and a rematerializable offset
 *************************************************************************/
static void decodePointerOffset (operand * opOffset, int * litOffset, char ** rematOffset)
{
  *litOffset = 0;
  *rematOffset = NULL;

  if (!opOffset)
    return;

  if (IS_OP_LITERAL (opOffset))
    {
      *litOffset = (int)operandLitValue (opOffset);
    }
  else if (IS_ITEMP (opOffset) && OP_SYMBOL (opOffset)->remat)
    {
      asmop * aop = aopForRemat (OP_SYMBOL (opOffset));

      if (aop->type == AOP_LIT)
	*litOffset = (int) floatFromVal (aop->aopu.aop_lit);
      else if (aop->type == AOP_IMMD)
	*rematOffset = aop->aopu.aop_immd;
    }
  else
    wassertl (0, "Pointer get/set with non-constant offset");
}

/**************************************************************************
 * does a BIT A with a constant, even for non-65C02
 *************************************************************************/
// TODO: lookup table for each new const?
static void bitAConst(int val)
{
  wassertl (val >= 0 && val <= 0xff, "bitAConst()");
  if (IS_MOS65C02)
    {
      emit6502op ("bit", IMMDFMT, (unsigned int)val);
    } 
  else 
    {
      reg_info *reg=getFreeByteReg();
      if(reg)
        {
          loadRegFromConst(reg,val);
          storeRegTempAlways (reg, true);
          emit6502op ("bit", TEMPFMT, getLastTempOfs() );
          loadRegTemp(NULL);
        }
      else
        {
          storeRegTemp (m6502_reg_a, true);
          emit6502op ("and", IMMDFMT, (unsigned int)val);
          loadRegTempNoFlags (m6502_reg_a, true);
        }
    }
}

/**************************************************************************
 * genUnpackBits - generates code for unpacking bits
 *************************************************************************/
static void genUnpackBits (operand * result, operand * left, operand * right, iCode * ifx)
{
  int offset = 0;               /* result byte offset */
  int rsize;                    /* result size */
  int rlen = 0;                 /* remaining bitfield length */
  unsigned blen;                /* bitfield length */
  unsigned bstr;                /* bitfield starting bit within byte */
  sym_link *etype;              /* bitfield type information */
  int litOffset = 0;
  char * rematOffset = NULL;
  bool needpulla = false;
  bool needpully = false;
  bool needpullx = false;
  emitComment (TRACEGEN, __func__);

  decodePointerOffset (right, &litOffset, &rematOffset);
  etype = getSpec (operandType (result));
  rsize = getSize (operandType (result));
  blen = SPEC_BLEN (etype);
  bstr = SPEC_BSTR (etype);

  //  needpulla = pushRegIfSurv (m6502_reg_a);
  needpulla = storeRegTempIfSurv (m6502_reg_a);

  // TODO: enable restoring from DPTR
  if (!IS_AOP_YX (AOP (left)))
    {
      needpullx = storeRegTempIfSurv (m6502_reg_x);
      needpully = storeRegTempIfSurv (m6502_reg_y);
    }

  int yoff= setupDPTR(left, litOffset, rematOffset, false);

  /* dptr now contains the address */

  if (ifx && blen <= 8)
    {
      loadRegFromConst(m6502_reg_y, yoff);
      emit6502op("lda", INDFMT_IY, "DPTR" );
      if (blen < 8)
	{
	  emit6502op ("and", IMMDFMT, (((unsigned char) - 1) >> (8 - blen)) << bstr);
	}
      //      emit6502op("php", "");//TODO
      loadOrFreeRegTemp (m6502_reg_y, needpully);
      loadOrFreeRegTemp (m6502_reg_x, needpullx);
      loadOrFreeRegTemp (m6502_reg_a, needpulla);
      //      emit6502op("plp", "");
      emit6502op("ERROR", "%s: unimplemented ifx & blen<=8",__func__);
      genIfxJump (ifx, "z");
      return;
    }

  if(ifx)
    emit6502op("ERROR", "%s: unimplemented ifx",__func__);

  /* If the bitfield length is less than a byte */
  if (blen < 8)
    {
      loadRegFromConst(m6502_reg_y, yoff);
      emit6502op("lda", INDFMT_IY, "DPTR");
      AccRsh (bstr, false);
      emit6502op ("and", IMMDFMT, ((unsigned char) - 1) >> (8 - blen));
      m6502_useReg(m6502_reg_a);
      if (!SPEC_USIGN (etype))
	{
	  // signed bitfield
	  symbol *tlbl = safeNewiTempLabel (NULL);

	  bitAConst(1 << (blen - 1));
	  emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
	  emit6502op ("ora", IMMDFMT, (unsigned char) (0xff << blen));
	  safeEmitLabel (tlbl);
	}
      storeRegToAop (m6502_reg_a, AOP (result), offset++);
      goto finish;
    }

  /* Bit length is greater than 7 bits. In this case, copy  */
  /* all except the partial byte at the end                 */
  for (rlen = blen; rlen >= 8; rlen -= 8)
    {
      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op("lda", INDFMT_IY, "DPTR");
      if (rlen > 8 && AOP_TYPE (result) == AOP_REG)
	pushReg (m6502_reg_a, true);
      else
	storeRegToAop (m6502_reg_a, AOP (result), offset);
      offset++;
    }

  /* Handle the partial byte at the end */
  if (rlen)
    {
      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op("lda", INDFMT_IY, "DPTR");
      emit6502op ("and", IMMDFMT, ((unsigned char) - 1) >> (8 - rlen));
      if (!SPEC_USIGN (etype))
	{
	  /* signed bitfield */
	  symbol *tlbl = safeNewiTempLabel (NULL);
	  // FIXME: works but very ugly
	  bitAConst(1 << (rlen - 1));
	  emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
	  emit6502op ("ora", IMMDFMT, (unsigned char) (0xff << rlen));
	  safeEmitLabel (tlbl);
	}
      storeRegToAop (m6502_reg_a, AOP (result), offset++);
    }
  if (blen > 8 && AOP_TYPE (result) == AOP_REG)
    {
      pullReg (AOP (result)->aopu.aop_reg[0]);
    }

 finish:
  if (offset < rsize)
    {
      rsize -= offset;
      if (SPEC_USIGN (etype))
	{
	  while (rsize--)
	    storeConstToAop (0, AOP (result), offset++);
	}
      else
	{
	  /* signed bitfield: sign extension with 0x00 or 0xff */
	  signExtendA();
	  while (rsize--)
	    storeRegToAop (m6502_reg_a, AOP (result), offset++);
	}
    }
  loadOrFreeRegTemp (m6502_reg_y, needpully);
  loadOrFreeRegTemp (m6502_reg_x, needpullx);
  loadOrFreeRegTemp (m6502_reg_a, needpulla);
}

/**************************************************************************
 * genUnpackBitsImmed - generates code for unpacking bits
 *************************************************************************/
static void genUnpackBitsImmed (operand * left, operand *right, operand * result, iCode * ic, iCode * ifx)
{
  int size;
  int offset = 0;               /* result byte offset */
  int litOffset = 0;
  char * rematOffset = NULL;
  int rsize;                    /* result size */
  int rlen = 0;                 /* remaining bitfield length */
  sym_link *etype;              /* bitfield type information */
  unsigned blen;                /* bitfield length */
  unsigned bstr;                /* bitfield starting bit within byte */
  asmop *derefaop;
  bool delayed_a = false;
  bool assigned_a = false;
  bool needpulla = false;

  emitComment (TRACEGEN, __func__);

  decodePointerOffset (right, &litOffset, &rematOffset);
  wassert (rematOffset==NULL);

  aopOp (result, ic);
  size = AOP_SIZE (result);

  derefaop = aopDerefAop (AOP (left), litOffset);
  freeAsmop (left, NULL);
  derefaop->size = size;

  etype = getSpec (operandType (result));
  rsize = getSize (operandType (result));
  blen = SPEC_BLEN (etype);
  bstr = SPEC_BSTR (etype);

  needpulla = pushRegIfSurv (m6502_reg_a);

  /* if the bitfield is a single bit in the direct page */
  if (blen == 1 && derefaop->type == AOP_DIR)
    {
      if (!ifx && bstr)
	{
	  symbol *tlbl = safeNewiTempLabel (NULL);

	  // FIXME: unimplemented
	  loadRegFromConst (m6502_reg_a, 0);
	  m6502_unimplemented("genUnpackBitsImmed");
	  //          emit6502op ("brclr", "#%d,%s,%05d$", bstr, aopAdrStr (derefaop, 0, false), safeLabelNum ((tlbl)));
	  if (SPEC_USIGN (etype))
	    rmwWithReg ("inc", m6502_reg_a);
	  else
	    rmwWithReg ("dec", m6502_reg_a);
	  safeEmitLabel (tlbl);
	  storeRegToAop (m6502_reg_a, AOP (result), offset);
	  if (AOP_TYPE (result) == AOP_REG && AOP(result)->aopu.aop_reg[offset]->rIdx == A_IDX)
	    assigned_a = true;
	  m6502_freeReg (m6502_reg_a);
	  offset++;
	  goto finish;
	}
      else if (ifx)
	{
	  symbol *tlbl = safeNewiTempLabel (NULL);
	  symbol *jlbl;
	  char *inst;

	  // FIXME
	  if (IC_TRUE (ifx))
	    {
	      jlbl = IC_TRUE (ifx);
	      inst = "brclr";
	    }
	  else
	    {
	      jlbl = IC_FALSE (ifx);
	      inst = "brset";
	    }
	  emit6502op (inst, "#%d,%s,%05d$", bstr, aopAdrStr (derefaop, 0, false), safeLabelNum ((tlbl)));
	  emitBranch ("jmp", jlbl);
	  safeEmitLabel (tlbl);
	  ifx->generated = 1;
	  offset++;
	  goto finish;
	}
    }

  /* If the bitfield length is less than a byte */
  if (blen < 8)
    {
      loadRegFromAop (m6502_reg_a, derefaop, 0);
      if (!ifx)
	{
	  // TODO: inefficient if just getting flags
	  AccRsh (bstr, false);
	  emit6502op ("and", IMMDFMT, ((unsigned char) - 1) >> (8 - blen));
	  if (!SPEC_USIGN (etype))
	    {
	      /* signed bitfield */
	      symbol *tlbl = safeNewiTempLabel (NULL);
	      bitAConst(1 << (blen - 1));
	      emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
	      emit6502op ("ora", IMMDFMT, (unsigned char) (0xff << blen));
	      safeEmitLabel (tlbl);
	    }
	  storeRegToAop (m6502_reg_a, AOP (result), offset);
	  if (AOP_TYPE (result) == AOP_REG && AOP(result)->aopu.aop_reg[offset]->rIdx == A_IDX)
	    assigned_a = true;
	}
      else
	{
	  emit6502op ("and", IMMDFMT, (((unsigned char) - 1) >> (8 - blen)) << bstr);
	}
      offset++;
      goto finish;
    }

  /* Bit field did not fit in a byte. Copy all
     but the partial byte at the end.  */
  for (rlen = blen; rlen >= 8; rlen -= 8)
    {
      if (assigned_a && !delayed_a)
	{
	  pushReg (m6502_reg_a, true);
	  delayed_a = true;
	}
      loadRegFromAop (m6502_reg_a, derefaop, offset);
      if (!ifx)
	{
	  storeRegToAop (m6502_reg_a, AOP (result), offset);
	  if (AOP_TYPE (result) == AOP_REG && AOP(result)->aopu.aop_reg[offset]->rIdx == A_IDX)
	    assigned_a = true;
	}
      else
        {
	  emitCmp(m6502_reg_a, 0);
        }
      offset++;
    }

  /* Handle the partial byte at the end */
  if (rlen)
    {
      if (assigned_a && !delayed_a)
	{
	  pushReg (m6502_reg_a, true);
	  delayed_a = true;
	}
      loadRegFromAop (m6502_reg_a, derefaop, offset);
      emit6502op ("and", IMMDFMT, ((unsigned char) - 1) >> (8 - rlen));
      if (!SPEC_USIGN (etype))
	{
	  /* signed bitfield */
	  symbol *tlbl = safeNewiTempLabel (NULL);

	  bitAConst (1 << (rlen - 1));
	  emit6502op ("beq", "%05d$", safeLabelNum (tlbl));
	  emit6502op ("ora", IMMDFMT, (unsigned char) (0xff << rlen));
	  safeEmitLabel (tlbl);
	}
      storeRegToAop (m6502_reg_a, AOP (result), offset);
      if (AOP_TYPE (result) == AOP_REG && AOP(result)->aopu.aop_reg[offset]->rIdx == A_IDX)
	assigned_a = true;
      offset++;
    }

 finish:
  if (offset < rsize)
    {
      rsize -= offset;
      if (SPEC_USIGN (etype))
	{
	  while (rsize--)
	    storeConstToAop (0, AOP (result), offset++);
	}
      else
	{
	  if (assigned_a && !delayed_a)
	    {
	      pushReg (m6502_reg_a, true);
	      delayed_a = true;
	    }

	  /* signed bitfield: sign extension with 0x00 or 0xff */
	  signExtendA();
	  while (rsize--)
	    storeRegToAop (m6502_reg_a, AOP (result), offset++);
	}
    }

  freeAsmop (NULL, derefaop);
  freeAsmop (result, NULL);

  if (ifx && !ifx->generated)
    genIfxJump (ifx, "z");

  if (delayed_a)
    pullReg (m6502_reg_a);

  // TODO? wrong plac?
  pullOrFreeReg (m6502_reg_a, needpulla);
}

/**************************************************************************
 * genDataPointerGet - generates code when ptr offset is known
 *************************************************************************/
static void genDataPointerGet (operand * left, operand * right, operand * result, iCode * ic, iCode * ifx)
{
  int size;
  int litOffset = 0;
  char * rematOffset = NULL;
  asmop *derefaop;
  bool needpulla = false;

  emitComment (TRACEGEN, __func__);

  decodePointerOffset (right, &litOffset, &rematOffset);
  wassert (rematOffset==NULL);

  aopOp (result, ic);
  size = AOP_SIZE (result);

  // TODO: aopDerefAop(IMMD(_ftest_a_65536_8)), why?
  derefaop = aopDerefAop (AOP (left), litOffset);
  freeAsmop (left, NULL);
  derefaop->size = size;

  if (ifx)
    needpulla = storeRegTempIfSurv (m6502_reg_a);

  if (IS_AOP_YX (AOP (result)))
    loadRegFromAop (m6502_reg_yx, derefaop, 0);
  else while (size--)
	 {
	   if (!ifx)
	     transferAopAop (derefaop, size, AOP (result), size);
	   else
	     loadRegFromAop (m6502_reg_a, derefaop, size);
	 }

  freeAsmop (NULL, derefaop);
  freeAsmop (result, NULL);

  if (ifx && !ifx->generated)
    {
      loadRegTempNoFlags (m6502_reg_a, needpulla);
      genIfxJump (ifx, "z");
    }
  else
    {
      if (needpulla) loadRegTemp (NULL);
    }
}

/**************************************************************************
 * genPointerGet - generate code for pointer get
 *************************************************************************/
static void genPointerGet (iCode * ic, iCode * ifx)
{
  operand *right  = IC_RIGHT (ic);
  operand *left = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);
  int size, offset;
  int litOffset = 0;
  char * rematOffset = NULL;
  sym_link *retype = getSpec (operandType (result));
  bool needpulla = false;

  emitComment (TRACEGEN, __func__);
  // result = *(left (register offset) + right (remat+literal_offset) )

  size = getSize (operandType (result));
  if (size > 1)
    ifx = NULL;

  aopOp (left, ic);
  aopOp (right, ic);
  aopOp (result, ic);

  /* if left is rematerialisable */
  if (AOP_TYPE (left) == AOP_IMMD || AOP_TYPE (left) == AOP_LIT)
    {
      /* if result is not bit variable type */
      if (!IS_BITVAR (retype))
	genDataPointerGet (left, right, result, ic, ifx);
      else
	genUnpackBitsImmed (left, right, result, ic, ifx);
      goto release;
    }

  /* if bit then unpack */
  if (IS_BITVAR (retype))
    {
      genUnpackBits (result, left, right, ifx);
      goto release;
    }

  decodePointerOffset (right, &litOffset, &rematOffset);

  printIC (ic);

  /* force offset to signed 16-bit range */
  litOffset &= 0xffff;
  if (litOffset & 0x8000)
    litOffset = 0x10000 - litOffset;

  emitComment (TRACEGEN|VVDBG, "  %s src: %s size=%d loff=%d rmoff=%s",
               __func__, aopName(AOP(left)), size, litOffset, rematOffset );
  emitComment (TRACEGEN|VVDBG, "  %s dst: %s size=%d",
               __func__, aopName(AOP(result)), AOP_SIZE(result) );

  if (AOP_TYPE (left) == AOP_REG)
    {
      char hstring[3] ="";
      char lstring[3] ="??";

      if (AOP(left)->aopu.aop_reg[0]->isLitConst)
	sprintf(lstring,"%02x",AOP(left)->aopu.aop_reg[0]->litConst);

      if (AOP_SIZE(left) == 2)
	{
	  if(AOP(left)->aopu.aop_reg[1]->isLitConst)
	    sprintf(hstring,"%02x",AOP(left)->aopu.aop_reg[1]->litConst);
	  else
	    sprintf(hstring,"??");
	}
      emitComment (TRACEGEN|VVDBG, "    %s (%s) = 0x%s%s",
                   __func__, aopName(AOP(left)), hstring, lstring );

    }

  if (AOP_TYPE (left) == AOP_DIR 
      && !rematOffset && litOffset >= 0 && litOffset <= 256-size)
    {
      // pointer is already in zero page & 8-bit offset
      emitComment (TRACEGEN|VVDBG, "      %s - pointer already in zp", __func__);
      bool needloady = storeRegTempIfSurv(m6502_reg_y);

#if 0
      // seem to make perf worse
      if (size == 1 && litOffset == 0
          && ( /*m6502_reg_x->isDead || */ (m6502_reg_x->isLitConst && m6502_reg_x->litConst == 0) ) )
	{
	  // [aa,x] x == 0
	  loadRegFromConst(m6502_reg_x,0);
	  emit6502op ("lda", "[%s,x]", aopAdrStr ( AOP(left), 0, true ) );
	  storeRegToAop (m6502_reg_a, AOP (result), 0);
	  goto release;
	}
#endif

      if (sameRegs(AOP(left), AOP(result)) )
	{
	  // pointer and destination is the same - need avoid overwriting
          // FIXME: this only works for size=2 which is likely ok as pointers are size 2
	  emitComment (TRACEGEN|VVDBG, "    %s - sameregs", __func__);
	  needpulla = storeRegTempIfSurv (m6502_reg_a);
	  for (int i=size-1; i>=0; i--)
	    {
	      loadRegFromConst(m6502_reg_y, litOffset + i);
	      emit6502op ("lda", INDFMT_IY, AOP(left)->aopu.aop_dir);
	      if (i>1)
		{
		  storeRegToAop (m6502_reg_a, AOP (result), i);
		}
	      else if (i==1)
		{
		  pushReg(m6502_reg_a, false);
		}
	      else if (i==0)
		{
		  storeRegToAop (m6502_reg_a, AOP (result), 0);
		  if (size>1)
		    {
		      pullReg(m6502_reg_a);
		      storeRegToAop (m6502_reg_a, AOP (result), 1);
		    }
		}
	    }
	}
      else
        {
	  // otherwise use [aa],y
	  if (IS_AOP_XA(AOP(result)))
	    {
	      // reverse order so A is last
	      emitComment (TRACEGEN|VVDBG, "    %s: dest XA", __func__);
	      for (int i=size-1; i>=0; i--)
		{
		  loadRegFromConst(m6502_reg_y, litOffset + i);
		  emit6502op ("lda", INDFMT_IY, aopAdrStr ( AOP(left), 0, true ) );
		  storeRegToAop (m6502_reg_a, AOP (result), i);
		}
	    }
	  else
	    {
	      // forward order
	      emitComment (TRACEGEN|VVDBG, "    %s: dest generic", __func__);
	      if (!IS_AOP_WITH_A(AOP(result))) needpulla = storeRegTempIfSurv (m6502_reg_a);
	      for (int i=0; i<size; i++)
		{
		  loadRegFromConst(m6502_reg_y, litOffset + i);
		  emit6502op ("lda", INDFMT_IY, aopAdrStr ( AOP(left), 0, true ) );
		  storeRegToAop (m6502_reg_a, AOP (result), i);
		}
	    }
	}
      loadOrFreeRegTemp (m6502_reg_a, needpulla);
      loadOrFreeRegTemp(m6502_reg_y, needloady);
      goto release;
    }

  // try absolute indexed
#if 0
  // allow index to be in memory
  if (rematOffset
      && ( AOP_SIZE(left)==1
           || ( AOP_TYPE(left) == AOP_REG && AOP(left)->aopu.aop_reg[1]->isLitConst ) ) )
#else
    // index can only be a register
    if (rematOffset && AOP_TYPE(left) == AOP_REG &&
        (AOP_SIZE(left) == 1|| AOP(left)->aopu.aop_reg[1]->isLitConst ))
#endif
      {
        emitComment (TRACEGEN|VVDBG,"    %s - absolute with 8-bit index", __func__);
        unsigned int hi_offset=0;
        char *dst_reg;
        char idx_reg;

        if(AOP_SIZE(left)==2)
          hi_offset=(AOP(left)->aopu.aop_reg[1]->litConst)<<8;

        if(AOP_TYPE(left)==AOP_REG)
	  {
	    switch(AOP(left)->aopu.aop_reg[0]->rIdx)
	      {
	      case X_IDX: idx_reg='x'; break;
	      case Y_IDX: idx_reg='y'; break;
	      case A_IDX: idx_reg='A'; break;
	      default: idx_reg='E'; break;
	      }
	  }
	else
	  {
	    idx_reg='M';
	  }

        if(IS_AOP_XA(AOP(result)))
          {
            if(idx_reg=='A')
              transferRegReg(m6502_reg_a, m6502_reg_y, true);
            idx_reg='y';
            dst_reg="lda";
          }
        else if(AOP_TYPE(result)==AOP_REG)
	  {
	    switch(AOP(result)->aopu.aop_reg[0]->rIdx)
	      {
	      case A_IDX: dst_reg="lda"; break;
	      case X_IDX: dst_reg="ldx"; break;
	      case Y_IDX: dst_reg="ldy"; break;
	      default: dst_reg="ERROR"; break;
	      }
	  }
	else
	  {
	    dst_reg="MEM";
	  }

        bool px = false;
        bool py = false;
        bool pa = false;

        if(idx_reg=='A' || idx_reg=='M')
	  {
            if(dst_reg[2]=='x' || m6502_reg_x->aop==&tsxaop)
	      {
		py = storeRegTempIfSurv(m6502_reg_y);
		loadRegFromAop(m6502_reg_y, AOP(left), 0 );
		idx_reg='y';
	      }
	    else
	      {
		// FIXME: should check for a free reg to avoid saving if possible
		px = storeRegTempIfSurv(m6502_reg_x);
		loadRegFromAop(m6502_reg_x, AOP(left), 0 );
		idx_reg='x';
	      }

	  }

        if(dst_reg[2] == idx_reg || dst_reg[0]=='M')
	  {
	    //             loadRegFromAop (m6502_reg_a, AOP (right), 0);
	    //             dst_reg="lda";
	    pa = storeRegTempIfSurv(m6502_reg_a);
	    emit6502op("lda", "(%s+%d+0x%04x),%c",
		       rematOffset, litOffset, hi_offset, idx_reg );

	    storeRegToAop (m6502_reg_a, AOP (result), 0);
            if(AOP_SIZE(result)==2)
              {
		emit6502op("lda", "(%s+%d+0x%04x),%c",
			   rematOffset, litOffset+1, hi_offset, idx_reg );
		storeRegToAop (m6502_reg_a, AOP (result), 1);
              }
	    loadOrFreeRegTemp(m6502_reg_a,pa);
	  }
	else
	  {
	    emit6502op(dst_reg, "(%s+%d+0x%04x),%c",
		       rematOffset, litOffset, hi_offset, idx_reg );
            if(IS_AOP_XA(AOP(result)))
              {
		emit6502op("ldx", "(%s+%d+0x%04x),%c",
			   rematOffset, litOffset+1, hi_offset, idx_reg );
                 
              }
	  }

        loadOrFreeRegTemp(m6502_reg_x,px);
        loadOrFreeRegTemp(m6502_reg_y,py);
        loadOrFreeRegTemp (m6502_reg_a, needpulla);
        goto release;
      }

  bool restore_a_from_dptr = false;
  bool restore_x_from_dptr = false;
  bool needloadx = false;
  bool need_x = false;

  need_x= (AOP_TYPE(left)==AOP_SOF || AOP_TYPE(result)==AOP_SOF);
  bool use_dptr = true;
  asmop *ptr_aop = m6502_reg_a->aop;
  int yoff;

  if(IS_AOP_XA(AOP(left)) && !rematOffset)
    {
      if(ptr_aop && ptr_aop->type==AOP_DIR)
        use_dptr=false;
      else
        {
          restore_a_from_dptr = !m6502_reg_a->isDead;
          if(need_x)
            restore_x_from_dptr = !m6502_reg_x->isDead;
        }
    }
  else
    {
      needpulla = storeRegTempIfSurv (m6502_reg_a);
      if(need_x)
	needloadx = storeRegTempIfSurv(m6502_reg_x);
    }
  bool needloady = storeRegTempIfSurv(m6502_reg_y);

  if(use_dptr)
    yoff = setupDPTR(left, litOffset, rematOffset, false);
  else
    yoff=litOffset;

  emitComment (TRACEGEN|VVDBG, "        %s: generic path", __func__);

  if (IS_AOP_XA (AOP (result)))
    {
      loadRegFromConst(m6502_reg_y, yoff + 1);
      if(use_dptr)
        emit6502op ("lda", INDFMT_IY, "DPTR");
      else
        emit6502op("lda", INDFMT_IY, ptr_aop->aopu.aop_dir);

      transferRegReg(m6502_reg_a, m6502_reg_x, true);
      loadRegFromConst(m6502_reg_y, yoff + 0);
      if(use_dptr)
        emit6502op ("lda", INDFMT_IY, "DPTR");
      else
        emit6502op("lda", INDFMT_IY, ptr_aop->aopu.aop_dir);
    }
  else
    {
      for (offset=0; offset<size; offset++)
        {
          loadRegFromConst(m6502_reg_y, yoff + offset);
	  if(use_dptr)
	    emit6502op ("lda", INDFMT_IY, "DPTR");
	  else
	    emit6502op("lda", INDFMT_IY, ptr_aop->aopu.aop_dir);
          storeRegToAop (m6502_reg_a, AOP (result), offset);
        }
    }
  loadOrFreeRegTemp (m6502_reg_y, needloady);
  loadOrFreeRegTemp (m6502_reg_x, needloadx);
  loadOrFreeRegTemp (m6502_reg_a, needpulla);

  if(restore_x_from_dptr)
    emit6502op ("ldx", "*(DPTR+1)");
  if(restore_a_from_dptr)
    emit6502op ("lda", "*(DPTR+0)");

 release:
  freeAsmop (left, NULL);
  freeAsmop (right, NULL);
  freeAsmop (result, NULL);

  if (ifx && !ifx->generated)
    genIfxJump (ifx, "z");
}

/**************************************************************************
 * genPackBits - generates code for packed bit storage
 *************************************************************************/
static void genPackBits (operand * result, operand * left, sym_link * etype, operand * right)
{
  int offset = 0;               /* source byte offset */
  int rlen = 0;                 /* remaining bitfield length */
  unsigned blen;                /* bitfield length */
  unsigned bstr;                /* bitfield starting bit within byte */
  unsigned long long  litval;   /* source literal value (if AOP_LIT) */
  unsigned char mask;           /* bitmask within current byte */
  int litOffset = 0;
  char *rematOffset = NULL;
  bool needpulla;

  emitComment (TRACEGEN, __func__);

  decodePointerOffset (left, &litOffset, &rematOffset);
  blen = SPEC_BLEN (etype);
  bstr = SPEC_BSTR (etype);

  needpulla = pushRegIfSurv (m6502_reg_a);
  if (AOP_TYPE (right) == AOP_REG)
    {
      /* Not optimal, but works for any register sources. */
      /* Just push the source values onto the stack and   */
      /* pull them off any needed. Better optimzed would  */
      /* be to do some of the shifting/masking now and    */
      /* push the intermediate result. */
      if (blen > 8)
        pushReg (AOP (right)->aopu.aop_reg[1], true);
      pushReg (AOP (right)->aopu.aop_reg[0], true);
    }

  int yoff= setupDPTR(result, litOffset, rematOffset, false);

  /* If the bitfield length is less than a byte */
  if (blen < 8)
    {
      mask = ((unsigned char) (0xFF << (blen + bstr)) | (unsigned char) (0xFF >> (8 - bstr)));

      if (AOP_TYPE (right) == AOP_LIT)
	{
	  // Case with a bitfield length <8 and literal source
	  litval = ullFromVal (AOP (right)->aopu.aop_lit);
	  litval <<= bstr;
	  litval &= (~mask) & 0xff;

	  loadRegFromConst(m6502_reg_y, yoff + offset);
	  emit6502op ("lda", INDFMT_IY, "DPTR");
	  if ((mask | litval) != 0xff)
            {
	      emit6502op ("and", IMMDFMT, (unsigned int)mask);
	    }
	  if (litval)
	    {
	      emit6502op ("ora", IMMDFMT, (unsigned int)litval);
	    }
	  loadRegFromConst(m6502_reg_y, yoff + offset);
	  emit6502op ("sta", INDFMT_IY, "DPTR");
	  pullOrFreeReg (m6502_reg_a, needpulla);
	  return;
	}

      // Case with a bitfield length < 8 and arbitrary source
      if (AOP_TYPE (right) == AOP_REG)
	pullReg (m6502_reg_a);
      else
	loadRegFromAop (m6502_reg_a, AOP (right), 0);
      // shift and mask source value
      AccLsh (bstr);
      emit6502op ("and", IMMDFMT, (unsigned int)(~mask) & 0xffu);
      storeRegTemp (m6502_reg_a, true);

      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op("lda", INDFMT_IY, "DPTR");
      emit6502op("and", IMMDFMT, (unsigned int)mask);
      emit6502op ("ora", TEMPFMT, getLastTempOfs() );
      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op ("sta", INDFMT_IY, "DPTR");
      //      loadRegTemp (m6502_reg_a);
      loadRegTemp (NULL);
      // TODO? redundant?
      pullOrFreeReg (m6502_reg_a, needpulla);
      return;
    }

  /* Bit length is greater than 7 bits. In this case, copy  */
  /* all except the partial byte at the end                 */
  for (rlen = blen; rlen >= 8; rlen -= 8)
    {
      if (AOP_TYPE (right) == AOP_REG)
        pullReg (m6502_reg_a);
      else
        loadRegFromAop (m6502_reg_a, AOP (right), offset);

      //          storeRegIndexed (m6502_reg_a, litOffset+offset, rematOffset);
      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op ("sta", INDFMT_IY, "DPTR");
      offset++;
    }

  /* If there was a partial byte at the end */
  if (rlen)
    {
      mask = (((unsigned char) - 1 << rlen) & 0xff);

      if (AOP_TYPE (right) == AOP_LIT)
	{
	  // Case with partial byte and literal source
	  litval = (int) ulFromVal (AOP (right)->aopu.aop_lit);
	  litval >>= (blen - rlen);
	  litval &= (~mask) & 0xff;
	  //          loadRegIndexed (m6502_reg_a, litOffset+offset, rematOffset);
	  loadRegFromConst(m6502_reg_y, yoff + offset);
	  emit6502op ("lda", INDFMT_IY, "DPTR");
	  if ((mask | litval) != 0xff)
	    {
	      emit6502op ("and", IMMDFMT, (unsigned int)mask);
	    }
	  if (litval)
	    {
	      emit6502op ("ora", IMMDFMT, (unsigned int)litval);
	    }
	  m6502_dirtyReg (m6502_reg_a);
	  //          storeRegIndexed (m6502_reg_a, litOffset+offset, rematOffset);
	  loadRegFromConst(m6502_reg_y, yoff + offset);
	  emit6502op ("sta", INDFMT_IY, "DPTR");
	  pullOrFreeReg (m6502_reg_a, needpulla);
	  return;
	}

      // Case with partial byte and arbitrary source
      if (AOP_TYPE (right) == AOP_REG)
	pullReg (m6502_reg_a);
      else
	loadRegFromAop (m6502_reg_a, AOP (right), offset);

      emit6502op ("and", IMMDFMT, (unsigned int)(~mask) & 0xffu);
      storeRegTemp(m6502_reg_a, true);
      loadRegFromConst(m6502_reg_y, yoff + offset);
      emit6502op ("lda", INDFMT_IY, "DPTR");
      emit6502op ("and", IMMDFMT, (unsigned int)mask);
      emit6502op("ora", TEMPFMT, getLastTempOfs() );
      loadRegTemp(NULL);
      emit6502op ("sta", INDFMT_IY, "DPTR");
    }

  pullOrFreeReg (m6502_reg_a, needpulla);
}

/**************************************************************************
 * genPackBitsImmed - generates code for packed bit storage
 *************************************************************************/
static void genPackBitsImmed (operand * result, operand * left, sym_link * etype, operand * right, iCode * ic)
{
  asmop *derefaop;
  int size;
  int offset = 0;               /* source byte offset */
  int rlen = 0;                 /* remaining bitfield length */
  unsigned blen;                /* bitfield length */
  unsigned bstr;                /* bitfield starting bit within byte */
  unsigned long long int litval;/* source literal value (if AOP_LIT) */
  unsigned char mask;           /* bitmask within current byte */
  bool needpulla;
  int litOffset = 0;
  char *rematOffset = NULL;
  
  emitComment (TRACEGEN, __func__);
  
  blen = SPEC_BLEN (etype);
  bstr = SPEC_BSTR (etype);
  
  aopOp (right, ic);
  size = AOP_SIZE (right);
  decodePointerOffset (left, &litOffset, &rematOffset);
  wassert (!rematOffset);
  
  derefaop = aopDerefAop (AOP (result), litOffset);
  freeAsmop (result, NULL);
  derefaop->size = size;
  
  /* if the bitfield is a single bit in the direct page */
  if (blen == 1 && derefaop->type == AOP_DIR)
    {
      if (AOP_TYPE (right) == AOP_LIT)
	{
	  litval = ullFromVal (AOP (right)->aopu.aop_lit);
	  // FIXME: unimplemented
	  m6502_unimplemented("genPackBitsImmed 1");
	  //emit6502op ((litval & 1) ? "bset" : "bclr", "#%d,%s", bstr, aopAdrStr (derefaop, 0, false));
	}
      else
	{
	  symbol *tlbl1 = safeNewiTempLabel (NULL);
	  symbol *tlbl2 = safeNewiTempLabel (NULL);
      
	  // FIXME: unimplemented
	  m6502_unimplemented("genPackBitsImmed 2");
	  needpulla = pushRegIfSurv (m6502_reg_a);
	  loadRegFromAop (m6502_reg_a, AOP (right), 0);
	  emit6502op ("lsr", "a");
	  emitBranch ("bcs", tlbl1);
	  emit6502op ("bclr", "#%d,%s", bstr, aopAdrStr (derefaop, 0, false));
	  emitBranch ("bra", tlbl2);
	  safeEmitLabel (tlbl1);
	  emit6502op ("bset", "#%d,%s", bstr, aopAdrStr (derefaop, 0, false));
	  safeEmitLabel (tlbl2);
	  pullOrFreeReg (m6502_reg_a, needpulla);
	}
      goto release;
    }
  
  /* If the bitfield length is less than a byte */
  if (blen < 8)
    {
      mask = ((unsigned char) (0xFF << (blen + bstr)) | (unsigned char) (0xFF >> (8 - bstr)));
    
      if (AOP_TYPE (right) == AOP_LIT)
        {
          // Case with a bitfield length <8 and literal source
          litval = (int) ulFromVal (AOP (right)->aopu.aop_lit);
          litval <<= bstr;
          litval &= (~mask) & 0xff;

          needpulla = pushRegIfSurv (m6502_reg_a);
          loadRegFromAop (m6502_reg_a, derefaop, 0);
          if ((mask | litval) != 0xff)
            emit6502op ("and", IMMDFMT, (unsigned int)mask);

          if (litval)
            emit6502op ("ora", IMMDFMT, (unsigned int)litval);

          m6502_dirtyReg (m6502_reg_a);
          storeRegToAop (m6502_reg_a, derefaop, 0);
          pullOrFreeReg (m6502_reg_a, needpulla);
          goto release;
        }

      // Case with a bitfield length < 8 and arbitrary source
      needpulla = pushRegIfSurv (m6502_reg_a);
      loadRegFromAop (m6502_reg_a, AOP (right), 0);
      /* shift and mask source value */
      AccLsh (bstr);
      emit6502op ("and", IMMDFMT, (unsigned int)(~mask) & 0xffu);
      storeRegTemp(m6502_reg_a, true);
    
      loadRegFromAop (m6502_reg_a, derefaop, 0);
      emit6502op("and", IMMDFMT, (unsigned int)mask);
      emit6502op ("ora", TEMPFMT, getLastTempOfs() );
      storeRegToAop (m6502_reg_a, derefaop, 0);
    
      pullOrFreeReg (m6502_reg_a, needpulla);
      loadRegTemp(NULL);
      goto release;
    }
  
  /* Bit length is greater than 7 bits. In this case, copy  */
  /* all except the partial byte at the end                 */
  for (rlen = blen; rlen >= 8; rlen -= 8)
    {
      transferAopAop (AOP (right), offset, derefaop, offset);
      offset++;
    }
  
  /* If there was a partial byte at the end */
  if (rlen)
    {
      mask = (((unsigned char) - 1 << rlen) & 0xff);
    
      if (AOP_TYPE (right) == AOP_LIT)
	{
	  // Case with partial byte and literal source
	  litval = (int) ulFromVal (AOP (right)->aopu.aop_lit);
	  litval >>= (blen - rlen);
	  litval &= (~mask) & 0xff;
	  needpulla = pushRegIfSurv (m6502_reg_a);
	  loadRegFromAop (m6502_reg_a, derefaop, offset);
	  if ((mask | litval) != 0xff)
	    emit6502op ("and", IMMDFMT, (unsigned int)mask);

	  if (litval)
	    emit6502op ("ora", IMMDFMT, (unsigned int)litval);

	  m6502_dirtyReg (m6502_reg_a);
	  storeRegToAop (m6502_reg_a, derefaop, offset);
	  m6502_dirtyReg (m6502_reg_a);
	  pullOrFreeReg (m6502_reg_a, needpulla);
	  goto release;
	}
    
      // Case with partial byte and arbitrary source
      needpulla = pushRegIfSurv (m6502_reg_a);
      loadRegFromAop (m6502_reg_a, AOP (right), offset);
      emit6502op ("and", IMMDFMT, (unsigned int)(~mask) & 0xffu);
      storeRegTemp (m6502_reg_a, true);
    
      loadRegFromAop (m6502_reg_a, derefaop, offset);
      emit6502op("and", IMMDFMT, (unsigned int)mask);
      emit6502op ("ora", TEMPFMT, getLastTempOfs() );
      storeRegToAop (m6502_reg_a, derefaop, offset);
      pullOrFreeReg (m6502_reg_a, needpulla);
      loadRegTemp (NULL);
    }
  
  m6502_freeReg (m6502_reg_a);
  
 release:
  freeAsmop (right, NULL);
  freeAsmop (NULL, derefaop);
}

/**************************************************************************
 * genDataPointerSet - remat pointer to data space
 *************************************************************************/
static void genDataPointerSet (operand * left, operand * right, operand * result, iCode * ic)
{
  int size;
  asmop *derefaop;
  int litOffset = 0;
  char *rematOffset = NULL;

  emitComment (TRACEGEN, __func__);

  aopOp (right, ic);
  size = AOP_SIZE (right);
  decodePointerOffset (left, &litOffset, &rematOffset);
  wassert (!rematOffset);

  derefaop = aopDerefAop (AOP (result), litOffset);
  freeAsmop (result, NULL);
  derefaop->size = size;

  while (size--)
    transferAopAop (AOP (right), size, derefaop, size);

  freeAsmop (right, NULL);
  freeAsmop (NULL, derefaop);
}

/**************************************************************************
 * genPointerSet - stores the value into a pointer location
 *************************************************************************/
static void
genPointerSet (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *left = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);
  int size, offset;
  bool needloada = false;
  bool needloadx = false;
  bool needloady = false;
  bool restore_a_from_dptr=false;
  bool restore_x_from_dptr=false;
  bool deadA = false;
  int litOffset = 0;
  char *rematOffset = NULL;
  wassert (operandType (result)->next);
  bool bit_field = IS_BITVAR (operandType (result)->next);

  emitComment (TRACEGEN, __func__);

  // *(result (reg) + left (rematoffset+litoffset)) = right


  aopOp (result, ic);

  /* if the result is rematerializable */
  if (AOP_TYPE (result) == AOP_IMMD || AOP_TYPE (result) == AOP_LIT)
    {
      if (!bit_field)
	genDataPointerSet (left, right, result, ic);
      else
	genPackBitsImmed (result, left, operandType (result)->next, right, ic);
      return;
    }

  aopOp (right, ic);
  //aopOp (left, ic);

  printIC(ic);


  size = AOP_SIZE (right);

  decodePointerOffset (left, &litOffset, &rematOffset);

  emitComment (TRACEGEN|VVDBG, "    %s  - *( reg=%s + litoffset=%d + rematoffset=%s) =",
               __func__, aopName(AOP(result)),  litOffset, rematOffset );
  emitComment (TRACEGEN|VVDBG, "                       %s, size=%d",
               aopName(AOP(right)), size );



  // shortcut for [aa],y (or [aa,x]) if already in zero-page
  // and we're not storing to the same pointer location

  if (!bit_field
      && AOP_TYPE (result) == AOP_DIR && !rematOffset && litOffset >= 0 && litOffset <= 256-size
      && !sameRegs(AOP(right), AOP(result)) )
    {

#if 0
      if (size == 1 && litOffset == 0 && m6502_reg_x->isLitConst && m6502_reg_x->litConst == 0)
	{
	  // use [aa,x] if only 1 byte and offset is 0
	  loadRegFromAop (m6502_reg_a, AOP (right), 0);
	  emit6502op ("sta", "[%s,x]", aopAdrStr ( AOP(result), 0, true ) );
	} 
      else
	{ }
#endif
      needloada = storeRegTempIfSurv (m6502_reg_a);
      needloady = storeRegTempIfUsed (m6502_reg_y);
    
      emitComment (TRACEGEN|VVDBG,"   %s - ptr already in zp ", __func__);
    
      if (IS_AOP_YX(AOP(right)))
	{
	  // reverse order so Y is first
	  for (int i=size-1; i>=0; i--)
	    {
	      loadRegFromAop (m6502_reg_a, AOP (right), i);
	      loadRegFromConst(m6502_reg_y, litOffset + i);
	      emit6502op ("sta", INDFMT_IY, aopAdrStr ( AOP(result), 0, true ) );
	    }
	}
      else
	{
	  // forward order
	  for (int i=0; i<size; i++)
	    {
	      loadRegFromAop (m6502_reg_a, AOP (right), i);
	      loadRegFromConst(m6502_reg_y, litOffset + i);
	      emit6502op ("sta", INDFMT_IY, aopAdrStr ( AOP(result), 0, true ) );
	    }
	}
      goto release;
    }
  
#if 1
  // abs,x or abs,y with index in register or memory
  if (rematOffset
      && ( AOP_SIZE(result)==1
           || ( AOP_TYPE(result) == AOP_REG && AOP(result)->aopu.aop_reg[1]->isLitConst ) ) )
#else
    // abs,x or abs,y with index in register
    if (rematOffset && AOP_TYPE(result)== AOP_REG
        && ( AOP_SIZE(result) == 1 || AOP(result)->aopu.aop_reg[1]->isLitConst ) )
#endif
      {
        emitComment (TRACEGEN|VVDBG,"  %s - absolute with 8-bit index", __func__);
        emitComment (TRACEGEN|VVDBG,"    reg : %d  size:%d", AOP(result)->aopu.aop_reg[0]->rIdx,  AOP_SIZE(result) );
        
        emitComment (TRACEGEN|VVDBG,"    AOP TYPE(result)=%d",AOP_TYPE (result));
        emitComment (TRACEGEN|VVDBG,"    AOP(result) reg=%d",AOP(result)->aopu.aop_reg[0]->rIdx);
        unsigned int hi_offset=0;
        char src_reg = 0;
        char idx_reg;
        bool px = false;
        bool py = false;
        bool pa = false;
        
        pa=pushRegIfSurv(m6502_reg_a);
        
        
        if(AOP_SIZE(result)==2)
          hi_offset=(AOP(result)->aopu.aop_reg[1]->litConst)<<8;
        
        //  if ( ( AOP_TYPE(result) == AOP_REG && AOP(result)->aopu.aop_reg[0]->isLitConst ) )
        //      emitcode("ERROR","");
        
        
        if(AOP_TYPE(result)==AOP_REG)
	  {
	    switch(AOP(result)->aopu.aop_reg[0]->rIdx)
	      {
	      case X_IDX: idx_reg='x'; break;
	      case Y_IDX: idx_reg='y'; break;
	      case A_IDX: idx_reg='A'; break;
	      default: idx_reg='E'; break;
	      }
	  }
	else
	  {
	    idx_reg='M';
	  }
        
        emitComment (TRACEGEN|VVDBG,"    idx_reg=%c",idx_reg);

        if(AOP_TYPE(right)==AOP_REG
           && AOP(right)->aopu.aop_reg[0]->rIdx == X_IDX )
          src_reg = 'x';
        if(AOP_TYPE(right)==AOP_REG
           && AOP(right)->aopu.aop_reg[0]->rIdx == Y_IDX )
          src_reg = 'y';
        
        if(idx_reg=='A' || idx_reg=='M')
	  {
	    if(src_reg=='x')
	      {
		py = storeRegTempIfSurv(m6502_reg_y);
		loadRegFromAop(m6502_reg_y, AOP(result), 0 );
		idx_reg='y';
	      }
	    else
	      {
		px = storeRegTempIfSurv(m6502_reg_x);
		loadRegFromAop(m6502_reg_x, AOP(result), 0 );
		idx_reg='x';
	      }
	  }
        
        loadRegFromAop (m6502_reg_a, AOP (right), 0);
        
        emit6502op("sta", "(%s+%d+0x%04x),%c",
                   rematOffset, litOffset, hi_offset, idx_reg );
        
        pullOrFreeReg(m6502_reg_a, pa);
        loadOrFreeRegTemp(m6502_reg_x,px);
        loadOrFreeRegTemp(m6502_reg_y,py);
        
        goto release;
      }

  // general case
  emitComment (TRACEGEN|VVDBG,"  %s - general case ", __func__);
  int aloc=0, xloc=0;
  int yloc=0;
  deadA = m6502_reg_a->isDead;
  bool need_x = false;

  need_x= (AOP_TYPE(right)==AOP_SOF);
  bool use_dptr = true;
  asmop *ptr_aop = m6502_reg_a->aop;
  int yoff;

  if(IS_AOP_XA(AOP(result)) && !rematOffset)
    {
      if(ptr_aop && ptr_aop->type==AOP_DIR)
        use_dptr=false;
      else
        {
          restore_a_from_dptr = !m6502_reg_a->isDead;
          if(need_x)
            restore_x_from_dptr = !m6502_reg_x->isDead;
        }
    }
  else
    {

      if(IS_SAME_DPTR_OP(result) && IS_AOP_A(AOP(right)) && !rematOffset && litOffset<255)
        {
          // do nothing: no need to save a in this case
          emitComment (TRACEGEN|VVDBG,"    %s : skip saving A", __func__ );
        }
      else if(AOP_TYPE(result)!=AOP_SOF && IS_AOP_A(AOP(right)) && !rematOffset && litOffset<255)
        {
          // do nothing: no need to save a in this case
          emitComment (TRACEGEN|VVDBG,"    %s : skip saving A2", __func__ );
        }
      else if(IS_AOP_WITH_A(AOP(right)))
        needloada = storeRegTempIfUsed (m6502_reg_a);
      else
        needloada = storeRegTempIfSurv (m6502_reg_a);
      aloc = getLastTempOfs();

      if(IS_AOP_WITH_X(AOP(right)) && AOP_TYPE(result)==AOP_SOF )
        needloadx = storeRegTempIfUsed (m6502_reg_x);
      else
	//   if(AOP_TYPE(result)==AOP_SOF || AOP_TYPE(right)==AOP_SOF)
        needloadx = storeRegTempIfSurv (m6502_reg_x);
      xloc = getLastTempOfs();
    } 

  if(IS_AOP_WITH_Y(AOP(right)))
    needloady = storeRegTempIfUsed (m6502_reg_y);
  else
    needloady = storeRegTempIfSurv (m6502_reg_y);
  yloc = getLastTempOfs();

  /* if bit-field then pack */
  if (bit_field)
    {
      emitComment (TRACEGEN|VVDBG,"    %s : bitvar", __func__ );

      if(needloada && IS_AOP_WITH_A (AOP(right)))
        loadRegTempAt(m6502_reg_a, aloc);
      genPackBits (result, left, operandType (result)->next, right);
      goto release;
    }

#if 0
  bool savea = false;
  if(!m6502_reg_a->isFree)
    {
      savea = true;
      transferRegReg(m6502_reg_a, m6502_reg_y, true);
    }
#endif

  if(use_dptr)
    yoff = setupDPTR(result, litOffset, rematOffset, false);
  else
    yoff=litOffset;

  if(IS_AOP_WITH_A (AOP(right)))
    if(needloada)
      loadRegTempAt(m6502_reg_a, aloc);

  if(IS_AOP_WITH_X (AOP(right)))
    if(needloadx)
      loadRegTempAt(m6502_reg_x, xloc);

  if(IS_AOP_WITH_Y (AOP(right)))
    if(needloady)
      loadRegTempAt(m6502_reg_y, yloc);

  for (offset=0; offset<size; offset++)
    {
      loadRegFromAop (m6502_reg_a, AOP (right), offset);
      loadRegFromConst(m6502_reg_y, yoff + offset);
      if(use_dptr)
        emit6502op("sta", INDFMT_IY, "DPTR");
      else
        emit6502op("sta", INDFMT_IY, ptr_aop->aopu.aop_dir);
    }

 release:
  freeAsmop (result, NULL);
  freeAsmop (right, NULL);

  loadOrFreeRegTemp (m6502_reg_y, needloady);
  loadOrFreeRegTemp (m6502_reg_x, needloadx);

  if(deadA)
    {
      if(needloada)
        loadRegTemp(NULL);
      m6502_freeReg(m6502_reg_a);
    }
  else
    {
      loadOrFreeRegTemp (m6502_reg_a, needloada);
    }

  if(restore_x_from_dptr)
    emit6502op ("ldx", "*(DPTR+1)");
  if(restore_a_from_dptr)
    emit6502op ("lda", "*(DPTR+0)");
    
}

// TODO: genIfx sometimes does a cmp #0 but has flags already, peephole might fix
/**************************************************************************
 * genIfx - generate code for Ifx statement
 *************************************************************************/
static void genIfx (iCode * ic, iCode * popIc)
{
  operand *cond = IC_COND (ic);
  
  emitComment (TRACEGEN, __func__);
  
  aopOp (cond, ic);
  
  /* If the condition is a literal, we can just do an unconditional */
  /* branch or no branch */
  if (AOP_TYPE (cond) == AOP_LIT)
    {
      unsigned long long lit = ullFromVal (AOP (cond)->aopu.aop_lit);
      freeAsmop (cond, NULL);
    
      if (lit)
        {
          if (IC_TRUE (ic))
            emitBranch ("jmp", IC_TRUE (ic));
        }
      else
        {
          if (IC_FALSE (ic))
            emitBranch ("jmp", IC_FALSE (ic));
        }
      ic->generated = 1;
      return;
    }

  /* evaluate the operand */
  if (AOP_TYPE (cond) != AOP_CRY)
    {
      emitComment (TRACEGEN|VVDBG, "     genIfx - !AOP_CRY");
      asmopToBool (AOP (cond), false);
    }
  /* the result is now in the z flag bit */
  freeAsmop (cond, NULL);

  // TODO: redundant bne/beq
  emitComment (TRACEGEN|VVDBG, "      genIfx - call jump");
  genIfxJump (ic, "z");

  ic->generated = 1;
}

/**************************************************************************
 * genAddrOf - generates code for address of
 *************************************************************************/
static void genAddrOf (iCode * ic)
{
  operand *result = IC_RESULT (ic);
  symbol *sym = OP_SYMBOL (IC_LEFT (ic));
  int size, offset;
  bool needloada, needloadx;
  struct dbuf_s dbuf;

  emitComment (TRACEGEN, __func__);

  aopOp (result, ic);

  /* if the operand is on the stack then we
     need to get the stack offset of this
     variable */
  if (sym->onStack)
    {
      needloada = storeRegTempIfSurv (m6502_reg_a);
      needloadx = storeRegTempIfSurv (m6502_reg_x);
      /* if it has an offset then we need to compute it */
      emitTSX();
      offset = _S.stackOfs + _S.tsxStackPushes + _S.stackPushes + sym->stack + 1;
      if(smallAdjustReg(m6502_reg_x, offset))
	offset=0;
      transferRegReg (m6502_reg_x, m6502_reg_a, true);
      if (offset)
	{
	  emitSetCarry(0);
	  emit6502op ("adc", IMMDFMT, (unsigned int)offset & 0xffu);
	}
      if(IS_AOP_XA(AOP(result)))
	{
	  loadRegFromConst(m6502_reg_x, 0x01); // stack top = 0x100
	}
      else
	{
	  storeRegToAop (m6502_reg_a, AOP (result), 0);
	  loadRegFromConst(m6502_reg_a, 0x01); // stack top = 0x100
	  storeRegToAop (m6502_reg_a, AOP (result), 1);
	}
      loadOrFreeRegTemp (m6502_reg_x, needloadx);
      loadOrFreeRegTemp (m6502_reg_a, needloada);
      goto release;
    }

  /* object not on stack then we need the name */
  size = AOP_SIZE (result);
  offset = 0;

  while (size--)
    {
      dbuf_init (&dbuf, 64);
      switch (offset)
	{
	case 0:
	  dbuf_printf (&dbuf, "#%s", sym->rname);
	  break;
	case 1:
	  dbuf_printf (&dbuf, "#>%s", sym->rname);
	  break;
	default:
	  dbuf_printf (&dbuf, "#0");
	}
      storeImmToAop (dbuf_detach_c_str (&dbuf), AOP (result), offset++);
    }

 release:
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genAssignLit - Try to generate code for literal assignment.
 *                result and right should already be asmOped
 *************************************************************************/
static bool genAssignLit (operand * result, operand * right)
{
  char assigned[8];
  unsigned char value[sizeof(assigned)];
  int size;
  int offset,offset2;

  /* Make sure this is a literal assignment */
  if (AOP_TYPE (right) != AOP_LIT)
    return false;

  /* The general case already handles register assignment well */
  if (AOP_TYPE (result) == AOP_REG)
    return false;

  /* Some hardware registers require MSB to LSB assignment order */
  /* so don't optimize the assignment order if volatile */
  if (isOperandVolatile (result, false))
    return false;

  /* Make sure the assignment is not larger than we can handle */
  size = AOP_SIZE (result);
  if (size > sizeof(assigned))
    return false;

  emitComment (TRACEGEN, __func__);

  for (offset=0; offset<size; offset++)
    {
      assigned[offset] = 0;
      value[offset] = byteOfVal (AOP (right)->aopu.aop_lit, offset);
    }

  for (offset=0; offset<size; offset++)
    {
      if (assigned[offset])
	continue;
      storeConstToAop (value[offset], AOP (result), offset);
      assigned[offset] = 1;
      // look for duplicates
      if ((AOP_TYPE (result) != AOP_DIR ))
	{
	  for(offset2=offset+1; offset2<size; offset2++)
	    {
	      if(value[offset]==value[offset2])
		{
		  storeConstToAop (value[offset], AOP (result), offset2);
		  assigned[offset2] = 1;
		}
	    }
	}
    }

  return true;
}

/**************************************************************************
 * genAssign - generate code for assignment
 *************************************************************************/
static void
genAssign (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *result = IC_RESULT (ic);

  emitComment (TRACEGEN, __func__);

  aopOp (right, ic);
  aopOp (result, ic);
  printIC(ic);

  if (!genAssignLit (result, right))
    genCopy (result, right);

  freeAsmop (right, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genJumpTab - generates code for jump table
 *************************************************************************/
static void genJumpTab (iCode * ic)
{
  symbol *jtab;
  symbol *jtablo = safeNewiTempLabel (NULL);
  symbol *jtabhi = safeNewiTempLabel (NULL);

  emitComment (TRACEGEN, __func__);

  aopOp (IC_JTCOND (ic), ic);

  // TODO
  {
    bool needpulla = pushRegIfSurv (m6502_reg_a);
    // use X or Y for index?
    bool needpullind = false;
    reg_info* indreg;
    if (IS_AOP_X (AOP (IC_JTCOND (ic)))) {
      indreg = m6502_reg_x;
    } else if (IS_AOP_Y (AOP (IC_JTCOND (ic)))) {
      indreg = m6502_reg_y;
    } else {
      indreg = m6502_reg_x->isFree ? m6502_reg_x : m6502_reg_y;
      needpullind = pushRegIfSurv (indreg);
      /* get the condition into indreg */
      loadRegFromAop (indreg, AOP (IC_JTCOND (ic)), 0);
    }
    freeAsmop (IC_JTCOND (ic), NULL);

    if (indreg == m6502_reg_x)
      {
	emit6502op ("lda", "%05d$,x", safeLabelNum (jtablo));
	storeRegTemp (m6502_reg_a, true);
	emit6502op ("lda", "%05d$,x", safeLabelNum (jtabhi));
	storeRegTemp (m6502_reg_a, true);
      }
    else
      {
	emit6502op ("lda", "%05d$,y", safeLabelNum (jtablo));
	storeRegTemp (m6502_reg_a, true);
	emit6502op ("lda", "%05d$,y", safeLabelNum (jtabhi));
	storeRegTemp (m6502_reg_a, true);
      }
    loadRegTemp(NULL);
    loadRegTemp(NULL);
    if (needpullind) pullReg(indreg);
    if (needpulla) pullReg(m6502_reg_a);
    emit6502op ("jmp", TEMPFMT_IND, getLastTempOfs()+1);

    m6502_dirtyAllRegs();
    m6502_freeAllRegs ();
  }

  /* now generate the jump labels */
  safeEmitLabel (jtablo);
  // FIXME: add this to gen6502op
  for (jtab = setFirstItem (IC_JTLABELS (ic)); jtab; jtab = setNextItem (IC_JTLABELS (ic)))
    {
      emitcode (".db", "%05d$", labelKey2num (jtab->key));
      regalloc_dry_run_cost_bytes++;
    }
  safeEmitLabel (jtabhi);
  for (jtab = setFirstItem (IC_JTLABELS (ic)); jtab; jtab = setNextItem (IC_JTLABELS (ic)))
    {
      emitcode (".db", ">%05d$", labelKey2num (jtab->key));
      regalloc_dry_run_cost_bytes++;
    }
}

/**************************************************************************
 * genCast - generate code for casting
 *************************************************************************/
static void genCast (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *result = IC_RESULT (ic);
  sym_link *resulttype = operandType (result);
  sym_link *righttype = operandType (right);
  int size, offset;
  bool signExtend;
  bool save_a = false;

  emitComment (TRACEGEN, __func__);

  /* if they are equivalent then do nothing */
  if (operandsEqu (result, right))
    return;

  unsigned topbytemask = (IS_BITINT (resulttype) && (SPEC_BITINTWIDTH (resulttype) % 8)) ?
    (0xff >> (8 - SPEC_BITINTWIDTH (resulttype) % 8)) : 0xff;

  aopOp (right, ic);
  aopOp (result, ic);
  printIC(ic);

  emitComment (TRACEGEN|VVDBG, "      genCast - size %d -> %d", right?AOP_SIZE(right):0, result?AOP_SIZE(result):0);

  // Cast to _BitInt can require mask of top byte.
  if (IS_BITINT (resulttype) && (SPEC_BITINTWIDTH (resulttype) % 8) && bitsForType (resulttype) < bitsForType (righttype))
    {
      genCopy (result, right);
      if (result->aop->type != AOP_REG || result->aop->aopu.aop_reg[result->aop->size - 1] != m6502_reg_a)
	{
	  save_a = result->aop->aopu.aop_reg[0] == m6502_reg_a || !m6502_reg_a->isDead;
	  if (save_a)
	    pushReg(m6502_reg_a, false);
	  loadRegFromAop (m6502_reg_a, result->aop, result->aop->size - 1);
	}
      emit6502op ("and", IMMDFMT, topbytemask);
      if (!SPEC_USIGN (resulttype))
	{
	  // sign extend
	  symbol *tlbl = safeNewiTempLabel (NULL);
	  pushReg (m6502_reg_a, true);
	  emit6502op ("and", IMMDFMT, 1u << (SPEC_BITINTWIDTH (resulttype) % 8 - 1));
	  emitBranch ("beq", tlbl);
	  pullReg (m6502_reg_a);
	  emit6502op ("ora", IMMDFMT, ~topbytemask & 0xff);
	  pushReg (m6502_reg_a, true);
	  safeEmitLabel (tlbl);
	  pullReg (m6502_reg_a);
	}
      storeRegToAop (m6502_reg_a, result->aop, result->aop->size - 1);
      goto release;
    }

  if (IS_BOOL (operandType (result)))
    {
      bool needpulla = pushRegIfSurv (m6502_reg_a);
      asmopToBool (AOP (right), true);
      storeRegToAop (m6502_reg_a, AOP (result), 0);
      pullOrFreeReg (m6502_reg_a, needpulla);
      goto release;
    }

  signExtend = AOP_SIZE (result) > AOP_SIZE (right) && !IS_BOOL (righttype) && IS_SPEC (righttype) && !SPEC_USIGN (righttype);
  bool masktopbyte = IS_BITINT (resulttype) && (SPEC_BITINTWIDTH (resulttype) % 8) && SPEC_USIGN (resulttype);
  
  emitComment (TRACEGEN|VVDBG, "      %s - signExtend: %s",
               __func__, signExtend?"YES":"NO");

  // if the result size is <= source just copy
  if( (AOP_SIZE (result) <= AOP_SIZE (right))  
      || ((!signExtend) && !IS_AOP_A(AOP(right))) 
      )
    {
      genCopy (result, right);
      goto release;
    }

  if(IS_AOP_XA(AOP(right)))
    {
      storeRegToFullAop (m6502_reg_xa, AOP (result), signExtend);
      goto release;
    }
  
  if (IS_AOP_XA (AOP (result) ) && AOP_SIZE (right) == 1 )
    {

      genCopy (result, right);
      if (signExtend)
        {
          symbol *tlbl = safeNewiTempLabel (NULL);
	  emitCmp(m6502_reg_a, 0);
          emitBranch ("bpl", tlbl);
          storeConstToAop (0xff, AOP (result), 1);
          safeEmitLabel (tlbl);
          m6502_dirtyReg(m6502_reg_x);
        }
      goto release;
    }
 
  wassert (AOP (result)->type != AOP_REG);
  
  save_a = !m6502_reg_a->isDead && signExtend;
  if (save_a)
    pushReg(m6502_reg_a, true);
  
  offset = 0;
  size = AOP_SIZE (right);

  while (size)
    {
      if (size == 1 && signExtend)
	{
	  loadRegFromAop (m6502_reg_a, AOP (right), offset);
	  storeRegToAop (m6502_reg_a, AOP (result), offset);
	  offset++;
	  size--;
	}
      else
	{
	  transferAopAop (AOP (right), offset, AOP (result), offset);
	  offset++;
	  size--;
	}
    }

  size = AOP_SIZE (result) - offset;
  if (size && !signExtend)
    {
      while (size--)
	storeConstToAop (0, AOP (result), offset++);
    }
  else if (size)
    {
      signExtendA();
      while (size--)
	{
	  if (!size && masktopbyte)
	    emit6502op ("and", IMMDFMT, topbytemask);
	  storeRegToAop (m6502_reg_a, AOP (result), offset++);
	}
    }

 release:
  if (save_a)
    pullReg(m6502_reg_a);

  freeAsmop (right, NULL);
  freeAsmop (result, NULL);
}

/**************************************************************************
 * genReceive - generate code for a receive iCode
 *************************************************************************/
static void
genReceive (iCode * ic)
{
  operand *result = IC_RESULT (ic);
  int size;
  int offset;
  bool delayed_x = false;

  emitComment (TRACEGEN, __func__);

  aopOp (result, ic);
  size = AOP_SIZE (result);
  offset = 0;

  emitComment (TRACEGEN|VVDBG, "  %s: size=%d regmask=%x",
               __func__, size, AOP (result)->regmask );

  if(ic->argreg)
    {
      if (AOP_TYPE(result)==AOP_SOF && size>1 )
        {
          storeRegTemp (m6502_reg_x, true);
          storeRegToAop (m6502_reg_a, AOP (result), 0);
          loadRegTemp (m6502_reg_a);
          storeRegToAop (m6502_reg_a, AOP (result), 1);
          if(size==3)
            storeRegToAop (m6502_reg_y, AOP (result), 2);
	}
      else
	{
	  while (size--)
	    {
	      if (AOP_TYPE (result) == AOP_REG && !(offset + (ic->argreg - 1)) 
		  && AOP (result)->aopu.aop_reg[0]->rIdx == X_IDX && size)
		{
		  storeRegTemp (m6502_reg_a, true);
		  delayed_x = true;
		}
	      else
		transferAopAop (m6502_aop_pass[offset + (ic->argreg - 1)], 0, AOP (result), offset);
	      // FIXME: this freereg is likely wrong
	      if (m6502_aop_pass[offset]->type == AOP_REG)
		m6502_freeReg (m6502_aop_pass[offset]->aopu.aop_reg[0]);
	      offset++;
	    }
	}
    }

  if (delayed_x)
    loadRegTemp (m6502_reg_x);

  freeAsmop (result, NULL);
}

// support routine for genDummyRead
static void dummyRead (iCode* ic, operand* op, reg_info* reg)
{
  if (op && IS_SYMOP (op))
    {
      aopOp (op, ic);
      int size = AOP_SIZE (op);
      for (int offset=0; offset<size; offset++)
        loadRegFromAop (reg, AOP (op), offset);

      freeAsmop (op, NULL);
    }
}

/**************************************************************************
 * genDummyRead - generate code for dummy read of volatiles
 *************************************************************************/
static void
genDummyRead (iCode * ic)
{
  bool needpulla = false;
    
  emitComment (TRACEGEN, __func__);

  reg_info* reg = getFreeByteReg();
  if (!reg)
    {
      needpulla = pushRegIfSurv (m6502_reg_a);
      reg = m6502_reg_a;
    }

  // TODO: use BIT? STA?
  dummyRead(ic, IC_RIGHT(ic), reg);
  dummyRead(ic, IC_LEFT(ic), reg);

  pullOrFreeReg (reg, needpulla);
}

/**************************************************************************
 * genCritical - generate code for start of a critical sequence
 *************************************************************************/
static void
genCritical (iCode * ic)
{
  operand *result = IC_RESULT (ic);
  emitComment (TRACEGEN, __func__);
  
  if (result)
    aopOp (result, ic);

  emit6502op ("php", "");
  emit6502op ("sei", "");

  if (result)
    {
      emit6502op ("plp", "");
      m6502_dirtyReg (m6502_reg_a);
      storeRegToAop (m6502_reg_a, AOP (result), 0);
    }

  m6502_freeReg (m6502_reg_a);
  if (result)
    freeAsmop (result, NULL);
}

/**************************************************************************
 * genEndCritical - generate code for end of a critical sequence
 *************************************************************************/
static void
genEndCritical (iCode * ic)
{
  operand *right  = IC_RIGHT (ic);
  emitComment (TRACEGEN, __func__);
  
  if (right)
    {
      aopOp (right, ic);
      loadRegFromAop (m6502_reg_a, AOP (right), 0);
      emit6502op ("pha", "");
      m6502_freeReg (m6502_reg_a);
      freeAsmop (right, NULL);
    }
  emit6502op ("plp", "");
}

static void
updateiTempRegisterUse (operand * op)
{
  symbol *sym;

  if (IS_ITEMP (op)) {
    sym = OP_SYMBOL (op);
    if (!sym->isspilt)
      {
        /* If only used by IFX, there might not be any register assigned */
        int i;
        for(i = 0; i < sym->nRegs; i++)
          if (sym->regs[i])
            m6502_useReg (sym->regs[i]);
      }
  }
}

/**************************************************************************
 * genm6502iCode - generate code for M6502 based controllers for a
 *                 single iCode instruction
 *************************************************************************/
static void
genm6502iCode (iCode *ic)
{
  operand *right  = IC_RIGHT (ic);
  operand *left   = IC_LEFT (ic);
  operand *result = IC_RESULT (ic);

  int i;

  initGenLineElement ();
  genLine.lineElement.ic = ic;

#if 0
  if (!regalloc_dry_run)
    printf ("ic %d op %d stack pushed %d\n", ic->key, ic->op, G.stack.pushed);
#endif

  if (resultRemat (ic))
    {
      if (!regalloc_dry_run)
	emitComment(TRACEGEN, "skipping iCode since result will be rematerialized");
      return;
    }

  if (ic->generated)
    {
      if (!regalloc_dry_run)
	emitComment(TRACEGEN, "skipping generated iCode");
      return;
    }

  m6502_freeAllRegs ();

  // FIXME: removing the following generates worse code
  if (regalloc_dry_run)
    m6502_dirtyAllRegs ();

  if (ic->op == IFX)
    updateiTempRegisterUse (IC_COND (ic));
  else if (ic->op == JUMPTABLE)
    updateiTempRegisterUse (IC_JTCOND (ic));
  else if (ic->op == RECEIVE)
    {
      // FIXME: should add entry icode to this.
      m6502_useReg (m6502_reg_a);
      m6502_useReg (m6502_reg_x); // TODO: x really is free if function only receives 1 byte
    }
  else
    {
      if (POINTER_SET (ic))
	updateiTempRegisterUse (result);
      updateiTempRegisterUse (left);
      updateiTempRegisterUse (right);
    }

  for (i = 0; i < HW_REG_SIZE; i++)
    {
      if (bitVectBitValue (ic->rSurv, i))
        {
          m6502_regWithIdx (i)->isDead = false;
	  m6502_regWithIdx (i)->isFree = false;
        }
      else
	m6502_regWithIdx (i)->isDead = true;
    }

  /* depending on the operation */
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

    case IPUSH_VALUE_AT_ADDRESS:
      genPointerPush (ic);
      break;

    case CALL:
      genCall (ic);
      break;

    case PCALL:
      genPcall (ic);
      break;

    case FUNCTION:
      genFunction (ic);
      break;

    case ENDFUNCTION:
      genEndFunction (ic);
      break;

    case RETURN:
      genRet (ic);
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
      genMult (ic);
      break;

    case '/':
      genDiv (ic);
      break;

    case '%':
      genMod (ic);
      break;

    case '>':
    case '<':
    case LE_OP:
    case GE_OP:
      genCmp (ic, ifxForOp (result, ic));
      break;

    case NE_OP:
    case EQ_OP:
      genCmpEQorNE (ic, ifxForOp (result, ic));
      break;

    case AND_OP:
      genAndOp (ic);
      break;

    case OR_OP:
      genOrOp (ic);
      break;

    case '^':
      genXor (ic, ifxForOp (result, ic));
      break;

    case '|':
      genOr (ic, ifxForOp (result, ic));
      break;

    case BITWISEAND:
      genAnd (ic, ifxForOp (result, ic));
      break;

    case INLINEASM:
      m6502_genInline (ic);
      break;

    case GETABIT:
      wassertl (0, "Unimplemented iCode: GETABIT");
      break;

    case GETBYTE:
      genGetByte(ic);
      break;

    case GETWORD:
      genGetWord(ic);
      break;

    case ROT:
      genRot (ic);
      break;

    case LEFT_OP:
      genLeftShift (ic);
      break;

    case RIGHT_OP:
      genRightShift (ic);
      break;

    case GET_VALUE_AT_ADDRESS:
      genPointerGet (ic, NULL); // TODO? ifxForOp (result, ic));
      break;

    case SET_VALUE_AT_ADDRESS:
      genPointerSet (ic);
      break;

    case '=':
      if (POINTER_SET (ic))
	genPointerSet (ic);
      else
	genAssign (ic);
      break;

    case IFX:
      genIfx (ic, NULL);
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
      genReceive (ic);
      break;

    case SEND:
      if (!regalloc_dry_run)
	addSet (&_S.sendSet, ic);
      else
	{
	  set * sendSet = NULL;
	  addSet (&sendSet, ic);
	  genSend (sendSet);
	  deleteSet (&sendSet);
	}
      break;

    case DUMMY_READ_VOLATILE:
      genDummyRead (ic);
      break;

    case CRITICAL:
      genCritical (ic);
      break;

    case ENDCRITICAL:
      genEndCritical (ic);
      break;

    default:
      emitcode("ERROR", "; Unimplemented iCode (%x)", ic->op);
      printIC(ic);
      //      wassertl (0, "Unknown iCode");
    }
}

static void
init_aop_pass(void)
{
  if (m6502_aop_pass[0])
    return;

  m6502_aop_pass[0] = newAsmop (AOP_REG);
  m6502_aop_pass[0]->size = 1;
  m6502_aop_pass[0]->aopu.aop_reg[0] = m6502_reg_a;
  m6502_aop_pass[1] = newAsmop (AOP_REG);
  m6502_aop_pass[1]->size = 1;
  m6502_aop_pass[1]->aopu.aop_reg[0] = m6502_reg_x;
  m6502_aop_pass[2] = newAsmop (AOP_DIR);
  m6502_aop_pass[2]->size = 1;
  m6502_aop_pass[2]->aopu.aop_dir = "___SDCC_m6502_ret2";
  m6502_aop_pass[3] = newAsmop (AOP_DIR);
  m6502_aop_pass[3]->size = 1;
  m6502_aop_pass[3]->aopu.aop_dir = "___SDCC_m6502_ret3";
  m6502_aop_pass[4] = newAsmop (AOP_DIR);
  m6502_aop_pass[4]->size = 1;
  m6502_aop_pass[4]->aopu.aop_dir = "___SDCC_m6502_ret4";
  m6502_aop_pass[5] = newAsmop (AOP_DIR);
  m6502_aop_pass[5]->size = 1;
  m6502_aop_pass[5]->aopu.aop_dir = "___SDCC_m6502_ret5";
  m6502_aop_pass[6] = newAsmop (AOP_DIR);
  m6502_aop_pass[6]->size = 1;
  m6502_aop_pass[6]->aopu.aop_dir = "___SDCC_m6502_ret6";
  m6502_aop_pass[7] = newAsmop (AOP_DIR);
  m6502_aop_pass[7]->size = 1;
  m6502_aop_pass[7]->aopu.aop_dir = "___SDCC_m6502_ret7";
}

float
drym6502iCode (iCode *ic)
{
  regalloc_dry_run = true;
  regalloc_dry_run_cost_bytes = 0;
  regalloc_dry_run_cost_cycles = 0;

  init_aop_pass();

  genm6502iCode (ic);

  destroy_line_list ();
  /*freeTrace (&_S.trace.aops);*/

  wassert (regalloc_dry_run);

  int byte_cost_weight = 1;
  if (optimize.codeSize)
    byte_cost_weight*=2;
  if (!optimize.codeSpeed)
    byte_cost_weight*=4;

  return ((float)regalloc_dry_run_cost_bytes * byte_cost_weight + regalloc_dry_run_cost_cycles * ic->count);
}

/**************************************************************************
 * genm6502Code - generate code for for a block of instructions
 *************************************************************************/
void
genm6502Code (iCode *lic)
{
  iCode *ic;
  int cln = 0;
  int clevel = 0;
  int cblock = 0;

  regalloc_dry_run = false;

  m6502_dirtyAllRegs ();
  _S.tempOfs = 0;

  /* print the allocation information */
  if (allocInfo && currFunc)
    printAllocInfo (currFunc, codeOutBuf);
  /* if debug information required */
  if (options.debug && currFunc && !regalloc_dry_run)
    debugFile->writeFunction (currFunc, lic);

  if (options.debug && !regalloc_dry_run)
    debugFile->writeFrameAddress (NULL, NULL, 0); /* have no idea where frame is now */

  init_aop_pass();

  /* Generate Code for all instructions */
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
	    emitComment (ALWAYS, "%s: %d: %s", ic->filename, ic->lineno, printCLine (ic->filename, ic->lineno));
	  cln = ic->lineno;
	}

      regalloc_dry_run_cost_bytes = 0;
      regalloc_dry_run_cost_cycles = 0;

      if (options.iCodeInAsm)
	{
	  char regsSurv[4];
	  const char *iLine;

	  regsSurv[0] = (bitVectBitValue (ic->rSurv, A_IDX)) ? 'a' : '-';
	  regsSurv[1] = (bitVectBitValue (ic->rSurv, Y_IDX)) ? 'y' : '-';
	  regsSurv[2] = (bitVectBitValue (ic->rSurv, X_IDX)) ? 'x' : '-';
	  regsSurv[3] = 0;
	  iLine = printILine (ic);
	  emitComment (ALWAYS, " [%s] ic:%d: %s", regsSurv, ic->key, iLine);
	  dbuf_free (iLine);
	}

      genm6502iCode(ic);
      emitComment (TRACEGEN, "Raw cost for generated ic %d : (%d, %f) count=%f", ic->key, regalloc_dry_run_cost_bytes, regalloc_dry_run_cost_cycles, ic->count);

      // TODO: should be asserts?
#if 1
      if (!m6502_reg_a->isFree)
	emitComment (REGOPS|VVDBG, "  forgot to free a");
      if (!m6502_reg_x->isFree)
	emitComment (REGOPS|VVDBG, "  forgot to free x");
      if (!m6502_reg_y->isFree)
	emitComment (REGOPS|VVDBG, "  forgot to free y");
      if (!m6502_reg_yx->isFree)
	emitComment (REGOPS|VVDBG, "  forgot to free yx");
      if (!m6502_reg_xa->isFree)
	emitComment (REGOPS|VVDBG, "  forgot to free xa");
#endif

      if (getLastTempOfs() != -1 )
	emitcode("ERROR", "; forgot to free temp stack (%d)", getLastTempOfs());
    }

  if (options.debug)
    debugFile->writeFrameAddress (NULL, NULL, 0); /* have no idea where frame is now */

  /* now we are ready to call the
     peep hole optimizer */
  if (!options.nopeep)
    peepHole (&genLine.lineHead);

  /* now do the actual printing */
  printLine (genLine.lineHead, codeOutBuf);

  /* destroy the line list */
  destroy_line_list ();
}

