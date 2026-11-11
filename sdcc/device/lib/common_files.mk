.DELETE_ON_ERROR:

INCDIR          = $(top_srcdir)/device/include

CC 		= ${abs_top_builddir}/bin/sdcc
SDAR 		= ${abs_top_builddir}/bin/sdar

ASFLAGS 	= -plosgff -I$(srcdir)
CPPFLAGS 	= -I$(INCDIR) -I$(INCDIR)/$(PORT) -I.
CFLAGS 		= $(MODELFLAGS) --std-c23

std_lib_dirs	= $(srcdir):$(srcdir)/..

# override PORTDIR defined by super (parent) makefile
override PORTDIR = ../build/$(PORT)

# SDCC LONG LONG support.
# Should be merged into COMMON_SDCC when all ports support long long
COMMON_SDCC_LL = \
  _divslonglong \
  _divulonglong \
  _modslonglong \
  _modulonglong \
  _mullonglong \
  __mulslong2slonglong \
  __mululong2ulonglong

# SDCC LONG LONG shifts support.
# Required for ports that implement long long shifts with support routines
COMMON_SDCC_LL_SHIFT = \
  _slslonglong \
  _slulonglong \
  _srslonglong \
  _srulonglong

# LIBC support for div, ldiv, lldiv, imaxdiv.
# Should be merged into COMMON_LIBC when all ports fully support struct return
COMMON_LIBC_STRUCT_RET = \
  div \
  ldiv \
  lldiv \
  imaxdiv

COMMON_SDCC = \
  _divslong \
  _divulong \
  _modslong \
  _modulong \
  _mullong

COMMON_FLOAT = \
  atof \
  _fs2schar \
  _fs2sint \
  _fs2slong \
  _fs2slonglong \
  _fs2uchar \
  _fs2uint \
  _fs2ulong \
  _fs2ulonglong \
  _fsadd \
  _fsdiv \
  _fseq \
  _fslt \
  _fsmul \
  _fsneq \
  _fssub \
  _schar2fs \
  _sint2fs \
  _slong2fs \
  _slonglong2fs \
  _uchar2fs \
  _uint2fs \
  _ulong2fs \
  _ulonglong2fs

COMMON_LIBC = \
  abs \
  aligned_alloc \
  __assert \
  atoi \
  atol \
  atoll \
  bsearch \
  btowc \
  c16rtomb \
  c16stombs \
  c32rtomb \
  calloc \
  call_once \
  ckd_add \
  ckd_mul \
  ckd_sub \
  errno \
  free \
  gets \
  heap \
  imaxabs \
  isalnum \
  isalpha \
  isblank \
  iscntrl \
  isdigit \
  isgraph \
  islower \
  isprint \
  ispunct \
  isspace \
  isupper \
  isxdigit \
  __itoa \
  __ltoa \
  labs \
  llabs \
  malloc \
  mblen \
  mbrlen \
  mbrtoc16 \
  mbrtoc32 \
  mbrtowc \
  mbsinit \
  mbstoc16s \
  mbstowcs \
  mbtowc \
  memalignment \
  memccpy \
  memchr \
  memcmp \
  memcpy \
  memmove \
  memset \
  memset_explicit \
  printf_large \
  puts \
  qsort \
  rand \
  realloc \
  setjmp \
  sprintf \
  __stdc_bit_ceilull \
  __stdc_bit_width \
  __stdc_count_ones \
  __stdc_first_leading_one \
  __stdc_first_trailing_one \
  __stdc_leading_zeros \
  stdc_load8 \
  stdc_memreverse8 \
  stdc_memreverse8u \
  stdc_store8 \
  __stdc_trailing_ones \
  strcat \
  strchr \
  strcmp \
  strcpy \
  strcspn \
  strdup \
  strlen \
  strncat \
  strncmp \
  strncpy \
  strndup \
  strnlen \
  strpbrk \
  strrchr \
  strsep \
  strspn \
  strstr \
  strtoimax \
  strtok \
  strtol \
  strtoll \
  strtoul \
  strtoull \
  strtoumax \
  strxfrm \
  time \
  tolower \
  toupper \
  uabs \
  ulabs \
  ullabs \
  umaxabs \
  vprintf \
  wcrtomb \
  wcscmp \
  wcslen \
  wcsncmp \
  wcsnlen \
  wcstoimax \
  wcstol \
  wcstoll \
  wcstombs \
  wcstoul \
  wcstoull \
  wcstoumax \
  wctob \
  wctomb

COMMON_LIBM = \
  acosf \
  asincosf \
  asinf \
  atan2f \
  atanf \
  ceilf \
  cosf \
  coshf \
  cotf \
  expf \
  fabsf \
  floorf \
  frexpf \
  isinf \
  isnan \
  ldexpf \
  log10f \
  logf \
  modff \
  powf \
  sincosf \
  sincoshf \
  sinf \
  sinhf \
  sqrtf \
  tancotf \
  tanf \
  tanhf

