/* pdk.h */

/*
 *  Copyright (C) 1998-2009  Alan R. Baldwin
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 *
 *   This Assember Ported by
 *      John L. Hartman (JLH)
 *      jhartman at compuserve dot com
 *      noice at noicedebugger dot com
 *
 */

/*)BUILD
        $(PROGRAM) =    ASPDK
        $(INCLUDE) = {
                ASXXXX.H
                PDK.H
        }
        $(FILES) = {
                I51MCH.C
                I51ADR.C
                I51PST.C
                ASMAIN.C
                ASDBG.C
                ASLEX.C
                ASSYM.C
                ASSUBR.C
                ASEXPR.C
                ASDATA.C
                ASLIST.C
                ASOUT.C
        }
        $(STACK) = 3000
*/

/*
 * Instructions.
 */
#define S_MOV     50
#define S_LDT16   51
#define S_STT16   52
#define S_IDXM    53
#define S_XCH     54
#define S_PUSHAF  55
#define S_POPAF   56
#define S_ADD     57
#define S_ADDC    58
#define S_SUB     59
#define S_SUBC    60
#define S_INC     61
#define S_DEC     62
#define S_CLEAR   63
#define S_SR      64
#define S_SRC     65
#define S_SL      66
#define S_SLC     67
#define S_SWAP    68
#define S_AND     69
#define S_OR      70
#define S_XOR     71
#define S_NOT     72
#define S_NEG     73
#define S_SET0    74
#define S_SET1    75
#define S_CEQSN   76
#define S_T0SN    77
#define S_T1SN    78
#define S_IZSN    79
#define S_DZSN    80
#define S_CALL    81
#define S_GOTO    82
#define S_RET     83
#define S_RETI    84
#define S_NOP     85
#define S_PCADD   86
#define S_ENGINT  87
#define S_DISGINT 88
#define S_STOPSYS 89
#define S_STOPEXE 90
#define S_RESET   91
#define S_WDRESET 92
#define S_SWAPC   93
#define S_CNEQSN  94 
#define S_LDSPTL  95
#define S_LDSPTH  96

/*
 * Addressing modes.
 */
#define S_K    31
#define S_A    32
#define S_M    33
#define S_IO   34

       /* machine dependent functions */

#ifdef  OTHERSYSTEM
        
        /* pdkaddr.c */
extern  int             addr(struct expr *esp);
extern  int             pdkbit(struct expr *esp);

        /* pdkmch.c */
extern  VOID            machine(struct mne *mp);
extern  int             mchpcr(struct expr *esp);
extern  VOID            minit(void);

#else

        /* pdkadr.c */
extern  int             addr();
extern  int             pdkbit();

        /* pdkmch.c */
extern  VOID            machine();
extern  int             mchpcr();
extern  VOID            minit();

#endif
