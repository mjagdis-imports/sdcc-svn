/* New tests for some existing z80 peephole optimizer rules that were no longer triggered by their original testcases. */

#include <testfwk.h>

unsigned int r(unsigned int i, unsigned int j) // rule 3b
{
	unsigned int t = (i << 1) | (i >> 15);
	t |= 0x00ff;
	return (t + j);
}

void testr(void)
{
	ASSERT(r(0x5555, 0) == 0xaaff);
}

unsigned char f2(unsigned char c) // rule 40b
{
	c |= 3;
	c |= 9;
	return c;
}

void testf2(void)
{
	ASSERT(f2(0) == 11);
}


unsigned char d0, d1;

void f(unsigned long l, unsigned char c0, unsigned char c1)
{
	ASSERT(l == 0 && c0 == d0 && c1 == d1);
}

void g(unsigned long l) // rule 44b
{
	l |= 7;
	if (l < 17)
		return;
	f(0, d0, d1);
}

void testg(void)
{
	g(20);
}

unsigned int fl(unsigned int j) // 45
{
	unsigned int i;
	if (d0 < 7)
		i = 0xa5a5;
	else
		i = 0xa3a3;
	i &= 0x00ff;
	i += j;
	return i;
}

void testfl(void)
{
	d0 = 0;
	ASSERT(fl(1) == 0x00a6);
}

unsigned int fh(unsigned int j) // 46
{
	unsigned int i;
	if (d0 < 7)
		i = 0xa5a5;
	else
		i = 0xa3a3;
	i &= 0xff00;
	i += j;
	return i;
}

void testfh(void)
{
#if !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15) // bug
	d0 = 0;
	ASSERT(fh(256) == 0xa600);
#endif
}

#pragma disable_warning 85
unsigned char o(long l, unsigned char c0, unsigned char c1) // 66a (with --reserve-regs-iy)
{
	return c0 | c1;
}

void testo(void)
{
	ASSERT(o(0, 1, 2) == 3);
}

volatile char vc;

void j(char *p, int i) // 115
{
	vc++;
	p[i]++;
}

void testj(void)
{
	char c = 1;
	j(&c, 0);
	ASSERT(c == 2);
}

unsigned char fc(unsigned char c0) // 125a
{
	unsigned char t0 = d0 + 3;
	unsigned char t1 = c0 & 3;
	if(!(++t1))
		t0 = 0;
	return t0 + t1;
}

void testfc(void)
{
#if !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15) // bug
	d0 = 1;
	ASSERT(fc(15) == 8);
#endif
}

