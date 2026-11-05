; assembler configuration
.optsdcc -mmcs51 --model-large

; defines for conditional code
.define MCS51
.define MODEL_LARGE
.define STACK_AUTO

; pseudo instructions
.define RET "ret"
