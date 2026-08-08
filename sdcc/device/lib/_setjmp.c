/*-------------------------------------------------------------------------
   setjmp.c - source file for ANSI routines setjmp & longjmp

   Copyright (C) 1999, Sandeep Dutta . sandeep.dutta@usa.net

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#include <sdcc-lib.h>
#define __SDCC_HIDE_LONGJMP
#include <setjmp.h>

#if defined(__SDCC_ds390)

#include <ds80c390.h>

int __setjmp (jmp_buf buf)
{
    unsigned char sp, esp;
    unsigned int lsp;
    unsigned char __xdata * xsp;

    /* registers would have been saved on the
       stack anyway so we need to save SP
       and the return address */
    __critical {
        sp = SP;
        esp = ESP;
    }
    *buf++ = sp;
    *buf++ = esp;
    lsp = sp;
    lsp |= esp << 8;
    xsp = (unsigned char __xdata *) (0x400000 | lsp);
    *buf = *xsp;
    *++buf = *--xsp;
    *++buf = *--xsp;
    return 0;
}

int longjmp (jmp_buf buf, int rv)
{
    unsigned char sp, esp;
    unsigned int lsp;
    unsigned char __xdata * xsp;

    sp = *buf++;
    esp = *buf++;
    __critical {
        SP = sp;
        ESP = esp;
    }
    lsp = sp;
    lsp |= esp << 8;
    xsp = (unsigned char __xdata *) (0x400000 | lsp);
    *xsp = *buf;
    *--xsp = *++buf;
    *--xsp = *++buf;
    return rv ? rv : 1;
}

#elif defined(__SDCC_STACK_AUTO) && defined(__SDCC_USE_XSTACK)

static void dummy (void) __naked
{
	__asm

;------------------------------------------------------------
;Allocation info for local variables in function 'setjmp'
;------------------------------------------------------------
;buf                       Allocated to registers dptr b
;------------------------------------------------------------
;../../device/lib/_setjmp.c:180:int setjmp (jmp_buf buf)
;	-----------------------------------------
;	 function setjmp
;	-----------------------------------------
	.globl ___setjmp
___setjmp:
	ar2 = 0x02
	ar3 = 0x03
	ar4 = 0x04
	ar5 = 0x05
	ar6 = 0x06
	ar7 = 0x07
	ar0 = 0x00
	ar1 = 0x01
;../../device/lib/_setjmp.c:183:*buf++ = bpx;
;     genPointerSet
;     genGenPointerSet
	mov	a,_bpx
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:184:*buf++ = spx;
;     genPointerSet
;     genGenPointerSet
	mov	a,_spx
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:185:*buf++ = bp;
;     genPointerSet
;     genGenPointerSet
	mov	a,_bp
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:186:*buf++ = SP;
;     genPointerSet
;     genGenPointerSet
	mov	a,sp
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:187:*buf++ = *((unsigned char __data *) SP  );
;     genCast
;     genPointerGet
;     genNearPointerGet
;     genPointerSet
;     genGenPointerSet
	mov	r0,a
	mov	a,@r0
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:188:*buf   = *((unsigned char __data *)SP - 1);
;     genCast
;     genMinus
;     genMinusDec
;	peephole 177.g	optimized mov sequence
	dec	r0
;     genPointerGet
;     genNearPointerGet
	mov	a,@r0
;     genPointerSet
;     genGenPointerSet
	lcall	__gptrput
#ifdef __SDCC_MODEL_HUGE
	inc	dptr
;../../device/lib/_setjmp.c:189:*buf   = *((unsigned char __data *)SP - 2);
;     genCast
;     genMinus
;     genMinusDec
;	peephole 177.g	optimized mov sequence
	dec	r0
;     genPointerGet
;     genNearPointerGet
	mov	a,@r0
;     genPointerSet
;     genGenPointerSet
	lcall	__gptrput
#endif
;../../device/lib/_setjmp.c:190:return 0;
;     genRet
	mov	dptr,#0x0000
	_RETURN

;------------------------------------------------------------
;Allocation info for local variables in function 'longjmp'
;------------------------------------------------------------
;rv                        Allocated to stack - offset -2 / r2 r3
;buf                       Allocated to registers dptr b
;lsp                       Allocated to registers r5
;------------------------------------------------------------
;../../device/lib/_setjmp.c:192:int longjmp (jmp_buf buf, int rv)
;	-----------------------------------------
;	 function longjmp
;	-----------------------------------------
	.globl _longjmp
_longjmp:
;     genReceive
	mov	r0,_spx
	dec	r0
	movx	a,@r0
	mov	r2,a
	dec	r0
	movx	a,@r0
	mov	r3,a
;../../device/lib/_setjmp.c:193:bpx = *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	mov	_bpx,a
	inc	dptr
;../../device/lib/_setjmp.c:194:spx = *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	mov	_spx,a
	inc	dptr
;../../device/lib/_setjmp.c:195:bp = *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	mov	_bp,a
	inc	dptr
;../../device/lib/_setjmp.c:196:lsp = (unsigned char __idata *) *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	inc	dptr
;     genCast
	mov	r0,a
;../../device/lib/_setjmp.c:197:SP = (unsigned char) lsp;
;     genAssign
	mov	sp,a
;../../device/lib/_setjmp.c:198:*lsp = *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	inc	dptr
;     genPointerSet
;     genNearPointerSet
	mov	@r0,a
;../../device/lib/_setjmp.c:199:*--lsp = *buf;
;     genMinus
;     genMinusDec
	dec	r0
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
;     genPointerSet
;     genNearPointerSet
	mov	@r0,a
#ifdef __SDCC_MODEL_HUGE
	inc	dptr
;../../device/lib/_setjmp.c:200:*--lsp = *buf;
;     genMinus
;     genMinusDec
	dec	r0
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
;     genPointerSet
;     genNearPointerSet
	mov	@r0,a
#endif
;../../device/lib/_setjmp.c:201:return rv ? rv : 1;
;     genAssign
	mov	dph,r2
	mov	dpl,r3
	mov	a,r2
	orl	a,r3
	jnz	00001$
	inc	dptr
;     genRet
00001$:
	_RETURN

	__endasm;
}

#elif defined(__SDCC_STACK_AUTO)

static void dummy (void) __naked
{
	__asm

;------------------------------------------------------------
;Allocation info for local variables in function 'setjmp'
;------------------------------------------------------------
;buf                       Allocated to registers dptr b
;------------------------------------------------------------
;../../device/lib/_setjmp.c:122:int setjmp (unsigned char *buf)
;	-----------------------------------------
;	 function setjmp
;	-----------------------------------------
	.globl ___setjmp
___setjmp:
	ar2 = 0x02
	ar3 = 0x03
	ar4 = 0x04
	ar5 = 0x05
	ar6 = 0x06
	ar7 = 0x07
	ar0 = 0x00
	ar1 = 0x01
;     genReceive
;../../device/lib/_setjmp.c:125:*buf   = BP;
;     genPointerSet
;     genGenPointerSet
	mov	a,_bp
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:126:*buf   = SP;
;     genPointerSet
;     genGenPointerSet
	mov	a,sp
	lcall	__gptrput
	inc	dptr
;../../device/lib/_setjmp.c:127:lsp = (unsigned char __idata *) SP;
;     genCast
	mov	r0,sp
;../../device/lib/_setjmp.c:128:*buf = *lsp;
;     genPointerGet
;     genNearPointerGet
	mov	a,@r0
;     genPointerSet
;     genGenPointerSet
	lcall	__gptrput
;../../device/lib/_setjmp.c:129:*++buf = *--lsp;
;     genMinus
;     genMinusDec
	dec	r0
;     genPointerGet
;     genNearPointerGet
	mov	a,@r0
;     genPointerSet
;     genGenPointerSet
	inc	dptr
	lcall	__gptrput
#ifdef __SDCC_MODEL_HUGE
;../../device/lib/_setjmp.c:130:*++buf = *--lsp;
;     genMinus
;     genMinusDec
	dec	r0
;     genPointerGet
;     genNearPointerGet
	mov	a,@r0
;     genPointerSet
;     genGenPointerSet
	inc	dptr
	lcall	__gptrput
#endif
;../../device/lib/_setjmp.c:131:return 0;
;     genRet
	mov	dptr,#0x0000
	_RETURN

;------------------------------------------------------------
;Allocation info for local variables in function 'longjmp'
;------------------------------------------------------------
;rv                        Allocated to stack - offset -3 / r2 r3
;buf                       Allocated to registers dptr b
;lsp                       Allocated to registers r5
;------------------------------------------------------------
;../../device/lib/_setjmp.c:28:int longjmp (jmp_buf buf, int rv)
;	-----------------------------------------
;	 function longjmp
;	-----------------------------------------
	.globl _longjmp
_longjmp:
	ar2 = 0x02
	ar3 = 0x03
	ar4 = 0x04
	ar5 = 0x05
	ar6 = 0x06
	ar7 = 0x07
	ar0 = 0x00
	ar1 = 0x01
;     genReceive
	mov	r0,sp
	dec	r0
	dec	r0
#ifdef __SDCC_MODEL_HUGE
	dec	r0
#endif
	mov	ar2,@r0
	dec	r0
	mov	ar3,@r0
;../../device/lib/_setjmp.c:30:bp = *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	inc	dptr
;     genAssign
	mov	_bp,a
;../../device/lib/_setjmp.c:31:lsp = (unsigned char __idata *) *buf++;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
	inc	dptr
;     genCast
	mov	r0,a
;../../device/lib/_setjmp.c:32:SP = (unsigned char) lsp;
;     genCast
	mov	sp,a
;../../device/lib/_setjmp.c:33:*lsp = *buf;
;     genPointerGet
;     genGenPointerGet
	lcall	__gptrget
;     genPointerSet
;     genNearPointerSet
	mov	@r0,a
;../../device/lib/_setjmp.c:34:*--lsp = *++buf;
;     genPointerGet
;     genGenPointerGet
	inc	dptr
	lcall	__gptrget
;     genPointerSet
;     genNearPointerSet
	dec	r0
	mov	@r0,a
#ifdef __SDCC_MODEL_HUGE
;../../device/lib/_setjmp.c:35:*--lsp = *++buf;
;     genPointerGet
;     genGenPointerGet
	inc	dptr
	lcall	__gptrget
;     genPointerSet
;     genNearPointerSet
	dec	r0
	mov	@r0,a
#endif
;../../device/lib/_setjmp.c:36:return rv ? rv : 1;
;     genAssign
	mov	dph,r2
	mov	dpl,r3
	mov	a,r2
	orl	a,r3
	jnz	00001$
	inc	dptr
;     genRet
00001$:
	_RETURN

	__endasm;
}

#else

#include <8051.h>

extern unsigned char __data spx;
extern unsigned char __data bpx;

int __setjmp (jmp_buf buf)
{
    unsigned char * p = buf;
    unsigned char __idata * lsp;
    /* registers would have been saved on the
       stack anyway so we need to save SP
       and the return address */
#ifdef __SDCC_USE_XSTACK
    *p++ = spx;
    *p++ = bpx;
#endif
    lsp = (unsigned char __idata *) SP;
    *p++ = (unsigned char) lsp;
    *p = *lsp;
    *++p = *--lsp;
#ifdef __SDCC_MODEL_HUGE
    *++p = *--lsp;
#endif
    return 0;
}

int longjmp (jmp_buf buf, int rv)
{
    unsigned char * p = buf;
    unsigned char __idata * lsp;

#ifdef __SDCC_USE_XSTACK
    spx = *p++;
    bpx = *p++;
#endif
    lsp = (unsigned char __idata *) *p++;
    SP = (unsigned char) lsp;
    *lsp = *p;
    *--lsp = *++p;
#ifdef __SDCC_MODEL_HUGE
    *--lsp = *++p;
#endif
    return rv ? rv : 1;
}

#endif
