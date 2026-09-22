/*-------------------------------------------------------------------------
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

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <vector>
#include <utility>
#include <set>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <random>

#include <sys/random.h>
#include <sys/stat.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#define NUM_OPCODES 256

const char *opcodenames_verilog[NUM_OPCODES] = {
	"OPCODE_TRAP",
	"OPCODE_SUB_XL_DIR",
	"OPCODE_SUB_XL_SPREL",
	"OPCODE_SUB_XL_ZREL",
	"OPCODE_SUB_XL_ZL",
	"OPCODE_SUB_XL_XH",
	"OPCODE_SUB_XL_YL",
	"OPCODE_SUB_XL_YH",
	"OPCODE_NOP",
	"OPCODE_SBC_XL_DIR",
	"OPCODE_SBC_XL_SPREL",
	"OPCODE_SBC_XL_ZREL",
	"OPCODE_SBC_XL_ZL",
	"OPCODE_SBC_XL_XH",
	"OPCODE_SBC_XL_YL",
	"OPCODE_SBC_XL_YH",
	"OPCODE_ADD_XL_IMMD",
	"OPCODE_ADD_XL_DIR",
	"OPCODE_ADD_XL_SPREL",
	"OPCODE_ADD_XL_ZREL",
	"OPCODE_ADD_XL_ZL",
	"OPCODE_ADD_XL_XH",
	"OPCODE_ADD_XL_YL",
	"OPCODE_ADD_XL_YH",
	"OPCODE_ADC_XL_IMMD",
	"OPCODE_ADC_XL_DIR",
	"OPCODE_ADC_XL_SPREL",
	"OPCODE_ADC_XL_ZREL",
	"OPCODE_ADC_XL_ZL",
	"OPCODE_ADC_XL_XH",
	"OPCODE_ADC_XL_YL",
	"OPCODE_ADC_XL_YH",
	"OPCODE_CP_XL_IMMD",
	"OPCODE_CP_XL_DIR",
	"OPCODE_CP_XL_SPREL",
	"OPCODE_CP_XL_ZREL",
	"OPCODE_CP_XL_ZL",
	"OPCODE_CP_XL_XH",
	"OPCODE_CP_XL_YL",
	"OPCODE_CP_XL_YH",
	"OPCODE_OR_XL_IMMD",
	"OPCODE_OR_XL_DIR",
	"OPCODE_OR_XL_SPREL",
	"OPCODE_OR_XL_ZREL",
	"OPCODE_OR_XL_ZL",
	"OPCODE_OR_XL_XH",
	"OPCODE_OR_XL_YL",
	"OPCODE_OR_XL_YH",
	"OPCODE_AND_XL_IMMD",
	"OPCODE_AND_XL_DIR",
	"OPCODE_AND_XL_SPREL",
	"OPCODE_AND_XL_ZREL",
	"OPCODE_AND_XL_ZL",
	"OPCODE_AND_XL_XH",
	"OPCODE_AND_XL_YL",
	"OPCODE_AND_XL_YH",
	"OPCODE_XOR_XL_IMMD",
	"OPCODE_XOR_XL_DIR",
	"OPCODE_XOR_XL_SPREL",
	"OPCODE_XOR_XL_ZREL",
	"OPCODE_XOR_XL_ZL",
	"OPCODE_XOR_XL_XH",
	"OPCODE_XOR_XL_YL",
	"OPCODE_XOR_XL_YH",
	"OPCODE_SRL_DIR",
	"OPCODE_SRL_SPREL",
	"OPCODE_SRL_XL",
	"OPCODE_SRL_YREL",
	"OPCODE_SLL_DIR",
	"OPCODE_SLL_SPREL",
	"OPCODE_SLL_XL",
	"OPCODE_SLL_YREL",
	"OPCODE_RRC_DIR",
	"OPCODE_RRC_SPREL",
	"OPCODE_RRC_XL",
	"OPCODE_RRC_YREL",
	"OPCODE_RLC_DIR",
	"OPCODE_RLC_SPREL",
	"OPCODE_RLC_XL",
	"OPCODE_RLC_YREL",
	"OPCODE_INC_DIR",
	"OPCODE_INC_SPREL",
	"OPCODE_INC_XL",
	"OPCODE_INC_YREL",
	"OPCODE_DEC_DIR",
	"OPCODE_DEC_SPREL",
	"OPCODE_DEC_XL",
	"OPCODE_DEC_YREL",
	"OPCODE_CLR_DIR",
	"OPCODE_CLR_SPREL",
	"OPCODE_CLR_XL",
	"OPCODE_CLR_YREL",
	"OPCODE_TST_DIR",
	"OPCODE_TST_SPREL",
	"OPCODE_TST_XL",
	"OPCODE_TST_YREL",
	"OPCODE_PUSH_DIR",
	"OPCODE_PUSH_SPREL",
	"OPCODE_PUSH_XL",
	"OPCODE_PUSH_YREL",
	"OPCODE_JP_IMMD",
	"OPCODE_JP_Y",
	"OPCODE_CALL_IMMD",
	"OPCODE_CALL_Y",
	"OPCODE_XCHB_XL_MM_0",
	"OPCODE_XCHB_XL_MM_1",
	"OPCODE_XCHB_XL_MM_2",
	"OPCODE_XCHB_XL_MM_3",
	"OPCODE_XCHB_XL_MM_4",
	"OPCODE_XCHB_XL_MM_5",
	"OPCODE_XCHB_XL_MM_6",
	"OPCODE_XCHB_XL_MM_7",
	"OPCODE_LDW_Y_SP",
	"OPCODE_SUBW_Y_DIR",
	"OPCODE_SUBW_Y_SPREL",
	"OPCODE_SUBW_Y_X",
	"OPCODE_LDW_ISPREL_Y",
	"OPCODE_SBCW_Y_DIR",
	"OPCODE_SBCW_Y_SPREL",
	"OPCODE_SBCW_Y_X",
	"OPCODE_ADDW_Y_IMMD",
	"OPCODE_ADDW_Y_DIR",
	"OPCODE_ADDW_Y_SPREL",
	"OPCODE_ADDW_Y_X",
	"OPCODE_ADCW_Y_IMMD",
	"OPCODE_ADCW_Y_DIR",
	"OPCODE_ADCW_Y_SPREL",
	"OPCODE_ADCW_Y_X",
	"OPCODE_LD_XL_IMMD",
	"OPCODE_LD_XL_DIR",
	"OPCODE_LD_XL_SPREL",
	"OPCODE_LD_XL_ZREL",
	"OPCODE_LD_XL_IY",
	"OPCODE_LD_XL_YREL",
	"OPCODE_LD_XL_XH",
	"OPCODE_LD_XL_YL",
	"OPCODE_LD_XL_YH",
	"OPCODE_LD_XL_ZL",
	"OPCODE_LD_XL_ZH",
	"OPCODE_LD_DIR_XL",
	"OPCODE_LD_SPREL_XL",
	"OPCODE_LD_ZREL_XL",
	"OPCODE_LD_IY_XL",
	"OPCODE_LD_YREL_XL",
	"OPCODE_PUSH_IMMD",
	"OPCODE_XCH_XL_SPREL",
	"OPCODE_XCH_XL_IY",
	"OPCODE_XCH_YL_YH",
	"OPCODE_ALTACC4",
	"OPCODE_ROT_XL_IMMD",
	"OPCODE_SRA_XL",
	"OPCODE_DA_XL",
	"OPCODE_BOOL_XL",
	"OPCODE_POP_XL",
	"OPCODE_THRD_XL",
	"OPCODE_CAX_IY_ZL_XL",
	"OPCODE_SWAPOP",
	"OPCODE_ALTACC1",
	"OPCODE_ALTACC2",
	"OPCODE_ALTACC3",
	"OPCODE_CLRW_DIR",
	"OPCODE_CLRW_SPREL",
	"OPCODE_CLRW_ZREL",
	"OPCODE_CLRW_Y",
	"OPCODE_INCW_DIR",
	"OPCODE_INCW_SPREL",
	"OPCODE_INCW_ZREL",
	"OPCODE_INCW_Y",
	"OPCODE_ADCW_DIR",
	"OPCODE_ADCW_SPREL",
	"OPCODE_ADCW_ZREL",
	"OPCODE_ADCW_Y",
	"OPCODE_SBCW_DIR",
	"OPCODE_SBCW_SPREL",
	"OPCODE_SBCW_ZREL",
	"OPCODE_SBCW_Y",
	"OPCODE_PUSHW_DIR",
	"OPCODE_PUSHW_SPREL",
	"OPCODE_PUSHW_ZREL",
	"OPCODE_PUSHW_Y",
	"OPCODE_TSTW_DIR",
	"OPCODE_TSTW_SPREL",
	"OPCODE_TSTW_ZREL",
	"OPCODE_TSTW_Y",
	"OPCODE_MSK_IY_XL_IMMD",
	"OPCODE_MUL_Y",
	"OPCODE_RET",
	"OPCODE_RETI",
	"OPCODE_MAD_X_DIR_YL",
	"OPCODE_MAD_X_SPREL_YL",
	"OPCODE_MAD_X_ZREL_YL",
	"OPCODE_MAD_X_IZ_YL",
	"OPCODE_LDW_Y_IMMD",
	"OPCODE_LDW_Y_DIR",
	"OPCODE_LDW_Y_SPREL",
	"OPCODE_LDW_Y_ZREL",
	"OPCODE_LDW_Y_YREL",
	"OPCODE_LDW_Y_IY",
	"OPCODE_LDW_Y_X",
	"OPCODE_LDW_Y_D",
	"OPCODE_LDW_DIR_Y",
	"OPCODE_LDW_SPREL_Y",
	"OPCODE_LDW_ZREL_Y",
	"OPCODE_LDW_X_Y",
	"OPCODE_LDW_Z_Y",
	"OPCODE_LDW_IY_X",
	"OPCODE_LDW_YREL_X",
	"OPCODE_LDWI_YREL_IZ",
	"OPCODE_JR_D",
	"OPCODE_DNJNZ_YH_D",
	"OPCODE_JRZ_D",
	"OPCODE_JRNZ_D",
	"OPCODE_JRC_D",
	"OPCODE_JRNC_D",
	"OPCODE_JRN_D",
	"OPCODE_JRNN_D",
	"OPCODE_ALTACC5",
	"OPCODE_JRNO_D",
	"OPCODE_JRSGE_D",
	"OPCODE_JRSLT_D",
	"OPCODE_LDW_Y_Z",
	"OPCODE_JRSLE_D",
	"OPCODE_LDW_X_IY",
	"OPCODE_JRLE_D",
	"OPCODE_SRLW_Y",
	"OPCODE_SLLW_Y",
	"OPCODE_RRCW_Y",
	"OPCODE_RLCW_Y",
	"OPCODE_SRAW_Y",
	"OPCODE_SLLW_Y_XL",
	"OPCODE_RRCW_SPREL",
	"OPCODE_RLCW_SPREL",
	"OPCODE_PUSHW_IMMD",
	"OPCODE_POPW_Y",
	"OPCODE_ADDW_SP_D",
	"OPCODE_ADDW_Y_D",
	"OPCODE_XCH_F_SPREL",
	"OPCODE_LDI_YREL_IZ",
	"OPCODE_SEX_Y_XL",
	"OPCODE_ZEX_Y_XL",
	"OPCODE_ORW_Y_IMMD",
	"OPCODE_ORW_Y_DIR",
	"OPCODE_ORW_Y_SPREL",
	"OPCODE_ORW_Y_X",
	"OPCODE_XCHW_X_IY",
	"OPCODE_XCHW_Y_SPREL",
	"OPCODE_INCNW_Y",
	"OPCODE_DECW_SPREL",
	"OPCODE_CPW_Y_IMMD",
	"OPCODE_CAXW_IY_Z_X",
	"OPCODE_NEGW_Y",
	"OPCODE_BOOLW_Y",
	"OPCODE_XORW_Y_IMMD",
	"OPCODE_XORW_Y_DIR",
	"OPCODE_XORW_Y_SPREL",
	"OPCODE_XORW_Y_X",
	};

const char *opcodenames_tex[NUM_OPCODES] = {
	"trap",
	"sub xl, mm",
	"sub xl, (n, sp)",
	"sub xl, (nn, z)",
	"sub xl, zl",
	"sub xl, xh",
	"sub xl, yl",
	"sub xl, yh",
	"",
	"sbc xl, mm",
	"sbc xl, (n, sp)",
	"sbc xl, (nn, z)",
	"sbc xl, zl",
	"sbc xl, xh",
	"sbc xl, yl",
	"sbc xl, yh",

	"add xl, \\#i",
	"add xl, mm",
	"add xl, (n, sp)",
	"add xl, (nn, z)",
	"add xl, zl",
	"add xl, xh",
	"add xl, yl",
	"add xl, yh",
	"adc xl, \\#i",
	"adc xl, mm",
	"adc xl, (n, sp)",
	"adc xl, (nn, z)",
	"adc xl, zl",
	"adc xl, xh",
	"adc xl, yl",
	"adc xl, yh",

	"cp xl, \\#i",
	"cp xl, mm",
	"cp xl, (n, sp)",
	"cp xl, (nn, z)",
	"cp xl, zl",
	"cp xl, xh",
	"cp xl, yl",
	"cp xl, yh",
	"or xl, \\#i",
	"or xl, mm",
	"or xl, (n, sp)",
	"or xl, (nn, z)",
	"or xl, zl",
	"or xl, xh",
	"or xl, yl",
	"or xl, yh",

	"and xl, \\#i",
	"and xl, mm",
	"and xl, (n, sp)",
	"and xl, (nn, z)",
	"and xl, zl",
	"and xl, xh",
	"and xl, yl",
	"and xl, yh",
	"xor xl, \\#i",
	"xor xl, mm",
	"xor xl, (n, sp)",
	"xor xl, (nn, z)",
	"xor xl, zl",
	"xor xl, xh",
	"xor xl, yl",
	"xor xl, yh",

	"srl mm",
	"srl (n, sp)",
	"srl xl",
	"srl (n, y)",
	"sll mm",
	"sll (n, sp)",
	"sll xl",
	"sll (n, y)",
	"rrc mm",
	"rrc (n, sp)",
	"rrc xl",
	"rrc (n, y)",
	"rlc mm",
	"rlc (n, sp)",
	"rlc xl",
	"rlc (n, y)",

	"inc mm",
	"inc (n, sp)",
	"inc xl",
	"inc (n, y)",
	"dec mm",
	"dec (n, sp)",
	"dec xl",
	"dec (n, y)",
	"clr mm",
	"clr (n, sp)",
	"clr xl",
	"clr (n, y)",
	"tst mm",
	"tst (n, sp)",
	"tst xl",
	"tst (n, y)",

	"push mm",
	"push (n, sp)",
	"push xl",
	"push (n, y)",
	"jp \\#ii",
	"jp y",
	"call \\#ii",
	"call y",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"ldw y, sp",
	"subw y, mm",
	"subw y, (n, sp)",
	"subw y, x",
	"ldw ((n,sp)), y",
	"sbcw y, mm",
	"sbcw y, (n, sp)",
	"sbcw y, x",
	"addw y, \\#ii",
	"addw y, mm",
	"addw y, (n, sp)",
	"addw y, x",
	"adcw y, \\#ii",
	"adcw y, mm",
	"adcw y, (n, sp)",
	"adcw y, x",

	"ld xl, \\#i",
	"ld xl, mm",
	"ld xl, (n, sp)",
	"ld xl, (nn, z)",
	"ld xl, (y)",
	"ld xl, (n, y)",
	"ld xl, xh",
	"ld xl, yl",
	"ld xl, yh",
	"ld xl, zl",
	"ld xl, zh",
	"ld mm, xl",
	"ld (n, sp), xl",
	"ld (nn, z), xl",
	"ld (y), xl",
	"ld (n, y), xl",

	"push \\#i",
	"xch xl, (n, sp)",
	"xch xl, (y)",
	"xch yl, yh",
	"altacc4",
	"rot xl, \\#i",
	"sra xl",
	"da xl",
	"bool xl",
	"pop xl",
	"thrd xl",
	"cax (y), zl, xl",
	"swapop",
	"altacc1",
	"altacc2",
	"altacc3",

	"clrw mm",
	"clrw (n, sp)",
	"clrw (nn, z)",
	"clrw y",
	"incw mm",
	"incw (n, sp)",
	"incw (nn, z)",
	"incw y",
	"adcw mm",
	"adcw (n, sp)",
	"adcw (nn, z)",
	"adcw y",
	"sbcw mm",
	"sbcw (n, sp)",
	"sbcw (nn, z)",
	"sbcw y",

	"pushw mm",
	"pushw (n, sp)",
	"pushw (nn, z)",
	"pushw y",
	"tstw mm",
	"tstw (n, sp)",
	"tstw (nn, z)",
	"tstw y",
	"msk (y), xl, \\#i",
	"mul y",
	"ret",
	"reti",
	"mad x, mm, yl",
	"mad x, (n, sp), yl",
	"mad x, (nn, z), yl",
	"mad x, (z), yl",

	"ldw y, \\#ii",
	"ldw y, mm",
	"ldw y, (n, sp)",
	"ldw y, (nn, z)",
	"ldw y, (n, y)",
	"ldw y, (y)",
	"ldw y, x",
	"ldw y, \\#d",
	"ldw mm, y",
	"ldw (n, sp), y",
	"ldw (nn, z), y",
	"ldw x, y",
	"ldw z, y",
	"ldw (y), x",
	"ldw (n, y), x",
	"ldwi (n, y), (z)",

	"jr \\#d",
	"dnjnz yh, \\#d",
	"jrz \\#d",
	"jrnz \\#d",
	"jrc \\#d",
	"jrnc \\#d",
	"jrn \\#d",
	"jrnn \\#d",
	"altacc5",
	"jrno \\#d",
	"jrsge \\#d",
	"jrslt \\#d",
	"ldw y, z",
	"jrsle \\#d",
	"ldw x, (y)",
	"jrle \\#d",

	"srlw y",
	"sllw y",
	"rrcw y",
	"rlcw y",
	"sraw y",
	"sllw y, xl",
	"rrcw (n, sp)",
	"rlcw (n, sp)",
	"pushw \\#ii",
	"popw y",
	"addw sp, \\#d",
	"addw y, \\#d",
	"xch f, (n, sp)",
	"ldi (n, y), (z)",
	"sex y, xl",
	"zex y, xl",

	"orw y, \\#ii",
	"orw y, mm",
	"orw y, (n, sp)",
	"orw y, x",
	"xchw x, (y)",
	"xchw y, (n, sp)",
	"incnw y",
	"decw (n, sp)",
	"cpw y, \\#ii",
	"caxw (y), z, x",
	"negw y",
	"boolw y",
	"xorw y, \\#ii",
	"xorw y, mm",
	"xorw y, (n, sp)",
	"xorw y, x",
	};

struct {int bytes; int cycles; const char *prefixes; int f8l; const char *macro; const char *name;} opcodenames_ucsim[NUM_OPCODES] = {
	{1, 0, "PN", 1, "TRAP", "trap"},

	{3, 3, "PA", 2, "SUB_M", "sub %a,'a16_8'" },	
	{2, 2, "PA", 2, "SUB_NSP", "sub %a,('nsp_8')" },
	{3, 3, "PA", 2, "SUB_NNZ","sub %a,('nnz_8')" },
	{1, 1, "PA", 2, "SUB_ZL","sub %a,zl" },
	{1, 1, "PA", 2, "SUB_XH","sub %a,xh" },
	{1, 1, "PA", 2, "SUB_YL","sub %a,yl" },
	{1, 1, "PA", 2, "SUB_YH","sub %a,yh" },

	{0, 0, "PN", 0, "", ""},

	{3, 3, "PA", 2, "SBC_M", "sbc %a,'a16_8'" },
	{2, 2, "PA", 2, "SBC_NSP", "sbc %a,('nsp_8')" },
	{3, 3, "PA", 2, "SBC_NNZ", "sbc %a,('nnz_8')" },
	{1, 1, "PA", 2, "SBC_ZL", "sbc %a,zl" },
	{1, 1, "PA", 2, "SBC_XH", "sbc %a,xh" },
	{1, 1, "PA", 2, "SBC_YL", "sbc %a,yl" },
	{1, 1, "PA", 2, "SBC_YH", "sbc %a,yh" },
  
	{2, 1, "PD", 1, "ADD_I", "add %a,#'i8'" },
	{3, 3, "PA", 1, "ADD_M", "add %a,'a16_8'" },
	{2, 2, "PA", 1, "ADD_NSP", "add %a,('nsp_8')" },
	{3, 3, "PA", 1, "ADD_NNZ", "add %a,('nnz_8')" },
	{1, 1, "PA", 1, "ADD_ZL", "add %a,zl" },
	{1, 1, "PA", 1, "ADD_XH", "add %a,xh" },
	{1, 1, "PA", 1, "ADD_YL", "add %a,yl" },
	{1, 1, "PA", 1, "ADD_YH", "add %a,yh" },

	{2, 1, "PD", 1, "ADC_I", "adc %a,#'i8'" },
	{3, 3, "PA", 1, "ADC_M", "adc %a,'a16_8'" },
	{2, 2, "PA", 1, "ADC_NSP", "adc %a,('nsp_8')" },
	{3, 3, "PA", 1, "ADC_NNZ", "adc %a,('nnz_8')" },
	{1, 1, "PA", 1, "ADC_ZL", "adc %a,zl" },
	{1, 1, "PA", 1, "ADC_XH", "adc %a,xh" },
	{1, 1, "PA", 1, "ADC_YL", "adc %a,yl" },
	{1, 1, "PA", 1, "ADC_YH", "adc %a,yh" },
      
	{2, 1, "PD", 2, "CP_I", "cp %a,#'i8'" },
	{3, 3, "PA", 2, "CP_M", "cp %a,'a16_8'" },
	{2, 2, "PA", 2, "CP_NSP", "cp %a,('nsp_8')" },
	{3, 3, "PA", 2, "CP_NNZ", "cp %a,('nnz_8')" },
	{1, 1, "PA", 2, "CP_ZL", "cp %a,zl" },
	{1, 1, "PA", 2, "CP_XH", "cp %a,xh" },
	{1, 1, "PA", 2, "CP_YL", "cp %a,yl" },
	{1, 1, "PA", 2, "CP_YH", "cp %a,yh" },

	{2, 1, "PD", 1, "OR_I", "or %a,#'i8'" },
	{3, 3, "PA", 1, "OR_M", "or %a,'a16_8'" },
	{2, 2, "PA", 1, "OR_NSP", "or %a,('nsp_8')" },
	{3, 3, "PA", 1, "OR_NNZ", "or %a,('nnz_8')" },
	{1, 1, "PA", 1, "OR_ZL", "or %a,zl" },
	{1, 1, "PA", 1, "OR_XH", "or %a,xh" },
	{1, 1, "PA", 1, "OR_YL", "or %a,yl" },
	{1, 1, "PA", 1, "OR_YH", "or %a,yh" },
    
	{2, 1, "PD", 1, "AND_I", "and %a,#'i8'" },
	{3, 3, "PA", 1, "AND_M", "and %a,'a16_8'" },
	{2, 2, "PA", 1, "AND_NSP", "and %a,('nsp_8')" },
	{3, 3, "PA", 1, "AND_NNZ", "and %a,('nnz_8')" },
	{1, 1, "PA", 1, "AND_ZL", "and %a,zl" },
	{1, 1, "PA", 1, "AND_XH", "and %a,xh" },
	{1, 1, "PA", 1, "AND_YL", "and %a,yl" },
	{1, 1, "PA", 1, "AND_YH", "and %a,yh" },

	{2, 1, "PD", 1, "XOR_I", "xor %a,#'i8'" },
	{3, 3, "PA", 1, "XOR_M", "xor %a,'a16_8'" },
	{2, 2, "PA", 1, "XOR_NSP", "xor %a,('nsp_8')" },
	{3, 3, "PA", 1, "XOR_NNZ", "xor %a,('nnz_8')" },
	{1, 1, "PA", 1, "XOR_ZL", "xor %a,zl" },
	{1, 1, "PA", 1, "XOR_XH", "xor %a,xh" },
	{1, 1, "PA", 1, "XOR_YL", "xor %a,yl" },
	{1, 1, "PA", 1, "XOR_YH", "xor %a,yh" },

	{3, 3, "PN", 1, "SRL_M", "srl 'a16_8'" },
	{2, 2, "PN", 1, "SRL_NSP", "srl ('nsp_8')" },
	{1, 1, "PD", 1, "SRL_A", "srl %a" },
	{2, 2, "PN", 0,"SRL_NY", "srl ('ny_8')" },
	{3, 3, "PN", 1, "SLL_M", "sll 'a16_8'" },
	{2, 2, "PN", 1, "SLL_NSP", "sll ('nsp_8')" },
	{1, 1, "PD", 1, "SLL_A", "sll %a" },
	{2, 2, "PN", 0,"SLL_NY", "sll ('ny_8')" },
	{3, 3, "PN", 1, "RRC_M", "rrc 'a16_8'" },
	{2, 2, "PN", 1, "RRC_NSP", "rrc ('nsp_8')" },
	{1, 1, "PD", 1, "RRC_A", "rrc %a" },
	{2, 2, "PN", 0,"RRC_NY", "rrc ('ny_8')" },
	{3, 3, "PN", 1, "RLC_M", "rlc 'a16_8'" },
	{2, 2, "PN", 1, "RLC_NSP", "rlc ('nsp_8')" },
	{1, 1, "PD", 1, "RLC_A", "rlc %a" },
	{2, 2, "PN", 0,"RLC_NY", "rlc ('ny_8')" },

	{3, 3, "PN", 1, "INC_M", "inc 'a16_8'" },
	{2, 2, "PN", 1, "INC_NSP", "inc ('nsp_8')" },
	{1, 1, "PD", 1, "INC_A", "inc %a" },
	{2, 2, "PN", 0,"INC_NY", "inc ('ny_8')" },
	{3, 3, "PN", 1, "DEC_M", "dec 'a16_8'" },
	{2, 2, "PN", 1, "DEC_NSP", "dec ('nsp_8')" },
	{1, 1, "PD", 1, "DEC_A", "dec %a" },
	{2, 2, "PN", 0,"DEC_NY", "dec ('ny_8')" },

	{3, 3, "PN", 1, "CLR_M", "clr 'a16_8'" },
	{2, 2, "PN", 1, "CLR_NSP", "clr ('nsp_8')" },
	{1, 1, "PD", 1, "CLR_A", "clr %a" },
	{2, 2, "PN", 0,"CLR_NY", "clr ('ny_8')" },

	{3, 3, "PN", 1, "TST_M", "tst 'a16_8'" },
	{2, 2, "PN", 1, "TST_NSP", "tst ('nsp_8')" },
	{1, 1, "PD", 1, "TST_A", "tst %a" },
	{1, 1, "PN", 0,"TST_NY", "tst ('ny_8')" },

	{3, 3, "PN", 1, "PUSH_M", "push 'a16_8'" },
	{2, 2, "PN", 1, "PUSH_NSP", "push ('nsp_8')" },
	{1, 1, "PD", 1, "PUSH_A", "push %a" },
	{2, 2, "PN", 0,"PUSH_NY", "push ('ny_8')" },
    
	{3, 3, "PN", 1, "JP_I", "jp #'a16'" },
	{1, 1, "PW", 1, "JP_A", "jp %A" },
	{3, 3, "PN", 1, "CALL_I", "call #'a16'" },
	{1, 1, "PW", 1, "CALL_A", "call %A" },
    
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0,  "", ""},
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0, "", ""},
	{0, 0, "PN", 0, "", ""},

	{1, 1, "PA", 1, "LDW_A_SP", "ldw %A, sp" }, // PA too permissive

	{3, 3, "PA", 0, "SUBW_M", "subw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "SUBW_NSP", "subw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "SUBW_X", "subw %A,%R" }, // PA too permissive

	{2, 1, "PD", 0, "LDW_DSP_A", "ldw (('nsp_16')),%A" }, // PD too permissive

	{3, 3, "PA", 0, "SBCW_M", "sbcw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "SBCW_NSP", "sbcw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "SBCW_X", "sbcw %A,%R" }, // PA too permissive

	{3, 2, "PW", 0, "ADDW_I", "addw %A,#'i16'" },
	{3, 3, "PA", 0, "ADDW_M", "addw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "ADDW_NSP", "addw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "ADDW_X", "addw %A,%R" }, // PA too permissive
	{3, 2, "PW", 0, "ADCW_I", "adcw %A,#'i16'" },
	{3, 3, "PA", 0, "ADCW_M", "adcw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "ADCW_NSP", "adcw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "ADCW_X", "adcw %A,%R" }, // PA too permissive

	{2, 1, "PD", 1, "LD8_A_I", "ld %a,#'i8'" },
	{3, 3, "PD", 1, "LD8_A_M", "ld %a,'a16_8'" },
	{2, 2, "PD", 1, "LD8_A_NSP", "ld %a,('nsp_8')" },
	{3, 3, "PD", 1, "LD8_A_NNZ", "ld %a,('nnz_8')" },
	{1, 2, "PD", 1, "LD8_A_Y", "ld %a,('y_8')" },
	{2, 2, "PD", 0, "LD8_A_NY", "ld %a,('ny_8')" },
	{1, 1, "PA", 1, "LD8_A_XH", "ld %a,xh" },
	{1, 1, "PA", 1, "LD8_A_YL", "ld %a,yl" },
	{1, 1, "PA", 1, "LD8_A_YH", "ld %a,yh" },
	{1, 1, "PA", 1, "LD8_A_ZL", "ld %a,zl" },
	{1, 1, "PA", 1, "LD8_A_ZH", "ld %a,zh" },
	{3, 2, "PD", 1, "LD8_M_A", "ld 'a16_8',%a" },
	{2, 1, "PD", 1, "LD8_NSP_A", "ld ('nsp_8'),%a" },
	{3, 2, "PD", 1, "LD8_NNZ_A", "ld ('nnz_8'),%a" },
	{1, 1, "PD", 1, "LD8_Y_A", "ld ('y_8'),%a" },
	{2, 1, "PD", 0, "LD8_NY_A", "ld ('ny_8'),%a" },

       
	{2, 1, "PN", 1, "PUSH_I", "push #'i8'" },

	{2, 2, "PD", 0, "XCH_A_NSP", "xch %a,('nsp_8')" },
	{1, 2, "PD", 1, "XCH_A_Y", "xch %a,('y_8')" },
	{1, 1, "PW", 0, "XCH_A_A", "xch %L,%H" },

	{1, 1, "PN", 1, "PREF_ALT4", "altacc4" },

	{2, 1, "PD", 0, "ROT", "rot %a,#'i8'" },
	{1, 1, "PD", 1, "SRA", "sra %a" },
	{1, 1, "PD", 1, "DAA", "da %a" },
	{1, 1, "PD", 1, "BOOL_A", "bool %a" },

	{1, 1, "PD", 1, "POP_A", "pop %a" },

	{1, 1, "PD", 1, "THRD", "thrd %a" },
    
	{1, 1, "PD", 1, "CAX", "cax ('y_8'),xh,yl" }, // PD too permissive

	{1, 1, "PN", 1, "PREF_SWAPOP", "swapop" },
	{1, 1, "PN", 1, "PREF_ALT1", "altacc1" },
	{1, 1, "PN", 1, "PREF_ALT2", "altacc2" },
	{1, 1, "PN", 1, "PREF_ALT3", "altacc3" },

	{3, 3, "PN", 1, "CLRW_M", "clrw 'a16_16'" },
	{2, 2, "PN", 1, "CLRW_NSP", "clrw ('nsp_16')" },
	{3, 3, "PN", 1, "CLRW_NNZ", "clrw ('nnz_16')" },
	{1, 1, "PW", 1, "CLRW_A", "clrw %A" },

	{3, 3, "PN", 1, "INCW_M", "incw 'a16_16'" },
	{2, 2, "PN", 1, "INCW_NSP", "incw ('nsp_16')" },
	{3, 3, "PN", 1, "INCW_NNZ", "incw ('nnz_16')" },
	{1, 1, "PW", 1, "INCW_A", "incw %A" },
	{3, 3, "PN", 0, "ADCW1_M", "adcw 'a16_16'" },
	{2, 2, "PN", 0, "ADCW1_NSP", "adcw ('nsp_16')" },
	{3, 3, "PN", 0, "ADCW1_NNZ", "adcw ('nnz_16')" },
	{1, 1, "PW", 0, "ADCW1_A", "adcw %A" },
	{3, 3, "PN", 0, "SBCW1_M", "sbcw 'a16_16'" },
	{2, 2, "PN", 0, "SBCW1_NSP", "sbcw ('nsp_16')" },
	{3, 3, "PN", 0, "SBCW1_NNZ", "sbcw ('nnz_16')" },
	{1, 1, "PW", 0, "SBCW1_A", "sbcw %A" },

	{3, 3, "PN", 1, "PUSHW_M", "pushw 'a16_16'" },
	{2, 2, "PN", 1, "PUSHW_NSP", "pushw ('nsp_16')" },
	{3, 3, "PN", 1, "PUSHW_NNZ", "pushw ('nnz_16')" },
	{1, 1, "PW", 1, "PUSHW_A", "pushw %A" },

	{3, 3, "PN", 1, "TSTW1_M", "tstw 'a16_16'" },
	{2, 2, "PN", 1, "TSTW1_NSP", "tstw ('nsp_16')" },
	{3, 3, "PN", 1, "TSTW1_NNZ", "tstw ('nnz_16')" },
	{1, 1, "PW", 1, "TSTW1_A", "tstw %A" },

	{2, 2, "PD", 1, "MSK", "msk (%A),%a,#'i8'" },

	{1, 1, "PW", 0, "MUL", "mul %A"},

	{1, 1, "PN", 1, "RET", "ret" },
	{1, 1, "PN", 1, "RETI", "reti" },

	{3, 3, "PN", 0, "MAD_M", "mad x,'a16_8',yl" },
	{2, 2, "PN", 0, "MAD_NSP", "mad x,('nsp_8'),yl" },
	{3, 3, "PN", 0, "MAD_NNZ", "mad x,('nnz_8'),yl" },
	{1, 2, "PN", 0, "MAD_Z", "mad x,('z_8'),yl" },

	{3, 2, "PW", 1, "LDW_A_I", "ldw %A,#'i16'" },
	{3, 3, "PW", 1, "LDW_A_M", "ldw %A,'a16_16'" },
	{2, 2, "PW", 1, "LDW_A_NSP", "ldw %A,('nsp_16')" },
	{3, 3, "PW", 1, "LDW_A_NNZ", "ldw %A,('nnz_16')" },
	{2, 2, "PW", 0, "LDW_A_NY", "ldw %A,('ny_16')" },
	{1, 2, "PW", 1, "LDW_A_AM", "ldw %A,(%A)" },
	{1, 1, "PW", 1, "LDW_A_X", "ldw %A,x" }, // PW too permissive
	{2, 1, "PW", 1, "LDW_A_D", "ldw %A,#%d" },
	{3, 2, "PW", 1, "LDW_M_A", "ldw 'a16_16',%A" },
	{2, 1, "PW", 1, "LDW_NSP_A", "ldw ('nsp_16'),%A" },
	{3, 2, "PW", 1, "LDW_NNZ_A", "ldw ('nnz_16'),%A" }, // PW too permissive
	{1, 1, "PW", 1, "LDW_X_A", "ldw x,%A" }, // PW too permissive
	{1, 1, "PW", 1, "LDW_Z_A", "ldw z,%A" }, // PW too permissive
	{1, 1, "PD", 1, "LDW_AM_X", "ldw (%A),%R" }, // PD too permissive
	{2, 2, "PW", 0, "LDW_NAM_X", "ldw ('nA_16'),%R" },

	{2, 2, "PN", 0, "LDWI_YREL_Z", "ldwi ('ny16'),(z)" },

	{2, 1, "PN", 1, "JR", "jr %r" },
	{2, 1, "PW", 0, "DNJNZ", "dnjnz yh,%r" },
	{2, 1, "PN", 1, "JRZ", "jrz %r" },
	{2, 1, "PN", 1, "JRNZ", "jrnz %r" },
	{2, 1, "PN", 1, "JRC", "jrc %r" },
	{2, 1, "PN", 1, "JRNC", "jrnc %r" },
	{2, 1, "PN", 1, "JRN", "jrn %r" },
	{2, 1, "PN", 1, "JRNN", "jrnn %r" },

	{1, 1, "PN", 1, "PREF_ALT5", "altacc5" },

	{2, 1, "PS", 1, "JRNO", "jrno %r" },
	{2, 1, "PN", 1, "JRSGE", "jrsge %r" },
	{2, 1, "PN", 1, "JRSLT", "jrslt %r" },

	{2, 1, "PN", 1, "LDW_A_Z", "ldw %A,z" },

	{2, 1, "PS", 1, "JRSLE", "jrsle %r" },

	{2, 1, "PD", 0, "LDW_X_AM", "ldw %R,(%A)" }, // PD too permissive

	{2, 1, "PS", 1, "JRLE", "jrle %r" },

	{1, 1, "PW", 0, "SRLW", "srlw %A"},
	{1, 1, "PW", 0, "SLLW", "sllw %A"},
	{1, 1, "PW", 0, "RRCW", "rrcw %A"},
	{1, 1, "PW", 0, "RLCW_A", "rlcw %A"},

	{1, 1, "PW", 0, "SRAW", "sraw %A"},
	{1, 1, "PD", 0, "SLLW_A_XL", "sllw %A,xl"},

	{2, 2, "PN", 0, "RRCW_NSP", "rrcw ('nsp_16')"},
	{2, 2, "PN", 0, "RLCW_NSP", "rlcw ('nsp_16')"},

	{3, 2, "PN", 1, "PUSHW_I", "pushw #'i16'" },

	{1, 1, "PW", 1, "POPW_A", "popw %A" },

	{2, 1, "PN", 1, "ADDW_SP_D", "addw sp,#%d"},

	{2, 1, "PW", 1, "ADDW_A_D", "addw %A,#%d"},

	{2, 2, "PN", 1, "XCH_F_NSP", "xch f,('nsp_16')"},

	{2, 2, "PN", 0, "LDI_YREL_Z", "ldi ('ny_8'),(z)" },

	{1, 1, "PD", 0, "SEX", "sex %A,%a"},
	{1, 1, "PD", 0, "ZEX", "zex %A,%a"},

	{3, 2, "PW", 0, "ORW_I", "orw %A,#'i16'" },
	{3, 3, "PA", 0, "ORW_M", "orw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "ORW_NSP", "orw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "ORW_X", "orw %A,%R" }, // PA too permissive

	{1, 2, "PD", 1, "XCHW_X_Y", "xchw %R,('%A')" }, // PD too permissive
	{1, 2, "PW", 0, "XCHW_Y_NSP", "xchw %A,('nsp_16')" },

	{1, 1, "PW", 0, "INCNW", "incnw %A"},

	{2, 2, "PN", 0, "DECW_NSP", "decw ('nsp_16')"},

	{3, 2, "PW", 0, "CPW", "cpw %A,#'i16'"},

	{1, 1, "PN", 1, "CAXW", "caxw ('y_16'),z,x" },

	{1, 1, "PW", 0, "NEGW", "negw %A"},

	{1, 1, "PW", 0, "BOOLW", "boolw %A"},

	{3, 2, "PW", 0, "XORW_I", "xorw %A,#'i16'" },
	{3, 3, "PA", 0, "XORW_M", "xorw %A,'a16_16'" }, // PA too permissive
	{2, 2, "PA", 0, "XORW_NSP", "xorw %A,('nsp_16')" }, // PA too permissive
	{1, 1, "PA", 0, "XORW_X", "xorw %A,%R" }, // PA too permissive
	};
	
inline static void init_table_order(uint8_t *table)
{
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
		table[i] = i;
}

inline static void init_table_random(uint8_t *table)
{
	bool assigned_opcodes[NUM_OPCODES] = {};
	getrandom (table, NUM_OPCODES, 0);
	table[0] = 0;
	assigned_opcodes[0] = true;
	for(unsigned int i = 1; i < NUM_OPCODES; i++)
	{
		while(assigned_opcodes[table[i]])
			table[i]++;
		assigned_opcodes[table[i]] = true;
	}
}

inline static void swap_columns(uint8_t *table, unsigned int rowlength, unsigned int columnheight, uint8_t s0, uint8_t s1)
{
	printf("%u x %u table, swapping columns %u, %u\n", (unsigned)rowlength, (unsigned)columnheight, (unsigned)s0, (unsigned)s1);
	for(unsigned int i = 0; i < columnheight; i++)
	{
		// trap stays at 0.
		if(rowlength * i + s0 == 0 || rowlength * i + s1 == 0)
			continue;

		uint8_t tmp = table[rowlength * i + s0];
		table[rowlength * i + s0] = table[rowlength * i + s1];
		table[rowlength * i + s1] = tmp;
	}
}

inline static void swap_rows(uint8_t *table, unsigned int rowlength, unsigned int columnheight, uint8_t s0, uint8_t s1)
{
	printf("%u x %u table, swapping rows %u, %u\n", (unsigned)rowlength, (unsigned)columnheight, (unsigned)s0, (unsigned)s1);
	for(unsigned int i = 0; i < rowlength; i++)
	{
		// trap stays at 0.
		if(rowlength * s0 + i == 0 || rowlength * s1 + i == 0)
			continue;

		uint8_t tmp = table[rowlength * s0 + i];
		table[rowlength * s0 + i] = table[rowlength * s1 + i];
		table[rowlength * s1 + i] = tmp;
	}
}

// Modify opcode table by swapping columns or rows in a random table layout.
inline static void modify_table_random(uint8_t *table)
{
	unsigned int rowlength, columnheight;
	bool rows;
	rowlength = 0;
	getrandom (&rowlength, 1, 0);
	rows = rowlength & 0x10;
	rowlength %= 9;
	rowlength = 1 << rowlength;
	columnheight = NUM_OPCODES / rowlength;

	uint8_t select[2];
	getrandom (&select, 2, 0);
	if(!rows) // Swap columns
	{
		select[0] %= rowlength;
		select[1] %= rowlength;
		swap_columns(table, rowlength, columnheight, select[0], select[1]);
	}
	else // Swap rows
	{
		select[0] %= columnheight;
		select[1] %= columnheight;
		printf("%u x %u table, swapping rows %u, %u\n", (unsigned)rowlength, (unsigned)columnheight, (unsigned)select[0], (unsigned)select[1]);
		swap_rows(table, rowlength, columnheight, select[0], select[1]);
	}
}

static void print_table_verilog(FILE *f, const uint8_t *table)
{
	fprintf(f, "typedef enum logic [7:0] {\n");
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
		fprintf(f, "\t%s = 8'h%02x%s\n", opcodenames_verilog[table[i]], i, i != 255 ? "," : "");
	fprintf(f, "} opcode_t;\n");
}

static void print_table_tex(FILE *f, const uint8_t *table)
{
	fprintf(f, "\\begin{tabular}{l||c|c|c|c|c|c|c|c|c|c|c|c|c|c|c|c}\n");
	fprintf(f, " &");
	for(unsigned int i = 0; i < 16; i++)
		fprintf(f, "x%x %s", i, i != 15 ? "& " : "\\\\\n\\hline\\hline\n");
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
	{
		if (!(i % 16))
			fprintf(f, "%xx & ", i / 16);
		fprintf(f, "%s %s", opcodenames_tex[table[i]], (i % 16 != 15) ? "& " : (i != 255) ? "\\\\\n\\hline\n" : "\n");
	}
	fprintf(f, "\\end{tabular}\n");
}


static void print_table_ucsim(FILE *f, const uint8_t *table)
{
	fprintf(f, "// For glob.cc:\n\n");

	fprintf(f, "struct dis_entry disass_f8[]=\n");
	fprintf(f, "  {\n");
	
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
		if (opcodenames_ucsim[table[i]].bytes)
			fprintf(f, "    { 0x%02x, 0xff, ' ', %d, \"%s\" },\n", i, opcodenames_ucsim[table[i]].bytes, opcodenames_ucsim[table[i]].name);

	fprintf(f, "    { 0, 0, 0, 0, 0, 0 }\n");
	fprintf(f, "  };\n\n");

	fprintf(f, "u8_t allowed_prefs[256]= {\n");
	fprintf(f, "  /*       ");
	for(unsigned int i = 0; i < 16; i++)
		fprintf(f, "x%x %s", i, i != 15 ? " " : "*/\n");
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
	{
		if (!(i % 16))
			fprintf(f, "  /* %xx */ ", i / 16);
		fprintf(f, "%s%s", opcodenames_ucsim[table[i]].prefixes, (i % 16 != 15) ? ", " : (i != 255) ? ",\n" : "\n");
	}
	fprintf(f, "};\n\n");

	fprintf(f, "u8_t f8l_instructions[256] = {\n");
	fprintf(f, "  /*       ");
	for(unsigned int i = 0; i < 16; i++)
		fprintf(f, "x%x %s", i, i != 15 ? " " : "*/\n");
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
	{
		if (!(i % 16))
			fprintf(f, "  /* %xx */ ", i / 16);
		fprintf(f, "%d%s", opcodenames_ucsim[table[i]].f8l, (i % 16 != 15) ? ",  " : (i != 255) ? ",\n" : "\n");
	}
	fprintf(f, "};\n\n");

	fprintf(f, "// For decode.h:\n\n");

	for(unsigned int i = 0; i < NUM_OPCODES; i++)
		if (opcodenames_ucsim[table[i]].bytes)
			fprintf(f, strncmp (opcodenames_ucsim[table[i]].macro, "PREF_", 5) ? "#define %s instruction_%02x\n" : "#define %s 0x%02x\n", opcodenames_ucsim[table[i]].macro, i);
}

void help(FILE *f)
{
	fprintf(f, "opcodemaptool <command> [n]\n");
	fprintf(f, "<command>:   create, startrandom <n>, walkrandom <n>, showbest <n>, deleteworst <n>\n");
	fprintf(f, "create:      init data structures, insert default opcode map\n");
	fprintf(f, "recreate:    reinit data structures (opcodemaps directory) from opcodemapstable\n");
	fprintf(f, "startrandom: add n random opcode maps\n");
	fprintf(f, "walkrandom:  add n random opcode maps that are similar to good existing ones\n");
	fprintf(f, "showbest:    add show best n opcode maps\n");
	fprintf(f, "deleteworst: delete worst n opcode maps\n");
	fprintf(f, "print_tex:   print table n as TeX table\n");
	fprintf(f, "print_ucsim: print table n as uCsim instruction tables\n");
}

std::map <std::vector<uint8_t>, unsigned long int> opcodemapstable;

void read_opcodemapstable(void)
{
	std::ifstream ifs("opcodemapstable");
	boost::archive::text_iarchive ia(ifs);
	ia >> opcodemapstable;
}

void write_opcodemapstable(void)
{
	std::ofstream ofs("opcodemapstable");
	boost::archive::text_oarchive oa(ofs);
	oa << opcodemapstable;
}

void add_opcodemap(const uint8_t *table)
{
	opcodemapstable.insert(std::pair <std::vector<uint8_t>, unsigned long int>(std::vector(table, table + NUM_OPCODES), opcodemapstable.size()));
}

void create_opcodemapsfiles(void)
{
	std::map<std::vector<uint8_t>, unsigned long int>::iterator it;
	for (it = opcodemapstable.begin(); it != opcodemapstable.end(); it++)
	{
		std::ostringstream name;
		name << "opcodemaps/";
		name << it->second;
		if (!std::filesystem::exists(name.str().c_str()))
			mkdir (name.str().c_str(), 0750);
		name << "/opcodemap.v";
		if (!std::filesystem::exists(name.str().c_str()))
		{
			FILE *file = fopen(name.str().c_str(), "wx");
			if (file)
			{
				print_table_verilog(file, it->first.data());
				fclose(file);
			}
			else
				fprintf(stderr, "Failed to open file %s", name.str().c_str());
		}
	}
}

std::multimap<unsigned int, std::vector<uint8_t>> sizes;

void get_sizes(void)
{
	sizes.clear();
	std::map<std::vector<uint8_t>, unsigned long int>::iterator it;
	for (it = opcodemapstable.begin(); it != opcodemapstable.end(); it++)
	{
		std::ostringstream name;
		name << "opcodemaps/";
		name << it->second;
		name << "/icesynth2.size";
		std::ifstream sizefile(name.str().c_str());
		unsigned int size;
		sizefile >> size;
		if(sizefile)
			sizes.insert(std::pair<unsigned int, std::vector<uint8_t>>(size, it->first));
		else
			std::cout << "No size available for " << "opcodemaps/" << it->second << "\n";
	}
}

void show_bestsizes(int n)
{
	std::multimap<unsigned int, std::vector<uint8_t>>::iterator it;
	for(it = sizes.begin(); n && it != sizes.end(); it++, n--)
	{
		std::cout << "size " << it->first << "\n";
	}
}

void delete_worst(int n)
{
	std::set<unsigned long int> deletedset;
	std::multimap<unsigned int, std::vector<uint8_t>>::reverse_iterator it;
	for(it = sizes.rbegin(); n && it != sizes.rend(); it++, n--)
	{
		std::ostringstream name;
		name << "opcodemaps/";
		name << opcodemapstable[it->second];
		if (!std::filesystem::exists(name.str().c_str()))
		{
			std::cout << "Missing directory for " << "opcodemaps/" << opcodemapstable[it->second] << "\n";
			continue;
		}
		std::filesystem::remove_all(name.str().c_str());
		deletedset.insert(opcodemapstable[it->second]);
		opcodemapstable.erase(it->second);
	}
	std::map<std::vector<uint8_t>, unsigned long int>::iterator mit;
	for(mit = opcodemapstable.begin(); mit != opcodemapstable.end(); mit++)
		if(mit->second >= opcodemapstable.size())
		{
			if(!deletedset.size())
			{
				std::cerr << "Error: data structure corrupted\n";
				return;
			}
			unsigned long int newdir = *deletedset.begin();
			std::ostringstream newname, oldname;
			oldname << "opcodemaps/"; newname << "opcodemaps/";
			oldname << mit->second; newname << newdir;
			if(!std::filesystem::exists(oldname.str().c_str()))
				std::cout << "Missing directory for " << oldname.str() << "\n";
			else if(std::filesystem::exists(newname.str().c_str()))
				std::cout << "Existing directory for " << newname.str() << "\n";
			else
				std::filesystem::rename(oldname.str().c_str(), newname.str().c_str());
			mit->second = newdir;
			deletedset.erase(newdir);
		}
}

int main(int argc, char **argv)
{
	uint8_t table[NUM_OPCODES];
	unsigned int n = 1;

	if (argc == 3 || argc == 5)
	{
		long l = strtol(argv[2], 0, 0);
		if(l < 0 || l == LONG_MAX || l > INT_MAX)
		{
			help(stderr);
			return(-1);
		}
		n = l;
	}

	if (argc == 2 && !strcmp(argv[1], "create"))
	{
		if (mkdir ("opcodemaps", 0750))
		{
			fprintf (stderr, "Failed to create opcodemaps directory\n");
			return (-1);
		}
		init_table_order(table);
		add_opcodemap(table);
		write_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 2 && !strcmp(argv[1], "recreate"))
	{
		if (mkdir ("opcodemaps", 0750))
		{
			fprintf (stderr, "Failed to create opcodemaps directory\n");
			return (-1);
		}
		read_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 3 && !strcmp(argv[1], "startrandom"))
	{
		read_opcodemapstable();
		for(unsigned int i = 0; i < n; i++)
		{
			init_table_random(table);
			add_opcodemap(table);
		}
		write_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 3 && !strcmp(argv[1], "walkrandom"))
	{
		read_opcodemapstable();
		get_sizes();
		if (!sizes.size())
		{
			std::cout << "Could not read any sizes.\n";
			return(0);
		}
		std::random_device rd;
		std::geometric_distribution<unsigned long int> dist(1.0 / 10.0);
		for(unsigned int i = 0; i < n; i++)
		{
			unsigned long int r = dist(rd);
			if (r >= sizes.size())
				r = 0;
			std::multimap<unsigned int, std::vector<uint8_t>>::iterator it = sizes.begin();
			while(r--)
				it++;
			memcpy(table, it->second.data(), NUM_OPCODES);
			unsigned long int d = dist(rd) + 1;
			if (d > 4)
				d = 1;
			while(d--)
				modify_table_random(table);
			add_opcodemap(table);
		}
		write_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 5 && !strcmp(argv[1], "swaprows"))
	{
		read_opcodemapstable();
		std::map <std::vector<uint8_t>, unsigned long int>::iterator it;
		for(it = opcodemapstable.begin(); it != opcodemapstable.end(); it++)
		{
			if (it->second == n)
				break;
		}
		memcpy(table, it->first.data(), NUM_OPCODES);
		swap_rows(table, 16, 16, strtol(argv[3], 0, 0), strtol(argv[4], 0, 0));
		add_opcodemap(table);
		write_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 5 && !strcmp(argv[1], "swapcolumns"))
	{
		read_opcodemapstable();
		std::map <std::vector<uint8_t>, unsigned long int>::iterator it;
		for(it = opcodemapstable.begin(); it != opcodemapstable.end(); it++)
		{
			if (it->second == n)
				break;
		}
		memcpy(table, it->first.data(), NUM_OPCODES);
		swap_columns(table, 16, 16, strtol(argv[3], 0, 0), strtol(argv[4], 0, 0));
		add_opcodemap(table);
		write_opcodemapstable();
		create_opcodemapsfiles();
	}
	else if (argc == 3 && !strcmp(argv[1], "showbest"))
	{
		read_opcodemapstable();
		get_sizes();
		show_bestsizes(n);
	}
	else if (argc == 3 && !strcmp(argv[1], "deleteworst"))
	{
		read_opcodemapstable();
		get_sizes();
		delete_worst(n);
		write_opcodemapstable();
	}
	else if (argc == 3 && (!strcmp(argv[1], "print_tex") || !strcmp(argv[1], "print_ucsim")))
	{
		read_opcodemapstable();
		std::map <std::vector<uint8_t>, unsigned long int>::iterator it;
		for(it = opcodemapstable.begin(); it != opcodemapstable.end(); it++)
		{
			if (it->second == n)
				break;
		}
		if (it != opcodemapstable.end())
		{
			if (!strcmp(argv[1], "print_tex"))
				print_table_tex(stdout, it->first.data());
			else
				print_table_ucsim(stdout, it->first.data());	
		}
		else
			std::cerr << "Entry not found.";
	}
	else
	{
		help(stderr);
		return(-1);
	}
}

