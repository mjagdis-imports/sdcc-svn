`begin_keywords "1800-2009"

`include "opcodemap.v"

function automatic logic opcode_is_prefix(opcode_t opcode);
	return(opcode == OPCODE_SWAPOP || opcode == OPCODE_ALTACC1 || opcode == OPCODE_ALTACC2 || opcode == OPCODE_ALTACC3);
endfunction

function automatic logic opcode_is_sub(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_DIR || opcode == OPCODE_SUB_XL_SPREL || opcode == OPCODE_SUB_XL_ZREL || opcode == OPCODE_SUB_XL_ZL || opcode == OPCODE_SUB_XL_XH || opcode == OPCODE_SUB_XL_YL || opcode == OPCODE_SUB_XL_YH);
endfunction

function automatic logic opcode_is_sbc(opcode_t opcode);
	return(opcode == OPCODE_SBC_XL_DIR || opcode == OPCODE_SBC_XL_SPREL || opcode == OPCODE_SBC_XL_ZREL || opcode == OPCODE_SBC_XL_ZL || opcode == OPCODE_SBC_XL_XH || opcode == OPCODE_SBC_XL_YL || opcode == OPCODE_SBC_XL_YH);
endfunction

function automatic logic opcode_is_add(opcode_t opcode);
	return(opcode == OPCODE_ADD_XL_IMMD || opcode == OPCODE_ADD_XL_DIR || opcode == OPCODE_ADD_XL_SPREL || opcode == OPCODE_ADD_XL_ZREL || opcode == OPCODE_ADD_XL_ZL || opcode == OPCODE_ADD_XL_XH || opcode == OPCODE_ADD_XL_YL || opcode == OPCODE_ADD_XL_YH);
endfunction

function automatic logic opcode_is_adc(opcode_t opcode);
	return(opcode == OPCODE_ADC_XL_IMMD || opcode == OPCODE_ADC_XL_DIR || opcode == OPCODE_ADC_XL_SPREL || opcode == OPCODE_ADC_XL_ZREL || opcode == OPCODE_ADC_XL_ZL || opcode == OPCODE_ADC_XL_XH || opcode == OPCODE_ADC_XL_YL || opcode == OPCODE_ADC_XL_YH);
endfunction

function automatic logic opcode_is_cp(opcode_t opcode);
	return(opcode == OPCODE_CP_XL_IMMD || opcode == OPCODE_CP_XL_DIR || opcode == OPCODE_CP_XL_SPREL || opcode == OPCODE_CP_XL_ZREL || opcode == OPCODE_CP_XL_ZL || opcode == OPCODE_CP_XL_XH || opcode == OPCODE_CP_XL_YL || opcode == OPCODE_CP_XL_YH);
endfunction

function automatic logic opcode_is_or(opcode_t opcode);
	return(opcode == OPCODE_OR_XL_IMMD || opcode == OPCODE_OR_XL_DIR || opcode == OPCODE_OR_XL_SPREL || opcode == OPCODE_OR_XL_ZREL || opcode == OPCODE_OR_XL_ZL || opcode == OPCODE_OR_XL_XH || opcode == OPCODE_OR_XL_YL || opcode == OPCODE_OR_XL_YH);
endfunction

function automatic logic opcode_is_and(opcode_t opcode);
	return(opcode == OPCODE_AND_XL_IMMD || opcode == OPCODE_AND_XL_DIR || opcode == OPCODE_AND_XL_SPREL || opcode == OPCODE_AND_XL_ZREL || opcode == OPCODE_AND_XL_ZL || opcode == OPCODE_AND_XL_XH || opcode == OPCODE_AND_XL_YL || opcode == OPCODE_AND_XL_YH);
endfunction

function automatic logic opcode_is_xor(opcode_t opcode);
	return(opcode == OPCODE_XOR_XL_IMMD || opcode == OPCODE_XOR_XL_DIR || opcode == OPCODE_XOR_XL_SPREL || opcode == OPCODE_XOR_XL_ZREL || opcode == OPCODE_XOR_XL_ZL || opcode == OPCODE_XOR_XL_XH || opcode == OPCODE_XOR_XL_YL || opcode == OPCODE_XOR_XL_YH);
endfunction

function automatic logic opcode_is_srl(opcode_t opcode);
	return(opcode == OPCODE_SRL_DIR || opcode == OPCODE_SRL_SPREL || opcode == OPCODE_SRL_XL || opcode == OPCODE_SRL_YREL);
endfunction

function automatic logic opcode_is_sll(opcode_t opcode);
	return(opcode == OPCODE_SLL_DIR || opcode == OPCODE_SLL_SPREL || opcode == OPCODE_SLL_XL || opcode == OPCODE_SLL_YREL);
endfunction

function automatic logic opcode_is_rrc(opcode_t opcode);
	return(opcode == OPCODE_RRC_DIR || opcode == OPCODE_RRC_SPREL || opcode == OPCODE_RRC_XL || opcode == OPCODE_RRC_YREL);
endfunction

function automatic logic opcode_is_rlc(opcode_t opcode);
	return(opcode == OPCODE_RLC_DIR || opcode == OPCODE_RLC_SPREL || opcode == OPCODE_RLC_XL || opcode == OPCODE_RLC_YREL);
endfunction

function automatic logic opcode_is_inc(opcode_t opcode);
	return(opcode == OPCODE_INC_DIR || opcode == OPCODE_INC_SPREL || opcode == OPCODE_INC_XL || opcode == OPCODE_INC_YREL);
endfunction

function automatic logic opcode_is_dec(opcode_t opcode);
	return(opcode == OPCODE_DEC_DIR || opcode == OPCODE_DEC_SPREL || opcode == OPCODE_DEC_XL || opcode == OPCODE_DEC_YREL);
endfunction

function automatic logic opcode_is_clr(opcode_t opcode);
	return(opcode == OPCODE_CLR_DIR || opcode == OPCODE_CLR_SPREL || opcode == OPCODE_CLR_XL || opcode == OPCODE_CLR_YREL);
endfunction

function automatic logic opcode_is_tst(opcode_t opcode);
	return(opcode == OPCODE_TST_DIR || opcode == OPCODE_TST_SPREL || opcode == OPCODE_TST_XL || opcode == OPCODE_TST_YREL);
endfunction

function automatic logic opcode_is_push(opcode_t opcode);
	return(opcode == OPCODE_PUSH_DIR || opcode == OPCODE_PUSH_SPREL || opcode == OPCODE_PUSH_XL || opcode == OPCODE_PUSH_YREL || opcode == OPCODE_PUSH_IMMD);
endfunction

function automatic logic opcode_is_xchb(opcode_t opcode);
	return(opcode == OPCODE_XCHB_XL_MM_0 || opcode == OPCODE_XCHB_XL_MM_1 || opcode == OPCODE_XCHB_XL_MM_2 || opcode == OPCODE_XCHB_XL_MM_3 || opcode == OPCODE_XCHB_XL_MM_4 || opcode == OPCODE_XCHB_XL_MM_5 || opcode == OPCODE_XCHB_XL_MM_6 || opcode == OPCODE_XCHB_XL_MM_7);
endfunction

function automatic logic opcode_is_subw(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_DIR || opcode == OPCODE_SUBW_Y_SPREL || opcode == OPCODE_SUBW_Y_X);
endfunction

function automatic logic opcode_is_sbcw(opcode_t opcode);
	return(opcode == OPCODE_SBCW_Y_DIR || opcode == OPCODE_SBCW_Y_SPREL || opcode == OPCODE_SBCW_Y_X);
endfunction

function automatic logic opcode_is_addw(opcode_t opcode);
	return(opcode == OPCODE_ADDW_Y_IMMD || opcode == OPCODE_ADDW_Y_DIR || opcode == OPCODE_ADDW_Y_SPREL || opcode == OPCODE_ADDW_Y_X);
endfunction

function automatic logic opcode_is_adcw(opcode_t opcode);
	return(opcode == OPCODE_ADCW_Y_IMMD || opcode == OPCODE_ADCW_Y_DIR || opcode == OPCODE_ADCW_Y_SPREL || opcode == OPCODE_ADCW_Y_X);
endfunction

function automatic logic opcode_is_orw(opcode_t opcode);
	return(opcode == OPCODE_ORW_Y_IMMD || opcode == OPCODE_ORW_Y_DIR || opcode == OPCODE_ORW_Y_SPREL || opcode == OPCODE_ORW_Y_X);
endfunction

function automatic logic opcode_is_xorw(opcode_t opcode);
	return(opcode == OPCODE_XORW_Y_IMMD || opcode == OPCODE_XORW_Y_DIR || opcode == OPCODE_XORW_Y_SPREL || opcode == OPCODE_XORW_Y_X);
endfunction

function automatic logic opcode_is_clrw(opcode_t opcode);
	return(opcode == OPCODE_CLRW_DIR || opcode == OPCODE_CLRW_SPREL || opcode == OPCODE_CLRW_ZREL || opcode == OPCODE_CLRW_Y);
endfunction

function automatic logic opcode_is_incw(opcode_t opcode);
	return(opcode == OPCODE_INCW_DIR || opcode == OPCODE_INCW_SPREL || opcode == OPCODE_INCW_ZREL || opcode == OPCODE_INCW_Y
`ifndef F8L
	|| opcode == OPCODE_INCNW_Y
`endif
	);
endfunction

function automatic logic opcode_is_adcw0(opcode_t opcode);
	return(opcode == OPCODE_ADCW_DIR || opcode == OPCODE_ADCW_SPREL || opcode == OPCODE_ADCW_ZREL || opcode == OPCODE_ADCW_Y);
endfunction

function automatic logic opcode_is_sbcw0(opcode_t opcode);
	return(opcode == OPCODE_SBCW_DIR || opcode == OPCODE_SBCW_SPREL || opcode == OPCODE_SBCW_ZREL || opcode == OPCODE_SBCW_Y);
endfunction

function automatic logic opcode_is_pushw(opcode_t opcode);
	return(opcode == OPCODE_PUSHW_DIR || opcode == OPCODE_PUSHW_SPREL || opcode == OPCODE_PUSHW_ZREL || opcode == OPCODE_PUSHW_Y || opcode == OPCODE_PUSHW_IMMD);
endfunction

function automatic logic opcode_is_tstw(opcode_t opcode);
	return(opcode == OPCODE_TSTW_DIR || opcode == OPCODE_TSTW_SPREL || opcode == OPCODE_TSTW_ZREL || opcode == OPCODE_TSTW_Y);
endfunction

function automatic logic opcode_is_rrcw(opcode_t opcode);
	return(opcode == OPCODE_RRCW_Y || opcode == OPCODE_RRCW_SPREL);
endfunction

function automatic logic opcode_is_rlcw(opcode_t opcode);
	return(opcode == OPCODE_RLCW_Y || opcode == OPCODE_RLCW_SPREL);
endfunction

function automatic logic opcode_is_ld_xl_mem(opcode_t opcode);
	return(opcode == OPCODE_LD_XL_DIR || opcode == OPCODE_LD_XL_SPREL || opcode == OPCODE_LD_XL_ZREL ||
		opcode == OPCODE_LD_XL_IY || opcode == OPCODE_LD_XL_YREL);
endfunction

function automatic logic opcode_is_ld_xl(opcode_t opcode);
	return(opcode == OPCODE_LD_XL_IMMD || opcode_is_ld_xl_mem(opcode) ||
		opcode == OPCODE_LD_XL_XH || opcode == OPCODE_LD_XL_YL || opcode == OPCODE_LD_XL_YH || opcode == OPCODE_LD_XL_ZL || opcode == OPCODE_LD_XL_ZH);
endfunction

function automatic logic opcode_is_ldw_y_mem(opcode_t opcode);
	return(opcode == OPCODE_LDW_Y_DIR || opcode == OPCODE_LDW_Y_SPREL || opcode == OPCODE_LDW_Y_ZREL || opcode == OPCODE_LDW_Y_YREL || opcode == OPCODE_LDW_Y_IY);
endfunction

function automatic logic opcode_is_ldw_y(opcode_t opcode);
	return(opcode == OPCODE_LDW_Y_SP || opcode == OPCODE_LDW_Y_IMMD || opcode_is_ldw_y_mem(opcode) || opcode == OPCODE_LDW_Y_X || opcode == OPCODE_LDW_Y_D);
endfunction

function automatic logic opcode_is_mad(opcode_t opcode);
	return(opcode == OPCODE_MAD_X_DIR_YL || opcode == OPCODE_MAD_X_SPREL_YL || opcode == OPCODE_MAD_X_ZREL_YL || opcode == OPCODE_MAD_X_IZ_YL);
endfunction

function automatic logic opcode_is_8_2_immd(opcode_t opcode);
	return(opcode == OPCODE_ADD_XL_IMMD || opcode == OPCODE_ADC_XL_IMMD || opcode == OPCODE_CP_XL_IMMD ||
		opcode == OPCODE_OR_XL_IMMD || opcode == OPCODE_AND_XL_IMMD || opcode == OPCODE_XOR_XL_IMMD );
endfunction

function automatic logic opcode_is_8_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_DIR || opcode == OPCODE_SBC_XL_DIR || opcode == OPCODE_ADD_XL_DIR || opcode == OPCODE_ADC_XL_DIR || opcode == OPCODE_CP_XL_DIR ||
		opcode == OPCODE_OR_XL_DIR || opcode == OPCODE_AND_XL_DIR || opcode == OPCODE_XOR_XL_DIR);
endfunction

function automatic logic opcode_is_8_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_SPREL || opcode == OPCODE_SBC_XL_SPREL || opcode == OPCODE_ADD_XL_SPREL || opcode == OPCODE_ADC_XL_SPREL || opcode == OPCODE_CP_XL_SPREL ||
	opcode == OPCODE_OR_XL_SPREL || opcode == OPCODE_AND_XL_SPREL || opcode == OPCODE_XOR_XL_SPREL);
endfunction

function automatic logic opcode_is_8_2_zrel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZREL || opcode == OPCODE_SBC_XL_ZREL || opcode == OPCODE_ADD_XL_ZREL || opcode == OPCODE_ADC_XL_ZREL || opcode == OPCODE_CP_XL_ZREL ||
	opcode == OPCODE_OR_XL_ZREL || opcode == OPCODE_AND_XL_ZREL || opcode == OPCODE_XOR_XL_ZREL);
endfunction

function automatic logic opcode_is_8_2_mem(opcode_t opcode);
	return(opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode));
endfunction

function automatic logic opcode_is_8_2_zl(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZL || opcode == OPCODE_SBC_XL_ZL || opcode == OPCODE_ADD_XL_ZL || opcode == OPCODE_ADC_XL_ZL || opcode == OPCODE_CP_XL_ZL ||
	opcode == OPCODE_OR_XL_ZL || opcode == OPCODE_AND_XL_ZL || opcode == OPCODE_XOR_XL_ZL);
endfunction

function automatic logic opcode_is_8_2_xh(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_XH|| opcode == OPCODE_SBC_XL_XH || opcode == OPCODE_ADD_XL_XH || opcode == OPCODE_ADC_XL_XH || opcode == OPCODE_CP_XL_XH ||
	opcode == OPCODE_OR_XL_XH || opcode == OPCODE_AND_XL_XH || opcode == OPCODE_XOR_XL_XH);
endfunction

function automatic logic opcode_is_8_2_yl(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_YL|| opcode == OPCODE_SBC_XL_YL || opcode == OPCODE_ADD_XL_YL || opcode == OPCODE_ADC_XL_YL || opcode == OPCODE_CP_XL_YL ||
		opcode == OPCODE_OR_XL_YL || opcode == OPCODE_AND_XL_YL || opcode == OPCODE_XOR_XL_YL);
endfunction

function automatic logic opcode_is_8_2_yh(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_YH|| opcode == OPCODE_SBC_XL_YH || opcode == OPCODE_ADD_XL_YH || opcode == OPCODE_ADC_XL_YH || opcode == OPCODE_CP_XL_YH ||
	opcode == OPCODE_OR_XL_YH || opcode == OPCODE_AND_XL_YH || opcode == OPCODE_XOR_XL_YH);
endfunction

function automatic logic opcode_is_8_2_reg(opcode_t opcode);
	return(opcode_is_8_2_zl(opcode) || opcode_is_8_2_xh(opcode) || opcode_is_8_2_yl(opcode) || opcode_is_8_2_yh(opcode));
endfunction

function automatic logic opcode_is_8_2(opcode_t opcode);
	return(opcode_is_8_2_immd(opcode) || opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode) || opcode_is_8_2_reg(opcode));
endfunction

function automatic logic opcode_is_8_1_dir(opcode_t opcode);
	return(opcode == OPCODE_SRL_DIR || opcode == OPCODE_SLL_DIR || opcode == OPCODE_RLC_DIR || opcode == OPCODE_RRC_DIR ||
		opcode == OPCODE_INC_DIR || opcode == OPCODE_DEC_DIR ||
		opcode == OPCODE_CLR_DIR || opcode == OPCODE_TST_DIR || opcode == OPCODE_PUSH_DIR);
endfunction

function automatic logic opcode_is_8_1_sprel(opcode_t opcode);
	return(opcode == OPCODE_SRL_SPREL || opcode == OPCODE_SLL_SPREL || opcode == OPCODE_RLC_SPREL || opcode == OPCODE_RRC_SPREL ||
		opcode == OPCODE_INC_SPREL || opcode == OPCODE_DEC_SPREL ||
		opcode == OPCODE_CLR_SPREL || opcode == OPCODE_TST_SPREL ||opcode == OPCODE_PUSH_SPREL);
endfunction

function automatic logic opcode_is_8_1_xl(opcode_t opcode);
	return(opcode == OPCODE_SRL_XL || opcode == OPCODE_SLL_XL || opcode == OPCODE_RLC_XL || opcode == OPCODE_RRC_XL ||  opcode == OPCODE_SRA_XL ||
		opcode == OPCODE_INC_XL || opcode == OPCODE_DEC_XL ||
		opcode == OPCODE_CLR_XL || opcode == OPCODE_TST_XL ||opcode == OPCODE_PUSH_XL ||
		opcode == OPCODE_DA_XL || opcode == OPCODE_BOOL_XL);
endfunction

function automatic logic opcode_is_8_1_yrel(opcode_t opcode);
	return(opcode == OPCODE_SRL_YREL || opcode == OPCODE_SLL_YREL || opcode == OPCODE_SRL_YREL || opcode == OPCODE_RRC_YREL || opcode == OPCODE_RLC_YREL ||
		opcode == OPCODE_INC_YREL || opcode == OPCODE_DEC_YREL ||
		opcode == OPCODE_CLR_YREL ||
		opcode == OPCODE_INC_YREL || opcode == OPCODE_TST_YREL || opcode == OPCODE_PUSH_YREL);
endfunction

function automatic logic opcode_is_8_1_mem(opcode_t opcode);
	return(opcode_is_8_1_dir(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_8_1_yrel(opcode));
endfunction

function automatic logic opcode_is_8_1(opcode_t opcode);
	return(opcode_is_8_1_dir(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_8_1_xl(opcode) || opcode_is_8_1_yrel(opcode));
endfunction

function automatic logic opcode_is_16_2_immd(opcode_t opcode);
	return(opcode == OPCODE_ADDW_Y_IMMD || opcode == OPCODE_ADCW_Y_IMMD || opcode == OPCODE_ORW_Y_IMMD || opcode == OPCODE_XORW_Y_IMMD);
endfunction

function automatic logic opcode_is_16_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_DIR || opcode == OPCODE_SBCW_Y_DIR || opcode == OPCODE_ADDW_Y_DIR || opcode == OPCODE_ADCW_Y_DIR || opcode == OPCODE_ORW_Y_DIR || opcode == OPCODE_XORW_Y_DIR);
endfunction

function automatic logic opcode_is_16_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_SPREL || opcode == OPCODE_SBCW_Y_SPREL || opcode == OPCODE_ADDW_Y_SPREL || opcode == OPCODE_ADCW_Y_SPREL || opcode == OPCODE_ORW_Y_SPREL || opcode == OPCODE_XORW_Y_SPREL);
endfunction

function automatic logic opcode_is_16_2_x(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_X || opcode == OPCODE_SBCW_Y_X || opcode == OPCODE_ADDW_Y_X || opcode == OPCODE_ADCW_Y_X || opcode == OPCODE_ORW_Y_X || opcode == OPCODE_XORW_Y_X);
endfunction

function automatic logic opcode_is_16_2_mem(opcode_t opcode);
	return(opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode));
endfunction

function automatic logic opcode_is_16_2(opcode_t opcode);
	return(opcode_is_16_2_immd(opcode) ||opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode) || opcode_is_16_2_x(opcode));
endfunction

function automatic logic opcode_is_16_1_dir(opcode_t opcode);
	return(opcode == OPCODE_CLRW_DIR || opcode == OPCODE_INCW_DIR || opcode == OPCODE_ADCW_DIR || opcode == OPCODE_SBCW_DIR ||
		opcode == OPCODE_PUSHW_DIR || opcode == OPCODE_TSTW_DIR);
endfunction

function automatic logic opcode_is_16_1_sprel(opcode_t opcode);
	return(opcode == OPCODE_CLRW_SPREL || opcode == OPCODE_INCW_SPREL || opcode == OPCODE_ADCW_SPREL || opcode == OPCODE_SBCW_SPREL ||
		opcode == OPCODE_DECW_SPREL ||
		opcode == OPCODE_PUSHW_SPREL || opcode == OPCODE_TSTW_SPREL ||
		opcode == OPCODE_RRCW_SPREL || opcode == OPCODE_RLCW_SPREL);
endfunction

function automatic logic opcode_is_16_1_zrel(opcode_t opcode);
	return(opcode == OPCODE_CLRW_ZREL || opcode == OPCODE_INCW_ZREL || opcode == OPCODE_ADCW_ZREL || opcode == OPCODE_SBCW_ZREL ||
		opcode == OPCODE_PUSHW_ZREL || opcode == OPCODE_TSTW_ZREL);
endfunction

function automatic logic opcode_is_16_1_mem(opcode_t opcode);
	return(opcode_is_16_1_dir(opcode) || opcode_is_16_1_sprel(opcode) || opcode_is_16_1_zrel(opcode));
endfunction

function automatic logic opcode_is_16_1_y(opcode_t opcode);
	return(opcode == OPCODE_CLRW_Y || opcode == OPCODE_INCW_Y || opcode == OPCODE_ADCW_Y || opcode == OPCODE_SBCW_Y ||
		opcode == OPCODE_PUSHW_Y || opcode == OPCODE_TSTW_Y ||
		opcode == OPCODE_SRLW_Y || opcode == OPCODE_RRCW_Y || opcode == OPCODE_RLCW_Y || opcode == OPCODE_SRAW_Y || 
		opcode == OPCODE_XCH_YL_YH
`ifndef F8L
		|| opcode == OPCODE_SLLW_Y || opcode == OPCODE_INCNW_Y || opcode == OPCODE_NEGW_Y || opcode == OPCODE_BOOLW_Y
`endif
);
endfunction

function automatic logic opcode_is_16_1(opcode_t opcode);
	return(opcode_is_16_1_dir(opcode) || opcode_is_16_1_sprel(opcode) || opcode_is_16_1_zrel(opcode) || opcode_is_16_1_y(opcode));
endfunction

function automatic logic opcode_is_8_immd(opcode_t opcode);
	return(opcode_is_8_2_immd(opcode) || opcode == OPCODE_PUSH_IMMD || opcode == OPCODE_ROT_XL_IMMD || opcode == OPCODE_MSK_IY_XL_IMMD || opcode == OPCODE_LD_XL_IMMD);
endfunction

function automatic logic opcode_is_16_immd(opcode_t opcode);
	return(opcode_is_16_2_immd(opcode) || opcode == OPCODE_PUSHW_IMMD || opcode == OPCODE_LDW_Y_IMMD || opcode == OPCODE_CPW_Y_IMMD);
endfunction

function automatic logic opcode_is_dir_read(opcode_t opcode);
	return(opcode_is_8_2_dir(opcode) || opcode_is_8_1_dir(opcode) || opcode_is_xchb(opcode) || opcode_is_16_2_dir(opcode) || opcode_is_16_1_dir(opcode) || opcode == OPCODE_MAD_X_DIR_YL || opcode == OPCODE_LD_XL_DIR || opcode == OPCODE_LDW_Y_DIR);
endfunction

function automatic logic opcode_is_dir(opcode_t opcode);
	return(opcode_is_dir_read(opcode) || opcode == OPCODE_LD_DIR_XL || opcode == OPCODE_LDW_DIR_Y);
endfunction

function automatic logic opcode_is_sprel(opcode_t opcode);
	return(opcode_is_8_2_sprel(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_16_2_sprel(opcode) || opcode_is_16_1_sprel(opcode) || opcode == OPCODE_XCH_XL_SPREL || opcode == OPCODE_MAD_X_SPREL_YL || opcode == OPCODE_XCH_F_SPREL || opcode == OPCODE_LDW_ISPREL_Y || opcode == OPCODE_LD_SPREL_XL || opcode == OPCODE_LD_XL_SPREL || opcode == OPCODE_LDW_Y_SPREL || opcode == OPCODE_LDW_SPREL_Y || opcode == OPCODE_XCHW_Y_SPREL);
endfunction

function automatic logic opcode_is_sprel_read(opcode_t opcode);
	return(opcode_is_sprel(opcode) && opcode != OPCODE_CLR_SPREL && opcode != OPCODE_CLRW_SPREL && opcode != OPCODE_LD_SPREL_XL && opcode != OPCODE_LDW_SPREL_Y);
endfunction

function automatic logic opcode_is_zrel_read(opcode_t opcode);
	return(opcode_is_8_2_zrel(opcode) || opcode_is_16_1_zrel(opcode) || opcode == OPCODE_LD_XL_ZREL || opcode == OPCODE_LDW_Y_ZREL
`ifndef F8L
	|| opcode == OPCODE_MAD_X_ZREL_YL
`endif
	);
endfunction

function automatic logic opcode_is_zrel(opcode_t opcode);
	return(opcode_is_zrel_read(opcode) || opcode == OPCODE_LD_ZREL_XL || opcode == OPCODE_LDW_ZREL_Y);
endfunction

function automatic logic opcode_is_yrel(opcode_t opcode);
	return(opcode == OPCODE_LD_XL_YREL || opcode == OPCODE_LD_YREL_XL || opcode == OPCODE_LDW_Y_YREL || opcode == OPCODE_LDW_YREL_X);
endfunction

function automatic logic opcode_is_jr_d(opcode_t opcode);
	return(opcode == OPCODE_JR_D || opcode == OPCODE_DNJNZ_YH_D || opcode == OPCODE_JRZ_D || opcode == OPCODE_JRNZ_D || opcode == OPCODE_JRC_D || opcode == OPCODE_JRNC_D || opcode == OPCODE_JRN_D || opcode == OPCODE_JRNN_D || opcode == OPCODE_JRNO_D || opcode == OPCODE_JRSGE_D || opcode == OPCODE_JRSLT_D || opcode == OPCODE_JRSLE_D || opcode == OPCODE_JRLE_D);
endfunction

function automatic logic[2:0] opcode_instsize(opcode_t opcode);
	if (opcode_is_8_immd(opcode) || opcode_is_sprel(opcode) || opcode_is_yrel(opcode) || opcode_is_jr_d(opcode) || opcode == OPCODE_LDW_Y_D || opcode == OPCODE_ADDW_Y_D || opcode == OPCODE_ADDW_SP_D)
		return(2);
	if(opcode_is_16_immd(opcode) || opcode_is_dir(opcode) || opcode_is_zrel(opcode) || opcode == OPCODE_JP_IMMD || opcode == OPCODE_CALL_IMMD)
		return(3);
	return(1);
endfunction

`end_keywords

