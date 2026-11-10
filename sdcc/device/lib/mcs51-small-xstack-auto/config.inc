; assembler configuration
.optsdcc -mmcs51 --model-small

; defines for conditional code
.define MCS51
.define MODEL_SMALL
.define XSTACK_AUTO

; pseudo instructions
.define RET "ret"

.area REG_BANK_0	(REL,OVR,DATA)
rbank0:	.ds 8

ar0 = rbank0 + 0x00
ar1 = rbank0 + 0x01
ar2 = rbank0 + 0x02
ar3 = rbank0 + 0x03
ar4 = rbank0 + 0x04
ar5 = rbank0 + 0x05
ar6 = rbank0 + 0x06
ar7 = rbank0 + 0x07
