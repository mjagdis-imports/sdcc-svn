; assembler configuration
.optsdcc -mmcs51 --model-small

; defines for conditional code
.define MCS51
.define MODEL_SMALL
.define XSTACK_AUTO

; pseudo instructions
.define RET "ret"

