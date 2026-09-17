/* asdbg.c */

/*
 *  Copyright (C) 2003-2026  Alan R. Baldwin
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
 *
 * NoICE code extracted from asnoice.c
 * written by:
 *	John L. Hartman	(JLH)
 *	3-Nov-1997
 */

#include <ctype.h>
#include "dbuf_string.h"
#include "asxxxx.h"

/*)Module	asdbg.c
 *
 *	The module asdbg.c contains the functions that
 *	are useful for program debugging.
 *
 * NOTE:
 *	SDCC and NoICE functions know nothing about
 *	'include' files and do not track their inclusion.
 */

/*
 * Debug Structure Printing
 */

#define	EXPR_PRINT	0

#if EXPR_PRINT	/* Conditionally Compile Expression Structure Print Function */

void
prntexpr(struct expr *esp, int flg)
{
	struct sym *sp;
	struct area *ap;
	struct bank *bp;
	char *p;

	sp = NULL;
	ap = NULL;
	bp = NULL;

	if (esp == NULL) {
		fprintf(stdout, "prntexpr - NULL Argument\n");
		return;
	}

	if (flg) fprintf(stdout, "BGN - prntexpr(%d)\n", flg);

	fprintf(stdout, "struct expr {\n");
/**/	fprintf(stdout, "\tchar\te_mode\t\t= 0x%02X", esp->e_mode);
	switch(esp->e_mode) {
	case S_USER:	fprintf(stdout, "\t(USER)\n");	break;
	case S_NEW:	fprintf(stdout, "\t(NEW)\n");	break;
	default:	fprintf(stdout, "\n");		break;
	}
/**/ 	fprintf(stdout, "\tchar\te_flag\t\t= 0x%02X", esp->e_flag);
	if ((esp->e_flag != 0) && (esp->e_base.e_sp != NULL)) {
		fprintf(stdout, "\t(SYMBOL)\n");
	} else
	if ((esp->e_flag == 0) && (esp->e_base.e_ap != NULL)) {
		fprintf(stdout, "\t(Area)\n");
	} else {
		fprintf(stdout, "\n");
	}
/**/	fprintf(stdout, "\ta_uint\te_addr\t\t= 0x%08X\n", esp->e_addr);
/**/	fprintf(stdout, "\tunion\t{\n");
	if (esp->e_flag) {
		fprintf(stdout, "\t    struct sym  *e_sp\t= ");
		if (esp->e_base.e_sp != NULL) {
			fprintf(stdout, "%p\n", (void *) esp->e_base.e_sp);
			fprintf(stdout, "\t\t      [->s_id]\t= %s\n", esp->e_base.e_sp->s_id);
		} else {
			fprintf(stdout, "NULL\n");
		}
	} else {
		fprintf(stdout, "\t    struct area *e_ap\t= ");
		if (esp->e_base.e_ap != NULL) {
			fprintf(stdout, "%p\n", (void *) esp->e_base.e_ap);
			fprintf(stdout, "\t\t      [->a_id]\t= %s\n", esp->e_base.e_ap->a_id);
		} else {
			fprintf(stdout, "NULL\n");
		}
	}
	fprintf(stdout, "\t} e_base\n");
/**/	fprintf(stdout, "\tchar\te_rlcf\t\t= 0x%02X\t(", esp->e_rlcf);
	switch(esp->e_rlcf & 0x03) {
	default:
	case R_1BYTE:	p = "BYTE";	break;
	case R_2BYTE:	p = "WORD";	break;
	case R_3BYTE:	p = "3 BYTES";	break;
	case R_4BYTE:	p = "4 BYTES";	break;
	}
	fprintf(stdout, "%s", p);
	if ((esp->e_rlcf & 0x70) == 0) {
		switch(esp->e_rlcf & 0x0C) {
		default:
		case R_LSB:	p = ", LSB";	break;
		case R_SGND:	p = ", SGND/MBRS";	break;
		case R_USGN:	p = ", USGN/OVRF/MBRU/MBRO";	break;
		case R_MSB:	p = ", MSB";	break;
		}
	} else {
		switch(esp->e_rlcf & 0x70) {
		default:
		case R_NOPAG:	p = ", NOPAGE";	break;
		case R_PAG0:	p = ", PAG0";	break;
		case R_PAGN:	p = ", PAGN";	break;
		case R_PAGX0:	p = ", PAGX0/PAGX";	break;
		case R_PAGX1:	p = ", PAGX1";	break;
		case R_PAGX2:	p = ", PAGX2";	break;
		case R_PAGX3:	p = ", PAGX3";	break;
		case R_PCR:	p = ", PCR";	break;
		case R_PCRN:	p = ", PCRN";	break;
		case R_PCR0:	p = ", PCR0";	break;
		case R_PCR1:	p = ", PCR1";	break;
		case R_PCR2:	p = ", PCR2";	break;
		case R_PCR3:	p = ", PCR3";	break;
		case R_PCR4:	p = ", PCR4";	break;
		case R_PCR0N:	p = ", PCR0N";	break;
		case R_PCR1N:	p = ", PCR1N";	break;
		case R_PCR2N:	p = ", PCR2N";	break;
		case R_PCR3N:	p = ", PCR3N";	break;
		case R_PCR4N:	p = ", PCR4N";	break;
		}
	}
	fprintf(stdout, "%s", p);
	switch(esp->e_rlcf & 0x80) {
	default:
	case R_SYM:	p = ", SYM";	break;
	case R_AREA:	p = ", AREA";	break;
	}
	fprintf(stdout, "%s)\n", p);
/**/	fprintf(stdout, "\tchar\te_inhbt\t\t= 0x%02X\n", esp->e_inhbt);
	fprintf(stdout, "}\n");

	if (esp->e_flag == 1) {
		sp = esp->e_base.e_sp;
		if (sp != NULL)
			if (flg & 1) prntsym(sp);
	} else {
		ap = esp->e_base.e_ap;
		if (ap != NULL) {
			if (flg & 2) prntarea(ap);
			bp = ap->b_bp;
			if (bp != NULL)
				if (flg & 4) prntbank(bp);
		}
	}

	if (flg) fprintf(stdout, "END - prntexpr(%d)\n", flg);
}

void
prntsym(struct sym *sp)
{
	int i;

	if (sp == NULL) {
		fprintf(stdout, "prntsym - NULL Argument\n");
		return;
	}

	fprintf(stdout, "struct sym {\n");
/**/	fprintf(stdout, "\tstruct sym *s_sp\t= ");
	if (sp->s_sp != NULL) {
		fprintf(stdout, "%p\n", (void *) sp->s_sp);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tstruct sym *s_tsym\t= ");
	if (sp->s_tsym != NULL) {
		fprintf(stdout, "%p\n", (void *) sp->s_tsym);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tchar\t*s_id\t\t= ");
	if (sp->s_id != NULL) {
		fprintf(stdout, "%p\n", (void *) sp->s_id);
		fprintf(stdout, "\t       [*s_id]\t\t= %s\n", sp->s_id);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tchar\ts_type\t\t= 0x%02X\t", sp->s_type);
	fprintf(stdout, "(%s)\n", sp->s_type ? "USER" : "NEW");
/**/ 	fprintf(stdout, "\tchar\ts_flag\t\t= 0x%02X", sp->s_flag);
	if (sp->s_flag) {
		i = 0;
		fprintf(stdout, "\t(");
		if (sp->s_flag & S_LCL)	{ fprintf(stdout, "LCL"); i++; }
		if (sp->s_flag & S_GBL)	{ fprintf(stdout, "%s%s", i++ ? ", " : "", "GBL"); }
		if (sp->s_flag & S_ASG)	{ fprintf(stdout, "%s%s", i++ ? ", " : "", "ASG"); }
		if (sp->s_flag & S_MDF)	{ fprintf(stdout, "%s%s", i++ ? ", " : "", "MDF"); }
		fprintf(stdout, ")");
	}
	fprintf(stdout, "\n");
/**/    fprintf(stdout, "\tstruct area *s_area\t= ");
	if (sp->s_area != NULL) {
		fprintf(stdout, "%p\n", (void *) sp->s_area);
		fprintf(stdout, "\t      [->a_id]\t\t= %s\n", sp->s_area->a_id);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tint\ts_ref\t\t= 0x%08X\n", sp->s_ref);
/**/	fprintf(stdout, "\ta_uint\ts_addr\t\t= 0x%08X\n", sp->s_addr);
	fprintf(stdout, "}\n");

	prnttsym(sp);
}

void
prnttsym(struct sym *sp)
{
	struct tsym *tp;

	tp = sp->s_tsym;
	if (tp == NULL) {
		fprintf(stdout, "prnttsym - NULL Argument\n");
		return;
	}

	while (tp) {
	 	fprintf(stdout, "struct tsym {\n");
/**/		fprintf(stdout, "\tstruct tsym *t_lnk\t= ");
			if (tp->t_lnk != NULL) {
				fprintf(stdout, "%p\n", (void *) tp->t_lnk);
			} else {
				fprintf(stdout, "NULL\n");
			}
/**/		fprintf(stdout, "\ta_uint\tt_num\t\t= 0x%08X\n", tp->t_num);
/**/	 	fprintf(stdout, "\tchar\tt_flg\t\t= 0x%02X", tp->t_flg);
			if (tp->t_flg) {
				fprintf(stdout, "\t(");
				if (tp->t_flg & S_MDF)	{ fprintf(stdout, "MDF"); }
				fprintf(stdout, ")");
			}
			fprintf(stdout, "\n");
/**/		fprintf(stdout, "\tstruct area *t_area\t= ");
			if (tp->t_area != NULL) {
				fprintf(stdout, "%p\n", (void *) tp->t_area);
				fprintf(stdout, "\t      [->a_id]\t\t= %s\n", tp->t_area->a_id);
			} else {
				fprintf(stdout, "NULL\n");
			}
/**/		fprintf(stdout, "\ta_uint\tt_addr\t\t= 0x%08X\n", tp->t_addr);
			fprintf(stdout, "}\n");

			tp = tp->t_lnk;
	}
}

void
prntarea(struct area *ap)
{
	int i;

	if (ap == NULL) {
		fprintf(stdout, "prntarea - NULL Argument\n");
		return;
	}

	fprintf(stdout, "struct area {\n");
/**/	fprintf(stdout, "\tstruct area *a_ap\t= ");
	if (ap->a_ap != NULL) {
		fprintf(stdout, "%p\n", (void *) ap->a_ap);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tstruct bank *b_bp\t= ");
	if (ap->b_bp != NULL) {
		fprintf(stdout, "%p\n", (void *) ap->b_bp);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tchar\t*a_id\t\t= ");
	if (ap->a_id != NULL) {
		fprintf(stdout, "%p\n", (void *) ap->a_id);
		fprintf(stdout, "\t       [*a_id]\t\t= %s\n", ap->a_id);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tint\ta_ref\t\t= 0x%08X\n", ap->a_ref);
/**/	fprintf(stdout, "\ta_uint\ta_size\t\t= 0x%08X\n", ap->a_size);
/**/	fprintf(stdout, "\ta_uint\ta_fuzz\t\t= 0x%08X\n", ap->a_fuzz);
/**/ 	fprintf(stdout, "\tchar\ts_flag\t\t= 0x%02X", ap->a_flag);
	if (ap->a_flag & 0xDCC) {
		i = 0;
		fprintf(stdout, "\t(");
		if ((ap->a_flag & A_OVR) == A_CON)	fprintf(stdout, "%s%s", i++ ? ", " : "", "CON");
		if ((ap->a_flag & A_OVR) == A_OVR)	fprintf(stdout, "%s%s", i++ ? ", " : "", "OVR");
		if ((ap->a_flag & A_ABS) == A_REL)	fprintf(stdout, "%s%s", i++ ? ", " : "", "REL");
		if ((ap->a_flag & A_ABS) == A_ABS)	fprintf(stdout, "%s%s", i++ ? ", " : "", "ABS");
		if ((ap->a_flag & A_PAG) == A_NOPAG)	fprintf(stdout, "%s%s", i++ ? ", " : "", "NOPAG");
		if ((ap->a_flag & A_PAG) == A_PAG)	fprintf(stdout, "%s%s", i++ ? ", " : "", "PAG");
		if ((ap->a_flag & A_DSEG) == A_CSEG)	fprintf(stdout, "%s%s", i++ ? ", " : "", "CSEG");
		if ((ap->a_flag & A_DSEG) == A_DSEG)	fprintf(stdout, "%s%s", i++ ? ", " : "", "DSEG");
		if ((ap->a_flag & A_BNK) == A_NOBNK)	fprintf(stdout, "%s%s", i++ ? ", " : "", "NOBNK");
		if ((ap->a_flag & A_BNK) == A_BNK)	fprintf(stdout, "%s%s", i++ ? ", " : "", "BNK");
		if ((ap->a_flag & A_OUT) == A_OUT)	fprintf(stdout, "%s%s", i++ ? ", " : "", "OUT");
		fprintf(stdout, ")");
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "}\n");
}

void
prntbank(struct bank *bp)
{
	int i;

	if (bp == NULL) {
		fprintf(stdout, "prntbank - NULL Argument\n");
		return;
	}

	fprintf(stdout, "struct bank {\n");
/**/	fprintf(stdout, "\tstruct bank *b_bp\t= ");
	if (bp->b_bp != NULL) {
		fprintf(stdout, "%p\n", (void *) bp->b_bp);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tchar\t*b_id\t\t= ");
	if (bp->b_id != NULL) {
		fprintf(stdout, "%p\n", (void *) bp->b_id);
		fprintf(stdout, "\t       [*b_id]\t\t= %s\n", bp->b_id);
	} else {
		fprintf(stdout, "NULL\n");
	}
/**/	fprintf(stdout, "\tchar\t*b_fsfx");
	if (bp->b_fsfx != NULL) {
		fprintf(stdout, "\t\t= %p\n", (void *) bp->b_fsfx);
		fprintf(stdout, "\t       [*b_fsfx]\t\t= %s\n", bp->b_fsfx);
	} else {
		fprintf(stdout, "\t\t= NULL\n");
	}
/**/	fprintf(stdout, "\tint\tb_ref\t\t= 0x%08X\n", bp->b_ref);
/**/	fprintf(stdout, "\ta_uint\tb_base\t\t= 0x%08X\n", bp->b_base);
/**/	fprintf(stdout, "\ta_uint\tb_size\t\t= 0x%08X\n", bp->b_size);
/**/	fprintf(stdout, "\ta_uint\tb_map\t\t= 0x%08X\n", bp->b_map);
/**/ 	fprintf(stdout, "\tint\tb_flag\t\t= 0x%04X", bp->b_flag);
	if (bp->b_flag) {
		i = 0;
		fprintf(stdout, "\t(");
		if (bp->b_flag & B_BASE)	fprintf(stdout, "%s%s", i++ ? ", " : "", "BASE");
		if (bp->b_flag & B_SIZE)	fprintf(stdout, "%s%s", i++ ? ", " : "", "SIZE");
		if (bp->b_flag & B_FSFX)	fprintf(stdout, "%s%s", i++ ? ", " : "", "FSFX");
		if (bp->b_flag & B_MAP)		fprintf(stdout, "%s%s", i++ ? ", " : "", "MAP");
		fprintf(stdout, ")");
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "}\n");
}

#endif

/*)SDCC and NoICE debug functions
 *
 *	1) generate debug symbols for assembler code
 *	   similiar to those generated by the SDCC compiler
 *
 *	2) generate debug symbols for the NoICE
 *	   remote debugger
 *
 *	asdbg.c contains the following functions:
 *		void	DefineSDCC_Line()
 *		void	DefineNoICE_Line()
 *		char *	BaseFileName()
 *
 *	asdbg.c contains the static variables:
 *		int	prevFile
 *		char	baseName[FILSPC]
 *
 *	    used by the BaseFileName function.
 *
 * NOTE:
 *	These functions know nothing about 'include' files
 *	and do not track their inclusion.
 */

/*)Function	void	DefineSDCC_Line(void)
 *
 *	The function DefineSDCC_Line() is called to create
 *	a symbol of the form A$FILE$nnn where FILE is the
 *	Base File Name, the file name without a path or
 *	an extension, and nnn is the line number.
 *
 *	local variables:
 *		struct dbuf_s	dbuf	a temporary to build the symbol
 *		struct sym *	pSym	pointer to the created symbol structure
 *
 *	global variables:
 *		asmf *	asmc	pointer to current assembler file structure
 *		int	srcline		array of source file line numbers
 *		a_uint	laddr		current assembler address
 *		area	dot.s_area	pointer to the current area
 *
 *	functions called:
 *		char *	BaseFileName()	asdbg.c
 *		sym *	lookup()	assym.c
 *		int	sprintf()	c_library
 *
 *	side effects:
 *		A new symbol of the form A$FILE$nnn is created.
 */

#if SDCDB

void
DefineSDCC_Line(void)
{
	struct dbuf_s dbuf;
	struct sym *pSym;

	/*
	 * Symbol is A$FILE$nnn
	 */
	dbuf_init (&dbuf, NCPS);
	dbuf_printf (&dbuf, "A$%s$%u", BaseFileName (asmc, 1), srcline);

	pSym = lookup (dbuf_c_str (&dbuf));
	dbuf_destroy (&dbuf);

	pSym->s_type = S_USER;
	pSym->s_area = dot.s_area;
	pSym->s_addr = laddr;
	pSym->s_flag |= S_GBL;
}
#endif


/*)Function	void	DefineNoICE_Line(void)
 *
 *	The function DefineNoICE_Line() is called to create
 *	a symbol of the form FILE.nnn where FILE is the
 *	Base File Name, the file name without a path or
 *	an extension, and nnn is the line number.
 *
 *	local variables:
 *		struct dbuf_s	dbuf	a temporary to build the symbol
 *		struct sym *	pSym	pointer to the created symbol structure
 *
 *	global variables:
 *		asmf *	asmc	pointer to current assembler file structure
 *		int	srcline		array of source file line numbers
 *		a_uint	laddr		current assembler address
 *		area	dot.s_area	pointer to the current area
 *
 *	functions called:
 *		char *	BaseFileName()	asdbg.c
 *		sym *	lookup()	assym.c
 *		int	sprintf()	c_library
 *
 *	side effects:
 *		A new symbol of the form FILE.nnn is created.
 */

#if NOICE

void
DefineNoICE_Line(void)
{
	struct dbuf_s dbuf;
	struct sym *pSym;

	/*
	 * Symbol is FILE.nnn
	 */
	dbuf_init (&dbuf, NCPS);
	dbuf_printf (&dbuf, "%s.%u", BaseFileName (asmc, 0), srcline);

	pSym = lookup (dbuf_c_str (&dbuf));
	dbuf_destroy (&dbuf);

        pSym->s_type = S_USER;
        pSym->s_area = dot.s_area;
        pSym->s_addr = laddr;
        pSym->s_flag |= S_GBL;
}

#endif


/*)Function	char *	BaseFileName(currFile, spacesToUnderscores)
 *
 *	The function BaseFileName() is called to extract
 *	the file name from a string containing a path,
 *	filename, and extension. If spacesToUnderscores != 0
 *	then spaces are converted to underscores
 *
 *		currFile		is a pointer to the
 *					current assembler object
 *		spacesToUnderscores
 *
 *	local variables:
 *		char	baseName[]	a place to put the file name
 *		int	prevFile	previous assembler object
 *		char *	p1		temporary string pointer
 *		char *	p2		temporary string pointer
 *
 *	global variables:
 *		FILE *	ofp		output file handle
 *
 *	functions called:
 *		int	fprintf()	c_library
 *		char *	strcpy()	c_library
 *		char *	strrchr()	c_library
 *		char *	isspace()	c_library
 *
 *	side effects:
 *		A FILE command of the form ';!FILE string'
 *		is written to the output file.
 */

#if (NOICE || SDCDB)

static	struct	asmf *prevFile = NULL;
static	char	baseName[FILSPC];

char*
BaseFileName(struct asmf * currFile, int spacesToUnderscores)
{
	char *p1, *p2;

	if (currFile != prevFile) {
        	prevFile = currFile;

		strcpy(baseName, afn);
                p1 = baseName;

                /*
	         * Dump a FILE command with full path and extension
		 */
		if (ofp)
			fprintf(ofp, ";!FILE %s\n", p1);

		/*
		 * The name starts after the last
		 * '/' (Unices) or
		 * ':' or '\' (DOS)
		 *
		 * and ends at the last
		 * separator 'FSEPX'
		 */
		if ((p2 = strrchr(p1,  '\\')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1,   '/')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1,   ':')) != NULL)  p1 = ++p2;
		if ((p2 = strrchr(p1, FSEPX)) != NULL) *p2 = 0;
		memmove(baseName, p1, strlen(p1)+1); /* Do not use strcpy(), since baseName and p1 may overlap */

		if (spacesToUnderscores) {
			/* Convert spaces to underscores */
			for (p1 = baseName; *p1; ++p1)
				if (isspace (*p1))
					*p1 = '_';
		}
	}
	return(baseName);
}

#endif

