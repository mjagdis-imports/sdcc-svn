/*-------------------------------------------------------------------------
   stdlib.h - General utilities (ISO C 11 7.22)

   Copyright (C) 1998, Sandeep Dutta . sandeep.dutta@usa.net
   Copyright (c) 2016-2021, Philipp Klaus Krause, pkk@spth.de

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

#ifndef __STDC_VERSION_STDLIB_H__
#define __STDC_VERSION_STDLIB_H__ 201710L /* TODO: replace by __STDC_VERSION__ when this header becomes C23-compliant! */

#if !defined(__SDCC_pic14)
#if __STDC_VERSION__ >= 199901L
#define __SDCC_LONGLONG
#endif
#endif

#if !defined(__SDCC_mcs51) && !defined(__SDCC_ds390) && !defined(__SDCC_ds400) && !defined(__SDCC_hc08) && !defined(__SDCC_s08) && !defined(__SDCC_mos6502) && !defined(__SDCC_mos65c02) && !defined(__SDCC_pic14) && !defined(__SDCC_pic16) && !defined(__SDCC_pdk13) && !defined(__SDCC_pdk14) && !defined(__SDCC_pdk15)
#define __reentrant
#endif

#ifndef __SIZE_T_DEFINED
#define __SIZE_T_DEFINED
  typedef unsigned int size_t;
#endif

#ifndef __WCHAR_T_DEFINED
#define __WCHAR_T_DEFINED
  typedef unsigned long int wchar_t;
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#define RAND_MAX 32767

#define MB_CUR_MAX 4

#if __STDC_VERSION__ >= 202311L
typedef bool once_flag;
#define ONCE_FLAG_INIT false
void call_once(once_flag *flag, void (*func)(void));
#endif

/* Numeric conversion functions (ISO C11 7.22.1) */
extern float atof (const char *nptr);
extern int atoi (const char *nptr);
extern long int atol (const char *nptr);
extern long int strtol(const char *nptr, char **endptr, int base);
extern unsigned long int strtoul(const char *nptr, char **endptr, int base);
#ifdef __SDCC_LONGLONG
extern long long int atoll (const char *nptr);
extern long long int strtoll(const char *nptr, char **endptr, int base);
extern unsigned long long int strtoull(const char *nptr, char **endptr, int base);
#endif

/* SDCC extensions */
extern void __uitoa(unsigned int, char *, unsigned char);
extern void __itoa(int, char *, unsigned char);
extern void __ultoa(unsigned long, char *, unsigned char);
extern void __ltoa(long, char *, unsigned char);

/* Pseudo-random sequence generation functions (ISO C11 7.22.2) */
int rand(void);
void srand(unsigned int seed);

/* Memory management functions (ISO C11 7.22.3) */
#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400)
void __xdata *calloc (size_t nmemb, size_t size);
void __xdata *malloc (size_t size);
void __xdata *realloc (void *ptr, size_t size);
#else
void *calloc (size_t nmemb, size_t size);
void *malloc (size_t size);
void *realloc (void *ptr, size_t size);
#endif
#if __STDC_VERSION__ >= 201112L
inline void *aligned_alloc(size_t alignment, size_t size)
{
  (void)alignment;
  return malloc(size);
}
#endif
extern void free (void * ptr);

#if __STDC_VERSION__ >= 202311L
inline void free_sized(void *ptr, size_t size)
{
  (void)size;
  free (ptr);
}

inline void free_aligned_sized(void *ptr, size_t alignment, size_t size)
{
  (void)alignment;
  (void)size;
  free (ptr);
}
#endif

/* Searching and sorting utilities (ISO C11 7.22.5) */
extern void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *) __reentrant);
extern void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *) __reentrant);

/* Integer arithmetic functions (ISO C11 7.22.6) */
#if defined(__SDCC_z80) || defined(__SDCC_z180) || defined(__SDCC_r2k) || defined(__SDCC_r2ka) || defined(__SDCC_r3ka) || defined(__SDCC_tlcs90) || defined (__SDCC_ez80_z80) || defined (__SDCC_z80n) || defined(__SDCC_r800)
int abs(int j) __preserves_regs(b, c, iyl, iyh);
#else
int abs(int j);
#endif
long int labs(long int j);
#ifdef __SDCC_LONGLONG
long long int llabs(long long int j);
#endif

typedef struct
{
	int quot;
	int rem;
} div_t;
typedef struct
{
	long int quot;
	long int rem;
} ldiv_t;
typedef struct
{
	long long int quot;
	long long int rem;
} lldiv_t;
#if !defined(__SDCC_ds390) && !defined(__SDCC_ds390) && !defined(__SDCC_hc08) && !defined(__SDCC_s08) // struct return not yet supported
div_t div(int numer, int denom);
ldiv_t ldiv(long int numer, long int denom);
#if !defined(__SDCC_mos6502) && !defined(__SDCC_mos65c02) // size of struct return is limited to <= 8 
lldiv_t lldiv(long long int numer, long long int denom);
#endif
#endif

/* C99 Multibyte/wide character conversion functions (ISO C11 7.22.7) */
#if __STDC_VERSION__ >= 199901L
int mblen(const char *s, size_t n);
int mbtowc(wchar_t *restrict pwc, const char *restrict s, size_t n);
int wctomb(char *s, wchar_t wc);
#endif

/* C99 Multibyte/wide string conversion functions (ISO C 11 7.22.8) */
#if __STDC_VERSION__ >= 199901L
size_t mbstowcs(wchar_t *restrict pwcs, const char *restrict s, size_t n);
size_t wcstombs(char *restrict s, const wchar_t *restrict pwcs, size_t n);
#endif

/* C2X Alignment of memory */
#if __STDC_VERSION__ >= 202311L
size_t memalignment(const void *p);
#endif

/* Bounds-checking interfaces from annex K of the C11 standard. */
#if defined (__STDC_WANT_LIB_EXT1__) && __STDC_WANT_LIB_EXT1__

#ifndef __RSIZE_T_DEFINED
#define __RSIZE_T_DEFINED
typedef size_t rsize_t;
#endif

#ifndef __ERRNO_T_DEFINED
#define __ERRNO_T_DEFINED
typedef int errno_t;
#endif

typedef void (*constraint_handler_t)(const char *restrict msg, void *restrict ptr, errno_t error);

#endif

#endif

