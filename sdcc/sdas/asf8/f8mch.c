/* stm8mch.c */

/*
 *  Copyright (C) 2010  Alan R. Baldwin
 *  Copyright (C) 2022  Philipp K. Krause
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
 */

#include "asxxxx.h"
#include "f8.h"

char	*cpu	= "f8";
char	*dsft	= "asm";

#define	NB	512

int	*bp;
int	bm;
int	bb[NB];

/*
 * Opcode Cycle Definitions
 */
#define	OPCY_SDP	((char) (0xFF))
#define	OPCY_ERR	((char) (0xFE))
#define	OPCY_SKP	((char)	(0xFD))

/*	OPCY_NONE	((char) (0x80))	*/
/*	OPCY_MASK	((char) (0x7F))	*/

#define	UN	((char) (OPCY_NONE | 0x00))
#define	P1	((char) (OPCY_NONE | 0x01))
#define	P2	((char) (OPCY_NONE | 0x02))
#define	P3	((char) (OPCY_NONE | 0x03))
#define	P4	((char) (OPCY_NONE | 0x04))

/*
 * stm8 Opcode Cycle Pages
 */

static char  stm8pg[256] = {
/*--*--* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/*--*--* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - */
/*00*/   1, 1, 1, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*10*/   1, 1, 1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2, 2, 2,
/*20*/   2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*30*/   1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*40*/   1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*50*/   2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 1, 1,
/*60*/   1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*70*/   1,UN,P1, 1, 1,UN, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*80*/  11, 4,UN, 9, 1, 2, 1, 5, 1, 2, 1,UN, 1, 5,10,10,
/*90*/  P2,P3,P4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*A0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 1,
/*B0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
/*C0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2,
/*D0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2,
/*E0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2,
/*F0*/   1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2
};

static char  pg72[256] = {  /* P1: PreByte == 72 */
/*--*--* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/*--*--* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - */
/*00*/   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/*10*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*20*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*30*/   4,UN,UN, 4, 4,UN, 4, 4, 4, 4, 4,UN, 4, 4, 4, 4,
/*40*/   1,UN,UN, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*50*/   1,UN,UN, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*60*/   4,UN,UN, 4, 4,UN, 4, 4, 4, 4, 4,UN, 4, 4, 4, 4,
/*70*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*80*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 1,
/*90*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*A0*/  UN,UN, 2,UN,UN,UN,UN,UN,UN, 2,UN,UN,UN,UN,UN,UN,
/*B0*/   2,UN, 2,UN,UN,UN,UN,UN,UN, 2,UN, 2,UN,UN,UN,UN,
/*C0*/   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 5, 5,
/*D0*/   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 5, 5,
/*E0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*F0*/   2,UN, 2,UN,UN,UN,UN,UN,UN, 2,UN, 2,UN,UN,UN,UN,
};

static char  pg90[256] = {  /* P2: PreByte == 90 */
/*--*--* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/*--*--* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - */
/*00*/  UN, 1, 1,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*10*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/*20*/  UN,UN,UN,UN,UN,UN,UN,UN, 1, 1,UN,UN, 1, 1, 1, 1,
/*30*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*40*/   1,UN, 4, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*50*/   2,UN,UN, 2, 2,UN, 2, 2, 2, 2, 2,UN, 1, 2, 1, 1,
/*60*/   1,UN, 2, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*70*/   1,UN,UN, 1, 1,UN, 1, 1, 1, 1, 1,UN, 1, 1, 1, 1,
/*80*/  UN,UN,UN,UN,UN, 2,UN,UN,UN, 2,UN,UN,UN,UN,UN,UN,
/*90*/  UN,UN,UN, 1, 1, 1, 1, 1,UN,UN,UN,UN,UN,UN, 1, 1,
/*A0*/  UN,UN,UN, 2,UN,UN,UN, 1,UN,UN,UN,UN,UN,UN, 2, 1,
/*B0*/  UN,UN,UN, 2,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 2, 2,
/*C0*/  UN,UN,UN, 2,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 2, 2,
/*D0*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2,
/*E0*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2,
/*F0*/   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 2, 2
};

static char  pg91[256] = {  /* P3: PreByte == 91 */
/*--*--* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/*--*--* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - */
/*00*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*10*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*20*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*30*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*40*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*50*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*60*/   4,UN,UN, 4, 4,UN, 4, 4, 4, 4, 4,UN, 4, 4, 4, 4,
/*70*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*80*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*90*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*A0*/  10,UN,UN,UN,UN,UN,UN, 1,UN,UN,UN,UN,UN,UN,UN, 1,
/*B0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*C0*/  UN,UN,UN, 5,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 5, 5,
/*D0*/   4, 4, 4, 5, 4, 5, 4, 4, 4, 4, 4, 4, 5, 6, 5, 5,
/*E0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*F0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN
};

static char  pg92[256] = {  /* P4: PreByte == 92 */
/*--*--* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
/*--*--* -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - */
/*00*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*10*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*20*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*30*/   4,UN,UN, 4, 4,UN, 4, 4, 4, 4, 4,UN, 4, 4, 4, 4,
/*40*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*50*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*60*/   4,UN,UN, 4, 4,UN, 4, 4, 4, 4, 4,UN, 4, 4, 4, 4,
/*70*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*80*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 8,UN,UN,
/*90*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*A0*/  UN,UN,UN,UN,UN,UN,UN, 4,UN,UN,UN,UN, 6,UN,UN, 5,
/*B0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN, 5, 4,UN,UN,
/*C0*/   4, 4, 4, 5, 4, 5, 4, 4, 4, 4, 4, 4, 5, 6, 5, 5,
/*D0*/   4, 4, 4, 5, 4, 5, 4, 4, 4, 4, 4, 4, 5, 6, 5, 5,
/*E0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,
/*F0*/  UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN,UN
};

static char *Page[5] = {
    stm8pg, pg72, pg90, pg91, pg92
};

/*
 * Process a machine op.
 */
VOID
machine(mp)
struct mne *mp;
{
	struct expr e1, e2, e3;
	char *p1, *p2;
	int t1, t2, t3;
	int r1, r2, r3;
	int op, rf;

	clrexpr(&e1);
	clrexpr(&e2);
	clrexpr(&e3);
	op = (int) mp->m_valu;
	rf = mp->m_type;

	switch(rf) {
	case S_2OP:
	case S_2OPSUB:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t1 != S_REG) // todo: implement swapped operand
			aerr();
		altacc(r1);

		switch(t2) {
		case S_IMM:
			if(rf == S_2OPSUB) // Immediate operand invalid for sub and sbc.
				aerr();
			outab(op | 0x00);
			outrb(&e2, R_USGN);
			break;
		case S_DIR:
			outab(op | 0x01);
			outrw(&e2, R_USGN);
			break;
		case S_SPREL:
			outab(op | 0x02);
			if(ls_mode(&e2))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		case S_ZREL:
			outab(op | 0x03);
			outrw(&e2, R_USGN);
			break;
		case S_REG:
			switch(r2) {
			case ZL:
				outab(op | 0x04);
				break;
			case XH:
				outab(op | 0x05);
				break;
			case YL:
				outab(op | 0x06);
				break;
			case YH:
				outab(op | 0x07);
				break;
			default:
				aerr();
			}
			break;
		default:
			aerr();
		}
		break;

	case S_1OP:
	case S_1OPPUSH:
		t1 = addr(&e1);
		r1 = rcode;	

		if(rf == S_1OPPUSH && t1 == S_IMM) { // push #i
			outab(0x90);
			outrb(&e1, R_USGN);
			break;
		}

		switch(t1) {
		case S_DIR:
			outab(op | 0x00);
			outrb(&e1, R_USGN);
			break;
		case S_SPREL:
			outab(op | 0x01);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
			break;
		case S_REG:
			if(r1 == ZH) {
				outab(op | 0x03);
				break;
			}		
			altacc(r1);
			outab(op | 0x02);
			break;

		default:
			aerr();
		}
		break;

	case S_2OPW:
	case S_2OPWSUB:
	case S_2OPWSBC:
	case S_2OPWADD:
	case S_2OPWADC:
		t1 = addr(&e1);
		r1 = rcode;
		if(!comma(rf != S_2OPWSBC && rf != S_2OPWADC)) { // Handle 1-op variants of sbcw and adcw
			if(rf == S_2OPWSBC)
				op = 0xac;
			else if(S_2OPWADC)
				op = 0xa8;
			else
				aerr();
			goto opw;
		}
		t2 = addr(&e2);
		r2 = rcode;

		if(rf == S_2OPWADD && t1 == S_REG && r1 == SP && !ls_mode(&e1)) { // addw sp, #d
			outab(0xea);
			outab(e2.e_addr);
			break;
		}
		else if(rf == S_2OPWADD && t1 == S_REG && t2 == S_REG && r2 == SP) { // addw y, sp
			altaccw(r1);
			outab(0xeb);
			break;
		}

		if(t1 != S_REG) // todo: implement swapped operand
			aerr();
		altaccw(r1);

		switch(t2) {
		case S_IMM:
			if(rf == S_2OPWSUB || rf == S_2OPWSBC) // Immediate operand invalid for subw and sbcw.
				aerr();
			outab(op | 0x00);
			outrw(&e2, R_USGN);
			break;
		case S_DIR:
			outab(op | 0x01);
			outrw(&e2, R_USGN);
			break;
		case S_SPREL:
			outab(op | 0x02);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		case S_REG:
			if(r2 == X) {
				outab(op | 0x03);
				break;
			}
		default:
			aerr();
		}
		break;

	case S_1OPW:
	case S_1OPWPUSH:
		t1 = addr(&e1);
		r1 = rcode;	
opw:
		if(rf == S_1OPWPUSH && t1 == S_IMM) { // pushw #ii
			outab(0xe8);
			outrw(&e1, R_USGN);
			break;
		}

		switch(t1) {
		case S_DIR:
			outab(op | 0x00);
			outrw(&e1, R_USGN);
			break;
		case S_SPREL:
			outab(op | 0x01);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
			break;
		case S_ZREL:
			outab(op | 0x02);
			outrw(&e1, R_USGN);
			break;
		case S_REG:
			altaccw(r1);
			outab(op | 0x03);
			break;
		default:
			aerr();
		}
		break;

	case S_LD:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t1 == S_REG && !(t2 == S_REG && r2 == XL)) {
			altacc(r1);
			switch(t2) {
			case S_IMM:
				outab(op | 0x00);
				outrb(&e2, R_USGN);
				break;
			case S_DIR:
				outab(op | 0x01);
				outrw(&e2, R_USGN);
				break;
			case S_SPREL:
				outab(op | 0x02);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_ZREL:
				outab(op | 0x03);
				outrw(&e2, R_USGN);
				break;
			case S_IX:
				if(r2 == Y)
					outab(op | 0x04);
				else
					aerr();
				break;
			case S_YREL:
				outab(op | 0x05);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_REG:
				if(r2 == XH)
					outab(0x86);
				else if(r2 == YL)
					outab(0x87);
				else if(r2 == YH)
					outab(0x88);
				else if(r2 == ZL)
					outab(0x89);
				else if(r2 == ZH)
					outab(0x8a);
				else
					aerr();
				break;
			default:
				aerr(); // todo
			}
			break;
		}
		else if(t1 == S_REG && t2 == S_REG && r2 == XL) { // Use swapop prefix
			outab(0x9c);
			if(r1 == XH)
				outab(0x86);
			else if(r1 == YL)
				outab(0x87);
			else if(r1 == YH)
				outab(0x88);
			else if(r1 == ZL)
				outab(0x89);
			else if(r1 == ZH)
				outab(0x8a);
			else
				aerr();
			break;
		}
		else if(t2 == S_REG) {
			altacc(r2);
			switch(t1) {
			case S_DIR:
				outab(op | 0x0b);
				outrw(&e1, R_USGN);
				break;
			case S_SPREL:
				outab(op | 0x0c);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			case S_ZREL:
				outab(op | 0x03);
				outrw(&e2, R_USGN);
				break;
			case S_IX:
				if(r1== Y)
					outab(op | 0x04);
				else
					aerr();
				break;
			case S_YREL:
				outab(op | 0x05);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			default:
				aerr();
			}
			break;
		}

		aerr(); // todo
		break;

	case S_LDW:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t1 == S_REG && r1 == X && t2 == S_REG && r2 == Y) {
			outab(op | 0x0b);
			break;
		}
		else if(t1 == S_REG && r1 == Z && t2 == S_REG && r2 == Y) {
			outab(op | 0x0c);
			break;
		}
		else if(t1 == S_REG && r1 == SP && t2 == S_REG) {
			altaccw(r2);
			outab(0x70);
			break;
		}
		else if(t1 == S_REG) {
			altaccw(r1);
			switch(t2) {
			case S_IMM:
				outab(op | 0x00);
				outrw(&e2, R_USGN);
				break;
			case S_DIR:
				outab(op | 0x01);
				outrw(&e2, R_USGN);
				break;
			case S_SPREL:
				outab(op | 0x02);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_ZREL:
				outab(op | 0x03);
				outrw(&e2, R_USGN);
				break;
			case S_YREL:
				outab(op | 0x04);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_IX:
				if(r2 == Y)
					outab(op | 0x05);
				else
					aerr();
				break;
			case S_REG:
				if(r2 == X)
					outab(op | 0x06);
				else
					aerr();
				break;
			default:
				aerr(); // todo
			}
			break;
		}
		else if(t1 == S_IX && r1 == Y && t2 == S_REG && r2 == X) {
			outab(0xcd);
			break;
		}
		else if(t1 == S_YREL && t2 == S_REG && r2 == X) {
			if(!ls_mode(&e2)) {
				outab(0xce);
				outrb(&e2, R_USGN);
			}
			else {
				outab(0xcf);
				outrw(&e2, R_USGN);
			}
			break;
		}
		else if(t2 == S_REG) {
			altaccw(r2);
			switch(t1) {
			case S_DIR:
				outab(op | 0x08);
				outrw(&e1, R_USGN);
				break;
			case S_SPREL:
				outab(op | 0x09);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			case S_ZREL:
				outab(op | 0x0a);
				outrw(&e1, R_USGN);
				break;
			case S_ISPREL:
				outab(0x74);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			default:
				aerr(); // todo
			}
			break;
		}

		aerr(); // todo
		break;

	case S_0OP:
		t1 = addr(&e1);
		r1 = rcode;

		if(t1 == S_REG) {
			altacc(r1);
			outab(op);
		}
		else
			aerr();

		break;

	case S_0OPROT:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if (t1 == S_REG && t2 == S_IMM) {
			altacc(r1);
			outab(op);
			outrb(&e2, R_USGN);
		}
		else
			aerr ();
		break;

	case S_0OPW:
	case S_0OPWSLL:
	case S_0OPWRLC:
	case S_0OPWDEC:
		t1 = addr(&e1);
		r1 = rcode;

		if(rf == S_0OPWSLL && comma(0)) {
			t2 = addr(&e2);
			r2 = rcode;
			goto sex;
		}
		
		if(t1 == S_REG && rf != S_0OPWDEC) {
			altaccw(r1);
			outab(op);
		}
		else if((rf == S_0OPWRLC || rf == S_0OPWDEC) && t1 == S_SPREL) {
			outab(op + 0x04);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
			break;
		}
		else
			aerr();
		break;

	case S_0OPWCP:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t1 == S_REG && t2 == S_IMM) {
			altaccw(r1);
			outab(op);
			outrw(&e1, R_USGN);
		}
		else
			aerr();
		break;
		
	case S_0OPWSEX:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
sex:
		if(t1 != S_REG || t2 != S_REG || r2 != XL)
			aerr();

		altaccw(r1);
		outab(op);
		break;	

	case S_BIT:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		comma(1);
		t3 = addr(&e3);
		int v3 = (int) e3.e_addr;

		if(t1 == S_REG && t2 == S_DIR && t3 == S_IMM && (v3 <= 7)) {
			altacc(r1);
			outab(op | v3);
			outrw(&e2, R_USGN);
		}
		else
			aerr();
		break;

	case S_JR:
		expr(&e1, 0);
		outab(op);
		if(mchpcr(&e1)) {
			int v1 = (int)(e1.e_addr - dot.s_addr - 1);
			if((v1 < -128) || (v1 > 127))
				aerr();
			outab(v1);
		} else {
			outrb(&e1, R_PCR);
		}
		if(e1.e_mode != S_USER) {
			rerr();
		}
		break;

	case S_JP:
		t1 = addr(&e1);
		r1 = rcode;

		if(t1 == S_REG && r1 == Y)
			outab(op | 0x01);
		else if(t1 == S_IMM) {
			outab(op);
			outrw(&e1, R_USGN);
		}
		else
			aerr();

		break;

	case S_RET:
		outab(op);
		break;
	
	default:
		opcycles = OPCY_ERR;
		err('o');
		break;
	}

	if (opcycles == OPCY_NONE) {
		opcycles = stm8pg[cb[0] & 0xFF];
		if ((opcycles & OPCY_NONE) && (opcycles & OPCY_MASK)) {
			opcycles = Page[opcycles & OPCY_MASK][cb[1] & 0xFF];
		}
	}
}

/*
 * Disable Opcode Cycles with aerr()
 */
VOID
opcy_aerr()
{
	opcycles = OPCY_SKP;
	aerr();
}

/*
 * Select the long or short addressing mode
 * based upon the expression type and value.
 * Return 1 for 16-bit offset, 0 for 8-bit offset.
 */
int
ls_mode(e)
struct expr *e;
{
	int flag, v;

	v = (int) e->e_addr;
	/*
	 * 1) area based arguments (e_base.e_ap != 0) use longer mode
	 * 2) constant arguments (e_base.e_ap == 0) use
	 * 	shorter mode if (arg & ~0xFF) == 0
	 *	longer  mode if (arg & ~0xFF) != 0
	 */
	if (pass == 0) {
		;
	} else
	if (e->e_base.e_ap) {
		;
	} else
	if (pass == 1) {
		if (e->e_addr >= dot.s_addr) {
			e->e_addr -= fuzz;
		}
		flag = (v & ~0xFF) ? 1 : 0;
		return(setbit(flag) ? 1 : 0);
	} else {
		return(getbit() ? 1 : 0);
	}
	return(1);
}

/*
 * Generate an 'a' error if the absolute
 * value is not a valid unsigned or signed value.
 */
VOID
valu_aerr(e, n)
struct expr *e;
int n;
{
	int v;

	if (is_abs(e)) {
		v = e->e_addr;
		switch(n) {
		default:
#ifdef	LONGINT
		case 1:	if ((v & ~0x000000FFl) && ((v & ~0x000000FFl) != ~0x000000FFl)) aerr();	break;
		case 2:	if ((v & ~0x0000FFFFl) && ((v & ~0x0000FFFFl) != ~0x0000FFFFl)) aerr();	break;
		case 3:	if ((v & ~0x00FFFFFFl) && ((v & ~0x00FFFFFFl) != ~0x00FFFFFFl)) aerr();	break;
		case 4:	if ((v & ~0xFFFFFFFFl) && ((v & ~0xFFFFFFFFl) != ~0xFFFFFFFFl)) aerr();	break;
#else
		case 1:	if ((v & ~0x000000FF) && ((v & ~0x000000FF) != ~0x000000FF)) aerr();	break;
		case 2:	if ((v & ~0x0000FFFF) && ((v & ~0x0000FFFF) != ~0x0000FFFF)) aerr();	break;
		case 3:	if ((v & ~0x00FFFFFF) && ((v & ~0x00FFFFFF) != ~0x00FFFFFF)) aerr();	break;
		case 4:	if ((v & ~0xFFFFFFFF) && ((v & ~0xFFFFFFFF) != ~0xFFFFFFFF)) aerr();	break;
#endif
		}
	}
}

/*
 * Branch/Jump PCR Mode Check
 */
int
mchpcr(esp)
struct expr *esp;
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
 * Machine specific initialization.
 */
VOID
minit()
{
	/*
	 * 24-Bit Machine
	 */
	exprmasks(3);

	/*
	 * Byte Order
	 */
	hilo = 1;

	/*
	 * Reset Bit Table
	 */
	bp = bb;
	bm = 1;
}

/*
 * Store `b' in the next slot of the bit table.
 * If no room, force the longer form of the offset.
 */
int
setbit(b)
int b;
{
	if (bp >= &bb[NB])
		return(1);
	if (b)
		*bp |= bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
	return(b);
}

/*
 * Get the next bit from the bit table.
 * If none left, return a `1'.
 * This will force the longer form of the offset.
 */
int
getbit()
{
	int f;

	if (bp >= &bb[NB])
		return (1);
	f = *bp & bm;
	bm <<= 1;
	if (bm == 0) {
		bm = 1;
		++bp;
	}
	return (f);
}

/* Emit prefix byte for alternative accumulator */
extern
void altacc(int reg)
{
	if(reg != XL) {
		if(reg == XH)
			outab(0x9d);
		else if(reg == YL)
			outab(0x9e);
		else if(reg == ZL)
			outab(0x9f);
		else
			aerr();
	}
}

extern
void altaccw(int reg)
{
	if(reg != Y) {
		if(reg == X)
			outab(0x9e);
		else if(reg == Z)
			outab(0x9f);
		else
			aerr();
	}
}

