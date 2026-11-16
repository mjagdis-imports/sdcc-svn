.DELETE_ON_ERROR:

INCDIR          = $(top_srcdir)/device/include

CC 		= ${abs_top_builddir}/bin/sdcc
SDAR 		= ${abs_top_builddir}/bin/sdar

ASFLAGS 	= -plosgff -I$(srcdir)
CPPFLAGS 	= -I$(INCDIR) -I$(INCDIR)/$(PORT) -I.

# --max-allocs-per-node 32768 looks give a good compromise between compilation time nad code quality for now. Since the library is compiled relatively rarely a higher default than for user code makes sense.
CFLAGS 		= $(MODELFLAGS) --std-c23 --max-allocs-per-node 32768

std_lib_dirs	= $(srcdir)/..

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

# common files for MCS-51
COMMON_SDCC_MCS51 = \
  bp \
  bpx \
  crtbank \
  crtcall \
  crtclear \
  crtpagesfr \
  crtstart \
  crtxclear \
  crtxinit \
  crtxpop \
  crtxpush \
  crtxstack \
  _decdptr \
  gptr_cmp \
  _gptrget \
  _gptrgetc \
  _gptrput \
  spx

COMMON_SDCC = \
  _divmixchar \
  _divschar \
  _divuchar \
  _modmixchar \
  _modschar \
  _moduchar \
  _divsint \
  _divuint \
  _modsint \
  _moduint \
  _mulint \
  _divslong \
  _divulong \
  _modslong \
  _modulong \
  _mullong \
  __sdcc_external_startup

COMMON_FLOAT_MCS51 = \
  _fscmp \
  fsget1arg \
  fsget2args \
  fsnormalize \
  _fsreturnval \
  _fsrshift \
  _fsswapargs \
  _logexpf

COMMON_FLOAT = \
  atof \
  __fs2schar \
  __fs2sint \
  __fs2slong \
  __fs2slonglong \
  __fs2uchar \
  __fs2uint \
  __fs2ulong \
  __fs2ulonglong \
  __fsadd \
  __fsdiv \
  __fseq \
  __fslt \
  __fsmul \
  __fsneq \
  __fssub \
  __schar2fs \
  __sint2fs \
  __slong2fs \
  __slonglong2fs \
  __uchar2fs \
  __uint2fs \
  __ulong2fs \
  __ulonglong2fs

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

