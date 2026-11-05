; assembler configuration
.optsdcc -mmcs51 --model-huge

; defines for conditional code
.define MCS51
.define MODEL_HUGE

; pseudo instructions
.define RET "ljmp sdcc_banked_ret"
