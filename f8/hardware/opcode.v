typedef enum logic [7:0] {
	OPCODE_TRAP =         8'b00000000, // trap
	OPCODE_SUB_XL_DIR =   8'b00000001, // sub xl, mm
	OPCODE_SUB_XL_SPREL = 8'b00000010, // sub xl, (n, sp)
	OPCODE_SUB_XL_ZREL =  8'b00000011, // sub xl, (nn, sp)
	OPCODE_SUB_XL_ZL =    8'b00000100, // sub xl, zl
	OPCODE_SUB_XL_XH =    8'b00000101, // sub xl, xh
	OPCODE_SUB_XL_YL =    8'b00000110, // sub xl, yl
	OPCODE_SUB_XL_YH =    8'b00000111, // sub xl, yh
	OPCODE_NOP =          8'b00001000, // nop
	OPCODE_SBC_XL_DIR =   8'b00001001, // sbc xl, mm
	OPCODE_SBC_XL_SPREL = 8'b00001010, // sbc xl, (n, sp)
	OPCODE_SBC_XL_ZREL =  8'b00001011, // sbc xl, (nn, sp)
	OPCODE_SBC_XL_ZL =    8'b00001100, // sbc xl, zl
	OPCODE_SBC_XL_XH =    8'b00001101, // sbc xl, xh
	OPCODE_SBC_XL_YL =    8'b00001110, // sbc xl, yl
	OPCODE_SBC_XL_YH =    8'b00001111, // sub xl, yh
	OPCODE_ADD_XL_IMMD =  8'b00010000, // add xl, #i
	OPCODE_ADD_XL_DIR =   8'b00010001, // add xl, mm
	OPCODE_ADD_XL_SPREL = 8'b00010010, // add xl, (n, sp)
	OPCODE_ADD_XL_ZREL =  8'b00010011, // add xl, (nn, sp)
	OPCODE_ADD_XL_ZL =    8'b00010100, // add xl, zl
	OPCODE_ADD_XL_XH =    8'b00010101, // add xl, xh
	OPCODE_ADD_XL_YL =    8'b00010110, // add xl, yl
	OPCODE_ADD_XL_YH =    8'b00010111, // add xl, yh
	OPCODE_ADC_XL_IMMD =  8'b00011000, // adc xl, #i
	OPCODE_ADC_XL_DIR =   8'b00011001, // adc xl, mm
	OPCODE_ADC_XL_SPREL = 8'b00011010, // adc xl, (n, sp)
	OPCODE_ADC_XL_ZREL =  8'b00011011, // adc xl, (nn, sp)
	OPCODE_ADC_XL_ZL =    8'b00011100, // adc xl, zl
	OPCODE_ADC_XL_XH =    8'b00011101, // adc xl, xh
	OPCODE_ADC_XL_YL =    8'b00011110, // adc xl, yl
	OPCODE_ADC_XL_YH =    8'b00011111, // adc xl, yh
	OPCODE_CP_XL_IMMD =   8'b00100000, // cp xl, #i
	OPCODE_CP_XL_DIR =    8'b00100001, // cp xl, mm
	OPCODE_CP_XL_SPREL =  8'b00100010, // cp xl, (n, sp)
	OPCODE_CP_XL_ZREL =   8'b00100011, // cp xl, (nn, sp)
	OPCODE_CP_XL_ZL =     8'b00100100, // cp xl, zl
	OPCODE_CP_XL_XH =     8'b00100101, // cp xl, xh
	OPCODE_CP_XL_YL =     8'b00100110, // cp xl, yl
	OPCODE_CP_XL_YH =     8'b00100111, // cp xl, yh
	// todo: or, and, xor.
	OPCODE_SRL_DIR =      8'b01000000, // srl mm
	OPCODE_SRL_SPREL =    8'b01000001, // srl (n, sp)
	OPCODE_SRL_XL =       8'b01000010, // srl xl
	OPCODE_SRL_ZH =       8'b01000011, // srl zh
	// todo: sll, rrc, rlc, inc, dec, clr, tst.
	OPCODE_PUSH_DIR =     8'b01100000, // push mm
	OPCODE_PUSH_SPREL =   8'b01100001, // push (n, sp)
	OPCODE_PUSH_XL =      8'b01100010, // push xl
	OPCODE_PUSH_ZH =      8'b01100011, // push zh
	OPCODE_JP_IMMD =      8'b01100100, // jp #ii
	OPCODE_JP_Y =         8'b01100101, // jp y
	OPCODE_LDW_SP_Y =     8'b01110000, // ld sp, y
	OPCODE_SUBW_Y_DIR =   8'b01110001, // subw y, mm
	OPCODE_SUBW_Y_SPREL = 8'b01110010, // subw y, (n, sp)
	OPCODE_SUBW_Y_X =     8'b01110011, // subw y, x
	OPCODE_LD_XL_IY =     8'b10001000, // ld xl, (y)
	OPCODE_LD_IY_XL =     8'b10001110, // ld (y), xl
    OPCODE_LDW_Y_IMMD =   8'b11001000, // ldw y, #ii
    OPCODE_RESERVED_FC,
    OPCODE_RESERVED_FD,
    OPCODE_RESERVED_FE,
    OPCODE_RESERVED_FF
	} opcode_t;

typedef enum {
	OPCODE_CLASS_8_2,
	OPCODE_CLASS_16_2,
	OPCODE_CLASS_8_1,
	OPCODE_CLASS_16_1,
	OPCODE_CLASS_OTHER
	} opcode_class_t;

typedef enum {
	OPCODE_ADDRMODE_8_2_IMMD,
	OPCODE_ADDRMODE_8_2_DIR,
	OPCODE_ADDRMODE_8_2_SPREL,
	OPCODE_ADDRMODE_8_2_ZREL,
	OPCODE_ADDRMODE_8_2_ZL,
	OPCODE_ADDRMODE_8_2_ZH,
	OPCODE_ADDRMODE_8_2_YL,
	OPCODE_ADDRMODE_8_2_YH,
	OPCODE_ADDRMODE_16_2_IMMD,
	OPCODE_ADDRMODE_16_2_DIR,
	OPCODE_ADDRMODE_16_2_SPREL,
	OPCODE_ADDRMODE_16_2_X,
	OPCODE_ADDRMODE_8_1_DIR,
	OPCODE_ADDRMODE_8_1_SPREL,
	OPCODE_ADDRMODE_8_1_XL,
	OPCODE_ADDRMODE_8_1_ZH,
	OPCODE_ADDRMODE_16_1_DIR,
	OPCODE_ADDRMODE_16_1_SPREL,
	OPCODE_ADDRMODE_16_1_ZREL,
	OPCODE_ADDRMODE_16_1_Y,
	OPCODE_ADDRMODE_OTHER
	} opcode_addrmode_t;

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

function automatic logic opcode_is_srl(opcode_t opcode);
	return(opcode == OPCODE_SRL_DIR || opcode == OPCODE_SRL_SPREL || opcode == OPCODE_SRL_XL || opcode == OPCODE_SRL_ZH);
endfunction

function automatic logic opcode_is_subw(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_DIR || opcode == OPCODE_SUBW_Y_SPREL || OPCODE_SUBW_Y_X);
endfunction

function automatic logic opcode_is_8_2_immd(opcode_t opcode);
	return(opcode == OPCODE_ADD_XL_IMMD || opcode == OPCODE_ADC_XL_IMMD);
endfunction

function automatic logic opcode_is_8_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_DIR || opcode == OPCODE_SBC_XL_DIR || opcode == OPCODE_ADD_XL_DIR || opcode == OPCODE_ADC_XL_DIR);
endfunction

function automatic logic opcode_is_8_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_SPREL || opcode == OPCODE_SBC_XL_SPREL || opcode == OPCODE_ADD_XL_SPREL || opcode == OPCODE_ADC_XL_SPREL);
endfunction

function automatic logic opcode_is_8_2_zrel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZREL || opcode == OPCODE_SBC_XL_ZREL || opcode == OPCODE_ADD_XL_ZREL || opcode == OPCODE_ADC_XL_ZREL);
endfunction

function automatic logic opcode_is_8_2_zl(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZL || opcode == OPCODE_SBC_XL_ZL || opcode == OPCODE_ADD_XL_ZL || opcode == OPCODE_ADC_XL_ZL);
endfunction

function automatic logic opcode_is_8_2_xh(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_XH|| opcode == OPCODE_SBC_XL_XH || opcode == OPCODE_ADD_XL_XH || opcode == OPCODE_ADC_XL_XH);
endfunction

function automatic logic opcode_is_8_2_reg(opcode_t opcode);
	return(opcode_is_8_2_zl(opcode) || opcode_is_8_2_xh(opcode));
endfunction

function automatic logic opcode_is_8_2(opcode_t opcode);
	return(opcode_is_8_2_immd(opcode) || opcode_is_8_2_dir(opcode) || opcode_is_8_2_sprel(opcode) || opcode_is_8_2_zrel(opcode) || opcode_is_8_2_reg(opcode));
endfunction

function automatic logic opcode_is_8_1_dir(opcode_t opcode);
	return(opcode == OPCODE_SRL_DIR || opcode == OPCODE_PUSH_DIR);
endfunction

function automatic logic opcode_is_8_1_sprel(opcode_t opcode);
	return(opcode == OPCODE_SRL_SPREL || opcode == OPCODE_PUSH_SPREL);
endfunction

function automatic logic opcode_is_8_1_xl(opcode_t opcode);
	return(opcode == OPCODE_SRL_XL || opcode == OPCODE_PUSH_XL);
endfunction

function automatic logic opcode_is_8_1_zh(opcode_t opcode);
	return(opcode == OPCODE_SRL_ZH || opcode == OPCODE_PUSH_ZH);
endfunction

function automatic logic opcode_is_16_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_DIR);
endfunction

function automatic logic opcode_is_16_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_SPREL);
endfunction

function automatic logic opcode_is_16_2(opcode_t opcode);
	return(opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode));
endfunction

function automatic logic opcode_is_8_immd(opcode_t opcode);
	return(opcode_is_8_2_immd(opcode));
endfunction

function automatic logic opcode_is_16_immd(opcode_t opcode);
	return(opcode == OPCODE_LDW_Y_IMMD);
endfunction

function automatic logic opcode_is_dir(opcode_t opcode);
	return(opcode_is_8_2_dir(opcode) || opcode_is_8_1_dir(opcode) || opcode_is_16_2_dir(opcode));
endfunction

function automatic logic opcode_is_sprel(opcode_t opcode);
	return(opcode_is_8_2_sprel(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_16_2_sprel(opcode));
endfunction

function automatic logic opcode_is_zrel(opcode_t opcode);
	return(opcode_is_8_2_zrel(opcode));
endfunction

