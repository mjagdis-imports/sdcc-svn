/* pdkmch.c */

/*
 *  Copyright (C) 1998-2011  Alan R. Baldwin
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
 *  Benny Kim (2011/07/21)
 *  bennykim at coreriver dot com
 *  Fixed bugs in relative address with "."
 */

#include "asxxxx.h"
#include "pdk.h"

char    *cpu    = "Padauk 14";
char    *dsft   = "asm";

/*
 * Process machine ops.
 */
VOID
machine(struct mne *mp)
{
        a_uint op;
        int t, t1, combine, combine1;
        struct expr e, e1, e2;

        clrexpr(&e);
        clrexpr(&e1);
        clrexpr(&e2);

        op = mp->m_valu;
        combine = combine1 = 0;
        switch (mp->m_type) {

        case S_MOV:
                t = addr(&e);
                comma(1);
                t1 = addr(&e1);
                if (t == S_IO && t1 == S_A) {
                        op = 0x0180;
                        op |= e.e_addr & 0x3F;
                } else
                if (t == S_A && t1 == S_IO) {
                        op = 0x01C0;
                        op |= e1.e_addr & 0x3F;
                } else
                if (t == S_M && t1 == S_A) {
                        op = 0xB80;
                        op |= e.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_M) {
                        op = 0x0F80;
                        op |= e1.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_K)
                        op |= e1.e_addr & 0xFF;
                else
                        aerr();

                outaw(op);
                break;

        case S_IDXM:
                t = addr(&e);
                comma(1);
                t1 = addr(&e1);
                if (t == S_A && t1 == S_M) {
                        op |= 1;
                        op |= e1.e_addr & 0x7F;
                } else
                if (t == S_M && t1 == S_A) {
                        op |= (e.e_addr & 0x7F) << 1;
                } else
                        aerr();

                outaw(op);
                break;

        case S_SUB:
                combine = 0x80;
                /* fallthrough */
        case S_ADD:
                t = addr(&e);
                comma(1);
                t1 = addr(&e1);
                if (t == S_M && t1 == S_A) {
                        op = 0x0800 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_M) {
                        op = 0x0C00 | combine;
                        op |= e1.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_K) {
                        op |= e1.e_addr & 0xFF;
                } else
                        aerr();

                outaw(op);
                break;

        case S_SUBC:
                combine = 0x80;
                /* fallthrough */
        case S_ADDC:
                t = addr(&e);
                if (comma(0)) {
                        t1 = addr(&e1);
                        if (t == S_M && t1 == S_A) {
                                op = 0x0900 | combine;
                                op |= e.e_addr & 0x7F;
                        } else
                        if (t == S_A && t1 == S_M) {
                                op = 0x0D00 | combine;
                                op |= e1.e_addr & 0x7F;
                        } else
                                aerr();
                } else
                if (t == S_M) {
                        op = 0x1000 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                if (t == S_A) {
                        op = 0x0060 + (combine ? 1 : 0);
                } else
                        aerr();

                outaw(op);
                break;

        case S_SLC:
        case S_SRC:
        case S_SL:
        case S_SR:
                if (mp->m_type == S_SRC || mp->m_type == S_SLC)
                        combine = 6;
                if (mp->m_type == S_SL || mp->m_type == S_SLC)
                        combine1 = 0x80;

                t = addr(&e);

                if (t == S_A) {
                        op = (0x006A ^ combine) + (combine1 ? 1 : 0);
                } else
                if (t == S_M) {
                        op = (0x1500 ^ combine) | combine1;
                        op |= e.e_addr & 0x7F;
                } else
                        aerr();

                outaw(op);
                break;

        case S_OR:
        case S_XOR:
        case S_AND:
                if (mp->m_type == S_OR) {
                        combine = 0x80;
                } else
                if (mp->m_type == S_XOR) {
                        combine = 0x100;
                }

                t = addr(&e);
                comma(1);
                t1 = addr(&e1);
                if (t == S_M && t1 == S_A) {
                        op = 0x0A00 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_M) {
                        op = 0x0E00 | combine;
                        op |= e1.e_addr & 0x7F;
                } else
                if (t == S_A && t1 == S_K) {
                        op |= e1.e_addr & 0xFF;
                } else
                if (t == S_IO && t1 == S_A && mp->m_type == S_XOR) {
                        op = 0x00C0;
                        op |= e.e_addr & 0x3F;
                } else 
                        aerr();

                outaw(op);
                break;

        case S_NEG:
                combine = 0x80;
                /* fallthrough */
        case S_NOT:
                t = addr(&e);
                if (t == S_M) {
                        op = 0x1400 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                if (t != S_A)
                        aerr();

                outaw(op);
                break;

        case S_SET1:
                combine = 0x200;
                /* fallthrough */
        case S_SET0:
                t = addr(&e);
                if (getnb() != '.')
                        aerr();
                t1 = addr(&e1);
                if (t1 != S_K)
                        aerr();

                if (t == S_IO) {
                        op = 0x1C00 | combine;
                        op |= e.e_addr & 0x3F;
                } else
                if (t == S_M) {
                        op = 0x2400 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                        aerr();

                op |= (e1.e_addr & 0x7) << 6;
                outaw(op);
                break;

        case S_CNEQSN:
                combine = 0x80;
                /* fallthrough */
        case S_CEQSN:
                t = addr(&e);
                if (t != S_A)
                        aerr();

                comma(1);
                t1 = addr(&e1);
                if (t1 == S_M) {
                        op = 0x1700 | combine;
                        op |= e1.e_addr & 0x7F;
                } else
                if (t1 == S_K) {
                        op = 0x2A00 | (combine << 1);
                        op |= e1.e_addr & 0xFF;
                } else
                        aerr();

                outaw(op);
                break;

        case S_T1SN:
                combine = 0x200;
                /* fallthrough */
        case S_T0SN:
                t = addr(&e);
                if (getnb() != '.')
                        aerr();
                t1 = addr(&e1);
                if (t1 != S_K)
                        aerr();

                if (t == S_IO) {
                        op = 0x1800 | combine;
                        op |= e.e_addr & 0x3F;
                } else
                if (t == S_M) {
                        op = 0x2000 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                        aerr();

                op |= (e1.e_addr & 0x7) << 6;
                outaw(op);
                break;

        case S_DZSN:
                combine = 0x80;
                /* fallthrough */
        case S_IZSN:
                t = addr(&e);
                if (t == S_M) {
                        op = 0x1100 | combine;
                        op |= e.e_addr & 0x7F;
                } else
                if (t != S_A)
                        aerr();

                outaw(op);
                break;

        case S_RET:
                if (more()) {
                        t = addr(&e);
                        if (t != S_K)
                                aerr();
                        op |= e.e_addr & 0xFF;
                }

                outaw(op);
                break;

        case S_INC:
        case S_DEC:
        case S_CLEAR:
                t = addr(&e);
                if (t != S_M)
                        aerr();
                op |= e.e_addr & 0x7F;

                outaw(op);
                break;

        case S_CALL:
        case S_GOTO:
                expr(&e, 0);
                op |= e.e_addr & 0xFF;
                outaw(op);
                break;

        case S_XCH:
                t = addr(&e);
                if (t == S_A) {
                        /* Ignore extra accumulator param. */
                        comma(1);
                        t = addr(&e);
                }
                if (t != S_M)
                        aerr();

                op |= e.e_addr & 0x7F;
                outaw(op);
                break;

        /* Simple instructions consisting of only one opcode and no args */
        case S_LDT16:
        case S_STT16:
        case S_PUSHAF:
        case S_POPAF:
        case S_SWAP:
        case S_RETI:
        case S_NOP:
        case S_PCADD:
        case S_ENGINT:
        case S_DISGINT:
        case S_STOPSYS:
        case S_STOPEXE:
        case S_RESET:
        case S_WDRESET:
        case S_SWAPC:
                outaw(op);
                break;
        }
}

/*
 * Branch/Jump PCR Mode Check
 */
int
mchpcr(struct expr *esp)
{
        if (esp->e_base.e_ap == dot.s_area) {
                return(1);
        }
        if (esp->e_flag==0 && esp->e_base.e_ap==NULL) {
                /*
                 * Absolute Destination
                 *
                 * Use the global symbol '.__.ABS.'
                 * of value zero and force the assembler
                 * to use this absolute constant as the
                 * base value for the relocation.
                 */
                esp->e_flag = 1;
                esp->e_base.e_sp = &sym[1];
        }
        return(0);
}

/*
 * Machine specific initialization
 */

VOID
minit(void)
{
        /*
         * Byte Order
         */
        hilo = 0;

        /*
         * Address Space
         */
        exprmasks(3);
}
