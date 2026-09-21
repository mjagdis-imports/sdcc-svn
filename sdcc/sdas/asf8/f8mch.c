/* f8mch.c */

/*
 *  Copyright (C) 2012-2026  Alan R. Baldwin
 *  Copyright (C) 2022-2026  Philipp K. Krause
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
 */

#include "asxxxx.h"
#include "f8.h"

char	*cpu	= "f8 / f8l";
char	*dsft	= "asm";

int	mchtyp;

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
 * F8 Opcode Cycle Pages
 */

static char  f8pg[256] = {
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
    f8pg, pg72, pg90, pg91, pg92
};

/*
 * Process a machine op.
 */
void
machine(struct mne *mp)
{
	struct expr e1, e2, e3;
	int t1, t2, t3;
	int r1, r2, r3;
	int op, rf;

	clrexpr(&e1);
	clrexpr(&e2);
	clrexpr(&e3);
	op = (int) mp->m_valu;
	rf = mp->m_type;

	switch(rf) {
	case S_CPU:
		mchtyp = op;
		break;
	case S_2OP:
	case S_2OPADC:
	case S_2OPADD:
	case S_2OPAND:
	case S_2OPSBC:
	case S_2OPSUB:
	case S_2OPXOR:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t2 == S_REG && r2 == XL) { // swapped operand.
			int tr = r1;
			r1 = r2;
			r2 = tr;
			int tt = t1;
			t1 = t2;
			t2 = tt;
			struct expr te = e1;
			e1 = e2;
			e2 = te;
			outab(OPCODE_SWAPOP);
		}
		else if(t1 != S_REG)
			aerr();
		altacc(r1);

		switch(t2) {
		case S_IMM:
			if(rf == S_2OPSBC || rf == S_2OPSUB) // Immediate operand invalid for sub and sbc.
				aerr();
			if (rf == S_2OPAND)
				outab(0x79);
			else if (rf == S_2OPXOR)
				outab(0x71);
			else
				outab(op | 0x01);
			outrb(&e2, R_NORM);
			break;
		case S_DIR:
			outab(op | 0x00 | (rf == S_2OPSUB));
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
				if (rf == S_2OPADD)
					outab(0x16);
				else
					outab(op | 0x05);
				break;
			case XH:
				if (rf == S_2OPAND)
					outab(0x7c);
				else if (rf == S_2OPXOR)
					outab(0x74);
				else
					outab(op | 0x04);
				break;
			case YL:
				if (rf == S_2OPADC)
					outab(0x76);
				else if (rf == S_2OPADD)
					outab(0x15);
				else if (rf == S_2OPAND)
					outab(0x1e);
				else
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
	case S_1OPINC:
	case S_1OPPUSH:
		t1 = addr(&e1);
		r1 = rcode;	

		if(rf == S_1OPPUSH && t1 == S_IMM) { // push #i
			outab(0xfc);
			outrb(&e1, R_NORM);
			break;
		}

		switch(t1) {
		case S_DIR:
			outab(op + 0x01);
			outrw(&e1, R_USGN);
			break;
		case S_YREL:
			if (mchtyp == X_F8L)
				aerr();
		case S_SPREL:
			if (rf == S_1OPINC  && t1 == S_SPREL)
				outab (0x39);
			else
				outab(op + (t1 == S_SPREL ? 0x00 : 0x03));
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
			break;
		case S_REG:		
			altacc(r1);
			outab(op + 0x02);
			break;

		default:
			aerr();
		}
		break;

	case S_2OPW:
	case S_2OPWADC:
	case S_2OPWADD:
	case S_2OPWSBC:
	case S_2OPWSUB:
	case S_2OPWXOR:
		t1 = addr(&e1);
		r1 = rcode;
		if(!comma(rf != S_2OPWSBC && rf != S_2OPWADC)) { // Handle 1-op variants of sbcw and adcw
			if (rf == S_2OPWSBC && t2 == S_ZREL) {
				outab(0xa6);
				outrw(&e1, R_USGN);
				break;
			}
			else if(rf == S_2OPWSBC)
				op = 0xac;
			else if(rf == S_2OPWADC)
				op = 0xa8;
			else
				aerr();
			goto opw;
		}
		t2 = addr(&e2);
		r2 = rcode;

		if(rf == S_2OPWADD && t1 == S_REG && r1 == SP && t2 == S_IMM && !d_mode(&e2)) { // addw sp, #d
			outab(0x8a);
			outab(e2.e_addr);
			break;
		}
		else if(rf == S_2OPWADD && t1 == S_REG && t2 == S_IMM && !d_mode(&e2)) { // addw y, #d
			altaccw(r1);
			outab(0x8b);
			outab(e2.e_addr);
			break;
		}

		if (mchtyp == X_F8L)
			aerr();
		
		if(t2 == S_REG && r1 == X && r2 == Y || t1 == S_SPREL || t1 == S_DIR) { // swapped operands.
			int tr = r1;
			r1 = r2;
			r2 = tr;
			int tt = t1;
			t1 = t2;
			t2 = tt;
			struct expr te = e1;
			e1 = e2;
			e2 = te;
			outab(OPCODE_SWAPOP);
		}
		else if(t1 != S_REG)
			aerr();

		switch(t2) {
		case S_IMM:
			altaccw(r1);
			if(rf == S_2OPWSUB || rf == S_2OPWSBC) // Immediate operand invalid for subw and sbcw.
				aerr();
			if (rf == S_2OPWADD)
				outab (0x50);
			else
				outab(op | 0x01);
			outrw(&e2, R_USGN);
			break;
		case S_DIR:
			altaccw(r1);
			if (rf == S_2OPWXOR)
				outab (0xa1);
			else
				outab(op | 0x00);
			outrw(&e2, R_USGN);
			break;
		case S_SPREL:
			altaccw(r1);
			outab(op | 0x02);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		case S_REG:
			altaccw2(r1, r2);
			outab(op | 0x03);
			break;
		default:
			aerr();
		}
		break;

	case S_1OPW:
	case S_1OPWINC:
	case S_1OPWPUSH:
	case S_1OPWTST:
		t1 = addr(&e1);
		r1 = rcode;	
opw:
		if(rf == S_1OPWPUSH && t1 == S_IMM) { // pushw #ii
			outab(0x89);
			outrw(&e1, R_USGN);
			break;
		}

		switch(t1) {
		case S_DIR:
			outab(op | 0x01);
			outrw(&e1, R_USGN);
			break;
		case S_SPREL:
			outab(op | 0x00);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
			break;
		case S_ZREL:
			if (rf == S_1OPWINC)
				outab (0xae);
			else if (rf == S_1OPWTST)
				outab (0x86);
			else
				outab(op | 0x02);
			outrw(&e1, R_USGN);
			break;
		case S_REG:
			altaccw(r1);
			if (rf == S_1OPWTST)
				outab (0x87);
			else
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
				outab(0xe9);
				outrb(&e2, R_NORM);
				break;
			case S_DIR:
				outab(0xc0);
				outrw(&e2, R_USGN);
				break;
			case S_SPREL:
				outab(0xc2);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_ZREL:
				outab(0xc3);
				outrw(&e2, R_USGN);
				break;
			case S_IX:
				if(r2 == Y)
					outab(0xc5);
				else if (r1 == ZL && r2 == X || r1 == YL && r2 == Z)
					outab(0x84);
				else
					aerr();
				break;
			case S_YREL:
				if (mchtyp == X_F8L)
					aerr();
				outab(0xc4);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_REG:
				if(r2 == XH)
					outab(0xc6);
				else if(r2 == YL)
					outab(0xc7);
				else if(r2 == YH)
					outab(0xc9);
				else if(r2 == ZL)
					outab(0xc8);
				else if(r2 == ZH)
					outab(0xca);
				else
					aerr();
				break;
			default:
				aerr(); // todo
			}
			break;
		}
		else if(t1 == S_REG && t2 == S_REG && r2 == XL) { // Use swapop prefix
			outab(OPCODE_SWAPOP);
			if(r1 == XH)
				outab(0xc6);
			else if(r1 == YL)
				outab(0xc7);
			else if(r1 == YH)
				outab(0xc9);
			else if(r1 == ZL)
				outab(0xc8);
			else if(r1 == ZH)
				outab(0xca);
			else
				aerr();
			break;
		}
		else if(t2 == S_REG) {
			altacc(r2);
			switch(t1) {
			case S_DIR:
				outab(0xcb);
				outrw(&e1, R_USGN);
				break;
			case S_SPREL:
				outab(0xcd);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			case S_ZREL:
				outab(0xcc);
				outrw(&e1, R_USGN);
				break;
			case S_IX:
				if(r1 == Y && (r2 == XL || r2 == XH || r2 == ZH) || r1 == Z && (r2 == YL || r2 == YH) || r1 == X && r2 == ZL)
					outab(0xce);
				else
					aerr();
				break;
			case S_YREL:
				if (mchtyp == X_F8L)
					aerr();
				outab(0xc1);
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

	case S_LDI:
		if (mchtyp == X_F8L)
			aerr();

		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if (t1 == S_YREL && r2 == Z)
		{
			outab(op);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e1, R_USGN);
		}
		else
			aerr();
		break;

	case S_LDW:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if(t1 == S_REG && r1 == X && t2 == S_REG && r2 == Y) {
			outab(0xbb);
			break;
		}
		else if(t1 == S_REG && r1 == Z && t2 == S_REG && r2 == Y) {
			outab(0x85);
			break;
		}
		else if(t1 == S_REG && r1 == X && t2 == S_REG && r2 == Z) {
			outab(OPCODE_ALTACC2);
			outab(0xbb);
			break;
		}
		else if(t1 == S_REG && r1 == Z && t2 == S_REG && r2 == X) {
			outab(OPCODE_ALTACC2);
			outab(0xb6);
			break;
		}
		else if(t1 == S_REG && r1 == SP && t2 == S_REG && r2 == Y) { // ldw sp, y
			outab(OPCODE_SWAPOP);
			outab(0x31);
			break;
		}
		else if (t1 == S_REG && t2 == S_REG && r2 == SP) { // ldw y, sp
			altaccw(r1);
			outab(0x31);
			break;
		}
		else if(t1 == S_REG && t2 == S_IX && (r1 == X && r2 == Y || r1 == Z && r2 == Y || r1 == Z && r2 == X || r1 == Y && r2 == Z)) { // ldw x, (y)
			if (r1 == Z && r2 == Y)
				outab (OPCODE_ALTACC5);
			else if (r1 == Z && r2 == X)
				outab (OPCODE_ALTACC3);
			else if (r1 == Y && r2 == Z)
				outab (OPCODE_ALTACC2);
			outab (0x66);
			break;
		}
		else if(t1 == S_REG) {
			altaccw(r1);
			switch(t2) {
			case S_IMM:
				if (!d_mode(&e2)) { // ldw y, #d
					outab(0xb7);
					outrb(&e2, R_USGN);
					break;
				}
				outab(0xb1);
				outrw(&e2, R_USGN);
				break;
			case S_DIR:
				outab(0xb0);
				outrw(&e2, R_USGN);
				break;
			case S_SPREL:
				outab(0xb2);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_ZREL:
				outab(0xb3);
				outrw(&e2, R_USGN);
				break;
			case S_YREL:
				if (mchtyp == X_F8L)
					aerr();
				outab(0xb5);
				if(ls_mode(&e2))
					aerr();
				else
					outrb(&e2, R_USGN);
				break;
			case S_IX:
				if((r1 == Y || r1 == Z || r1 == X) && r1 == r2)
					outab(0xb4);
				else
					aerr();
				break;
			case S_REG:
				if(r2 == X)
					outab(0xb6);
				else if (r2 == Z)
					outab(0x6e);
				else
					aerr();
				break;
			default:
				aerr(); // todo
			}
			break;
		}
		else if(t1 == S_IX && t2 == S_REG && (r1 == Y && r2 == X || r1 == Z && r2 == Y || r1 == X && r2 == Z || r1 == Y && r2 == Z)) {
			altaccw2(r1, r2);
			outab(0x84);
			break;
		}
		else if(t1 == S_YREL && t2 == S_REG && (r2 == X || r2 == Z)) {
			if (mchtyp == X_F8L)
					aerr();
			if (r2 == Z)
				outab (OPCODE_ALTACC3);
			if(!ls_mode(&e2)) {
				outab(0x88);
				outrb(&e1, R_USGN);
			}
			else
				aerr();
			break;
		}
		else if(t2 == S_REG) {
			altaccw(r2);
			switch(t1) {
			case S_DIR:
				outab(0xed);
				outrw(&e1, R_USGN);
				break;
			case S_SPREL:
				outab(0xb8);
				if(ls_mode(&e1))
					aerr();
				else
					outrb(&e1, R_USGN);
				break;
			case S_ZREL:
				outab(0xba);
				outrw(&e1, R_USGN);
				break;
			case S_ISPREL:
				outab(0x35);
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

	case S_0OPXCH:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if (t1 == S_REG && r1 == F && t2 == S_SPREL) { // xch f, (n, sp)
			outab(0xbf);
			if(ls_mode(&e2))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		}

		if (t1 != S_REG)
			aerr();
		switch(t2) {
		case S_SPREL:
			if (mchtyp == X_F8L)
				aerr();

			altacc(r1);
			outab(0xa0);
			if(ls_mode(&e1))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		case S_IX:
			altacc(r1);
			if (!((r1 == XL || r1 == XH) && r2 == Y) && !(r1 == YL && r2 == Z) && !(r1 == ZL && r2 == X))
				aerr();
			outab(0xa2);
			break;
		case S_REG:
			if (mchtyp == X_F8L)
				aerr();

			if (r1 == YL && r2 == YH)
				outab(0xa3);
			else if (r1 == XL && r2 == XH) {
				outab(OPCODE_ALTACC3);
				outab(0xa3);
			}
			else if (r1 == ZL && r2 == ZH) {
				outab(OPCODE_ALTACC2);
				outab(0xa3);
			}
			else
				aerr();
			break;
		default:
			aerr();
		}
		break;

	case S_0OPROT:
		if (mchtyp == X_F8L)
			aerr();

		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;

		if (t1 == S_REG && t2 == S_IMM) {
			altacc(r1);
			outab(op);
			outrb(&e2, R_NORM);
		}
		else
			aerr ();
		break;

	case S_0OPMAD:
		if (mchtyp == X_F8L)
			aerr();

		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		comma(1);
		t3 = addr(&e3);
		r3 = rcode;

		if(t1 != S_REG || r1 != X)
			aerr();
		if(t3 != S_REG || r3 != YL)
			aerr();

		switch(t2) {
		case S_DIR:
			outab(0x9d);
			outrw(&e2, R_USGN);
			break;
		case S_SPREL:
			outab(0xfa);
			if(ls_mode(&e2))
				aerr();
			else
				outrb(&e2, R_USGN);
			break;
		case S_ZREL:
			outab(0x9e);
			outrw(&e2, R_USGN);
			break;
		case S_IX:
			if(r2 == Z)
				outab(0x9f);
			else
				aerr();
			break;
		default:
			aerr();
		}
		break;

	case S_0OPWXCH:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		if(t1 == S_REG && t2 == S_IX && (r1 == X && r2 == Y || r1 == Y && r2 == Z || r1 == Z && r2 == X)) {
			altaccw(r2);
			outab(0xe5);
		}
		else if(t1 == S_REG && t2 == S_IX && (r1 == Z && r2 == Y)) {
			outab(OPCODE_ALTACC5);
			outab(0xe5);
		}
		else if(t2 != S_SPREL || ls_mode(&e2) || mchtyp == X_F8L)
			aerr();
		else {
			altaccw(r1);
			outab(0xe4);
			outrb(&e2, R_USGN);
		}
		break;


	case S_0OPW:
	case S_0OPWSLL:
	case S_0OPWRLC:
	case S_0OPWRRC:
	case S_0OPWDEC:
		t1 = addr(&e1);
		r1 = rcode;

		if(rf == S_0OPWSLL && comma(0)) {
			t2 = addr(&e2);
			r2 = rcode;
			op = 0x44;
			goto sex;
		}
		
		if(t1 == S_REG && rf != S_0OPWDEC) {
			altaccw(r1);
			outab(op);
		}
		else if((rf == S_0OPWRLC || rf == S_0OPWRRC || rf == S_0OPWDEC) && t1 == S_SPREL) {
			if (rf == S_0OPWRLC)
				outab (0xe7);
			else if (rf == S_0OPWRRC)
				outab (0xe6);
			else if (rf == S_0OPWDEC)
				outab (0x47);
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
			outrw(&e2, R_USGN);
		}
		else
			aerr();
		break;
		
	case S_0OPWSEX:
		if (mchtyp == X_F8L)
			aerr();

		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
sex:
		altaccw2(r1, r2);
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

        case S_JR2:
                outab(OPCODE_SWAPOP);
	case S_JR:
		expr(&e1);
		outab(op);
		if(mchpcr(&e1)) {
			int v1 = (int)(e1.e_addr - dot.s_addr + 1);
			if((v1 < -128) || (v1 > 127))
				aerr();
			outab(v1);
		} else {
			e1.e_addr -= 1;
			outrb(&e1, R_PCR);
		}
		if(e1.e_mode != S_USER) {
			rerr();
		}
		break;

	case S_DNJNZ:
		if (mchtyp == X_F8L)
			aerr();

		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		if (t1 == S_REG && r1 == YH)
			altaccw(Y);
		else if (t1 == S_REG && r1 == XH)
			altaccw(X);
		else if (t1 == S_REG && r1 == ZH)
			altaccw(Z);
		else
			aerr();
		expr(&e2);
		outab(op);
		if(mchpcr(&e2)) {
			int v2 = (int)(e2.e_addr - dot.s_addr + 1);
			if((v2 < -128) || (v2 > 127))
				aerr();
			outab(v2);
		} else {
			e1.e_addr -= 1;
			outrb(&e2, R_PCR);
		}
		if(e2.e_mode != S_USER) {
			rerr();
		}
		break;

        case S_CALL:  
	case S_JP:
		t1 = addr(&e1);
		r1 = rcode;

		if(t1 == S_REG) {
			altaccw(r1);
			outab(rf == S_JP ? 0x4c : 0x4f);
		}
		else if(t1 == S_IMM) {
			outab(op);
			outrw(&e1, R_USGN);
		}
		else
			aerr();

		break;

	case S_RET:
	case S_NOP:
	case S_TRAP:
		outab(op);
		break;

	case S_0OPMSK:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		comma(1);
		t3 = addr(&e3);
		r3 = rcode;

		if(t1 == S_IX && t2 == S_REG && t3 == S_IMM &&
			(r1 == Y && r2 == XL || r1 == Y && r2 == XH || r1 == Z && r2 == YL || r1 == X && r2 == ZL || r1 == Y && r2 == ZH)) {
			altacc(r2);
			outab(op);
			outrb(&e3, R_NORM);
		}
		else
			aerr();

		break;

	case S_0OPCAX:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		comma(1);
		t3 = addr(&e3);
		r3 = rcode;

		if(t1 == S_IX && r1 == Y && t2 == S_REG && r2 == ZL && t3 == S_REG && (r3 == XL || r3 == XH)) {
			altacc(r3);
			outab(op);
		}
		else
			aerr();

		break;

	case S_0OPWCAX:
		t1 = addr(&e1);
		r1 = rcode;
		comma(1);
		t2 = addr(&e2);
		r2 = rcode;
		comma(1);
		t3 = addr(&e3);
		r3 = rcode;

		if(t1 == S_IX && r1 == Y && t2 == S_REG && r2 == Z && t3 == S_REG && r3 == X)
			outab(op);
		else
			aerr();

		break;

	default:
		opcycles = OPCY_ERR;
		err('o');
		break;
	}

	if (opcycles == OPCY_NONE) {
		opcycles = f8pg[cb[0] & 0xFF];
		if ((opcycles & OPCY_NONE) && (opcycles & OPCY_MASK)) {
			opcycles = Page[opcycles & OPCY_MASK][cb[1] & 0xFF];
		}
	}
}

/*
 * Disable Opcode Cycles with aerr()
 */
void
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
ls_mode(struct expr *e)
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
 * Select the long or short immediate mode

 * based upon the expression type and value.
 * Return 1 for 16-bit, 0 for 8-bit.
 */
int
d_mode(struct expr *e)
{
	int flag, v;

	v = (int) e->e_addr;
	/*
	 * 1) area based arguments (e_base.e_ap != 0) use longer mode
	 * 2) constant arguments (e_base.e_ap == 0) use
	 * 	shorter mode if (arg & ~0x7F) == 0 or ~0x7f
	 *	longer  mode otherwise
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
		flag = (((v & ~0x7f) & 0xffff) == (~0x7f & 0xffff) || (v & ~0x7f) == 0) ? 0 : 1;
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
void
valu_aerr(struct expr *e, int n)
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
 * Machine specific initialization.
 */
void
minit()
{
	/*
	 * 24-Bit Machine
	 */
	exprmasks(3);

	/*
	 * Byte Order
	 */
	hilo = 0; // Little-endian

	/*
	 * Reset Bit Table
	 */
	bp = bb;
	bm = 1;

	if (pass == 0) // Default to full f8 instruction set.
                mchtyp = X_F8;
}

/*
 * Store `b' in the next slot of the bit table.
 * If no room, force the longer form of the offset.
 */
int
setbit(int b)
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
			outab(OPCODE_ALTACC1);
		else if(reg == YL)
			outab(OPCODE_ALTACC2);
		else if(reg == ZL)
			outab(OPCODE_ALTACC3);
		else if(reg == YH)
			outab(OPCODE_ALTACC4);
		else if(reg == ZH)
			outab(OPCODE_ALTACC5);
		else
			aerr();
	}
}

extern
void altaccw(int reg)
{
	if(reg != Y) {
		if(reg == X)
			outab(OPCODE_ALTACC3);
		else if(reg == Z)
			outab(OPCODE_ALTACC2);
		else
			aerr();
	}
}

extern
void altaccw2(int reg0, int reg1)
{
	if(reg0 == Y && (reg1 == X || reg1 == XL))
		;
	else if (reg0 == Y && (reg1 == XH))
		outab(OPCODE_ALTACC1);
	else if (reg0 == Z && (reg1 == Y  || reg1 == YL))
		outab(OPCODE_ALTACC2);
	else if (reg0 == X && (reg1 == Z  || reg1 == ZL))
		outab(OPCODE_ALTACC3);
	else if (reg0 == Z && (reg1 == YH))
		outab(OPCODE_ALTACC4);
	else if (reg0 == Y && (reg1 == Z  || reg1 == ZH))
		outab(OPCODE_ALTACC5);
	else
		aerr ();
}

