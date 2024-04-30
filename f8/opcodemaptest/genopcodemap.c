#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <sys/random.h>
#include <sys/stat.h>

#define NUM_OPCODES 256

const char *opcodenames[NUM_OPCODES] =	{
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
	"OPCODE_SRL_ZH",
	"OPCODE_SLL_DIR",
	"OPCODE_SLL_SPREL",
	"OPCODE_SLL_XL",
	"OPCODE_SLL_ZH",
	"OPCODE_RRC_DIR",
	"OPCODE_RRC_SPREL",
	"OPCODE_RRC_XL",
	"OPCODE_RRC_ZH",
	"OPCODE_RLC_DIR",
	"OPCODE_RLC_SPREL",
	"OPCODE_RLC_XL",
	"OPCODE_RLC_ZH",
	"OPCODE_INC_DIR",
	"OPCODE_INC_SPREL",
	"OPCODE_INC_XL",
	"OPCODE_INC_ZH",
	"OPCODE_DEC_DIR",
	"OPCODE_DEC_SPREL",
	"OPCODE_DEC_XL",
	"OPCODE_DEC_ZH",
	"OPCODE_CLR_DIR",
	"OPCODE_CLR_SPREL",
	"OPCODE_CLR_XL",
	"OPCODE_CLR_ZH",
	"OPCODE_TST_DIR",
	"OPCODE_TST_SPREL",
	"OPCODE_TST_XL",
	"OPCODE_TST_ZH",
	"OPCODE_PUSH_DIR",
	"OPCODE_PUSH_SPREL",
	"OPCODE_PUSH_XL",
	"OPCODE_PUSH_ZH",
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
	"OPCODE_LD_YH_IMMD",
	"OPCODE_ROT_XL_IMMD",
	"OPCODE_SRA_XL",
	"OPCODE_DAA_XL",
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
	"OPCODE_LDW_ZREL_X",
	"OPCODE_JR_D",
	"OPCODE_DNJNZ_YH_D",
	"OPCODE_JRZ_D",
	"OPCODE_JRNZ_D",
	"OPCODE_JRC_D",
	"OPCODE_JRNC_D",
	"OPCODE_JRN_D",
	"OPCODE_JRNN_D",
	"OPCODE_JRO_D",
	"OPCODE_JRNO_D",
	"OPCODE_JRSGE_D",
	"OPCODE_JRSLT_D",
	"OPCODE_JRSGT_D",
	"OPCODE_JRSLE_D",
	"OPCODE_JRGT_D",
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
	"OPCODE_RESERVED_ED",
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
	"OPCODE_RESERVED_FC",
	"OPCODE_RESERVED_FD",
	"OPCODE_RESERVED_FE",
	"OPCODE_RESERVED_FF",
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

inline static void init_table_arg(uint8_t *table, const char *arg)
{
	for(unsigned int i = 0, j = 9; i < NUM_OPCODES; i++)
	{
	  unsigned int o;
	  sscanf(arg + j, "%02x", &o);
	  table[i] = o;
	  j += 2;
	  if(i % 64 == 63)
	  	j++;
	}
}

// Modify opcode table by swapping columns or rows in a random table layout.
inline static void modify_table_random(uint8_t *table)
{
	unsigned int rowlength, columnheight;
	bool swap_rows;
	rowlength = 0;
	getrandom (&rowlength, 1, 0);
	swap_rows = rowlength & 0x10;
	rowlength %= 9;
	rowlength = 1 << rowlength;
	columnheight = NUM_OPCODES / rowlength;

	uint8_t select[2];
	getrandom (&select, 1, 0);
	if(!swap_rows) // Swap columns
	{
		select[0] %= rowlength;
		select[1] %= rowlength;
		printf("%u x %u table, swapping columns %u, %u\n", (unsigned)rowlength, (unsigned)columnheight, (unsigned)select[0], (unsigned)select[1]);
		for(unsigned int i = 0; i < columnheight; i++)
		{
			// trap stays at 0.
			if(rowlength * i + select[0] == 0 || rowlength * i + select[1] == 0)
				continue;

			uint8_t tmp = table[rowlength * i + select[0]];
			table[rowlength * i + select[0]] = table[rowlength * i + select[1]];
			table[rowlength * i + select[1]] = tmp;
		}
	}
	else // Swap rows
	{
		select[0] %= columnheight;
		select[1] %= columnheight;
		printf("%u x %u table, swapping rows %u, %u\n", (unsigned)rowlength, (unsigned)columnheight, (unsigned)select[0], (unsigned)select[1]);
		for(unsigned int i = 0; i < rowlength; i++)
		{
			// trap stays at 0.
			if(rowlength * select[0] + i == 0 || rowlength * select[1] + i == 0)
				continue;

			uint8_t tmp = table[rowlength * select[0] + i];
			table[rowlength * select[0] + i] = table[rowlength * select[1] + i];
			table[rowlength * select[1] + i] = tmp;
		}
	}
}

static void print_table(FILE *f, const uint8_t * table)
{
	fprintf(f, "typedef enum logic [7:0] {\n");
	for(unsigned int i = 0; i < NUM_OPCODES; i++)
		fprintf(f, "\t%s = 8'h%02x%s\n", opcodenames[table[i]], i, i != 255 ? "," : "");
	fprintf(f, "} opcode_t;\n");
}

int main(int argc, char **argv)
{
	uint8_t table[NUM_OPCODES];

	if(argc < 2)
		init_table_random(table);
	else if(!strcmp(argv[1], "order"))
		init_table_order(table);
	else if(strlen(argv[1]) >= 64 * 4 * 2 + 3)
	{
		init_table_arg(table, argv[1]);
		modify_table_random(table);
	}
	else
	{
		fprintf(stderr, "genopcodemap [order|<path>]\n");
		fprintf(stderr, "<path> format: testdata_<128 hex digits>/<128 hex digits>/<128 hex digits>/<128 hex digits>[/opcodemap.v]\n");
		return(-1);
	}

	char dirname[9 + NUM_OPCODES * 2 + 3 + 1] = "testdata_";
	char filename[9 + NUM_OPCODES * 2 + 3 + 1 + 11 + 1] = "testdata_";

	for(unsigned int i = 0, j = 9; i < NUM_OPCODES; i++)
	{
	  sprintf(dirname + j, "%02x", table[i]);
	  sprintf(filename + j, "%02x", table[i]);
	  j += 2;
	  if(i % 64 == 63)
	  {
	  	dirname[j] = 0;
	  	filename[j] = '/';
	  	j++;
	  }
	}
	strcpy(filename + 9 + NUM_OPCODES * 2 + 3 + 1, "opcodemap.v");

	mkdir(dirname, 0700);
	dirname[9 + 64 * 2 * 1 + 0] = '/';
	mkdir(dirname, 0700);
	dirname[9 + 64 * 2 * 2 + 1] = '/';
	mkdir(dirname, 0700);
	dirname[9 + 64 * 2 * 3 + 2] = '/';
	mkdir(dirname, 0700);

	FILE *file = fopen(filename, "wx");
	if(file)
		print_table(file, table);

	return(0);
}

