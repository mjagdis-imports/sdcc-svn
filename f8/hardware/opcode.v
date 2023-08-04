typedef enum logic [7:0] {
	OPCODE_TRAP =         8'h00, // trap
	OPCODE_SUB_XL_DIR =   8'h01, // sub xl, mm
	OPCODE_SUB_XL_SPREL = 8'h02, // sub xl, (n, sp)
	OPCODE_SUB_XL_ZREL =  8'h03, // sub xl, (nn, sp)
	OPCODE_SUB_XL_ZL =    8'h04, // sub xl, zl
	OPCODE_SUB_XL_XH =    8'h05, // sub xl, xh
	OPCODE_SUB_XL_YL =    8'h06, // sub xl, yl
	OPCODE_SUB_XL_YH =    8'h07, // sub xl, yh
	OPCODE_NOP =          8'h08, // nop
	OPCODE_SBC_XL_DIR =   8'h09, // sbc xl, mm
	OPCODE_SBC_XL_SPREL = 8'h0a, // sbc xl, (n, sp)
	OPCODE_SBC_XL_ZREL =  8'h0b, // sbc xl, (nn, sp)
	OPCODE_SBC_XL_ZL =    8'h0c, // sbc xl, zl
	OPCODE_SBC_XL_XH =    8'h0d, // sbc xl, xh
	OPCODE_SBC_XL_YL =    8'h0e, // sbc xl, yl
	OPCODE_SBC_XL_YH =    8'h0f, // sub xl, yh
	OPCODE_ADD_XL_IMMD =  8'h10, // add xl, #i
	OPCODE_ADD_XL_DIR =   8'h11, // add xl, mm
	OPCODE_ADD_XL_SPREL = 8'h12, // add xl, (n, sp)
	OPCODE_ADD_XL_ZREL =  8'h13, // add xl, (nn, sp)
	OPCODE_ADD_XL_ZL =    8'h14, // add xl, zl
	OPCODE_ADD_XL_XH =    8'h15, // add xl, xh
	OPCODE_ADD_XL_YL =    8'h16, // add xl, yl
	OPCODE_ADD_XL_YH =    8'h17, // add xl, yh
	OPCODE_ADC_XL_IMMD =  8'h18, // adc xl, #i
	OPCODE_ADC_XL_DIR =   8'h19, // adc xl, mm
	OPCODE_ADC_XL_SPREL = 8'h1a, // adc xl, (n, sp)
	OPCODE_ADC_XL_ZREL =  8'h1b, // adc xl, (nn, sp)
	OPCODE_ADC_XL_ZL =    8'h1c, // adc xl, zl
	OPCODE_ADC_XL_XH =    8'h1d, // adc xl, xh
	OPCODE_ADC_XL_YL =    8'h1e, // adc xl, yl
	OPCODE_ADC_XL_YH =    8'h1f, // adc xl, yh
	OPCODE_CP_XL_IMMD =   8'h20, // cp xl, #i
	OPCODE_CP_XL_DIR =    8'h21, // cp xl, mm
	OPCODE_CP_XL_SPREL =  8'h22, // cp xl, (n, sp)
	OPCODE_CP_XL_ZREL =   8'h23, // cp xl, (nn, sp)
	OPCODE_CP_XL_ZL =     8'h24, // cp xl, zl
	OPCODE_CP_XL_XH =     8'h25, // cp xl, xh
	OPCODE_CP_XL_YL =     8'h26, // cp xl, yl
	OPCODE_CP_XL_YH =     8'h27, // cp xl, yh
	OPCODE_OR_XL_IMMD =   8'h28, // or xl, #i
	OPCODE_OR_XL_DIR =    8'h29, // or xl, mm
	OPCODE_OR_XL_SPREL =  8'h2a, // or xl, (n, sp)
	OPCODE_OR_XL_ZREL =   8'h2b, // or xl, (nn, sp)
	OPCODE_OR_XL_ZL =     8'h2c, // or xl, zl
	OPCODE_OR_XL_XH =     8'h2d, // or xl, xh
	OPCODE_OR_XL_YL =     8'h2e, // or xl, yl
	OPCODE_OR_XL_YH =     8'h2f, // or xl, yh
	OPCODE_AND_XL_IMMD =  8'h30, // and xl, #i
	OPCODE_AND_XL_DIR =   8'h31, // and xl, mm
	OPCODE_AND_XL_SPREL = 8'h32, // and xl, (n, sp)
	OPCODE_AND_XL_ZREL =  8'h33, // and xl, (nn, sp)
	OPCODE_AND_XL_ZL =    8'h34, // and xl, zl
	OPCODE_AND_XL_XH =    8'h35, // and xl, xh
	OPCODE_AND_XL_YL =    8'h36, // and xl, yl
	OPCODE_AND_XL_YH =    8'h37, // and xl, yh
	OPCODE_XOR_XL_IMMD =  8'h38, // xor xl, #i
	OPCODE_XOR_XL_DIR =   8'h39, // xor xl, mm
	OPCODE_XOR_XL_SPREL = 8'h3a, // xor xl, (n, sp)
	OPCODE_XOR_XL_ZREL =  8'h3b, // xor xl, (nn, sp)
	OPCODE_XOR_XL_ZL =    8'h3c, // xor xl, zl
	OPCODE_XOR_XL_XH =    8'h3d, // xor xl, xh
	OPCODE_XOR_XL_YL =    8'h3e, // xor xl, yl
	OPCODE_XOR_XL_YH =    8'h3f, // xor xl, yh
	OPCODE_SRL_DIR =      8'h40, // srl mm
	OPCODE_SRL_SPREL =    8'h41, // srl (n, sp)
	OPCODE_SRL_XL =       8'h42, // srl xl
	OPCODE_SRL_ZH =       8'h43, // srl zh
	OPCODE_SLL_DIR =      8'h44, // sll mm
	OPCODE_SLL_SPREL =    8'h45, // sll (n, sp)
	OPCODE_SLL_XL =       8'h46, // sll xl
	OPCODE_SLL_ZH =       8'h47, // sll zh
	OPCODE_RRC_DIR =      8'h48, // rrc mm
	OPCODE_RRC_SPREL =    8'h49, // rrc (n, sp)
	OPCODE_RRC_XL =       8'h4a, // rrc xl
	OPCODE_RRC_ZH =       8'h4b, // rrc zh
	OPCODE_RLC_DIR =      8'h4c, // rlc mm
	OPCODE_RLC_SPREL =    8'h4d, // rlc (n, sp)
	OPCODE_RLC_XL =       8'h4e, // rlc xl
	OPCODE_RLC_ZH =       8'h4f, // rlc zh
	OPCODE_INC_DIR =      8'h50, // inc mm
	OPCODE_INC_SPREL =    8'h51, // inc (n, sp)
	OPCODE_INC_XL =       8'h52, // inc xl
	OPCODE_INC_ZH =       8'h53, // inc zh
	OPCODE_DEC_DIR =      8'h54, // dec mm
	OPCODE_DEC_SPREL =    8'h55, // dec (n, sp)
	OPCODE_DEC_XL =       8'h56, // dec xl
	OPCODE_DEC_ZH =       8'h57, // dec zh
	OPCODE_CLR_DIR =      8'h58, // clr mm
	OPCODE_CLR_SPREL =    8'h59, // clr (n, sp)
	OPCODE_CLR_XL =       8'h5a, // clr xl
	OPCODE_CLR_ZH =       8'h5b, // clr zh
	OPCODE_TST_DIR =      8'h5c, // tst mm
	OPCODE_TST_SPREL =    8'h5d, // tst (n, sp)
	OPCODE_TST_XL =       8'h5e, // tst xl
	OPCODE_TST_ZH =       8'h5f, // tst zh
	OPCODE_PUSH_DIR =     8'h60, // push mm
	OPCODE_PUSH_SPREL =   8'h61, // push (n, sp)
	OPCODE_PUSH_XL =      8'h62, // push xl
	OPCODE_PUSH_ZH =      8'h63, // push zh
	OPCODE_JP_IMMD =      8'h64, // jp #ii
	OPCODE_JP_Y =         8'h65, // jp y
	OPCODE_CALL_IMMD =    8'h66, // call #ii
	OPCODE_CALL_Y =       8'h67, // call y
	// todo
	OPCODE_LDW_Y_SP =     8'h70, // ld y, sp
	OPCODE_SUBW_Y_DIR =   8'h71, // subw y, mm
	OPCODE_SUBW_Y_SPREL = 8'h72, // subw y, (n, sp)
	OPCODE_SUBW_Y_X =     8'h73, // subw y, x
	OPCODE_LDW_SPRELREL_Y = 8'h74, // ldw ((sp, n)), y
	OPCODE_SBCW_Y_DIR =   8'h75, // sbcw y, mm
	OPCODE_SBCW_Y_SPREL = 8'h76, // sbcw y, (n, sp)
	OPCODE_SBCW_Y_X =     8'h77, // sbcw y, x
	OPCODE_ADDW_Y_IMMD =  8'h78, // addw y, #ii
	OPCODE_ADDW_Y_DIR =   8'h79, // addw y, mm
	OPCODE_ADDW_Y_SPREL = 8'h7a, // addw y, (n, sp)
	OPCODE_ADDW_Y_X =     8'h7b, // addw y, x
	OPCODE_ADCW_Y_IMMD =  8'h7c, // adcw y, #ii
	OPCODE_ADCW_Y_DIR =   8'h7d, // adcw y, mm
	OPCODE_ADCW_Y_SPREL = 8'h7e, // adcw y, (n, sp)
	OPCODE_ADCW_Y_X =     8'h7f, // adcw y, x
	OPCODE_LD_XL_IMMD =   8'h80, // ld xl, #i
	OPCODE_LD_XL_IY =     8'h88, // ld xl, (y)
	OPCODE_LD_IY_XL =     8'h8e, // ld (y), xl
	OPCODE_PUSH_IMMD =    8'h90, // push #i
	OPCODE_POP_XL =       8'h99, // pop xl
	OPCODE_SWAPOP =       8'h9c, // swapop (prefix)
	OPCODE_ALTACC1 =      8'h9d, // altacc (prefix)
	OPCODE_ALTACC2 =      8'h9e, // altacc' (prefix)
	OPCODE_ALTACC3 =      8'h9f, // altacc'' (prefix)
	OPCODE_RET =          8'hba, // ret???
	OPCODE_LDW_Y_IMMD =   8'hc0, // ldw y, #ii
	OPCODE_LDW_Y_D =      8'hc7, // ldw y, #d
	// todo
	OPCODE_LDW_X_Y =      8'hcb, // ldw x, y
	// todo
	OPCODE_JR_D =         8'hd0, // jr #d
	OPCODE_DNJNZ_YH_D =   8'hd1, // dnjnz yh, #d
	OPCODE_JRZ_D =        8'hd2, // jrz #d
	OPCODE_JRNZ_D =       8'hd3, // jrnz #d
	OPCODE_JRC_D =        8'hd4, // jrc #d
	OPCODE_JRNC_D =       8'hd5, // jrnc #d
	OPCODE_JRN_D =        8'hd6, // jrn #d
	OPCODE_JRNN_D =       8'hd7, // jrnn #d
	// todo
	OPCODE_PUSHW_IMMD =   8'he8, // pushw #ii
	OPCODE_POPW_Y =       8'he9, // popw y
	// todo
	OPCODE_ADDW_Y_D =     8'heb, // addw y, #d
	// todo
	OPCODE_CPW_Y_IMMD =   8'hf8, // cpw y, #ii
	// todo
	OPCODE_RESERVED_FC =  8'hfc,
	OPCODE_RESERVED_FD =  8'hfd,
	OPCODE_RESERVED_FE =  8'hfe,
	OPCODE_RESERVED_FF =  8'hff
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
	return(opcode == OPCODE_SRL_DIR || opcode == OPCODE_SRL_SPREL || opcode == OPCODE_SRL_XL || opcode == OPCODE_SRL_ZH);
endfunction

function automatic logic opcode_is_sll(opcode_t opcode);
	return(opcode == OPCODE_SLL_DIR || opcode == OPCODE_SLL_SPREL || opcode == OPCODE_SLL_XL || opcode == OPCODE_SLL_ZH);
endfunction

function automatic logic opcode_is_rrc(opcode_t opcode);
	return(opcode == OPCODE_RRC_DIR || opcode == OPCODE_RRC_SPREL || opcode == OPCODE_RRC_XL || opcode == OPCODE_RRC_ZH);
endfunction

function automatic logic opcode_is_rlc(opcode_t opcode);
	return(opcode == OPCODE_RLC_DIR || opcode == OPCODE_RLC_SPREL || opcode == OPCODE_RLC_XL || opcode == OPCODE_RLC_ZH);
endfunction

function automatic logic opcode_is_inc(opcode_t opcode);
	return(opcode == OPCODE_INC_DIR || opcode == OPCODE_INC_SPREL || opcode == OPCODE_INC_XL || opcode == OPCODE_INC_ZH);
endfunction

function automatic logic opcode_is_dec(opcode_t opcode);
	return(opcode == OPCODE_DEC_DIR || opcode == OPCODE_DEC_SPREL || opcode == OPCODE_DEC_XL || opcode == OPCODE_DEC_ZH);
endfunction

function automatic logic opcode_is_clr(opcode_t opcode);
	return(opcode == OPCODE_CLR_DIR || opcode == OPCODE_CLR_SPREL || opcode == OPCODE_CLR_XL || opcode == OPCODE_CLR_ZH);
endfunction

function automatic logic opcode_is_tst(opcode_t opcode);
	return(opcode == OPCODE_TST_DIR || opcode == OPCODE_TST_SPREL || opcode == OPCODE_TST_XL || opcode == OPCODE_TST_ZH);
endfunction

function automatic logic opcode_is_push(opcode_t opcode);
	return(opcode == OPCODE_PUSH_DIR || opcode == OPCODE_PUSH_SPREL || opcode == OPCODE_PUSH_XL || opcode == OPCODE_PUSH_ZH || opcode == OPCODE_PUSH_IMMD);
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
	return(opcode == OPCODE_ADDW_Y_IMMD || opcode == OPCODE_ADCW_Y_DIR || opcode == OPCODE_ADCW_Y_SPREL || opcode == OPCODE_ADCW_Y_X);
endfunction

function automatic logic opcode_is_pushw(opcode_t opcode);
	return(opcode == OPCODE_PUSHW_IMMD);
endfunction

function automatic logic opcode_is_8_2_immd(opcode_t opcode);
	return(opcode == OPCODE_ADD_XL_IMMD || opcode == OPCODE_ADC_XL_IMMD || opcode == OPCODE_CP_XL_IMMD ||
		opcode == OPCODE_OR_XL_IMMD || opcode == OPCODE_AND_XL_IMMD || opcode == OPCODE_XOR_XL_IMMD );
endfunction

function automatic logic opcode_is_8_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_DIR || opcode == OPCODE_SBC_XL_DIR || opcode == OPCODE_ADD_XL_DIR || opcode == OPCODE_ADC_XL_DIR ||
		opcode == OPCODE_OR_XL_DIR || opcode == OPCODE_AND_XL_DIR || opcode == OPCODE_XOR_XL_DIR);
endfunction

function automatic logic opcode_is_8_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_SPREL || opcode == OPCODE_SBC_XL_SPREL || opcode == OPCODE_ADD_XL_SPREL || opcode == OPCODE_ADC_XL_SPREL || opcode == OPCODE_CP_XL_SPREL ||
	opcode == OPCODE_OR_XL_SPREL || opcode == OPCODE_AND_XL_SPREL || opcode == OPCODE_XOR_XL_SPREL);
endfunction

function automatic logic opcode_is_8_2_zrel(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZREL || opcode == OPCODE_SBC_XL_ZREL || opcode == OPCODE_ADD_XL_ZREL || opcode == OPCODE_ADC_XL_ZREL ||
	opcode == OPCODE_OR_XL_ZREL || opcode == OPCODE_AND_XL_ZREL || opcode == OPCODE_XOR_XL_ZREL);
endfunction

function automatic logic opcode_is_8_2_zl(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_ZL || opcode == OPCODE_SBC_XL_ZL || opcode == OPCODE_ADD_XL_ZL || opcode == OPCODE_ADC_XL_ZL ||
	opcode == OPCODE_OR_XL_ZL || opcode == OPCODE_AND_XL_ZL || opcode == OPCODE_AND_XL_ZL);
endfunction

function automatic logic opcode_is_8_2_xh(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_XH|| opcode == OPCODE_SBC_XL_XH || opcode == OPCODE_ADD_XL_XH || opcode == OPCODE_ADC_XL_XH ||
	opcode == OPCODE_OR_XL_XH || opcode == OPCODE_AND_XL_XH || opcode == OPCODE_XOR_XL_XH);
endfunction

function automatic logic opcode_is_8_2_yl(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_YL|| opcode == OPCODE_SBC_XL_YL || opcode == OPCODE_ADD_XL_YL || opcode == OPCODE_ADC_XL_YL || opcode == OPCODE_OR_XL_YL);
endfunction

function automatic logic opcode_is_8_2_yh(opcode_t opcode);
	return(opcode == OPCODE_SUB_XL_YH|| opcode == OPCODE_SBC_XL_YH || opcode == OPCODE_ADD_XL_YH || opcode == OPCODE_ADC_XL_YH || opcode == OPCODE_OR_XL_YH);
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
	return(opcode == OPCODE_SRL_XL || opcode == OPCODE_SLL_XL || opcode == OPCODE_RLC_XL || opcode == OPCODE_RRC_XL ||
		opcode == OPCODE_INC_XL || opcode == OPCODE_DEC_XL ||
		opcode == OPCODE_CLR_XL || opcode == OPCODE_TST_XL ||opcode == OPCODE_PUSH_XL);
endfunction

function automatic logic opcode_is_8_1_zh(opcode_t opcode);
	return(opcode == OPCODE_SRL_ZH || opcode == OPCODE_SLL_ZH || opcode == OPCODE_SRL_ZH || opcode == OPCODE_RRC_ZH ||
		opcode == OPCODE_INC_ZH || opcode == OPCODE_DEC_ZH ||
		opcode == OPCODE_INC_ZH || opcode == OPCODE_TST_ZH ||opcode == OPCODE_PUSH_ZH);
endfunction

function automatic logic opcode_is_8_1(opcode_t opcode);
	return(opcode_is_8_1_dir(opcode) || opcode_is_8_1_sprel(opcode) || opcode_is_8_1_xl(opcode) || opcode_is_8_1_zh(opcode));
endfunction

function automatic logic opcode_is_16_2_immd(opcode_t opcode);
	return(opcode == OPCODE_ADDW_Y_IMMD || opcode == OPCODE_ADCW_Y_IMMD);
endfunction

function automatic logic opcode_is_16_2_dir(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_DIR || opcode == OPCODE_SBCW_Y_DIR || opcode == OPCODE_ADDW_Y_DIR || opcode == OPCODE_ADCW_Y_DIR);
endfunction

function automatic logic opcode_is_16_2_sprel(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_SPREL || opcode == OPCODE_SBCW_Y_SPREL || opcode == OPCODE_ADDW_Y_SPREL || opcode == OPCODE_ADCW_Y_SPREL);
endfunction

function automatic logic opcode_is_16_2_x(opcode_t opcode);
	return(opcode == OPCODE_SUBW_Y_X || opcode == OPCODE_SBCW_Y_X || opcode == OPCODE_ADDW_Y_X || opcode == OPCODE_ADCW_Y_X);
endfunction

function automatic logic opcode_is_16_2(opcode_t opcode);
	return(opcode_is_16_2_immd(opcode) ||opcode_is_16_2_dir(opcode) || opcode_is_16_2_sprel(opcode) || opcode_is_16_2_x(opcode));
endfunction

function automatic logic opcode_is_8_immd(opcode_t opcode);
	return(opcode_is_8_2_immd(opcode) || opcode == OPCODE_LD_XL_IMMD);
endfunction

function automatic logic opcode_is_16_immd(opcode_t opcode);
	return(opcode_is_16_2_immd(opcode) || opcode == OPCODE_PUSHW_IMMD || opcode == OPCODE_LDW_Y_IMMD || opcode == OPCODE_CPW_Y_IMMD);
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

function automatic logic opcode_is_jr_d(opcode_t opcode);
	return(opcode == OPCODE_JR_D || opcode == OPCODE_DNJNZ_YH_D || opcode == OPCODE_JRZ_D || opcode == OPCODE_JRNZ_D || opcode == OPCODE_JRC_D || opcode == OPCODE_JRNC_D || opcode == OPCODE_JRN_D || opcode == OPCODE_JRNN_D);
endfunction

