; assembler configuration
.optsdcc -mds390 --model-flat24
.DS80C390
.amode	2	; 24 bit flat addressing

; defines for conditional code
.define DS390
.define DSXXX

; pseudo instructions
.define RET "ret"

dpl	=	0x82
dph	=	0x83
dpl1	=	0x84
dph1	=	0x85
dps	=	0x86
dpx	=	0x93
dpx1	=	0x95
esp	=	0x9B
ap	=	0x9C
acc1	=	0x9C
mcnt0	=	0xD1
mcnt1	=	0xD2
ma	=	0xD3
mb	=	0xD4
mc	=	0xD5
acon	=	0x9D
mcon	=	0xC6
F1	=	0xD1	; user flag

