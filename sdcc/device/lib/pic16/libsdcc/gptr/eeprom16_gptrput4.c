/*-------------------------------------------------------------------------
   eeprom16_gptrput4.c - write 4 byte value to EEPROM via a generic pointer

   Copyright (C) 2012 Raphael Neider <rneider AT web.de>

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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

extern int EEADR;
extern int EEADRH;
extern int EECON1;
extern int EEDATA;
extern int FSR0H;
extern int FSR0L;
extern int INTCON;
extern int PREINC1;
extern int PRODH;
extern int PRODL;
extern int TBLPTRL;
extern int TBLPTRH;
extern int TABLAT;
extern int __eeprom16_write;

/* Write 4 bytes to 16-bit EEPROM address, increment EEPROM address by 4 for
 * next read.  Data in TABLAT:PRODH:PRODL:WREG. */
void
__eeprom16_gptrput4(void) __naked
{
    __asm
        MOVFF   _INTCON, _FSR0L     ; save previous interrupt state
        BCF     _INTCON, 7, 0       ; GIE = 0: disable interrupts

        BSF     _EECON1, 2, 0       ; WREN = 1: enable write access

        MOVWF   _EEDATA             ; load first byte
        CALL    ___eeprom16_write   ; write and address next byte

        MOVFF   _PRODL, _EEDATA     ; load second byte
        CALL    ___eeprom16_write   ; write and address next byte

        MOVFF   _PRODH, _EEDATA     ; load third byte
        CALL    ___eeprom16_write   ; write and address next byte

        MOVFF   _TABLAT, _EEDATA    ; load fourth byte
        CALL    ___eeprom16_write   ; write and address next byte

        BCF     _EECON1, 2, 0       ; WREN = 0: disable write access

        BTFSC   _FSR0L, 7, 0        ; check previous interrupt state
        BSF     _INTCON, 7, 0       ; conditionally re-enable interrupts

        RETURN
    __endasm;
}
