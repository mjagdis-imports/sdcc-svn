/* ST8.h */

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

/*)BUILD
	$(PROGRAM) =	ASSTM8
	$(INCLUDE) = {
		ASXXXX.H
		ST8.H
	}
	$(FILES) = {
		ST8MCH.C
		ST8ADR.C
		ST8PST.C
		ASMAIN.C
		ASMCRO.C
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
 * Registers
 */
#define XL		 0
#define XH		 1
#define YL		 2
#define YH		 3
#define ZL		 4
#define ZH		 5
#define F		 7
#define SP		 8
#define X		 9
#define Y		10
#define Z		11

/*
 * Addressing Modes
 */
#define	S_IMM		0x00
#define	S_DIR		0x01
#define	S_SPREL		0x02
#define	S_ZREL		0x03
#define	S_REG		0x04
#define	S_IX		0x05
#define	S_IXREL		0x07

/*
 * Instruction types
 */
#define	S_2OP		60
#define	S_1OP		61
#define	S_2OPW		62
#define	S_1OPW		63
#define	S_LD		64
#define	S_LDW		65
#define	S_0OP		66
#define	S_0OPW		67
#define	S_BIT		68
#define	S_JR		69
#define	S_TRAP		70

struct adsym
{
	char	a_str[4];	/* addressing string */
	int	a_val;		/* addressing mode value */
};

extern	struct	adsym	REG[];

extern	int	rcode;

	/* machine dependent functions */

#ifdef	OTHERSYSTEM
	
        /* ST8adr.c */
extern	int		addr(struct expr *esp);
extern	int		addr1(struct expr *esp);
extern	int		addrsl(struct expr *esp);
extern	int		admode(struct adsym *sp);
extern	int		any(int c, char *str);
extern	int		srch(char *str);

	/* ST8mch.c */
extern	VOID		machine(struct mne *mp);
extern	int		mchpcr(struct expr *esp);
extern	VOID		minit(void);
extern	VOID		opcy_aerr(void);
extern	VOID		valu_aerr(struct expr *e, int n);
extern	int		ls_mode(struct expr *e);
extern	int		setbit(int b);
extern	int		getbit(void);

#else

	/* ST8adr.c */
extern	int		addr();
extern	int		addr1();
extern	int		addrsl();
extern	int		admode();
extern	int		any();
extern	int		srch();

	/* ST8mch.c */
extern	VOID		machine();
extern	int		mchpcr();
extern	VOID		minit();
extern	VOID		opcy_aerr();
extern	VOID		valu_aerr();
extern	int		ls_mode();
extern	int		setbit();
extern	int		getbit();

#endif

