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

enum asminst
{
  A_ADC,
  A_ADCW,
  A_ADD,
  A_ADDW,
  A_AND,
  A_CLR,
  A_CLRW,
  A_CP,
  A_LD,
  A_LDW,
  A_OR,
  A_SBC,
  A_SBCW,
  A_SUB,
  A_SUBW,
  A_TST,
  A_TSTW,
  A_XCH,
  A_XOR
};

static const char *asminstnames[] =
{
  "adc",
  "adcw",
  "add",
  "addw",
  "and",
  "clr",
  "clrw",
  "cp",
  "ld",
  "ldw",
  "or",
  "sbc",
  "sbcw",
  "sub",
  "subw",
  "tst",
  "tstw",
  "xch",
  "xor",
};

static struct asmop asmop_xl, asmop_x, asmop_y, asmop_z, asmop_zero;
static struct asmop *const ASMOP_XL = &asmop_xl;
static struct asmop *const ASMOP_X = &asmop_x;
static struct asmop *const ASMOP_Y = &asmop_y;
static struct asmop *const ASMOP_Z = &asmop_z;
static struct asmop *const ASMOP_ZERO = &asmop_zero;

// Init aop as a an asmop for data in registers, as given by the -1-terminated array regidx.
static void
f8_init_reg_asmop(asmop *aop, const signed char *regidx)
{
  aop->type = AOP_REG;
  aop->size = 0;
  memset (aop->regs, -1, sizeof(aop->regs));
  
  for(int i = 0; regidx[i] >= 0; i++)
    {
      aop->aopu.bytes[i].byteu.reg = f8_regs + regidx[i];
      aop->regs[regidx[i]] = i;
      aop->aopu.bytes[i].in_reg = true;
      aop->size++;
    }
}

void
f8_init_asmops (void)
{
  f8_init_reg_asmop(&asmop_xl, (const signed char[]){XL_IDX, -1});
  f8_init_reg_asmop(&asmop_x, (const signed char[]){XL_IDX, XH_IDX, -1});
  f8_init_reg_asmop(&asmop_y, (const signed char[]){YL_IDX, YH_IDX, -1});
  f8_init_reg_asmop(&asmop_z, (const signed char[]){ZL_IDX, ZH_IDX, -1});

  asmop_zero.type = AOP_LIT;
  asmop_zero.size = 1;
  asmop_zero.aopu.aop_lit = constVal ("0");
  asmop_zero.regs[XL_IDX] = -1;
  asmop_zero.regs[XH_IDX] = -1;
  asmop_zero.regs[YL_IDX] = -1;
  asmop_zero.regs[YH_IDX] = -1;
  asmop_zero.regs[ZL_IDX] = -1;
  asmop_zero.regs[ZH_IDX] = -1;
  asmop_zero.regs[C_IDX] = -1;
}

static void
cost(unsigned int bytes, unsigned int cycles)
{
  regalloc_dry_run_cost_bytes += bytes;
  regalloc_dry_run_cost_cycles += cycles * regalloc_dry_run_cycle_scale;
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
emitJP(const symbol *target, float probability)
{
  if (!regalloc_dry_run)
     emit2 ("jp", "!tlabel", labelKey2num (target->key));
  cost (3, 1);
}

/*-----------------------------------------------------------------*/
/* aopRS - asmop in register or on stack                           */
/*-----------------------------------------------------------------*/
static bool
aopRS (const asmop *aop)
{
  return (aop->type == AOP_REG || aop->type == AOP_REGSTK || aop->type == AOP_STK);
}

/*-----------------------------------------------------------------*/
/* aopInReg - asmop from offset in the register                    */
/*-----------------------------------------------------------------*/
static bool
aopInReg (const asmop *aop, int offset, short rIdx)
{
  if (!(aop->type == AOP_REG || aop->type == AOP_REGSTK))
    return (false);

  if (offset >= aop->size || offset < 0)
    return (false);

  if (rIdx == X_IDX)
    return (aopInReg (aop, offset, XL_IDX) && aopInReg (aop, offset + 1, XH_IDX));
  else if (rIdx == Y_IDX)
    return (aopInReg (aop, offset, YL_IDX) && aopInReg (aop, offset + 1, YH_IDX));
  else if (rIdx == Z_IDX)
    return (aopInReg (aop, offset, ZL_IDX) && aopInReg (aop, offset + 1, ZH_IDX));

  return (aop->aopu.bytes[offset].in_reg && aop->aopu.bytes[offset].byteu.reg->rIdx == rIdx);
}

/*-----------------------------------------------------------------*/
/* aopOnStack - asmop from offset on stack in consecutive memory   */
/*-----------------------------------------------------------------*/
static bool
aopOnStack (const asmop *aop, int offset, int size)
{
  int i;
  long int stk_base;

  if (!(aop->type == AOP_STK || aop->type == AOP_REGSTK))
    return (false);

  if (offset + size > aop->size)
    return (false);

  // Fully on stack?
  for (i = offset; i < offset + size; i++)
    if (aop->aopu.bytes[i].in_reg)
      return (false);

  // Consecutive?
  stk_base = aop->aopu.bytes[offset].byteu.stk;
  for (i = 1; i < size; i++)
    if (!regalloc_dry_run && aop->aopu.bytes[offset + i].byteu.stk != stk_base - i) // Todo: Stack offsets might be unavailable during dry run (messes with addition costs, so we should have a mechanism to do it better).
      return (false);

  return (true);
}

/*-----------------------------------------------------------------*/
/* aopIsOp8_2 - asmop at offset can be used at 8-bit operand to
                two-operand instruction                            */
/*-----------------------------------------------------------------*/
static bool
aopIsOp8_2 (const asmop *aop, int offset)
{
  return (aop->type == AOP_LIT || aop->type == AOP_DIR || aop->type == AOP_IMMD ||
    aopOnStack (aop, offset, 1) ||
    aopInReg (aop, offset, XH_IDX) || aopInReg (aop, offset, YL_IDX) || aopInReg (aop, offset, YH_IDX) || aopInReg (aop, offset, ZL_IDX));
}

/*-----------------------------------------------------------------*/
/* aopIsOp8_1 - asmop at offset can be used at 8-bit operand to
                one-operand instruction                            */
/*-----------------------------------------------------------------*/
static bool
aopIsOp8_1 (const asmop *aop, int offset)
{
  return (aop->type == AOP_IMMD ||
    aopOnStack (aop, offset, 1) ||
    aopRS (aop) && aop->aopu.bytes[offset].in_reg && !aopInReg (aop, offset, YH_IDX));
}

/*-----------------------------------------------------------------*/
/* aopIsOp16_2 - asmop at offset can be used at 8-bit operand to
                 two-operand instruction                            */
/*-----------------------------------------------------------------*/
static bool
aopIsOp16_2 (const asmop *aop, int offset)
{
  return (aop->type == AOP_LIT || aop->type == AOP_DIR || aop->type == AOP_IMMD ||
    aopOnStack (aop, offset, 2) ||
    aopInReg (aop, offset, X_IDX));
}

/*-----------------------------------------------------------------*/
/* aopIsOp16_1 - asmop at offset can be used at 8-bit operand to
                 one-operand instruction                            */
/*-----------------------------------------------------------------*/
static bool
aopIsOp16_1 (const asmop *aop, int offset)
{
  return (aop->type == AOP_IMMD ||
    aopOnStack (aop, offset, 2) ||
    aopInReg (aop, offset, Y_IDX) || aopInReg (aop, offset, X_IDX) || aopInReg (aop, offset, Z_IDX));
}

/*-----------------------------------------------------------------*/
/* aopOnStack - asmop from offset on stack (excl. extended stack)  */
/*-----------------------------------------------------------------*/
static bool
aopOnStackNotExt (const asmop *aop, int offset, int size)
{
  return (aopOnStack (aop, offset, size) && (aop->aopu.bytes[offset].byteu.stk + G.stack.pushed <= 255 || regalloc_dry_run)); // Todo: Stack offsets might be unavailable during dry run (messes with addition costs, so we should have a mechanism to do it better).
}

// Get aop at offset as 8-bit operand.
static const char *
aopGet(const asmop *aop, int offset)
{
  static char buffer[256];

  /* Don't really need the value during dry runs, so save some time. */
  if (regalloc_dry_run)
    return ("");

  if (aop->type == AOP_LIT)
    {
      SNPRINTF (buffer, sizeof(buffer), "#0x%02x", byteOfVal (aop->aopu.aop_lit, offset));
      return (buffer);
    }

  if (offset >= aop->size)
    return ("#0x00");

  if (aopRS (aop) && aop->aopu.bytes[offset].in_reg)
    return (aop->aopu.bytes[offset].byteu.reg->name);

  if (aopRS (aop) && !aop->aopu.bytes[offset].in_reg)
    {
      long int soffset = aop->aopu.bytes[offset].byteu.stk + G.stack.pushed;
      
      wassert (soffset < (1 << 16) && soffset >= 0);

      if (soffset > 255)
        {
          long int eoffset = (long int)(aop->aopu.bytes[offset].byteu.stk) + G.stack.size - 256l;

          wassertl_bt (regalloc_dry_run || f8_extend_stack, "Extended stack access, but z not prepared for extended stack access.");
          wassertl_bt (regalloc_dry_run || eoffset >= 0l && eoffset <= 0xffffl, "Stack access out of extended stack range."); // Stack > 64K.

          SNPRINTF (buffer, sizeof(buffer), "(0x%x, z)", (unsigned)eoffset);
        }
      else
        SNPRINTF (buffer, sizeof(buffer), "(0x%02x, sp)", (unsigned)soffset);
      return (buffer);
    }

  if (aop->type == AOP_IMMD)
    {
      if (offset == 0)
        SNPRINTF (buffer, sizeof(buffer), "#<(%s+%d)", aop->aopu.immd, aop->aopu.immd_off);
      else
        SNPRINTF (buffer, sizeof(buffer), "#((%s+%d) >> %d)", aop->aopu.immd, aop->aopu.immd_off, offset * 8);
      return (buffer);
    }

  if (aop->type == AOP_DIR)
    {
      SNPRINTF (buffer, sizeof(buffer), "%s+%d", aop->aopu.aop_dir, offset);
      return (buffer);
    }

  wassert_bt (0);
  return ("dummy");
}

// Get aop at offset as 16-bit operand.
static const char *
aopGet2(const asmop *aop, int offset)
{
  static char buffer[256];

  /* Don't really need the value during dry runs, so save some time. */
  if (regalloc_dry_run)
    return ("");

  if (aopInReg (aop, offset, X_IDX))
    return("x");
  if (aopInReg (aop, offset, Y_IDX))
    return("y");
  if (aopInReg (aop, offset, Z_IDX))
    return("z");

  if (aop->type != AOP_LIT && !aopOnStack (aop, offset, 2) && aop->type != AOP_IMMD && aop->type != AOP_DIR)
    fprintf (stderr, "Invalid aop for aopGet2. aop->type %d. aop->size %d.\n", aop->type, aop->size);
  wassert_bt (aop->type == AOP_LIT || aopOnStack (aop, offset, 2) || aop->type == AOP_IMMD || aop->type == AOP_DIR);

  if (aop->type == AOP_LIT)
    {
      SNPRINTF (buffer, sizeof(buffer), "#0x%02x%02x", byteOfVal (aop->aopu.aop_lit, offset + 1), byteOfVal (aop->aopu.aop_lit, offset));
      return (buffer);
    }
  else if (aop->type == AOP_IMMD)
    {
      if (offset)
        SNPRINTF (buffer, sizeof(buffer), "#((%s+%d) >> %d)", aop->aopu.immd, aop->aopu.immd_off, offset * 8);
      else
        SNPRINTF (buffer, sizeof(buffer), "#(%s+%d)", aop->aopu.immd, aop->aopu.immd_off);
      return (buffer);
    }

  return (aopGet (aop, offset));
}

// How many bytes would it take for the two-operand 8-bit instruction (including prefixes)? Also return the number of prefix bytes in *prefix.
// returns -1 if impossible.
static int
op2_bytes (int *prefixes, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int r0Idx = ((aopRS (op0) && op0->aopu.bytes[offset0].in_reg)) ? op0->aopu.bytes[offset0].byteu.reg->rIdx : -1;
  int r1Idx = ((aopRS (op1) && op1->aopu.bytes[offset1].in_reg)) ? op1->aopu.bytes[offset1].byteu.reg->rIdx : -1;

  if (r0Idx == XL_IDX)
    if (r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == YH_IDX || r1Idx == ZL_IDX)
      {
        *prefixes = 0;
        return 1;
      }
    else if (op1->type == AOP_LIT || op1->type == AOP_IMMD ||
      offset1 >= op1->size ||
      op1->type == AOP_STK || op1->type == AOP_REGSTK && r1Idx == -1)
      {
        *prefixes = 0;
        return 2;
      }
    else if (op1->type == AOP_DIR)
      {
        *prefixes = 0;
        return 3;
      }
    else
      return -1;

  if (r0Idx == XH_IDX || r0Idx == YL_IDX || r0Idx == ZL_IDX) // Try with alternate accumulator prefix.
    {
      int bytes = op2_bytes (prefixes, ASMOP_XL, 0, op1, offset1);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }
  if (r1Idx == XL_IDX || r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == ZL_IDX) // Try with swap prefix
    {
      int bytes = op2_bytes (prefixes, op1, offset1, op0, offset0);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }

  return -1;
}

static void
op2_cost (const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int prefixes;
  int bytes = op2_bytes (&prefixes, op0, offset0, op1, offset1);

  wassert_bt (bytes > 0);

  cost (bytes, prefixes + 1);
}

static void
op_cost (const asmop *op0, int offset0)
{
  if (op0->type == AOP_DIR)
    cost (3, 1);
  else if (aopOnStack (op0, offset0, 1))
    cost (2, 1);
  else if (aopInReg (op0, offset0, XL_IDX) || aopInReg (op0, offset0, ZH_IDX))
    cost (1, 1);
  else if (aopInReg (op0, offset0, XH_IDX) || aopInReg (op0, offset0, YL_IDX) || aopInReg (op0, offset0, ZL_IDX))
    cost (2, 2);
  else
    wassert (0);
}

// How many bytes would it take for the two-operand 16-bit instruction (including prefixes)? Also return the number of prefix bytes in *prefix.
// returns -1 if impossible.
static int
op2w_bytes (int *prefixes, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int r0Idx = ((aopRS (op0) && op0->aopu.bytes[offset0].in_reg)) ? op0->aopu.bytes[offset0].byteu.reg->rIdx : -1;
  int r1Idx = ((aopRS (op1) && op1->aopu.bytes[offset1].in_reg)) ? op0->aopu.bytes[offset1].byteu.reg->rIdx : -1;

  if (r0Idx >= 0)
    {
      if (!op0->aopu.bytes[offset0 + 1].in_reg || op0->aopu.bytes[offset0 + 1].byteu.reg->rIdx != r0Idx + 1);
        return -1;
      switch (r0Idx)
        {
        case XL_IDX:
          r0Idx = X_IDX;
          break;
        case YL_IDX:
          r0Idx = Y_IDX;
          break;
        case ZL_IDX:
          r0Idx = Z_IDX;
          break;
        default:
          return -1;
        }
    }
  if (r1Idx >= 0)
    {
      if (!op1->aopu.bytes[offset1 + 1].in_reg || op1->aopu.bytes[offset1 + 1].byteu.reg->rIdx != r1Idx + 1);
        return -1;
      switch (r1Idx)
        {
        case XL_IDX:
          r1Idx = X_IDX;
          break;
        case YL_IDX:
          r1Idx = Y_IDX;
          break;
        case ZL_IDX:
          r1Idx = Z_IDX;
          break;
        default:
          return -1;
        }
    }
    
  if (r0Idx == Y_IDX)
    if (r1Idx == X_IDX)
      {
        *prefixes = 0;
        return 1;
      }
    else if (op1->type == AOP_LIT || op1->type == AOP_IMMD ||
      offset1 >= op1->size ||
      op1->type == AOP_STK || op1->type == AOP_REGSTK && r1Idx == -1 ||
      op1->type == AOP_DIR)
      {
        *prefixes = 0;
        return 3;
      }
    else
      return -1;

  if (r0Idx == X_IDX || r0Idx == Z_IDX) // Try with alternate accumulator prefix.
    {
      int bytes = op2_bytes (prefixes, ASMOP_Y, 0, op1, offset1);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }    
  if (r1Idx == Y_IDX || r1Idx == X_IDX || r1Idx == Z_IDX) // Try with swap prefix
    {
      int bytes = op2_bytes (prefixes, op1, offset1, op0, offset0);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }

  return -1;
}

static void
op2w_cost (const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int prefixes;
  int bytes = op2w_bytes (&prefixes, op0, offset0, op1, offset1);

  wassert_bt (bytes > 0);

  cost (bytes, prefixes + 1);
}

// How many bytes would it take for the two-operand 8-bit load (including prefixes)? Also return the number of prefix bytes in *prefix.
// returns -1 if impossible.
static int
ld_bytes (int *prefixes, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int r0Idx = ((aopRS (op0) && op0->aopu.bytes[offset0].in_reg)) ? op0->aopu.bytes[offset0].byteu.reg->rIdx : -1;
  int r1Idx = ((aopRS (op1) && op1->aopu.bytes[offset1].in_reg)) ? op1->aopu.bytes[offset1].byteu.reg->rIdx : -1;

  if (r0Idx == XL_IDX)
    if (r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == YH_IDX || r1Idx == ZL_IDX || r1Idx == ZH_IDX)
      {
        *prefixes = 0;
        return 1;
      }
    else if (op1->type == AOP_LIT || op1->type == AOP_IMMD ||
      offset1 >= op1->size ||
      op1->type == AOP_STK || op1->type == AOP_REGSTK && r1Idx == -1)
      {
        *prefixes = 0;
        return 2;
      }
    else if (op1->type == AOP_DIR)
      {
        *prefixes = 0;
        return 3;
      }
    else
      return -1;
  else if (r1Idx == XL_IDX && op1->type == AOP_STK || op1->type == AOP_REGSTK && r0Idx == -1) // ld (n, sp), xl
    {
      *prefixes = 0;
      return 2;
    }
  else if (r1Idx == XL_IDX && op1->type == AOP_DIR) // ld mm, xl
    {
      *prefixes = 0;
      return 3;
    }

  if (r0Idx == XH_IDX || r0Idx == YL_IDX || r0Idx == ZL_IDX || // Try with alternate accumulator prefix.
    r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == ZL_IDX)
    {
      bool replace0 = (r0Idx == XH_IDX || r0Idx == YL_IDX || r0Idx == ZL_IDX);
      bool replace1 = (r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == ZL_IDX); // TODO: Each prefix can only replave one!
      int bytes = ld_bytes (prefixes, replace0 ? ASMOP_XL : op0, replace0 ? 0 : offset0, replace1 ? ASMOP_XL : op1, replace1 ? 0 : offset1);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }
  if (r1Idx == XL_IDX || r1Idx == XH_IDX || r1Idx == YL_IDX || r1Idx == ZL_IDX) // Try with swap prefix
    {
      int bytes = ld_bytes (prefixes, op1, offset1, op0, offset0);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }

  return -1;
}

static void
ld_cost (const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int prefixes;
  int bytes = ld_bytes (&prefixes, op0, offset0, op1, offset1);

  wassert_bt (bytes > 0);

  cost (bytes, prefixes + 1);
}

// How many bytes would it take for the two-operand 8-bit load (including prefixes)? Also return the number of prefix bytes in *prefix.
// returns -1 if impossible.
static int
ldw_bytes (int *prefixes, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int r0Idx = ((aopRS (op0) && op0->aopu.bytes[offset0].in_reg)) ? op0->aopu.bytes[offset0].byteu.reg->rIdx : -1;
  int r1Idx = ((aopRS (op1) && op1->aopu.bytes[offset1].in_reg)) ? op1->aopu.bytes[offset1].byteu.reg->rIdx : -1;

  if (r0Idx >= 0)
    {
      if (!op0->aopu.bytes[offset0 + 1].in_reg || op0->aopu.bytes[offset0 + 1].byteu.reg->rIdx != r0Idx + 1);
        return -1;
      switch (r0Idx)
        {
        case XL_IDX:
          r0Idx = X_IDX;
          break;
        case YL_IDX:
          r0Idx = Y_IDX;
          break;
        case ZL_IDX:
          r0Idx = Z_IDX;
          break;
        default:
          return -1;
        }
    }
  if (r1Idx >= 0)
    {
      if (!op1->aopu.bytes[offset1 + 1].in_reg || op1->aopu.bytes[offset1 + 1].byteu.reg->rIdx != r1Idx + 1);
        return -1;
      switch (r1Idx)
        {
        case XL_IDX:
          r1Idx = X_IDX;
          break;
        case YL_IDX:
          r1Idx = Y_IDX;
          break;
        case ZL_IDX:
          r1Idx = Z_IDX;
          break;
        default:
          return -1;
        }
    }

  if (r0Idx == Y_IDX)
    if (r1Idx == X_IDX) // ldw y, x
      {
        *prefixes = 0;
        return 1;
      }
    else if (op1->type == AOP_LIT && byteOfVal (op1->aopu.aop_lit, offset1 + 1) == ((byteOfVal (op1->aopu.aop_lit, offset1) & 0x80) ? 0xff : 0x00) || offset1 >= op1->size) // ldw y, #d
      {
        *prefixes = 0;
        return 2;
      }
    else if (op1->type == AOP_LIT || op1->type == AOP_IMMD || // ldw y, #ii
      op1->type == AOP_DIR) // ldw y, mm
      {
        *prefixes = 0;
        return 3;
      }
    else if (op1->type == AOP_STK || op1->type == AOP_REGSTK && r1Idx == -1) // ldw y, (n, sp)
      {
        *prefixes = 0;
        return 2;
      }
    else
      return -1;
  else if (r1Idx == Y_IDX)
    if (r0Idx == X_IDX) // ldw x, y
      {
        *prefixes = 0;
        return 1;
      }
    else if (op0->type == AOP_DIR) // ldw mm, y
      {
        *prefixes = 0;
        return 3;
      }
    else if (op0->type == AOP_STK || op0->type == AOP_REGSTK && r0Idx == -1) // ldw (n, sp), y
      {
        *prefixes = 0;
        return 2;
      }

  if (r0Idx == X_IDX || r0Idx == Z_IDX || // Try with alternate accumulator prefix.
    r1Idx == X_IDX || r1Idx == Z_IDX)
    {
      bool replace0 = (r0Idx == X_IDX || r0Idx == Z_IDX);
      bool replace1 = (r1Idx == X_IDX || r1Idx == Z_IDX); // TODO: Each prefix can only replave one!
      int bytes = ldw_bytes (prefixes, replace0 ? ASMOP_Y : op0, replace0 ? 0 : offset0, replace1 ? ASMOP_Y : op1, replace1 ? 0 : offset1);
      if (bytes >= 0)
        {
          (*prefixes)++;
          return bytes + 1;
        }
    }

  return -1;
}

static void
ldw_cost (const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  int prefixes;
  int bytes = ldw_bytes (&prefixes, op0, offset0, op1, offset1);

  wassert_bt (bytes > 0);

  cost (bytes, prefixes + 1);
}

static void
opw_cost (const asmop *op0, int offset0)
{
  int r0Idx = ((aopRS (op0) && op0->aopu.bytes[offset0].in_reg)) ? op0->aopu.bytes[offset0].byteu.reg->rIdx : -1;

  if (r0Idx < 0)
    wassert(0);

  if (!op0->aopu.bytes[offset0 + 1].in_reg || op0->aopu.bytes[offset0 + 1].byteu.reg->rIdx != r0Idx + 1);
    wassert(0);
  switch (r0Idx)
    {
    case XL_IDX:
      r0Idx = X_IDX;
      break;
    case YL_IDX:
      r0Idx = Y_IDX;
      break;
    case ZL_IDX:
      r0Idx = Z_IDX;
      break;
    default:
      wassert(0);
    }

  cost (1 + (r0Idx != Y_IDX), 1 + (r0Idx != Y_IDX));
}

static void
xch_cost (const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  if (op0->aopu.bytes[offset0].in_reg && op1->aopu.bytes[offset1].in_reg)
    if (op0->aopu.bytes[offset0].byteu.reg->rIdx == XL_IDX && op1->aopu.bytes[offset1].byteu.reg->rIdx == XH_IDX)
      opw_cost (ASMOP_X, 0);
    else if (op0->aopu.bytes[offset0].byteu.reg->rIdx == YL_IDX && op1->aopu.bytes[offset1].byteu.reg->rIdx == YH_IDX)
      opw_cost (ASMOP_Y, 0);
    else if (op0->aopu.bytes[offset0].byteu.reg->rIdx == ZL_IDX && op1->aopu.bytes[offset1].byteu.reg->rIdx == ZH_IDX)
      opw_cost (ASMOP_Z, 0);
    else
      wassert_bt (0);
  else if ((op1->type == AOP_STK || op1->type == AOP_REGSTK) && !op1->aopu.bytes[offset1].in_reg)
    cost (2, 1);
  else 
    wassert (0);
}

static void
emit3cost (enum asminst inst, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  switch (inst)
  {
  case A_SBC:
  case A_SUB:
    wassertl_bt (op1->type != AOP_LIT && op1->type != AOP_IMMD, "Subtraction with constant right operand not available.");
  case A_ADC:
  case A_ADD:
  case A_AND:
  case A_CP:
  case A_OR:
  case A_XOR:
    op2_cost (op0, offset0, op1, offset1);
    break;
  case A_LD:
    ld_cost (op0, offset0, op1, offset1);
    break;
  case A_CLR:
  case A_TST:
    op_cost (op0, offset0);
    break;
  case A_XCH:
    xch_cost (op0, offset0, op1, offset1);
    break;
  default:
    wassertl_bt (0, "Tried to get cost for unknown 8-bit instruction");
  }
}

static void
emit3wcost (enum asminst inst, const asmop *op0, int offset0, const asmop *op1, int offset1)
{
  switch (inst)
  {
  case A_SBCW:
  case A_SUBW:
    wassertl_bt (op1->type != AOP_LIT && op1->type != AOP_IMMD, "Subtraction with constant right operand not available.");
  case A_ADCW:
  case A_ADDW:
    op2w_cost (op0, offset0, op1, offset1);
    break;
  case A_LDW:
    ldw_cost (op0, offset0, op1, offset1);
    break;
  default:
    wassertl_bt (0, "Tried to get cost for unknown 8-bit instruction");
  }
}

static void
emit3_o (enum asminst inst, asmop *op0, int offset0, asmop *op1, int offset1)
{
  emit3cost (inst, op0, offset0, op1, offset1);
  if (regalloc_dry_run)
    return;

  if (op1)
    {
      char *l = Safe_strdup (aopGet (op0, offset0));
      emit2 (asminstnames[inst], "%s, %s", l, aopGet (op1, offset1));
      Safe_free (l);
    }
  else
    emit2 (asminstnames[inst], "%s", aopGet (op0, offset0));
}

static void
emit3 (enum asminst inst, asmop *op0, asmop *op1)
{
  emit3_o (inst, op0, 0, op1, 0);
}

static void
emit3w_o (enum asminst inst, asmop *op0, int offset0, asmop *op1, int offset1)
{
  emit3wcost (inst, op0, offset0, op1, offset1);
  if (regalloc_dry_run)
    return;

  if (op1)
    {
      char *l = Safe_strdup (aopGet2 (op0, offset0));
      emit2 (asminstnames[inst], "%s, %s", l, aopGet2 (op1, offset1));
      Safe_free (l);
    }
  else
    emit2 (asminstnames[inst], "%s", aopGet2 (op0, offset0));
}

static void
emit3w (enum asminst inst, asmop *op0, asmop *op1)
{
  emit3w_o (inst, op0, 0, op1, 0);
}

static bool
regDead (int idx, const iCode *ic)
{
  if (idx == X_IDX)
    return (regDead (XL_IDX, ic) && regDead (XH_IDX, ic));
  if (idx == Y_IDX)
    return (regDead (YL_IDX, ic) && regDead (YH_IDX, ic));
  if (idx == Z_IDX)
    return (regDead (ZL_IDX, ic) && regDead (ZH_IDX, ic));

  if ((idx == ZL_IDX || idx == ZH_IDX) && f8_extend_stack)
    return false;

  return (!bitVectBitValue (ic->rSurv, idx));
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

  aop->regs[XL_IDX] = -1;
  aop->regs[XH_IDX] = -1;
  aop->regs[YL_IDX] = -1;
  aop->regs[YH_IDX] = -1;
  aop->regs[ZL_IDX] = -1;
  aop->regs[ZH_IDX] = -1;
  aop->regs[C_IDX] = -1;

  return (aop);
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

  // Unlike other backends we really free asmops; to avoid a double-free, we need to support multiple asmops for the same symbol.

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
      int offset;
      long int base;

      aop = newAsmop (AOP_STK);
      aop->size = getSize (sym->type);

      base = sym->stack + (sym->stack > 0 ? G.stack.param_offset : 0);

      if (labs(base) > (1 << 15))
      {
        if (!regalloc_dry_run)
          werror (W_INVALID_STACK_LOCATION);
        base = 0;
      }

      for(offset = 0; offset < aop->size; offset++)
        aop->aopu.bytes[offset].byteu.stk = base + aop->size - offset;
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
/* aopForRemat - rematerializes an object                          */
/*-----------------------------------------------------------------*/
static asmop *
aopForRemat (symbol *sym)
{
  iCode *ic = sym->rematiCode;
  asmop *aop;
  int val = 0;

  wassert_bt (ic);

  for (;;)
    {
      if (ic->op == '+')
        {
          if (isOperandLiteral (IC_RIGHT (ic)))
            {
              val += (int) operandLitValue (IC_RIGHT (ic));
              ic = OP_SYMBOL (IC_LEFT (ic))->rematiCode;
            }
          else
            {
              val += (int) operandLitValue (IC_LEFT (ic));
              ic = OP_SYMBOL (IC_RIGHT (ic))->rematiCode;
            }
        }
      else if (ic->op == '-')
        {
          val -= (int) operandLitValue (IC_RIGHT (ic));
          ic = OP_SYMBOL (IC_LEFT (ic))->rematiCode;
        }
      else if (IS_CAST_ICODE (ic))
        {
          ic = OP_SYMBOL (IC_RIGHT (ic))->rematiCode;
        }
      else if (ic->op == ADDRESS_OF)
        {
          val += (int) operandLitValue (IC_RIGHT (ic));
          break;
        }
      else
        wassert_bt (0);
    }

  if (OP_SYMBOL (IC_LEFT (ic))->onStack)
    {
      aop = newAsmop (AOP_STL);
      aop->aopu.stk_off = (long)(OP_SYMBOL (IC_LEFT (ic))->stack) + 1 + val;
    }
  else
    {
      aop = newAsmop (AOP_IMMD);
      aop->aopu.immd = OP_SYMBOL (IC_LEFT (ic))->rname;
      aop->aopu.immd_off = val;
    }

  aop->size = getSize (sym->type);

  return aop;
}

/*-----------------------------------------------------------------*/
/* aopOp - allocates an asmop for an operand  :                    */
/*-----------------------------------------------------------------*/
static void
aopOp (operand *op, const iCode *ic)
{
  symbol *sym;
  unsigned int i;

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

  sym = OP_SYMBOL (op);

  /* if this is a true symbol */
  if (IS_TRUE_SYMOP (op))
    {
      op->aop = aopForSym (ic, sym);
      return;
    }

  /* Rematerialize symbols where all bytes are spilt. */
  if (sym->remat && (sym->isspilt || regalloc_dry_run))
    {
      bool completely_spilt = TRUE;
      for (i = 0; i < getSize (sym->type); i++)
        if (sym->regs[i])
          completely_spilt = FALSE;
      if (completely_spilt)
        {
          op->aop = aopForRemat (sym);
          return;
        }
    }

  /* if the type is a conditional */
  if (sym->regType == REG_CND)
    {
      asmop *aop = newAsmop (AOP_CND);
      op->aop = aop;
      sym->aop = sym->aop;
      return;
    }

  /* None of the above, which only leaves temporaries. */
  { 
    bool completely_in_regs = TRUE;
    bool completely_on_stack = TRUE;
    asmop *aop = newAsmop (AOP_REGSTK);

    aop->size = getSize (operandType (op));
    op->aop = aop;

    for (i = 0; i < aop->size; i++)
      {
        aop->aopu.bytes[i].in_reg = !!sym->regs[i];
        if (sym->regs[i])
          {
            completely_on_stack = FALSE;
            aop->aopu.bytes[i].byteu.reg = sym->regs[i];
            aop->regs[sym->regs[i]->rIdx] = i;
          }
        else if (sym->isspilt && sym->usl.spillLoc || sym->nRegs && regalloc_dry_run)
          {
            completely_in_regs = false;

            if (!regalloc_dry_run)
              {
                aop->aopu.bytes[i].byteu.stk = (long int)(sym->usl.spillLoc->stack) + i;

                if (sym->usl.spillLoc->stack + i < -G.stack.pushed)
                  {
                    fprintf (stderr, "%s %d %d %d %d at ic %d\n", sym->name, (int)(sym->usl.spillLoc->stack), (int)(aop->size), (int)(i), (int)(G.stack.pushed), ic->key);
                    wassertl_bt (0, "Invalid stack offset.");
                  }
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

                aop->aopu.bytes[i].byteu.stk = old_base + aop->size - i;
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
    else if (completely_on_stack)
      aop->type = AOP_STK;

    return;
  }
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

static void
push (const asmop *op, int offset, int size)
{
  if (size == 1)
    {
      emit2 ("push", "%s", aopGet (op, offset));
      if (aopInReg (op, offset, XL_IDX) || aopInReg (op, offset, XH_IDX) || aopInReg (op, offset, YL_IDX) || aopInReg (op, offset, YH_IDX) || aopInReg (op, offset, ZL_IDX))
        cost (1, 1);
      else if (op->type == AOP_LIT || op->type == AOP_IMMD)
        cost (2, 1);
      else if (op->type == AOP_DIR)
        cost (3, 1);
      else if (aopOnStack (op, offset, 1))
        cost (2, 1);
      else
        wassertl_bt (0, "Invalid aop type for size 1 for push");
    }
  else if (size == 2)
    {
      emit2 ("pushw", aopGet2 (op, offset));
      if (aopInReg (op, offset, Y_IDX) || aopInReg (op, offset, X_IDX))
        cost (1, 1);
      else if (aopInReg (op, offset, Z_IDX))
        cost (2, 2);
      else if (op->type == AOP_LIT || op->type == AOP_IMMD || op->type == AOP_DIR)
        cost (1, 3);
      else if (aopOnStack (op, offset, 2))
        cost (1, 2);
      else
        wassertl_bt (0, "Invalid aop type for size 2 for pushw");
    }
  else
    wassertl_bt (0, "Invalid size for push/pushw");

  G.stack.pushed += size;
  updateCFA ();
}

static void
pop (const asmop *op, int offset, int size)
{
  if (size == 1)
    {
      emit2 ("pop", "%s", aopGet (op, offset));
      if (aopInReg (op, offset, XL_IDX))
        cost (1, 1);
      else if (aopInReg (op, offset, XH_IDX) || aopInReg (op, offset, YL_IDX) || aopInReg (op, offset, YH_IDX) || aopInReg (op, offset, ZL_IDX))
        cost (2, 2);
      else
        wassertl_bt (0, "Invalid aop type for size 1 for pop");
    }
  else if (size == 2)
    {
      emit2 ("popw", aopGet2 (op, offset));
      if (aopInReg (op, offset, Y_IDX))
        cost (1, 1);
      else if (aopInReg (op, offset, X_IDX) || aopInReg (op, offset, Z_IDX))
        cost (2, 2);
      else
        wassertl_bt (0, "Invalid aop type for size 2 for popw");
    }
  else
    wassertl_bt (0, "Invalid size for pop/popw");

  G.stack.pushed -= size;
  updateCFA ();
}

/*-----------------------------------------------------------------*/
/* genCopyStack - Copy the value - stack to stack only             */
/*-----------------------------------------------------------------*/
static void
genCopyStack (asmop *result, int roffset, asmop *source, int soffset, int n, bool *assigned, int *size, bool xl_free, bool xh_free, bool y_free, bool z_free, bool really_do_it_now)
{
  for (int i = 0; i < n;)
    {
      if (!aopOnStack (result, roffset + i, 1) || !aopOnStack (source, soffset + i, 1))
        {
          i++;
          continue;
        }

      // Same location.
      if (!assigned[i] &&
        result->aopu.bytes[roffset + i].byteu.stk == source->aopu.bytes[soffset + i].byteu.stk)
        {
          wassert_bt (*size >= 1);

          assigned[i] = TRUE;
          (*size)--;
          i++;
          continue;
        }

      if (xl_free || really_do_it_now)
        {
          if (!xl_free)
            push (ASMOP_XL, 0, 1);
          emit3_o (A_LD, ASMOP_XL, 0, source, soffset + i);
          emit3_o (A_LD, result, roffset + i, ASMOP_XL, 0);
          if (!xl_free)
            pop (ASMOP_XL, 0, 1);
          assigned[i] = true;
        }
      i++;
      
    }
}

static void
genMove_o (asmop *result, int roffset, asmop *source, int soffset, int size, bool xl_dead_global, bool xh_dead_global, bool y_dead_global, bool z_dead_global);

/*-----------------------------------------------------------------*/
/* genCopy - Copy the value from one reg/stk asmop to another      */
/*-----------------------------------------------------------------*/
static void
genCopy (asmop *result, int roffset, asmop *source, int soffset, int sizex, bool xl_dead_global, bool xh_dead_global, bool y_dead_global, bool z_dead_global)
{
  int n = (sizex < source->size - soffset) ? sizex : (source->size - soffset);
  bool assigned[8] = {false, false, false, false, false, false, false, false};
  int size = n;
  int regsize = 0;
  for (int i = 0; i < n; i++)
    regsize += source->aopu.bytes[soffset + i].in_reg;

  // Do nothing for coalesced bytes.
  for (int i = 0; i < n; i++)
    if (result->aopu.bytes[roffset + i].in_reg && source->aopu.bytes[soffset + i].in_reg && result->aopu.bytes[roffset + i].byteu.reg == source->aopu.bytes[soffset + i].byteu.reg)
      {
        assigned[i] = true;
        regsize--;
        size--;
      }

  // Move everything from registers to the stack.
  for (int i = 0; i < n;)
    {
      if (aopOnStack (result, roffset + i, 1) &&
        (aopInReg (source, soffset + i, XL_IDX) || aopInReg (source, soffset + i, XH_IDX) || aopInReg (source, soffset + i, YL_IDX) || aopInReg (source, soffset + i, ZL_IDX)))
        {
          emit3_o (A_LD, result, roffset + i, source, soffset + i);
          assigned[i] = true;
          size--;
          regsize--;
          i++;
        }
      else if (aopOnStack (result, roffset + i, 1) && !aopOnStack (source, soffset + i, 1))
        {
          wassert (regalloc_dry_run);
          cost (100, 100);
          assigned[i] = true;
          size--;
          i++;
        }
      else // This byte is not a register-to-stack copy.
        i++;
    }

  // Copy (stack-to-stack) what we can with whatever free regs we have.
  genCopyStack (result, roffset, source, soffset, n, assigned, &size, false, false, false, false, false);

  // Now do the register shuffling.
  if (regsize)
    {
      wassert (regalloc_dry_run);
      cost (100, 100);
    }

  // Move everything from stack to registers.
  for (int i = 0; i < n;)
    {
      if (aopOnStack (source, soffset + i, 1) &&
        (aopInReg (result, roffset + i, XL_IDX) || aopInReg (result, roffset + i, XH_IDX) || aopInReg (result, roffset + i, YL_IDX) || aopInReg (result, roffset + i, ZL_IDX)))
        {
          emit3_o (A_LD, result, roffset + i, source, soffset + i);
          assigned[i] = true;
          size--;
          i++;
        }
      else if (!aopOnStack (result, roffset + i, 1) && aopOnStack (source, soffset + i, 1))
        {
          wassert (regalloc_dry_run);
          cost (100, 100);
          assigned[i] = true;
          size--;
          i++;
        }
      else // This byte is not a stack-to-register copy.
        i++;
    }

  // Copy (stack-to-stack) whatever is left.
  genCopyStack (result, roffset, source, soffset, n, assigned, &size, false, false, false, false, true);
  
  wassertl_bt (size >= 0, "genCopy() copied more than there is to be copied.");

// Place leading zeroes.
  for (int i = source->size - soffset; i < sizex;)
    {
      if (assigned[i])
        {
          i++;
          continue;
        }
        
      int s = 1;
      for (int j = i + 1; j < sizex && !assigned[j]; j++, s++);

      genMove_o (result, roffset + i, ASMOP_ZERO, 0, s, false, false, false, false);
      i += s;
    }

  if (size)
    {
      if (!regalloc_dry_run)
        {
          wassertl_bt (0, "genCopy failed to completely copy operands.");
          fprintf (stderr, "%d bytes left.\n", size);
          fprintf (stderr, "left type %d source type %d\n", result->type, source->type);
          for (int i = 0; i < n ; i++)
            fprintf (stderr, "Byte %d, result in reg %d, source in reg %d. %s assigned.\n", i, result->aopu.bytes[roffset + i].in_reg ? result->aopu.bytes[roffset + i].byteu.reg->rIdx : -1, source->aopu.bytes[soffset + i].in_reg ? source->aopu.bytes[soffset + i].byteu.reg->rIdx : -1, assigned[i] ? "" : "not");
        }
      cost (180, 180);
    }
}

/*-----------------------------------------------------------------*/
/* genMove - Copy part of one asmop to another                     */
/*-----------------------------------------------------------------*/
static void
genMove_o (asmop *result, int roffset, asmop *source, int soffset, int size, bool xl_dead_global, bool xh_dead_global, bool y_dead_global, bool z_dead_global)
{
  wassertl_bt (result->type != AOP_LIT, "Trying to write to literal.");
  wassertl_bt (result->type != AOP_IMMD, "Trying to write to immediate.");
  wassertl_bt (roffset + size <= result->size, "Trying to write beyond end of operand");

  if (aopRS (result) && aopRS (source))
    {
      genCopy (result, roffset, source, soffset, size, xl_dead_global, xh_dead_global, y_dead_global, z_dead_global);
      return;
    }

  if (source->type == AOP_STL)
    {
      wassert(0); // Todo: Implement.
      return;
    }

  for (int i = 0; i < size;)
    {
      const bool xl_dead = xl_dead_global &&
        (!aopRS (result) || (result->regs[XL_IDX] >= (roffset + i) || result->regs[XL_IDX] < 0)) &&
        (!aopRS (source) || source->regs[XL_IDX] <= i);
      const bool xh_dead = xh_dead_global &&
        (!aopRS (result) || (result->regs[XH_IDX] >= (roffset + i) || result->regs[XH_IDX] < 0)) &&
        (!aopRS (source) || source->regs[XH_IDX] <= i);
      const bool y_dead = y_dead_global &&
        (!aopRS (result) || (result->regs[YL_IDX] >= (roffset + i) || result->regs[YL_IDX] < 0) && (result->regs[YH_IDX] >= (roffset + i) || result->regs[YH_IDX] < 0)) &&
        (!aopRS (source) || source->regs[YL_IDX] <= i + 1 && source->regs[YH_IDX] <= i + 1);
      const bool z_dead = z_dead_global &&
        (!aopRS (result) || (result->regs[ZL_IDX] >= (roffset + i) || result->regs[ZL_IDX] < 0) && (result->regs[ZH_IDX] >= (roffset + i) || result->regs[ZH_IDX] < 0)) &&
        (!aopRS (source) || source->regs[ZL_IDX] <= i + 1 && source->regs[ZH_IDX] <= i + 1);

      bool via_xl =
        !aopInReg (result, roffset + i, XL_IDX) && !aopInReg (source, soffset + i, XL_IDX) &&
        !((aopInReg (result, roffset + i, XH_IDX) || aopInReg (result, roffset + i, YL_IDX) || aopInReg (result, roffset + i, ZL_IDX)) && (source->type == AOP_LIT || source->type == AOP_IMMD || source->type == AOP_DIR)) &&
        !(result->type == AOP_DIR && (aopInReg (source, soffset + i, XH_IDX) || aopInReg (source, soffset + i, YL_IDX) || aopInReg (source, soffset + i, ZL_IDX)));
      if (via_xl)
        {
          if (!xl_dead)
            push (ASMOP_XL, 0, 1);
          emit3_o (A_LD, ASMOP_XL, 0, source, soffset + i);
          emit3_o (A_LD, result, roffset + i, ASMOP_XL, 0);
          if (!xl_dead)
            pop (ASMOP_XL, 0, 1);
        }
      else
        emit3_o (A_LD, result, roffset + i, source, soffset + i);
      i++;
    }
}

/*-----------------------------------------------------------------*/
/* genMove - Copy the value from one asmop to another              */
/*-----------------------------------------------------------------*/
static void
genMove (asmop *result, asmop *source, bool xl_dead, bool xh_dead, bool y_dead, bool z_dead)
{
  genMove_o (result, 0, source, 0, result->size, xl_dead, xh_dead, y_dead, z_dead);
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

// Get asmop for registers containing the return type of function
// Returns 0 if the function does not have a return value or it is not returned in registers.
static asmop *
aopRet (sym_link *ftype)
{
  wassert (IS_FUNC (ftype));

  int size = getSize (ftype->next);

  switch (size)
    {
    case 1:
      return (ASMOP_XL);
    case 2:
      return (ASMOP_Y);
    default:
      return 0;
    }
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
    	G.stack.pushed -= m;
        updateCFA ();
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
/* genReturn - generate code for return statement                  */
/*-----------------------------------------------------------------*/
static void
genReturn (const iCode *ic)
{
  operand *left = IC_LEFT (ic);
  int size;

  D (emit2 ("; genReturn", ""));

  /* if we have no return value then
     just generate the "ret" */
  if (!IC_LEFT (ic))
    goto jumpret;

  /* we have something to return then
     move the return value into place */
  aopOp (left, ic);
  size = left->aop->size;

  switch (size)
    {
    case 0:
      break;
    case 1:
    case 2:
      genMove (aopRet (currFunc->type), left->aop, true, true, true, true);
      break;
    default:
      wassertl (0, "Return not implemented for return value of this size.");
    }

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

  if (options.debug && !regalloc_dry_run)
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

  emitJP(IC_LABEL (ic), 1.0f);
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

  asmop *leftop;
  asmop *rightop;

  D (emit2 ("; genPlus", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = result->aop->size;

  /* Swap if left is literal. */
  if (leftop->type == AOP_LIT || rightop->type != AOP_LIT && leftop->type == AOP_IMMD) // todo: Swap in more cases when right in reg, left not. Swap individually per-byte.
    {
      operand *t = right;
      right = left;
      left = t;
    }

  leftop = left->aop;
  rightop = right->aop;

  for (int i = 0, started = false; i < size;)
    {
       bool xl_free = regDead (XL_IDX, ic) && leftop->regs[XL_IDX] <= i && rightop->regs[XL_IDX] <= i && (result->aop->regs[XL_IDX] < 0 || result->aop->regs[XL_IDX] >= i);

       if (!xl_free || aopInReg (rightop, XL_IDX, i))
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }

       genMove_o (ASMOP_XL, 0, leftop, i, true, false, false, false, false);
       if (aopIsOp8_2 (rightop, i))
         emit3_o (started ? A_ADC : A_ADD, ASMOP_XL, 0, rightop, i);
       else
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }
       genMove_o (result->aop, i, ASMOP_XL, 0, true, false, false, false, false);
       i++;
       started = true;
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genXor - code for xor                                           */
/*-----------------------------------------------------------------*/
static void
genXor (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genXor", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = getSize (operandType (result));

  /* Prefer literal on right. */
  if (left->aop->type == AOP_LIT || right->aop->type != AOP_LIT && left->aop->type == AOP_IMMD) // todo: Swap in more cases when right in reg, left not. Swap individually per-byte.
    {
      operand *t = right;
      right = left;
      left = t;
    }

  for (int i = 0; i < size;)
    {
       bool xl_free = regDead (XL_IDX, ic) && left->aop->regs[XL_IDX] <= i && right->aop->regs[XL_IDX] <= i && (result->aop->regs[XL_IDX] < 0 || result->aop->regs[XL_IDX] >= i);

       if (!xl_free || aopInReg (right->aop, XL_IDX, i))
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }

       genMove_o (ASMOP_XL, 0, left->aop, i, true, false, false, false, false);
       if (aopIsOp8_2 (right->aop, i))
         emit3_o (A_XOR, ASMOP_XL, 0, right->aop, i);
       else
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }
       genMove_o (result->aop, i, ASMOP_XL, 0, true, false, false, false, false);
       i++;
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genOr - code for and                                            */
/*-----------------------------------------------------------------*/
static void
genOr (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genOr", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = getSize (operandType (result));

  /* Prefer literal on right. */
  if (left->aop->type == AOP_LIT || right->aop->type != AOP_LIT && left->aop->type == AOP_IMMD) // todo: Swap in more cases when right in reg, left not. Swap individually per-byte.
    {
      operand *t = right;
      right = left;
      left = t;
    }

  for (int i = 0; i < size;)
    {
       bool xl_free = regDead (XL_IDX, ic) && left->aop->regs[XL_IDX] <= i && right->aop->regs[XL_IDX] <= i && (result->aop->regs[XL_IDX] < 0 || result->aop->regs[XL_IDX] >= i);

       if (!xl_free || aopInReg (right->aop, XL_IDX, i))
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }

       genMove_o (ASMOP_XL, 0, left->aop, i, true, false, false, false, false);
       if (aopIsOp8_2 (right->aop, i))
         emit3_o (A_OR, ASMOP_XL, 0, right->aop, i);
       else
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }
       genMove_o (result->aop, i, ASMOP_XL, 0, true, false, false, false, false);
       i++;
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genAnd - code for and                                           */
/*-----------------------------------------------------------------*/
static void
genAnd (const iCode *ic, iCode *ifx)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  D (emit2 ("; genAnd", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  int size = getSize (operandType (result));

  /* Prefer literal on right. */
  if (left->aop->type == AOP_LIT || right->aop->type != AOP_LIT && left->aop->type == AOP_IMMD) // todo: Swap in more cases when right in reg, left not. Swap individually per-byte.
    {
      operand *t = right;
      right = left;
      left = t;
    }

  wassert (!ifx);

  for (int i = 0; i < size;)
    {
       bool xl_free = regDead (XL_IDX, ic) && left->aop->regs[XL_IDX] <= i && right->aop->regs[XL_IDX] <= i && (result->aop->regs[XL_IDX] < 0 || result->aop->regs[XL_IDX] >= i);

       if (!xl_free || aopInReg (right->aop, XL_IDX, i))
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }

       genMove_o (ASMOP_XL, 0, left->aop, i, true, false, false, false, false);
       if (aopIsOp8_2 (right->aop, i))
         emit3_o (A_AND, ASMOP_XL, 0, right->aop, i);
       else
         {
           wassert (regalloc_dry_run);
           cost (100, 100);
         }
       genMove_o (result->aop, i, ASMOP_XL, 0, true, false, false, false, false);
       i++;
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genPointerGet - generate code for read via pointer              */
/*-----------------------------------------------------------------*/
static void
genPointerGet (const iCode *ic)
{
  operand *result = IC_RESULT (ic);
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  bool bit_field = IS_BITVAR (getSpec (operandType (result)));
  int blen = bit_field ? SPEC_BLEN (getSpec (operandType (result))) : 0;
  int bstr = bit_field ? SPEC_BSTR (getSpec (operandType (result))) : 0;
  
  D (emit2 ("; genPointerGet", ""));

  aopOp (IC_LEFT (ic), ic);
  aopOp (IC_RIGHT (ic), ic);
  aopOp (IC_RESULT (ic), ic);

  if (result->aop->type == AOP_DUMMY)
    D (emit2 ("; Dummy read", ""));

  wassertl (right, "GET_VALUE_AT_ADDRESS without right operand");
  wassertl (IS_OP_LITERAL (IC_RIGHT (ic)), "GET_VALUE_AT_ADDRESS with non-literal right operand");

  int size = result->aop->size;
  // todo: What if right operand is negative?
  int offset = byteOfVal (right->aop->aopu.aop_lit, 1) * 256 + byteOfVal (right->aop->aopu.aop_lit, 0);
  
  wassert (!bit_field);
  wassert (abs (offset) <= 200);

  bool y_dead = regDead (Y_IDX, ic) && right->aop->regs[YL_IDX] < 0 && right->aop->regs[YH_IDX] < 0;

  if (aopInReg (left->aop, 0, Y_IDX))
    ;
  else if (y_dead)
    genMove (ASMOP_Y, left->aop, false, false, y_dead, false);
  else
    {
      wassert (regalloc_dry_run);
      cost (100, 100);
    }

  for (int i = 0; !bit_field ? i < size : blen > 0; i++, blen -= 8)
    {
      bool xl_dead = regDead (XL_IDX, ic) && (result->aop->regs[XL_IDX] < 0 || result->aop->regs[XL_IDX] >= i);

      if (!xl_dead)
        {
          wassert (regalloc_dry_run);
          cost (100, 100);
        }

      emit2 ("ld", "xl, (%d, y)", offset + i);
      cost (2, 1);

      genMove_o (result->aop, i, ASMOP_XL, 0, 1, true, false, false, false);
    }

  freeAsmop (right);
  freeAsmop (left);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genPointerSet - generate code for write via pointer             */
/*-----------------------------------------------------------------*/
static void
genPointerSet (const iCode *ic)
{
  operand *left = IC_LEFT (ic);
  operand *right = IC_RIGHT (ic);

  bool bit_field = IS_BITVAR (getSpec (operandType (right))) || IS_BITVAR (getSpec (operandType (left)));
  int blen = bit_field ? (SPEC_BLEN (getSpec (operandType (IS_BITVAR (getSpec (operandType (right))) ? right : left)))) : 0;
  int bstr = bit_field ? (SPEC_BSTR (getSpec (operandType (IS_BITVAR (getSpec (operandType (right))) ? right : left)))) : 0;

  D (emit2 ("; genPointerSet", ""));

  aopOp (left, ic);
  aopOp (right, ic);

  int size = right->aop->size;

  wassert (!bit_field);

  bool y_dead = regDead (Y_IDX, ic) && right->aop->regs[YL_IDX] < 0 && right->aop->regs[YH_IDX] < 0;

  if (aopInReg (left->aop, 0, Y_IDX))
    ;
  else if (y_dead)
    genMove (ASMOP_Y, left->aop, false, false, y_dead, false);
  else
    {
      wassert (regalloc_dry_run);
      cost (100, 100);
    }

  for (int i = 0; !bit_field ? i < size : blen > 0; i++, blen -= 8)
    {
      bool xl_dead = regDead (XL_IDX, ic) && (right->aop->regs[XL_IDX] <= i);

      if (aopInReg (right->aop, 0, XL_IDX))
        ;
      else if (xl_dead)
        genMove_o (ASMOP_XL, 0, right->aop, i, 1, true, false, false, false);
      else
        {
          wassert (regalloc_dry_run);
          cost (100, 100);
        }

      emit2 ("ld", "(%d, y), xl", i);
      cost (2, 1);
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
      D (emit2 ("; Dummy write", ""));
      wassert (0);
    }
  else if (result->aop->type == AOP_DUMMY)
    {
      D (emit2 ("; Dummy read", ""));
      wassert (0);
    }
  else
   genMove(result->aop, right->aop, regDead (XL_IDX, ic), regDead (XH_IDX, ic), regDead (Y_IDX, ic), regDead (Z_IDX, ic));

  freeAsmop (right);
  freeAsmop (result);
}

/*-----------------------------------------------------------------*/
/* genIfx - generate code for conditional jump                     */
/*-----------------------------------------------------------------*/
static void
genIfx (const iCode *ic)
{
  operand *const cond = IC_COND (ic);
  sym_link *type = operandType (cond);
  symbol *const tlbl = (regalloc_dry_run ? 0 : newiTempLabel (NULL));
  aopOp (cond, ic);

  D (emit2 ("; genIfx", ""));

  if (!regDead (XL_IDX, ic) || cond->aop->regs[XL_IDX] > 0)
    {
      wassert (regalloc_dry_run);
      cost (100, 100);
    }

  genMove (ASMOP_XL, cond->aop, true, regDead (XH_IDX, ic) && cond->aop->regs[XH_IDX] <= 0, regDead (Y_IDX, ic) && cond->aop->regs[YL_IDX] <= 0 && cond->aop->regs[YH_IDX] <= 0, false);

  for (int i = 1; i < cond->aop->size;)
    {
      if (aopIsOp8_2 (cond->aop, i))
        emit3_o (A_OR, ASMOP_XL, 0, cond->aop, i);
      else
        {
          wassert (regalloc_dry_run);
          cost (100, 100);
        }
      i++;
    }

  emit3 (A_TST, ASMOP_XL, 0);

  if (tlbl)
    emit2 (IC_FALSE (ic) ? "jrz" : "jrnz", "!tlabel", labelKey2num (tlbl->key));
  emitJP (IC_TRUE (ic) ? IC_TRUE (ic) : IC_FALSE (ic), 0.0f);
  emitLabel (tlbl);
  // todo : cost.

  freeAsmop (cond);
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

    case '^':
      genXor (ic);
      break;

    case '|':
      genOr (ic);
      break;

    case BITWISEAND:
      genAnd (ic, /*ifxForOp (IC_RESULT (ic), ic)*/0);
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

