#   Copyright (C) 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000,
#   2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012,
#   2014, 2015, 2016, 2019, 2022 Free Software Foundation, Inc.
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

##############################################################################
### WARNING: this file contains embedded tabs.  Do not run untabify on this file.

m4_include(config/acx.m4)
m4_include(config/override.m4)
m4_include(config/proginstall.m4)
m4_include(config/elf.m4)
m4_include(config/ax_cxx_compile_stdcxx.m4)
m4_include([libtool.m4])
m4_include([ltoptions.m4])
m4_include([ltsugar.m4])
m4_include([ltversion.m4])
m4_include([lt~obsolete.m4])
m4_include([config/isl.m4])

AC_INIT(move-if-change)
AC_DISABLE_OPTION_CHECKING

progname=$0
# if PWD already has a value, it is probably wrong.
if test -n "$PWD" ; then PWD=`${PWDCMD-pwd}`; fi

# Export original configure arguments for use by sub-configures.
# Quote arguments with shell meta charatcers.
TOPLEVEL_CONFIGURE_ARGUMENTS=
set -- "$progname" "$@"
for ac_arg
do
  case "$ac_arg" in
  *" "*|*"	"*|*[[\[\]\~\#\$\^\&\*\(\)\{\}\\\|\;\<\>\?\']]*)
    ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"`
    # if the argument is of the form -foo=baz, quote the baz part only
    ac_arg=`echo "'$ac_arg'" | sed "s/^'\([[-a-zA-Z0-9]]*=\)/\\1'/"` ;;
  *) ;;
  esac
  # Add the quoted argument to the list.
  TOPLEVEL_CONFIGURE_ARGUMENTS="$TOPLEVEL_CONFIGURE_ARGUMENTS $ac_arg"
done
if test "$silent" = yes; then
  TOPLEVEL_CONFIGURE_ARGUMENTS="$TOPLEVEL_CONFIGURE_ARGUMENTS --silent"
fi
# Remove the initial space we just introduced and, as these will be
# expanded by make, quote '$'.
TOPLEVEL_CONFIGURE_ARGUMENTS=`echo "x$TOPLEVEL_CONFIGURE_ARGUMENTS" | sed -e 's/^x *//' -e 's,\\$,$$,g'`
AC_SUBST(TOPLEVEL_CONFIGURE_ARGUMENTS)

# Find the build, host, and target systems.
ACX_NONCANONICAL_BUILD
ACX_NONCANONICAL_HOST
ACX_NONCANONICAL_TARGET
host_subdir=.
AC_SUBST(host_subdir)

# not here
AC_SUBST(stage1_ldflags)
AC_SUBST(poststage1_libs)
AC_SUBST(poststage1_ldflags)
AC_SUBST(isllibs)
AC_SUBST(islinc)

# By default, C and C++ are the only stage 1 languages.
stage1_languages=,c,

dnl Autoconf 2.5x and later will set a default program prefix if
dnl --target was used, even if it was the same as --host.  Disable
dnl that behavior.  This must be done before AC_CANONICAL_SYSTEM
dnl to take effect.
test "$host_noncanonical" = "$target_noncanonical" &&
  test "$program_prefix$program_suffix$program_transform_name" = \
    NONENONEs,x,x, &&
  program_transform_name=s,y,y,

AC_CANONICAL_SYSTEM
AC_ARG_PROGRAM

m4_pattern_allow([^AS_FOR_TARGET$])dnl
m4_pattern_allow([^AS_FOR_BUILD$])dnl

# Get 'install' or 'install-sh' and its variants.
AC_PROG_INSTALL
ACX_PROG_LN
AC_PROG_LN_S
AC_PROG_SED
AC_PROG_AWK

srcpwd=`cd ${srcdir} ; ${PWDCMD-pwd}`

# We pass INSTALL explicitly to sub-makes.  Make sure that it is not
# a relative path.
if test "$INSTALL" = "${srcdir}/install-sh -c"; then
  INSTALL="${srcpwd}/install-sh -c"
fi

# Set srcdir to "." if that's what it is.
# This is important for multilib support.
pwd=`${PWDCMD-pwd}`
if test "${pwd}" = "${srcpwd}" ; then
  srcdir=.
fi

topsrcdir=$srcpwd

extra_host_args=

# Work in distributions that contain no compiler tools, like Autoconf.
host_makefile_frag=/dev/null
if test -d ${srcdir}/config ; then
case "${host}" in
  i[[3456789]]86-*-msdosdjgpp*)
    host_makefile_frag="config/mh-djgpp"
    ;;
  *-cygwin*)
    ACX_CHECK_CYGWIN_CAT_WORKS
    host_makefile_frag="config/mh-cygwin"
    ;;
  *-mingw*)
    host_makefile_frag="config/mh-mingw"
    ;;
  alpha*-linux*)
    host_makefile_frag="config/mh-alpha-linux"
    ;;
  hppa*-hp-hpux10*)
    host_makefile_frag="config/mh-pa-hpux10"
    ;;
  hppa*-hp-hpux*)
    host_makefile_frag="config/mh-pa"
    ;;
  hppa*-*)	
    host_makefile_frag="config/mh-pa"
    ;;
  i?86-*-darwin[[89]]* | i?86-*-darwin1[[0-7]]* | powerpc*-*-darwin*)
    host_makefile_frag="config/mh-darwin"
    ;;
  powerpc-*-aix*)
    host_makefile_frag="config/mh-ppc-aix"
    ;;
  rs6000-*-aix*)
    host_makefile_frag="config/mh-ppc-aix"
    ;;
esac
fi

if test "${build}" != "${host}" ; then
  AR_FOR_BUILD=${AR_FOR_BUILD-ar}
  AS_FOR_BUILD=${AS_FOR_BUILD-as}
  CC_FOR_BUILD=${CC_FOR_BUILD-gcc}
  CPP_FOR_BUILD="${CPP_FOR_BUILD-\$(CC_FOR_BUILD) -E}"
  CXX_FOR_BUILD=${CXX_FOR_BUILD-g++}
  DSYMUTIL_FOR_BUILD=${DSYMUTIL_FOR_BUILD-dsymutil}
  GFORTRAN_FOR_BUILD=${GFORTRAN_FOR_BUILD-gfortran}
  GOC_FOR_BUILD=${GOC_FOR_BUILD-gccgo}
  GDC_FOR_BUILD=${GDC_FOR_BUILD-gdc}
  DLLTOOL_FOR_BUILD=${DLLTOOL_FOR_BUILD-dlltool}
  LD_FOR_BUILD=${LD_FOR_BUILD-ld}
  NM_FOR_BUILD=${NM_FOR_BUILD-nm}
  RANLIB_FOR_BUILD=${RANLIB_FOR_BUILD-ranlib}
  WINDRES_FOR_BUILD=${WINDRES_FOR_BUILD-windres}
  WINDMC_FOR_BUILD=${WINDMC_FOR_BUILD-windmc}
else
  AR_FOR_BUILD="\$(AR)"
  AS_FOR_BUILD="\$(AS)"
  CC_FOR_BUILD="\$(CC)"
  CXX_FOR_BUILD="\$(CXX)"
  DSYMUTIL_FOR_BUILD="\$(DSYMUTIL)"
  GFORTRAN_FOR_BUILD="\$(GFORTRAN)"
  GOC_FOR_BUILD="\$(GOC)"
  GDC_FOR_BUILD="\$(GDC)"
  DLLTOOL_FOR_BUILD="\$(DLLTOOL)"
  LD_FOR_BUILD="\$(LD)"
  NM_FOR_BUILD="\$(NM)"
  RANLIB_FOR_BUILD="\$(RANLIB)"
  WINDRES_FOR_BUILD="\$(WINDRES)"
  WINDMC_FOR_BUILD="\$(WINDMC)"
fi

AC_PROG_CC_C99
AC_PROG_CXX

# We must set the default linker to the linker used by gcc for the correct
# operation of libtool.  If LD is not defined and we are using gcc, try to
# set the LD default to the ld used by gcc.
if test -z "$LD"; then
  if test "$GCC" = yes; then
    case $build in
    *-*-mingw*)
      gcc_prog_ld=`$CC -print-prog-name=ld 2>&1 | tr -d '\015'` ;;
    *)
      gcc_prog_ld=`$CC -print-prog-name=ld 2>&1` ;;
    esac
    case $gcc_prog_ld in
    # Accept absolute paths.
    [[\\/]* | [A-Za-z]:[\\/]*)]
      LD="$gcc_prog_ld" ;;
    esac
  fi
fi

# Check whether -static-libstdc++ -static-libgcc is supported.
have_static_libs=no
if test "$GCC" = yes; then
  saved_LDFLAGS="$LDFLAGS"

  LDFLAGS="$LDFLAGS -static-libstdc++ -static-libgcc"
  AC_MSG_CHECKING([whether g++ accepts -static-libstdc++ -static-libgcc])
  AC_LANG_PUSH(C++)
  AC_LINK_IFELSE([AC_LANG_SOURCE([
#if (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 5)
#error -static-libstdc++ not implemented
#endif
int main() {}])],
    [AC_MSG_RESULT([yes]); have_static_libs=yes],
    [AC_MSG_RESULT([no])])
  AC_LANG_POP(C++)

  LDFLAGS="$saved_LDFLAGS"
fi

# Flags needed for both GMP, MPFR and/or MPC.
AC_SUBST(gmplibs)
AC_SUBST(gmpinc)
AC_SUBST(extra_mpfr_configure_flags)
AC_SUBST(extra_mpc_gmp_configure_flags)
AC_SUBST(extra_mpc_mpfr_configure_flags)
AC_SUBST(extra_isl_gmp_configure_flags)

# Libraries to use for stage1 or when not bootstrapping.
AC_ARG_WITH(stage1-libs,
[AS_HELP_STRING([--with-stage1-libs=LIBS], [libraries for stage1])],
[if test "$withval" = "no" -o "$withval" = "yes"; then
   stage1_libs=
 else
   stage1_libs=$withval
 fi],
[stage1_libs=])
AC_SUBST(stage1_libs)

# Used for setting $lt_cv_objdir
_LT_CHECK_OBJDIR

# Check for GMP, MPFR and MPC
gmplibs="-lmpc -lmpfr -lgmp"
gmpinc=
have_gmp=no

### To add a new directory to the tree, first choose whether it is a target
### or a host dependent tool.  Then put it into the appropriate list
### (library or tools, host or target), doing a dependency sort.

# Subdirs will be configured in the order listed in build_configdirs, 
# configdirs, or target_configdirs; see the serialization section below.

# Dependency sorting is only needed when *configuration* must be done in 
# a particular order.  In all cases a dependency should be specified in 
# the Makefile, whether or not it's implicitly specified here.

# Double entries in build_configdirs, configdirs, or target_configdirs may
# cause circular dependencies and break everything horribly.

# these library is used by various programs built for the build
# environment
#
build_libs="build-libiberty build-libcpp"

# these tools are built for the build environment
build_tools="build-texinfo build-flex build-bison build-m4 build-fixincludes"

# these libraries are used by various programs built for the host environment
#f
host_libs="intl libiberty opcodes bfd readline tcl tk itcl libgui zlib libbacktrace libcpp libcody libdecnumber gmp mpfr mpc isl libelf libiconv libctf"

# these tools are built for the host environment
# Note, the powerpc-eabi build depends on sim occurring before gdb in order to
# know that we are building the simulator.
# binutils, gas and ld appear in that order because it makes sense to run
# "make check" in that particular order.
# If --enable-gold is used, "gold" may replace "ld".
host_tools="texinfo flex bison binutils gas ld fixincludes gcc cgen sid sim gdb gdbserver gprof etc expect dejagnu m4 utils guile fastjar gnattools libcc1 gotools c++tools"

# these libraries are built for the target environment, and are built after
# the host libraries and the host tools (which may be a cross compiler)
# Note that libiberty is not a target library.
sdcpp_hack_target_libraries="target-libgcc \
		target-libbacktrace \
		target-libgloss \
		target-newlib \
		target-libgomp \
		target-liboffloadmic \
		target-libatomic \
		target-libitm \
		target-libstdc++-v3 \
		target-libsanitizer \
		target-libvtv \
		target-libssp \
		target-libquadmath \
		target-libgfortran \
		target-libffi \
		target-libobjc \
		target-libada \
		target-libgo \
		target-libphobos \
		target-zlib"

# these tools are built using the target libraries, and are intended to
# run only in the target environment
#
# note: any program that *uses* libraries that are in the "target_libraries"
# list belongs in this list.
#
target_tools="target-rda"

################################################################################

## All tools belong in one of the four categories, and are assigned above
## We assign ${configdirs} this way to remove all embedded newlines.  This
## is important because configure will choke if they ever get through.
## ${configdirs} is directories we build using the host tools.
## ${target_configdirs} is directories we build using the target tools.
configdirs=`echo ${host_libs} ${host_tools}`
target_configdirs=`echo ${target_libraries} ${target_tools}`
build_configdirs=`echo ${build_libs} ${build_tools}`

m4_divert_text([PARSE_ARGS],
[case $srcdir in
  *" "*)
m4_pushdef([AS_MESSAGE_LOG_FD], [])dnl
    AC_MSG_ERROR([path to source, $srcdir, contains spaces])
m4_popdef([AS_MESSAGE_LOG_FD])dnl
    ;;
esac
ac_subdirs_all=`cd $srcdir && echo */configure | sed 's,/configure,,g'`
])

################################################################################

# mkdir takes a single argument on some systems.
gcc_AC_FUNC_MKDIR_TAKES_ONE_ARG

# File extensions
manext='.1'
objext='.o'
AC_SUBST(manext)
AC_SUBST(objext)

build_xm_file=
build_xm_defines=
build_install_headers_dir=install-headers-tar
build_exeext=
host_xm_file=
host_xm_defines=
host_xmake_file=
host_truncate_target=
host_exeext=

# Decode the host machine, then the target machine.
# For the host machine, we save the xm_file variable as host_xm_file;
# then we decode the target machine and forget everything else
# that came from the host machine.
#for machine in $build $host $target; do
#       . ${srcdir}/config.gcc
#done

extra_objs="${host_extra_objs} ${extra_objs}"

# Default the target-machine variables that were not explicitly set.
if test x"$tm_file" = x
then tm_file=$cpu_type/$cpu_type.h; fi

if test x"$extra_headers" = x
then extra_headers=; fi

if test x$md_file = x
then md_file=$cpu_type/$cpu_type.md; fi

if test x$out_file = x
then out_file=$cpu_type/$cpu_type.c; fi

if test x"$tmake_file" = x
then tmake_file=$cpu_type/t-$cpu_type
fi

if test x"$dwarf2" = xyes
then tm_file="$tm_file tm-dwarf2.h"
fi

# Handle cpp installation.
if test x$enable_cpp != xno
then
  tmake_file="$tmake_file t-install-cpp"
fi

# auto-host.h is the file containing items generated by autoconf and is
# the first file included by config.h.
# If host=build, it is correct to have hconfig include auto-host.h
# as well.  If host!=build, we are in error and need to do more
# work to find out the build config parameters.
if test x$host = x$build
then
        build_auto=auto-host.h
else
        # We create a subdir, then run autoconf in the subdir.
        # To prevent recursion we set host and build for the new
        # invocation of configure to the build for this invocation
        # of configure.
        tempdir=build.$$
        rm -rf $tempdir
        mkdir $tempdir
        cd $tempdir
        case ${srcdir} in
        /* | [A-Za-z]:[\\/]* ) realsrcdir=${srcdir};;
        *) realsrcdir=../${srcdir};;
        esac
        CC=${CC_FOR_BUILD} ${realsrcdir}/configure \
                --target=$target --host=$build --build=$build

        # We just finished tests for the build machine, so rename
        # the file auto-build.h in the gcc directory.
        mv auto-host.h ../auto-build.h
        cd ..
        rm -rf $tempdir
        build_auto=auto-build.h
fi

tm_file="${tm_file} defaults.h"
host_xm_file="auto-host.h ansidecl.h ${host_xm_file} ${tm_file}"
build_xm_file="${build_auto} ansidecl.h ${build_xm_file} ${tm_file}"
xm_file="ansidecl.h ${xm_file} ${tm_file}"

# Truncate the target if necessary
if test x$host_truncate_target != x; then
        target=`echo $target | sed -e 's/\(..............\).*/\1/'`
fi

# Get the version trigger filename from the toplevel
if test "${with_gcc_version_trigger+set}" = set; then
        gcc_version_trigger=$with_gcc_version_trigger
else
        gcc_version_trigger=${srcdir}/version.c
fi
changequote(,)dnl
gcc_version_full=`grep version_string ${gcc_version_trigger} | sed -e 's/.*"\([^"]*\)".*/\1/'`
gcc_version=`echo ${gcc_version_full} | sed -e 's/\([^ ]*\) .*/\1/'`

changequote([,])dnl

# Internationalization
PACKAGE=sdcc
VERSION="$gcc_version"
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE",
        [Define to the name of the distribution.])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION",
        [Define to the version of the distribution.])
AC_SUBST(PACKAGE)
AC_SUBST(VERSION)

ALL_LINGUAS="sv"

# Get an absolute path to the GCC top-level source directory
holddir=`pwd`
cd $srcdir
topdir=`pwd`
cd $holddir

out_object_file=`basename $out_file .c`.o

# Figure out what assembler we will be using.
AC_MSG_CHECKING(what assembler to use)
gcc_cv_as=
gcc_cv_gas_major_version=
gcc_cv_gas_minor_version=
gcc_cv_as_gas_srcdir=`echo $srcdir | sed -e 's,/gcc$,,'`/gas
if test -x "$DEFAULT_ASSEMBLER"; then
        gcc_cv_as="$DEFAULT_ASSEMBLER"
elif test -x "$AS"; then
        gcc_cv_as="$AS"
elif test -x as$host_exeext; then
        # Build using assembler in the current directory.
        gcc_cv_as=./as$host_exeext
elif test -f $gcc_cv_as_gas_srcdir/configure.in -a -f ../gas/Makefile; then
        # Single tree build which includes gas.
        for f in $gcc_cv_as_gas_srcdir/configure $gcc_cv_as_gas_srcdir/configure.in $gcc_cv_as_gas_srcdir/Makefile.in
        do
changequote(,)dnl
                gcc_cv_gas_version=`grep '^VERSION=[0-9]*\.[0-9]*' $f`
changequote([,])dnl
                if test x$gcc_cv_gas_version != x; then
                        break
                fi
        done
changequote(,)dnl
        gcc_cv_gas_major_version=`expr "$gcc_cv_gas_version" : "VERSION=\([0-9]*\)"`
        gcc_cv_gas_minor_version=`expr "$gcc_cv_gas_version" : "VERSION=[0-9]*\.\([0-9]*\)"`
changequote([,])dnl
fi

if test "x$gcc_cv_as" = x -a x$host = x$target; then
        # Native build.
        # Search the same directories that the installed compiler will
        # search.  Else we may find the wrong assembler and lose.  If we
        # do not find a suitable assembler binary, then try the user's
        # path.
        #
        # Also note we have to check MD_EXEC_PREFIX before checking the
        # user's path.  Unfortunately, there is no good way to get at the
        # value of MD_EXEC_PREFIX here.  So we do a brute force search
        # through all the known MD_EXEC_PREFIX values.  Ugh.  This needs
        # to be fixed as part of the make/configure rewrite too.

        if test "x$exec_prefix" = xNONE; then
                if test "x$prefix" = xNONE; then
                        test_prefix=/usr/local
                else
                        test_prefix=$prefix
                fi
        else
                test_prefix=$exec_prefix
        fi

        # If the loop below does not find an assembler, then use whatever
        # one we can find in the users's path.
        # user's path.
        gcc_cv_as=as$host_exeext

        test_dirs="$test_prefix/lib/gcc-lib/$target/$gcc_version \
                   $test_prefix/lib/gcc-lib/$target \
                   /usr/lib/gcc/$target/$gcc_version \
                   /usr/lib/gcc/$target \
                   $test_prefix/$target/bin/$target/$gcc_version \
                   $test_prefix/$target/bin \
                   /usr/libexec \
                   /usr/ccs/gcc \
                   /usr/ccs/bin \
                   /udk/usr/ccs/bin \
                   /bsd43/usr/lib/cmplrs/cc \
                   /usr/cross64/usr/bin \
                   /usr/lib/cmplrs/cc \
                   /sysv/usr/lib/cmplrs/cc \
                   /svr4/usr/lib/cmplrs/cc \
                   /usr/bin"

        for dir in $test_dirs; do
                if test -f $dir/as$host_exeext; then
                        gcc_cv_as=$dir/as$host_exeext
                        break;
                fi
        done
fi
if test x$gcc_cv_gas_major_version != x -a x$gcc_cv_gas_minor_version != x; then
  AC_MSG_RESULT("newly built gas")
else
  AC_MSG_RESULT($gcc_cv_as)
fi

# Figure out what nm we will be using.
AC_MSG_CHECKING(what nm to use)
if test -x nm$host_exeext; then
        gcc_cv_nm=./nm$host_exeext
elif test x$host = x$target; then
        # Native build.
        gcc_cv_nm=nm$host_exeext
fi
AC_MSG_RESULT($gcc_cv_nm)

AC_SUBST(BUILD_CONFIG)

# Use same top-level configure hooks in libgcc/libstdc++/libvtv.
AC_MSG_CHECKING([for --enable-vtable-verify])
AC_ARG_ENABLE(vtable-verify,
[AS_HELP_STRING([--enable-vtable-verify],
		[Enable vtable verification feature])],
[case "$enableval" in
 yes) enable_vtable_verify=yes ;;
 no)  enable_vtable_verify=no ;;
 *)   enable_vtable_verify=no;;
 esac],
[enable_vtable_verify=no])
AC_MSG_RESULT($enable_vtable_verify)

# Record target_configdirs and the configure arguments for target and
# build configuration in Makefile.
target_configdirs=`echo "${target_configdirs}" | sed -e 's/target-//g'`
build_configdirs=`echo "${build_configdirs}" | sed -e 's/build-//g'`
bootstrap_fixincludes=no

# If we are building libgomp, bootstrap it.
if echo " ${target_configdirs} " | grep " libgomp " > /dev/null 2>&1 ; then
  bootstrap_target_libs=${bootstrap_target_libs}target-libgomp,
fi

# If we are building libsanitizer and $BUILD_CONFIG contains bootstrap-asan
# or bootstrap-ubsan, bootstrap it.
if echo " ${target_configdirs} " | grep " libsanitizer " > /dev/null 2>&1; then
  case "$BUILD_CONFIG" in
    *bootstrap-hwasan* | *bootstrap-asan* | *bootstrap-ubsan* )
      bootstrap_target_libs=${bootstrap_target_libs}target-libsanitizer,
      bootstrap_fixincludes=yes
      ;;
  esac
fi

# If we are building libvtv and --enable-vtable-verify, bootstrap it.
if echo " ${target_configdirs} " | grep " libvtv " > /dev/null 2>&1 &&
   test "$enable_vtable_verify" != no; then
  bootstrap_target_libs=${bootstrap_target_libs}target-libvtv,
fi

# If we are building libatomic and the list of enabled languages includes the
# D frontend, bootstrap it.
if echo " ${target_configdirs} " | grep " libatomic " > /dev/null 2>&1; then
  case ,${enable_languages}, in
    *,d,*)
      bootstrap_target_libs=${bootstrap_target_libs}target-libatomic,
      ;;
  esac
fi

# Determine whether gdb needs tk/tcl or not.
# Use 'maybe' since enable_gdbtk might be true even if tk isn't available
# and in that case we want gdb to be built without tk.  Ugh!
# In fact I believe gdb is the *only* package directly dependent on tk,
# so we should be able to put the 'maybe's in unconditionally and
# leave out the maybe dependencies when enable_gdbtk is false.  I'm not
# 100% sure that that's safe though.

gdb_tk="maybe-all-tcl maybe-all-tk maybe-all-itcl maybe-all-libgui"
case "$enable_gdbtk" in
  no)
    GDB_TK="" ;;
  yes)
    GDB_TK="${gdb_tk}" ;;
  *)
    # Only add the dependency on gdbtk when GDBtk is part of the gdb
    # distro.  Eventually someone will fix this and move Insight, nee
    # gdbtk to a separate directory.
    if test -d ${srcdir}/gdb/gdbtk ; then
      GDB_TK="${gdb_tk}"
    else
      GDB_TK=""
    fi
    ;;
esac
CONFIGURE_GDB_TK=`echo ${GDB_TK} | sed s/-all-/-configure-/g`
INSTALL_GDB_TK=`echo ${GDB_TK} | sed s/-all-/-install-/g`

# gdb and gdbserver depend on gnulib and gdbsupport, but as nothing
# else does, only include them if one of these is built.  The Makefile
# provides the ordering, so it's enough here to add to the list.
case " ${configdirs} " in
  *\ gdb\ *)
    configdirs="${configdirs} gnulib gdbsupport"
    ;;
  *\ gdbserver\ *)
    configdirs="${configdirs} gnulib gdbsupport"
    ;;
  *\ sim\ *)
    configdirs="${configdirs} gnulib"
    ;;
esac

# Strip out unwanted targets.

# While at that, we remove Makefiles if we were started for recursive
# configuration, so that the top-level Makefile reconfigures them,
# like we used to do when configure itself was recursive.

# Loop over modules.  We used to use the "$extrasub" feature from Autoconf
# but now we're fixing up the Makefile ourselves with the additional
# commands passed to AC_CONFIG_FILES.  Use separate variables
# extrasub-{build,host,target} not because there is any reason to split
# the substitutions up that way, but only to remain below the limit of
# 99 commands in a script, for HP-UX sed.

# Do not nest @if/@endif or @unless/@endunless pairs, because
# configure will not warn you at all.

case "$enable_bootstrap:$ENABLE_GOLD: $configdirs :,$stage1_languages," in
  yes:yes:*\ gold\ *:*,c++,*) ;;
  yes:yes:*\ gold\ *:*)
    AC_MSG_ERROR([in a combined tree, bootstrapping with --enable-gold requires c++ in stage1_languages])
    ;;
esac

extrasub_build=
for module in ${build_configdirs} ; do
  if test -z "${no_recursion}" \
     && test -f ${build_subdir}/${module}/Makefile; then
    echo 1>&2 "*** removing ${build_subdir}/${module}/Makefile to force reconfigure"
    rm -f ${build_subdir}/${module}/Makefile
  fi
  extrasub_build="$extrasub_build
/^@if build-$module\$/d
/^@endif build-$module\$/d
/^@unless build-$module\$/,/^@endunless build-$module\$/d
/^@if build-$module-$bootstrap_suffix\$/d
/^@endif build-$module-$bootstrap_suffix\$/d
/^@unless build-$module-$bootstrap_suffix\$/,/^@endunless build-$module-$bootstrap_suffix\$/d"
done
extrasub_host=
for module in ${configdirs} ; do
  if test -z "${no_recursion}"; then
    for file in stage*-${module}/Makefile prev-${module}/Makefile ${module}/Makefile; do
      if test -f ${file}; then
	echo 1>&2 "*** removing ${file} to force reconfigure"
	rm -f ${file}
      fi
    done
  fi
  case ${module},${bootstrap_fixincludes} in
    fixincludes,no) host_bootstrap_suffix=no-bootstrap ;;
    *) host_bootstrap_suffix=$bootstrap_suffix ;;
  esac
  extrasub_host="$extrasub_host
/^@if $module\$/d
/^@endif $module\$/d
/^@unless $module\$/,/^@endunless $module\$/d
/^@if $module-$host_bootstrap_suffix\$/d
/^@endif $module-$host_bootstrap_suffix\$/d
/^@unless $module-$host_bootstrap_suffix\$/,/^@endunless $module-$host_bootstrap_suffix\$/d"
done
extrasub_target=
for module in ${target_configdirs} ; do
  if test -z "${no_recursion}" \
     && test -f ${target_subdir}/${module}/Makefile; then
    echo 1>&2 "*** removing ${target_subdir}/${module}/Makefile to force reconfigure"
    rm -f ${target_subdir}/${module}/Makefile
  fi

  # We only bootstrap target libraries listed in bootstrap_target_libs.
  case $bootstrap_target_libs in
    *,target-$module,*) target_bootstrap_suffix=$bootstrap_suffix ;;
    *) target_bootstrap_suffix=no-bootstrap ;;
  esac

  extrasub_target="$extrasub_target
/^@if target-$module\$/d
/^@endif target-$module\$/d
/^@unless target-$module\$/,/^@endunless target-$module\$/d
/^@if target-$module-$target_bootstrap_suffix\$/d
/^@endif target-$module-$target_bootstrap_suffix\$/d
/^@unless target-$module-$target_bootstrap_suffix\$/,/^@endunless target-$module-$target_bootstrap_suffix\$/d"
done

# Do the final fixup along with target modules.
extrasub_target="$extrasub_target
/^@if /,/^@endif /d
/^@unless /d
/^@endunless /d"

if test "$enable_pgo_build" != "no"; then
  extrasub_build="$extrasub_build
/^@if pgo-build\$/d
/^@endif pgo-build\$/d"
fi

# Create the serialization dependencies.  This uses a temporary file.

AC_ARG_ENABLE([serial-configure],
[AS_HELP_STRING([[--enable-serial-[{host,target,build}-]configure]],
		[force sequential configuration of
		 sub-packages for the host, target or build
		 machine, or all sub-packages])])

case ${enable_serial_configure} in
  yes)
    enable_serial_build_configure=yes
    enable_serial_host_configure=yes
    enable_serial_target_configure=yes
    ;;
esac

# These force 'configure's to be done one at a time, to avoid problems
# with contention over a shared config.cache.
rm -f serdep.tmp
echo '# serdep.tmp' > serdep.tmp
olditem=
test "x${enable_serial_build_configure}" = xyes &&
for item in ${build_configdirs} ; do
  case ${olditem} in
    "") ;;
    *) echo "configure-build-${item}: configure-build-${olditem}" >> serdep.tmp ;;
  esac
  olditem=${item}
done
olditem=
test "x${enable_serial_host_configure}" = xyes &&
for item in ${configdirs} ; do
  case ${olditem} in
    "") ;;
    *) echo "configure-${item}: configure-${olditem}" >> serdep.tmp ;;
  esac
  olditem=${item}
done
olditem=
test "x${enable_serial_target_configure}" = xyes &&
for item in ${target_configdirs} ; do
  case ${olditem} in
    "") ;;
    *) echo "configure-target-${item}: configure-target-${olditem}" >> serdep.tmp ;;
  esac
  olditem=${item}
done
serialization_dependencies=serdep.tmp
AC_SUBST_FILE(serialization_dependencies)

# Base args.  Strip norecursion, cache-file, srcdir, host, build,
# target, nonopt, and variable assignments.  These are the ones we
# might not want to pass down to subconfigures.  The exception being
# --cache-file=/dev/null, which is used to turn off the use of cache
# files altogether, and which should be passed on to subconfigures.
# Also strip program-prefix, program-suffix, and program-transform-name,
# so that we can pass down a consistent program-transform-name.
baseargs=
tbaseargs=
keep_next=no
skip_next=no
eval "set -- $ac_configure_args"
for ac_arg
do
  if test X"$skip_next" = X"yes"; then
    skip_next=no
    continue
  fi
  if test X"$keep_next" = X"yes"; then
    case $ac_arg in
      *\'*)
	ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
    esac
    baseargs="$baseargs '$ac_arg'"
    tbaseargs="$tbaseargs '$ac_arg'"
    keep_next=no
    continue
  fi

  # Handle separated arguments.  Based on the logic generated by
  # autoconf 2.59.
  case $ac_arg in
    *=* | --config-cache | -C | -disable-* | --disable-* \
      | -enable-* | --enable-* | -gas | --g* | -nfp | --nf* \
      | -q | -quiet | --q* | -silent | --sil* | -v | -verb* \
      | -with-* | --with-* | -without-* | --without-* | --x)
      separate_arg=no
      ;;
    -*)
      separate_arg=yes
      ;;
    *)
      separate_arg=no
      ;;
  esac

  skip_targ=no
  case $ac_arg in
changequote(,)
  --with-* | --without-*)
    libopt=`echo "$ac_arg" | sed -e 's,^--[^-_]*[-_],,' -e 's,=.*$,,'`

    case $libopt in
    *[-_]include)
      lib=`echo "$libopt" | sed 's,[-_]include$,,'`
      ;;
    *[-_]lib)
      lib=`echo "$libopt" | sed 's,[-_]lib$,,'`
      ;;
    *)
      lib=$libopt
      ;;
    esac
changequote([,])

    case $lib in
    mpc | mpfr | gmp | isl)
      # If we're processing --with-$lib, --with-$lib-include or
      # --with-$lib-lib, for one of the libs above, and target is
      # different from host, don't pass the current argument to any
      # target library's configure.
      if test x$is_cross_compiler = xyes; then
        skip_targ=yes
      fi
      ;;
    esac
    ;;
  esac

  case "$ac_arg" in
    --cache-file=/dev/null | \
    -cache-file=/dev/null )
      # Handled here to avoid the test to skip args below.
      baseargs="$baseargs '$ac_arg'"
      tbaseargs="$tbaseargs '$ac_arg'"
      # Assert: $separate_arg should always be no.
      keep_next=$separate_arg
      ;;
    --no*)
      continue
      ;;
    --c* | \
    --sr* | \
    --ho* | \
    --bu* | \
    --t* | \
    --program-* | \
    -cache_file* | \
    -srcdir* | \
    -host* | \
    -build* | \
    -target* | \
    -program-prefix* | \
    -program-suffix* | \
    -program-transform-name* )
      skip_next=$separate_arg
      continue
      ;;
    -*)
      # An option.  Add it.
      case $ac_arg in
	*\'*)
	  ac_arg=`echo "$ac_arg" | sed "s/'/'\\\\\\\\''/g"` ;;
      esac
      baseargs="$baseargs '$ac_arg'"
      if test X"$skip_targ" = Xno; then
        tbaseargs="$tbaseargs '$ac_arg'"
      fi
      keep_next=$separate_arg
      ;;
    *)
      # Either a variable assignment, or a nonopt (triplet).  Don't
      # pass it down; let the Makefile handle this.
      continue
      ;;
  esac
done
# Remove the initial space we just introduced and, as these will be
# expanded by make, quote '$'.
baseargs=`echo "x$baseargs" | sed -e 's/^x *//' -e 's,\\$,$$,g'`

# Add in --program-transform-name, after --program-prefix and
# --program-suffix have been applied to it.  Autoconf has already
# doubled dollar signs and backslashes in program_transform_name; we want
# the backslashes un-doubled, and then the entire thing wrapped in single
# quotes, because this will be expanded first by make and then by the shell. 
# Also, because we want to override the logic in subdir configure scripts to
# choose program_transform_name, replace any s,x,x, with s,y,y,.
sed -e "s,\\\\\\\\,\\\\,g; s,','\\\\'',g; s/s,x,x,/s,y,y,/" <<EOF_SED > conftestsed.out
${program_transform_name}
EOF_SED
gcc_transform_name=`cat conftestsed.out`
rm -f conftestsed.out
baseargs="$baseargs --program-transform-name='${gcc_transform_name}'"
tbaseargs="$tbaseargs --program-transform-name='${gcc_transform_name}'"
if test "$silent" = yes; then
  baseargs="$baseargs --silent"
  tbaseargs="$tbaseargs --silent"
fi
baseargs="$baseargs --disable-option-checking"
tbaseargs="$tbaseargs --disable-option-checking"

# Record and document user additions to sub configure arguments.
AC_ARG_VAR([build_configargs],
	   [additional configure arguments for build directories])
AC_ARG_VAR([host_configargs],
	   [additional configure arguments for host directories])
AC_ARG_VAR([target_configargs],
	   [additional configure arguments for target directories])

# For the build-side libraries, we just need to pretend we're native,
# and not use the same cache file.  Multilibs are neither needed nor
# desired.  We can't even use the same cache file for all build-side
# libraries, as they're compiled differently; some with C, some with
# C++ or with different feature-enabling options.
build_configargs="$build_configargs --cache-file=./config.cache ${baseargs}"

# For host modules, accept cache file option, or specification as blank.
case "${cache_file}" in
"") # empty
  cache_file_option="" ;;
/* | [[A-Za-z]]:[[\\/]]* ) # absolute path
  cache_file_option="--cache-file=${cache_file}" ;;
*) # relative path
  cache_file_option="--cache-file=../${cache_file}" ;;
esac

# Host dirs don't like to share a cache file either, horribly enough.
# This seems to be due to autoconf 2.5x stupidity.
host_configargs="$host_configargs --cache-file=./config.cache ${extra_host_args} ${baseargs}"

target_configargs="$target_configargs ${tbaseargs}"

# Passing a --with-cross-host argument lets the target libraries know
# whether they are being built with a cross-compiler or being built
# native.  However, it would be better to use other mechanisms to make the
# sorts of decisions they want to make on this basis.  Please consider
# this option to be deprecated.  FIXME.
if test x${is_cross_compiler} = xyes ; then
  target_configargs="--with-cross-host=${host_noncanonical} ${target_configargs}"
fi

# Special user-friendly check for native x86_64-linux build, if
# multilib is not explicitly enabled.
case "$target:$have_compiler:$host:$target:$enable_multilib" in
  x86_64-*linux*:yes:$build:$build:)
    # Make sure we have a development environment that handles 32-bit
    dev64=no
    echo "int main () { return 0; }" > conftest.c
    ${CC} -m32 -o conftest ${CFLAGS} ${CPPFLAGS} ${LDFLAGS} conftest.c
    if test $? = 0 ; then
      if test -s conftest || test -s conftest.exe ; then
	dev64=yes
      fi
    fi 
    rm -f conftest*
    if test x${dev64} != xyes ; then
      AC_MSG_ERROR([I suspect your system does not have 32-bit development libraries (libc and headers). If you have them, rerun configure with --enable-multilib. If you do not have them, and want to build a 64-bit-only compiler, rerun configure with --disable-multilib.])
    fi
    ;;
esac

# Default to --enable-multilib.
if test x${enable_multilib} = x ; then
  target_configargs="--enable-multilib ${target_configargs}"
fi

# Pass --with-newlib if appropriate.  Note that target_configdirs has
# changed from the earlier setting of with_newlib.
if test x${with_newlib} != xno && echo " ${target_configdirs} " | grep " newlib " > /dev/null 2>&1 && test -d ${srcdir}/newlib ; then
  target_configargs="--with-newlib ${target_configargs}"
fi

# Different target subdirs use different values of certain variables
# (notably CXX).  Worse, multilibs use *lots* of different values.
# Worse yet, autoconf 2.5x makes some of these 'precious', meaning that
# it doesn't automatically accept command-line overrides of them.
# This means it's not safe for target subdirs to share a cache file,
# which is disgusting, but there you have it.  Hopefully this can be
# fixed in future.  It's still worthwhile to use a cache file for each
# directory.  I think.

# Pass the appropriate --build, --host, --target and --cache-file arguments.
# We need to pass --target, as newer autoconf's requires consistency
# for target_alias and gcc doesn't manage it consistently.
target_configargs="--cache-file=./config.cache ${target_configargs}"

FLAGS_FOR_TARGET=
case " $target_configdirs " in
 *" newlib "*)
  case " $target_configargs " in
  *" --with-newlib "*)
   case "$target" in
    *-cygwin*)
      FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L$$r/$(TARGET_SUBDIR)/winsup/cygwin -isystem $$s/winsup/cygwin/include'
      ;;
   esac

   # If we're not building GCC, don't discard standard headers.
   if test -d ${srcdir}/gcc; then
     FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -nostdinc'

     if test "${build}" != "${host}"; then
       # On Canadian crosses, CC_FOR_TARGET will have already been set
       # by `configure', so we won't have an opportunity to add -Bgcc/
       # to it.  This is right: we don't want to search that directory
       # for binaries, but we want the header files in there, so add
       # them explicitly.
       FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -isystem $$r/$(HOST_SUBDIR)/gcc/include -isystem $$r/$(HOST_SUBDIR)/gcc/include-fixed'

       # Someone might think of using the pre-installed headers on
       # Canadian crosses, in case the installed compiler is not fully
       # compatible with the compiler being built.  In this case, it
       # would be better to flag an error than risking having
       # incompatible object files being constructed.  We can't
       # guarantee that an error will be flagged, but let's hope the
       # compiler will do it, when presented with incompatible header
       # files.
     fi
   fi

   case "${target}-${is_cross_compiler}" in
   i[[3456789]]86-*-linux*-no)
      # Here host == target, so we don't need to build gcc,
      # so we don't want to discard standard headers.
      FLAGS_FOR_TARGET=`echo " $FLAGS_FOR_TARGET " | sed -e 's/ -nostdinc / /'`
      ;;
   *)
      # If we're building newlib, use its generic headers last, but search
      # for any libc-related directories first (so make it the last -B
      # switch).
      FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$$r/$(TARGET_SUBDIR)/newlib/ -isystem $$r/$(TARGET_SUBDIR)/newlib/targ-include -isystem $$s/newlib/libc/include'

      # If we're building libgloss, find the startup file, simulator library
      # and linker script.
      case " $target_configdirs " in
	*" libgloss "*)
	# Look for startup file, simulator library and maybe linker script.
	FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$$r/$(TARGET_SUBDIR)/libgloss/'"$libgloss_dir"
	# Look for libnosys.a in case the target needs it.
	FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L$$r/$(TARGET_SUBDIR)/libgloss/libnosys'
	# Most targets have the linker script in the source directory.
	FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L$$s/libgloss/'"$libgloss_dir"
	;;
      esac
      ;;
   esac
   ;;
  esac
  ;;
esac

case "$target" in
  x86_64-*mingw* | *-w64-mingw*)
  # MinGW-w64 does not use newlib, nor does it use winsup. It may,
  # however, use a symlink named 'mingw' in ${prefix} .
    FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L${prefix}/${target}/lib -L${prefix}/mingw/lib -isystem ${prefix}/${target}/include -isystem ${prefix}/mingw/include'
    ;;
  *-mingw*)
  # MinGW can't be handled as Cygwin above since it does not use newlib.
    FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L$$r/$(TARGET_SUBDIR)/winsup/mingw -L$$r/$(TARGET_SUBDIR)/winsup/w32api/lib -isystem $$s/winsup/mingw/include -isystem $$s/winsup/w32api/include'
    ;;
esac

# Allow the user to override the flags for
# our build compiler if desired.
if test x"${build}" = x"${host}" ; then
  CFLAGS_FOR_BUILD=${CFLAGS_FOR_BUILD-${CFLAGS}}
  CPPFLAGS_FOR_BUILD=${CPPFLAGS_FOR_BUILD-${CPPFLAGS}}
  CXXFLAGS_FOR_BUILD=${CXXFLAGS_FOR_BUILD-${CXXFLAGS}}
  LDFLAGS_FOR_BUILD=${LDFLAGS_FOR_BUILD-${LDFLAGS}}
fi

# On Canadian crosses, we'll be searching the right directories for
# the previously-installed cross compiler, so don't bother to add
# flags for directories within the install tree of the compiler
# being built; programs in there won't even run.
if test "${build}" = "${host}" && test -d ${srcdir}/gcc; then
  # Search for pre-installed headers if nothing else fits.
  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -B$(build_tooldir)/bin/ -B$(build_tooldir)/lib/ -isystem $(build_tooldir)/include -isystem $(build_tooldir)/sys-include'
fi

if test "x${use_gnu_ld}" = x &&
   echo " ${configdirs} " | grep " ld " > /dev/null ; then
  # Arrange for us to find uninstalled linker scripts.
  FLAGS_FOR_TARGET=$FLAGS_FOR_TARGET' -L$$r/$(HOST_SUBDIR)/ld'
fi

# Search for other target-specific linker scripts and such.
case "${target}" in
  mep*)
    FLAGS_FOR_TARGET="$FLAGS_FOR_TARGET -mlibrary"
    ;;
esac

dnl Very limited version of automake's enable-maintainer-mode

AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode
                          enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      maintainer_mode=$enableval,
      maintainer_mode=no)

AC_MSG_RESULT($maintainer_mode)

if test "$maintainer_mode" = "yes"; then
  MAINT=''
else
  MAINT='#'
fi
AC_SUBST(MAINT)dnl

# These (without "all_") are set in each config-lang.in.
# `language' must be a single word so is spelled singularly.
all_stagestuff=
all_outputs='Makefile libcpp/Makefile'
# List of language makefile fragments.

# Add the language fragments.
# Languages are added via two mechanisms.  Some information must be
# recorded in makefile variables, these are defined in config-lang.in.
# We accumulate them and plug them into the main Makefile.
# The other mechanism is a set of hooks for each of the main targets
# like `clean', `install', etc.

language_fragments="Make-lang"

# Define variables host_canonical and build_canonical
# because some Cygnus local changes in the Makefile depend on them.
build_canonical=${build}
host_canonical=${host}
target_subdir=
if test "${host}" != "${target}" ; then
    target_subdir=${target_alias}/
fi
AC_SUBST(build_canonical)
AC_SUBST(host_canonical)
AC_SUBST(target_subdir)

# Nothing to do for FLOAT_H, float_format already handled.
objdir=`pwd`
AC_SUBST(objdir)

# -Wall c flag if GNU C
if test "$ac_cv_c_compiler_gnu" = "yes"; then
  WALL_FLAG="-Wall"
else
  WALL_FLAG=
fi
AC_SUBST(WALL_FLAG)

# Record and document user additions to sub configure arguments.
AC_ARG_VAR([build_configargs],
	   [additional configure arguments for build directories])
AC_ARG_VAR([host_configargs],
	   [additional configure arguments for host directories])
AC_ARG_VAR([target_configargs],
	   [additional configure arguments for target directories])

# Substitute configuration variables
AC_SUBST(all_stagestuff)
AC_SUBST(build_exeext)
AC_SUBST(build_install_headers_dir)
AC_SUBST(build_xm_file_list)
AC_SUBST(build_xm_file)
AC_SUBST(build_xm_defines)
AC_SUBST(check_languages)
AC_SUBST(dep_host_xmake_file)
AC_SUBST(dep_tmake_file)
AC_SUBST(extra_c_flags)
AC_SUBST(extra_headers_list)
AC_SUBST(extra_objs)
AC_SUBST(extra_parts)
AC_SUBST(extra_passes)
AC_SUBST(extra_programs)
AC_SUBST(gcc_config_arguments)
AC_SUBST(gcc_version)
AC_SUBST(gcc_version_full)
AC_SUBST(gcc_version_trigger)
AC_SUBST(host_exeext)
AC_SUBST(host_extra_gcc_objs)
AC_SUBST(install)
AC_SUBST(lang_tree_files)
AC_SUBST(local_prefix)
AC_SUBST(md_file)
AC_SUBST(out_file)
AC_SUBST(out_object_file)
AC_SUBST(symbolic_link)
AC_SUBST(thread_file)
AC_SUBST(c_target_objs)

#AC_SUBST_FILE(target_overrides)
#AC_SUBST_FILE(host_overrides)
#AC_SUBST(cross_defines)
#AC_SUBST_FILE(cross_overrides)
#AC_SUBST_FILE(build_overrides)

AC_SUBST_FILE(host_makefile_frag)
AC_SUBST_FILE(target_makefile_frag)
AC_SUBST_FILE(alphaieee_frag)
AC_SUBST_FILE(ospace_frag)

# why? (not in gcc/configure.ac)
AC_SUBST(host_makefile_frag)
AC_SUBST(target_makefile_frag)
AC_SUBST(alphaieee_frag)
AC_SUBST(ospace_frag)

# Miscellanea: directories, flags, etc.
AC_SUBST(RPATH_ENVVAR)
AC_SUBST(GCC_SHLIB_SUBDIR)
AC_SUBST(tooldir)
AC_SUBST(build_tooldir)
AC_SUBST(CONFIGURE_GDB_TK)
AC_SUBST(GDB_TK)
AC_SUBST(INSTALL_GDB_TK)

# Build module lists & subconfigure args.
AC_SUBST(build_configargs)
AC_SUBST(build_configdirs)

# Host module lists & subconfigure args.
AC_SUBST(host_configargs)
AC_SUBST(configdirs)
AC_SUBST(target_configdirs)

# Target module lists & subconfigure args.
AC_SUBST(target_configargs)

# Build tools.
AC_SUBST(AR_FOR_BUILD)
AC_SUBST(AS_FOR_BUILD)
AC_SUBST(CC_FOR_BUILD)
AC_SUBST(CFLAGS_FOR_BUILD)
AC_SUBST(CPP_FOR_BUILD)
AC_SUBST(CPPFLAGS_FOR_BUILD)
AC_SUBST(CXXFLAGS_FOR_BUILD)
AC_SUBST(CXX_FOR_BUILD)
AC_SUBST(DLLTOOL_FOR_BUILD)
AC_SUBST(DSYMUTIL_FOR_BUILD)
AC_SUBST(GFORTRAN_FOR_BUILD)
AC_SUBST(GOC_FOR_BUILD)
AC_SUBST(GDC_FOR_BUILD)
AC_SUBST(LDFLAGS_FOR_BUILD)
AC_SUBST(LD_FOR_BUILD)
AC_SUBST(NM_FOR_BUILD)
AC_SUBST(RANLIB_FOR_BUILD)
AC_SUBST(WINDMC_FOR_BUILD)
AC_SUBST(WINDRES_FOR_BUILD)

# Generate default definitions for YACC, M4, LEX and other programs that run
# on the build machine.  These are used if the Makefile can't locate these
# programs in objdir.
MISSING=`cd $ac_aux_dir && ${PWDCMD-pwd}`/missing

AC_CHECK_PROGS([YACC], ['bison -y' byacc yacc], [$MISSING bison -y])
# case " $build_configdirs " in
#   *" bison "*) YACC='$$r/$(BUILD_SUBDIR)/bison/tests/bison -y' ;;
# esac

AC_CHECK_PROGS([BISON], [bison], [$MISSING bison])
# case " $build_configdirs " in
#   *" bison "*) BISON='$$r/$(BUILD_SUBDIR)/bison/tests/bison' ;;
# esac

AC_CHECK_PROGS([M4], [gm4 gnum4 m4], [$MISSING m4])
# case " $build_configdirs " in
#   *" m4 "*) M4='$$r/$(BUILD_SUBDIR)/m4/m4' ;;
# esac

AC_CHECK_PROGS([LEX], [flex lex], [$MISSING flex])
# case " $build_configdirs " in
#   *" flex "*) LEX='$$r/$(BUILD_SUBDIR)/flex/flex' ;;
#   *" lex "*) LEX='$$r/$(BUILD_SUBDIR)/lex/lex' ;;
# esac

AC_CHECK_PROGS([FLEX], [flex], [$MISSING flex])
# case " $build_configdirs " in
#   *" flex "*) FLEX='$$r/$(BUILD_SUBDIR)/flex/flex' ;;
# esac

AC_CHECK_PROGS([MAKEINFO], makeinfo, [$MISSING makeinfo])
case " $build_configdirs " in
  *" texinfo "*) MAKEINFO='$$r/$(BUILD_SUBDIR)/texinfo/makeinfo/makeinfo' ;;
  *)
changequote(,)
    # For an installed makeinfo, we require it to be from texinfo 4.7 or
    # higher, else we use the "missing" dummy.
    if ${MAKEINFO} --version \
       | egrep 'texinfo[^0-9]*(4\.([7-9]|[1-9][0-9])|[5-9]|[1-9][0-9])' >/dev/null 2>&1; then
      :
    else
      MAKEINFO="$MISSING makeinfo"
    fi
    ;;
changequote([,])
esac

# FIXME: expect and dejagnu may become build tools?

AC_CHECK_PROGS(EXPECT, expect, expect)
case " $configdirs " in
  *" expect "*)
    test $host = $build && EXPECT='$$r/$(HOST_SUBDIR)/expect/expect'
    ;;
esac

AC_CHECK_PROGS(RUNTEST, runtest, runtest)
case " $configdirs " in
  *" dejagnu "*)
    test $host = $build && RUNTEST='$$s/$(HOST_SUBDIR)/dejagnu/runtest'
    ;;
esac


# Host tools.
NCN_STRICT_CHECK_TOOLS(AR, ar)
NCN_STRICT_CHECK_TOOLS(AS, as)
NCN_STRICT_CHECK_TOOLS(DLLTOOL, dlltool)
NCN_STRICT_CHECK_TOOLS(DSYMUTIL, dsymutil)
NCN_STRICT_CHECK_TOOLS(LD, ld)
NCN_STRICT_CHECK_TOOLS(LIPO, lipo)
NCN_STRICT_CHECK_TOOLS(NM, nm)
NCN_STRICT_CHECK_TOOLS(RANLIB, ranlib, true)
NCN_STRICT_CHECK_TOOLS(STRIP, strip, true)
NCN_STRICT_CHECK_TOOLS(WINDRES, windres)
NCN_STRICT_CHECK_TOOLS(WINDMC, windmc)
NCN_STRICT_CHECK_TOOLS(OBJCOPY, objcopy)
NCN_STRICT_CHECK_TOOLS(OBJDUMP, objdump)
NCN_STRICT_CHECK_TOOLS(OTOOL, otool)
NCN_STRICT_CHECK_TOOLS(READELF, readelf)
AC_SUBST(CC)
AC_SUBST(CXX)
AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)
AC_SUBST(GDC)
AC_SUBST(GDCFLAGS)

# Target tools.
AC_ARG_WITH([build-time-tools], 
  [AS_HELP_STRING([--with-build-time-tools=PATH],
		  [use given path to find target tools during the build])],
  [case x"$withval" in
     x/*) ;;
     *)
       with_build_time_tools=
       AC_MSG_WARN([argument to --with-build-time-tools must be an absolute path])
       ;;
   esac],
  [with_build_time_tools=])

NCN_STRICT_CHECK_TARGET_TOOLS(CC_FOR_TARGET, cc gcc)
NCN_STRICT_CHECK_TARGET_TOOLS(CXX_FOR_TARGET, c++ g++ cxx gxx)
NCN_STRICT_CHECK_TARGET_TOOLS(GCC_FOR_TARGET, gcc, ${CC_FOR_TARGET})
NCN_STRICT_CHECK_TARGET_TOOLS(GFORTRAN_FOR_TARGET, gfortran)
NCN_STRICT_CHECK_TARGET_TOOLS(GOC_FOR_TARGET, gccgo)
NCN_STRICT_CHECK_TARGET_TOOLS(GDC_FOR_TARGET, gdc)

ACX_CHECK_INSTALLED_TARGET_TOOL(AR_FOR_TARGET, ar)
ACX_CHECK_INSTALLED_TARGET_TOOL(AS_FOR_TARGET, as)
ACX_CHECK_INSTALLED_TARGET_TOOL(DLLTOOL_FOR_TARGET, dlltool)
ACX_CHECK_INSTALLED_TARGET_TOOL(DSYMUTIL_FOR_TARGET, dsymutil)
ACX_CHECK_INSTALLED_TARGET_TOOL(LD_FOR_TARGET, ld)
ACX_CHECK_INSTALLED_TARGET_TOOL(LIPO_FOR_TARGET, lipo)
ACX_CHECK_INSTALLED_TARGET_TOOL(NM_FOR_TARGET, nm)
ACX_CHECK_INSTALLED_TARGET_TOOL(OBJCOPY_FOR_TARGET, objcopy)
ACX_CHECK_INSTALLED_TARGET_TOOL(OBJDUMP_FOR_TARGET, objdump)
ACX_CHECK_INSTALLED_TARGET_TOOL(OTOOL_FOR_TARGET, otool)
ACX_CHECK_INSTALLED_TARGET_TOOL(RANLIB_FOR_TARGET, ranlib)
ACX_CHECK_INSTALLED_TARGET_TOOL(READELF_FOR_TARGET, readelf)
ACX_CHECK_INSTALLED_TARGET_TOOL(STRIP_FOR_TARGET, strip)
ACX_CHECK_INSTALLED_TARGET_TOOL(WINDRES_FOR_TARGET, windres)
ACX_CHECK_INSTALLED_TARGET_TOOL(WINDMC_FOR_TARGET, windmc)

RAW_CXX_FOR_TARGET="$CXX_FOR_TARGET"

GCC_TARGET_TOOL(ar, AR_FOR_TARGET, AR, [binutils/ar])
GCC_TARGET_TOOL(as, AS_FOR_TARGET, AS, [gas/as-new])
GCC_TARGET_TOOL(cc, CC_FOR_TARGET, CC, [gcc/xgcc -B$$r/$(HOST_SUBDIR)/gcc/])
dnl see comments for CXX_FOR_TARGET_FLAG_TO_PASS
GCC_TARGET_TOOL(c++, CXX_FOR_TARGET, CXX,
		[gcc/xg++ -B$$r/$(HOST_SUBDIR)/gcc/ -nostdinc++ `if test -f $$r/$(TARGET_SUBDIR)/libstdc++-v3/scripts/testsuite_flags; then $(SHELL) $$r/$(TARGET_SUBDIR)/libstdc++-v3/scripts/testsuite_flags --build-includes; else echo -funconfigured-libstdc++-v3 ; fi` -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/src -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/src/.libs -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/libsupc++/.libs],
		c++)
GCC_TARGET_TOOL(c++ for libstdc++, RAW_CXX_FOR_TARGET, CXX,
		[gcc/xgcc -shared-libgcc -B$$r/$(HOST_SUBDIR)/gcc -nostdinc++ -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/src -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/src/.libs -L$$r/$(TARGET_SUBDIR)/libstdc++-v3/libsupc++/.libs],
		c++)
GCC_TARGET_TOOL(dlltool, DLLTOOL_FOR_TARGET, DLLTOOL, [binutils/dlltool])
GCC_TARGET_TOOL(dsymutil, DSYMUTIL_FOR_TARGET, DSYMUTIL)
GCC_TARGET_TOOL(gcc, GCC_FOR_TARGET, , [gcc/xgcc -B$$r/$(HOST_SUBDIR)/gcc/])
GCC_TARGET_TOOL(gfortran, GFORTRAN_FOR_TARGET, GFORTRAN,
		[gcc/gfortran -B$$r/$(HOST_SUBDIR)/gcc/], fortran)
GCC_TARGET_TOOL(gccgo, GOC_FOR_TARGET, GOC,
		[gcc/gccgo -B$$r/$(HOST_SUBDIR)/gcc/], go)
GCC_TARGET_TOOL(gdc, GDC_FOR_TARGET, GDC,
		[gcc/gdc -B$$r/$(HOST_SUBDIR)/gcc/], d)
GCC_TARGET_TOOL(ld, LD_FOR_TARGET, LD, [ld/ld-new])
GCC_TARGET_TOOL(lipo, LIPO_FOR_TARGET, LIPO)
GCC_TARGET_TOOL(nm, NM_FOR_TARGET, NM, [binutils/nm-new])
GCC_TARGET_TOOL(objcopy, OBJCOPY_FOR_TARGET, OBJCOPY, [binutils/objcopy])
GCC_TARGET_TOOL(objdump, OBJDUMP_FOR_TARGET, OBJDUMP, [binutils/objdump])
GCC_TARGET_TOOL(otool, OTOOL_FOR_TARGET, OTOOL)
GCC_TARGET_TOOL(ranlib, RANLIB_FOR_TARGET, RANLIB, [binutils/ranlib])
GCC_TARGET_TOOL(readelf, READELF_FOR_TARGET, READELF, [binutils/readelf])
GCC_TARGET_TOOL(strip, STRIP_FOR_TARGET, STRIP, [binutils/strip-new])
GCC_TARGET_TOOL(windres, WINDRES_FOR_TARGET, WINDRES, [binutils/windres])
GCC_TARGET_TOOL(windmc, WINDMC_FOR_TARGET, WINDMC, [binutils/windmc])

AC_SUBST(FLAGS_FOR_TARGET)
AC_SUBST(RAW_CXX_FOR_TARGET)

# Certain tools may need extra flags.
AR_FOR_TARGET=${AR_FOR_TARGET}${extra_arflags_for_target}
RANLIB_FOR_TARGET=${RANLIB_FOR_TARGET}${extra_ranlibflags_for_target}
NM_FOR_TARGET=${NM_FOR_TARGET}${extra_nmflags_for_target}

# When building target libraries, except in a Canadian cross, we use
# the same toolchain as the compiler we just built.
COMPILER_AS_FOR_TARGET='$(AS_FOR_TARGET)'
COMPILER_LD_FOR_TARGET='$(LD_FOR_TARGET)'
COMPILER_NM_FOR_TARGET='$(NM_FOR_TARGET)'
if test $host = $build; then
  case " $configdirs " in
    *" gcc "*)
      COMPILER_AS_FOR_TARGET='$$r/$(HOST_SUBDIR)/gcc/as'
      COMPILER_LD_FOR_TARGET='$$r/$(HOST_SUBDIR)/gcc/collect-ld'
      COMPILER_NM_FOR_TARGET='$$r/$(HOST_SUBDIR)/gcc/nm'${extra_nmflags_for_target}
      ;;
  esac
fi

AC_SUBST(COMPILER_AS_FOR_TARGET)
AC_SUBST(COMPILER_LD_FOR_TARGET)
AC_SUBST(COMPILER_NM_FOR_TARGET)

AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
AC_ARG_ENABLE(maintainer-mode,
[AS_HELP_STRING([--enable-maintainer-mode],
		[enable make rules and dependencies not useful
		 (and sometimes confusing) to the casual installer])],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
AC_MSG_RESULT($USE_MAINTAINER_MODE)
AC_SUBST(MAINTAINER_MODE_TRUE)
AC_SUBST(MAINTAINER_MODE_FALSE)
if test "$USE_MAINTAINER_MODE" = yes; then
  MAINTAINER_MODE_TRUE=
  MAINTAINER_MODE_FALSE='#'
else
  MAINTAINER_MODE_TRUE='#'
  MAINTAINER_MODE_FALSE=
fi	
MAINT=$MAINTAINER_MODE_TRUE
AC_SUBST(MAINT)dnl

# ---------------------
# GCC bootstrap support
# ---------------------
# omitted.

#### OLD sdcpp # Create the Makefile
#### OLD sdcpp # and configure language subdirectories
#### OLD sdcpp AC_OUTPUT(Makefile libcpp/Makefile,
#### OLD sdcpp [
#### OLD sdcpp case x$CONFIG_HEADERS in
#### OLD sdcpp xauto-host.h:config.in)
#### OLD sdcpp echo > cstamp-h ;;
#### OLD sdcpp esac
#### OLD sdcpp 
#### OLD sdcpp # Avoid having to add intl to our include paths.
#### OLD sdcpp if test -f intl/libintl.h; then
#### OLD sdcpp   echo creating libintl.h
#### OLD sdcpp   echo '#include "intl/libintl.h"' >libintl.h
#### OLD sdcpp fi
#### OLD sdcpp ],
#### OLD sdcpp [
#### OLD sdcpp host='${host}'
#### OLD sdcpp build='${build}'
#### OLD sdcpp target='${target}'
#### OLD sdcpp target_alias='${target_alias}'
#### OLD sdcpp srcdir='${srcdir}'
#### OLD sdcpp symbolic_link='${symbolic_link}'
#### OLD sdcpp program_transform_set='${program_transform_set}'
#### OLD sdcpp program_transform_name='${program_transform_name}'
#### OLD sdcpp dep_host_xmake_file='${dep_host_xmake_file}'
#### OLD sdcpp host_xmake_file='${host_xmake_file}'
#### OLD sdcpp dep_tmake_file='${dep_tmake_file}'
#### OLD sdcpp tmake_file='${tmake_file}'
#### OLD sdcpp thread_file='${thread_file}'
#### OLD sdcpp gcc_config_arguments='${gcc_config_arguments}'
#### OLD sdcpp gcc_version='${gcc_version}'
#### OLD sdcpp gcc_version_full='${gcc_version_full}'
#### OLD sdcpp gcc_version_trigger='${gcc_version_trigger}'
#### OLD sdcpp local_prefix='${local_prefix}'
#### OLD sdcpp build_install_headers_dir='${build_install_headers_dir}'
#### OLD sdcpp build_exeext='${build_exeext}'
#### OLD sdcpp host_exeext='${host_exeext}'
#### OLD sdcpp out_file='${out_file}'
#### OLD sdcpp gdb_needs_out_file_path='${gdb_needs_out_file_path}'
#### OLD sdcpp SET_MAKE='${SET_MAKE}'
#### OLD sdcpp target_list='${target_list}'
#### OLD sdcpp target_overrides='${target_overrides}'
#### OLD sdcpp host_overrides='${host_overrides}'
#### OLD sdcpp cross_defines='${cross_defines}'
#### OLD sdcpp cross_overrides='${cross_overrides}'
#### OLD sdcpp build_overrides='${build_overrides}'
#### OLD sdcpp cpp_install_dir='${cpp_install_dir}'
#### OLD sdcpp ])


AC_CONFIG_FILES([Makefile],
  [sed "$extrasub_build" Makefile |
   sed "$extrasub_host" |
   sed "$extrasub_target" > mf$$
   mv -f mf$$ Makefile],
  [extrasub_build="$extrasub_build"
   extrasub_host="$extrasub_host"
   extrasub_target="$extrasub_target"])
AC_OUTPUT
