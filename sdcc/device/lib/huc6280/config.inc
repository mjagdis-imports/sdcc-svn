; assembler configuration
.optsdcc -mhuc6280
.huc6280

; defines for conditional code
.define MOS65C02
.define HUC6280

; pseudo instructions
.define CLA "cla"
.define CLX "clx"
.define CLY "cly"

; stack location
STACK = 0x2100

