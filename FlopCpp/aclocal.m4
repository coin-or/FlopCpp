# generated automatically by aclocal 1.17 -*- Autoconf -*-

# Copyright (C) 1996-2024 Free Software Foundation, Inc.

# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

m4_ifndef([AC_CONFIG_MACRO_DIRS], [m4_defun([_AM_CONFIG_MACRO_DIRS], [])m4_defun([AC_CONFIG_MACRO_DIRS], [_AM_CONFIG_MACRO_DIRS($@)])])
# AM_AUX_DIR_EXPAND                                         -*- Autoconf -*-

# Copyright (C) 2001-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# For projects using AC_CONFIG_AUX_DIR([foo]), Autoconf sets
# $ac_aux_dir to '$srcdir/foo'.  In other projects, it is set to
# '$srcdir', '$srcdir/..', or '$srcdir/../..'.
#
# Of course, Automake must honor this variable whenever it calls a
# tool from the auxiliary directory.  The problem is that $srcdir (and
# therefore $ac_aux_dir as well) can be either absolute or relative,
# depending on how configure is run.  This is pretty annoying, since
# it makes $ac_aux_dir quite unusable in subdirectories: in the top
# source directory, any form will work fine, but in subdirectories a
# relative path needs to be adjusted first.
#
# $ac_aux_dir/missing
#    fails when called from a subdirectory if $ac_aux_dir is relative
# $top_srcdir/$ac_aux_dir/missing
#    fails if $ac_aux_dir is absolute,
#    fails when called from a subdirectory in a VPATH build with
#          a relative $ac_aux_dir
#
# The reason of the latter failure is that $top_srcdir and $ac_aux_dir
# are both prefixed by $srcdir.  In an in-source build this is usually
# harmless because $srcdir is '.', but things will broke when you
# start a VPATH build or use an absolute $srcdir.
#
# So we could use something similar to $top_srcdir/$ac_aux_dir/missing,
# iff we strip the leading $srcdir from $ac_aux_dir.  That would be:
#   am_aux_dir='\$(top_srcdir)/'`expr "$ac_aux_dir" : "$srcdir//*\(.*\)"`
# and then we would define $MISSING as
#   MISSING="\${SHELL} $am_aux_dir/missing"
# This will work as long as MISSING is not called from configure, because
# unfortunately $(top_srcdir) has no meaning in configure.
# However there are other variables, like CC, which are often used in
# configure, and could therefore not use this "fixed" $ac_aux_dir.
#
# Another solution, used here, is to always expand $ac_aux_dir to an
# absolute PATH.  The drawback is that using absolute paths prevent a
# configured tree to be moved without reconfiguration.

AC_DEFUN([AM_AUX_DIR_EXPAND],
[AC_REQUIRE([AC_CONFIG_AUX_DIR_DEFAULT])dnl
# Expand $ac_aux_dir to an absolute path.
am_aux_dir=`cd "$ac_aux_dir" && pwd`
])

# AM_CONDITIONAL                                            -*- Autoconf -*-

# Copyright (C) 1997-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_CONDITIONAL(NAME, SHELL-CONDITION)
# -------------------------------------
# Define a conditional.
AC_DEFUN([AM_CONDITIONAL],
[AC_PREREQ([2.52])dnl
 m4_if([$1], [TRUE],  [AC_FATAL([$0: invalid condition: $1])],
       [$1], [FALSE], [AC_FATAL([$0: invalid condition: $1])])dnl
AC_SUBST([$1_TRUE])dnl
AC_SUBST([$1_FALSE])dnl
_AM_SUBST_NOTMAKE([$1_TRUE])dnl
_AM_SUBST_NOTMAKE([$1_FALSE])dnl
m4_define([_AM_COND_VALUE_$1], [$2])dnl
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi
AC_CONFIG_COMMANDS_PRE(
[if test -z "${$1_TRUE}" && test -z "${$1_FALSE}"; then
  AC_MSG_ERROR([[conditional "$1" was never defined.
Usually this means the macro was only invoked conditionally.]])
fi])])

# Do all the work for Automake.                             -*- Autoconf -*-

# Copyright (C) 1996-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This macro actually does too much.  Some checks are only needed if
# your package does certain things.  But this isn't really a big deal.

dnl Redefine AC_PROG_CC to automatically invoke _AM_PROG_CC_C_O.
m4_define([AC_PROG_CC],
m4_defn([AC_PROG_CC])
[_AM_PROG_CC_C_O
])

# AM_INIT_AUTOMAKE(PACKAGE, VERSION, [NO-DEFINE])
# AM_INIT_AUTOMAKE([OPTIONS])
# -----------------------------------------------
# The call with PACKAGE and VERSION arguments is the old style
# call (pre autoconf-2.50), which is being phased out.  PACKAGE
# and VERSION should now be passed to AC_INIT and removed from
# the call to AM_INIT_AUTOMAKE.
# We support both call styles for the transition.  After
# the next Automake release, Autoconf can make the AC_INIT
# arguments mandatory, and then we can depend on a new Autoconf
# release and drop the old call support.
AC_DEFUN([AM_INIT_AUTOMAKE],
[AC_PREREQ([2.65])dnl
m4_ifdef([_$0_ALREADY_INIT],
  [m4_fatal([$0 expanded multiple times
]m4_defn([_$0_ALREADY_INIT]))],
  [m4_define([_$0_ALREADY_INIT], m4_expansion_stack)])dnl
dnl Autoconf wants to disallow AM_ names.  We explicitly allow
dnl the ones we care about.
m4_pattern_allow([^AM_[A-Z]+FLAGS$])dnl
AC_REQUIRE([AM_SET_CURRENT_AUTOMAKE_VERSION])dnl
AC_REQUIRE([AC_PROG_INSTALL])dnl
if test "`cd $srcdir && pwd`" != "`pwd`"; then
  # Use -I$(srcdir) only when $(srcdir) != ., so that make's output
  # is not polluted with repeated "-I."
  AC_SUBST([am__isrc], [' -I$(srcdir)'])_AM_SUBST_NOTMAKE([am__isrc])dnl
  # test to see if srcdir already configured
  if test -f $srcdir/config.status; then
    AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
  fi
fi

# test whether we have cygpath
if test -z "$CYGPATH_W"; then
  if (cygpath --version) >/dev/null 2>/dev/null; then
    CYGPATH_W='cygpath -w'
  else
    CYGPATH_W=echo
  fi
fi
AC_SUBST([CYGPATH_W])

# Define the identity of the package.
dnl Distinguish between old-style and new-style calls.
m4_ifval([$2],
[AC_DIAGNOSE([obsolete],
             [$0: two- and three-arguments forms are deprecated.])
m4_ifval([$3], [_AM_SET_OPTION([no-define])])dnl
 AC_SUBST([PACKAGE], [$1])dnl
 AC_SUBST([VERSION], [$2])],
[_AM_SET_OPTIONS([$1])dnl
dnl Diagnose old-style AC_INIT with new-style AM_AUTOMAKE_INIT.
m4_if(
  m4_ifset([AC_PACKAGE_NAME], [ok]):m4_ifset([AC_PACKAGE_VERSION], [ok]),
  [ok:ok],,
  [m4_fatal([AC_INIT should be called with package and version arguments])])dnl
 AC_SUBST([PACKAGE], ['AC_PACKAGE_TARNAME'])dnl
 AC_SUBST([VERSION], ['AC_PACKAGE_VERSION'])])dnl

_AM_IF_OPTION([no-define],,
[AC_DEFINE_UNQUOTED([PACKAGE], ["$PACKAGE"], [Name of package])
 AC_DEFINE_UNQUOTED([VERSION], ["$VERSION"], [Version number of package])])dnl

# Some tools Automake needs.
AC_REQUIRE([AM_SANITY_CHECK])dnl
AC_REQUIRE([AC_ARG_PROGRAM])dnl
AM_MISSING_PROG([ACLOCAL], [aclocal-${am__api_version}])
AM_MISSING_PROG([AUTOCONF], [autoconf])
AM_MISSING_PROG([AUTOMAKE], [automake-${am__api_version}])
AM_MISSING_PROG([AUTOHEADER], [autoheader])
AM_MISSING_PROG([MAKEINFO], [makeinfo])
AC_REQUIRE([AM_PROG_INSTALL_SH])dnl
AC_REQUIRE([AM_PROG_INSTALL_STRIP])dnl
AC_REQUIRE([AC_PROG_MKDIR_P])dnl
# For better backward compatibility.  To be removed once Automake 1.9.x
# dies out for good.  For more background, see:
# <https://lists.gnu.org/archive/html/automake/2012-07/msg00001.html>
# <https://lists.gnu.org/archive/html/automake/2012-07/msg00014.html>
AC_SUBST([mkdir_p], ['$(MKDIR_P)'])
# We need awk for the "check" target (and possibly the TAP driver).  The
# system "awk" is bad on some platforms.
AC_REQUIRE([AC_PROG_AWK])dnl
AC_REQUIRE([AC_PROG_MAKE_SET])dnl
AC_REQUIRE([AM_SET_LEADING_DOT])dnl
_AM_IF_OPTION([tar-ustar], [_AM_PROG_TAR([ustar])],
	      [_AM_IF_OPTION([tar-pax], [_AM_PROG_TAR([pax])],
			     [_AM_PROG_TAR([v7])])])
_AM_IF_OPTION([no-dependencies],,
[AC_PROVIDE_IFELSE([AC_PROG_CC],
		  [_AM_DEPENDENCIES([CC])],
		  [m4_define([AC_PROG_CC],
			     m4_defn([AC_PROG_CC])[_AM_DEPENDENCIES([CC])])])dnl
AC_PROVIDE_IFELSE([AC_PROG_CXX],
		  [_AM_DEPENDENCIES([CXX])],
		  [m4_define([AC_PROG_CXX],
			     m4_defn([AC_PROG_CXX])[_AM_DEPENDENCIES([CXX])])])dnl
AC_PROVIDE_IFELSE([AC_PROG_OBJC],
		  [_AM_DEPENDENCIES([OBJC])],
		  [m4_define([AC_PROG_OBJC],
			     m4_defn([AC_PROG_OBJC])[_AM_DEPENDENCIES([OBJC])])])dnl
AC_PROVIDE_IFELSE([AC_PROG_OBJCXX],
		  [_AM_DEPENDENCIES([OBJCXX])],
		  [m4_define([AC_PROG_OBJCXX],
			     m4_defn([AC_PROG_OBJCXX])[_AM_DEPENDENCIES([OBJCXX])])])dnl
])
# Variables for tags utilities; see am/tags.am
if test -z "$CTAGS"; then
  CTAGS=ctags
fi
AC_SUBST([CTAGS])
if test -z "$ETAGS"; then
  ETAGS=etags
fi
AC_SUBST([ETAGS])
if test -z "$CSCOPE"; then
  CSCOPE=cscope
fi
AC_SUBST([CSCOPE])

AC_REQUIRE([_AM_SILENT_RULES])dnl
dnl The testsuite driver may need to know about EXEEXT, so add the
dnl 'am__EXEEXT' conditional if _AM_COMPILER_EXEEXT was seen.  This
dnl macro is hooked onto _AC_COMPILER_EXEEXT early, see below.
AC_CONFIG_COMMANDS_PRE(dnl
[m4_provide_if([_AM_COMPILER_EXEEXT],
  [AM_CONDITIONAL([am__EXEEXT], [test -n "$EXEEXT"])])])dnl

AC_REQUIRE([_AM_PROG_RM_F])
AC_REQUIRE([_AM_PROG_XARGS_N])

dnl The trailing newline in this macro's definition is deliberate, for
dnl backward compatibility and to allow trailing 'dnl'-style comments
dnl after the AM_INIT_AUTOMAKE invocation. See automake bug#16841.
])

dnl Hook into '_AC_COMPILER_EXEEXT' early to learn its expansion.  Do not
dnl add the conditional right here, as _AC_COMPILER_EXEEXT may be further
dnl mangled by Autoconf and run in a shell conditional statement.
m4_define([_AC_COMPILER_EXEEXT],
m4_defn([_AC_COMPILER_EXEEXT])[m4_provide([_AM_COMPILER_EXEEXT])])

# When config.status generates a header, we must update the stamp-h file.
# This file resides in the same directory as the config header
# that is generated.  The stamp files are numbered to have different names.

# Autoconf calls _AC_AM_CONFIG_HEADER_HOOK (when defined) in the
# loop where config.status creates the headers, so we can generate
# our stamp files there.
AC_DEFUN([_AC_AM_CONFIG_HEADER_HOOK],
[# Compute $1's index in $config_headers.
_am_arg=$1
_am_stamp_count=1
for _am_header in $config_headers :; do
  case $_am_header in
    $_am_arg | $_am_arg:* )
      break ;;
    * )
      _am_stamp_count=`expr $_am_stamp_count + 1` ;;
  esac
done
echo "timestamp for $_am_arg" >`AS_DIRNAME(["$_am_arg"])`/stamp-h[]$_am_stamp_count])

# Copyright (C) 1999-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# _AM_PROG_CC_C_O
# ---------------
# Like AC_PROG_CC_C_O, but changed for automake.  We rewrite AC_PROG_CC
# to automatically call this.
AC_DEFUN([_AM_PROG_CC_C_O],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
AC_REQUIRE_AUX_FILE([compile])dnl
AC_LANG_PUSH([C])dnl
AC_CACHE_CHECK(
  [whether $CC understands -c and -o together],
  [am_cv_prog_cc_c_o],
  [AC_LANG_CONFTEST([AC_LANG_PROGRAM([])])
  # Make sure it works both with $CC and with simple cc.
  # Following AC_PROG_CC_C_O, we do the test twice because some
  # compilers refuse to overwrite an existing .o file with -o,
  # though they will create one.
  am_cv_prog_cc_c_o=yes
  for am_i in 1 2; do
    if AM_RUN_LOG([$CC -c conftest.$ac_ext -o conftest2.$ac_objext]) \
         && test -f conftest2.$ac_objext; then
      : OK
    else
      am_cv_prog_cc_c_o=no
      break
    fi
  done
  rm -f core conftest*
  unset am_i])
if test "$am_cv_prog_cc_c_o" != yes; then
   # Losing compiler, so override with the script.
   # FIXME: It is wrong to rewrite CC.
   # But if we don't then we get into trouble of one sort or another.
   # A longer-term fix would be to have automake use am__CC in this case,
   # and then we could set am__CC="\$(top_srcdir)/compile \$(CC)"
   CC="$am_aux_dir/compile $CC"
fi
AC_LANG_POP([C])])

# For backward compatibility.
AC_DEFUN_ONCE([AM_PROG_CC_C_O], [AC_REQUIRE([AC_PROG_CC])])

# Copyright (C) 2001-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# AM_RUN_LOG(COMMAND)
# -------------------
# Run COMMAND, save the exit status in ac_status, and log it.
# (This has been adapted from Autoconf's _AC_RUN_LOG macro.)
AC_DEFUN([AM_RUN_LOG],
[{ echo "$as_me:$LINENO: $1" >&AS_MESSAGE_LOG_FD
   ($1) >&AS_MESSAGE_LOG_FD 2>&AS_MESSAGE_LOG_FD
   ac_status=$?
   echo "$as_me:$LINENO: \$? = $ac_status" >&AS_MESSAGE_LOG_FD
   (exit $ac_status); }])

# Copyright (C) 2006-2024 Free Software Foundation, Inc.
#
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# _AM_SUBST_NOTMAKE(VARIABLE)
# ---------------------------
# Prevent Automake from outputting VARIABLE = @VARIABLE@ in Makefile.in.
# This macro is traced by Automake.
AC_DEFUN([_AM_SUBST_NOTMAKE])

# AM_SUBST_NOTMAKE(VARIABLE)
# --------------------------
# Public sister of _AM_SUBST_NOTMAKE.
AC_DEFUN([AM_SUBST_NOTMAKE], [_AM_SUBST_NOTMAKE($@)])

# Copyright (C) 2020 COIN-OR Foundation
# All Rights Reserved.
# This file is distributed under the Eclipse Public License 2.0.
# See LICENSE for details.
#
# This file defines the common autoconf macros for COIN-OR

# Check requirements
AC_PREREQ([2.71])

#                           COIN_CHECK_VPATH                              #

# This macro sets the variable coin_vpath_config to true if this is a VPATH
# configuration, otherwise it sets it to false. `VPATH' just means we're
# following best practices and not building in the source directory.

AC_DEFUN([AC_COIN_CHECK_VPATH],
[
  AC_MSG_CHECKING(whether this is a VPATH configuration)
  if test `cd $srcdir ; pwd` != `pwd`; then
    coin_vpath_config=yes
  else
    coin_vpath_config=no
  fi
  AC_MSG_RESULT($coin_vpath_config)
])


#                            COIN_VPATH_LINK                              #

# This macro ensures that the given files are available in a VPATH
# configuration, using the same name and relative path as in the source
# tree. It expects a white-space separated list of files.
# This macro is a small wrapper around AC_CONFIG_LINKS.

AC_DEFUN([AC_COIN_VPATH_LINK],
[
  m4_foreach_w(linkvar,[$1],[AC_CONFIG_LINKS(linkvar:linkvar)])
])


#                          COIN_PROJECTVERSION                            #

# This macro is used by COIN_INITIALIZE and sets up variables related to
# versioning numbers of the project.
#   COIN_PROJECTVERSION([libtool_version_string])
#
# If libtool_version_string is given, then defines AC_COIN_LIBTOOL_VERSIONINFO,
# which will be picked up by AC_COIN_PROG_LIBTOOL to set libtools -version-info flag.
#
# Further, breaks up AC_PACKAGE_VERSION into AC_PACKAGE_VERSION_MAJOR,
# AC_PACKAGE_VERSION_MINOR, AC_PACKAGE_VERSION_RELEASE, assuming it has
# the form major.minor.release, but use 9999 for missing values.
# AC_PACKAGE_VERSION is defined by AC_INIT

AC_DEFUN([AC_COIN_PROJECTVERSION],
[
  m4_define(AC_PACKAGE_VERSION_MAJOR, m4_bregexp(AC_PACKAGE_VERSION, [^\([0-9]*\).*], [\1]))
  m4_define(AC_PACKAGE_VERSION_MINOR, m4_bregexp(AC_PACKAGE_VERSION, [^[0-9]*\.\([0-9]*\).*], [\1]))
  m4_define(AC_PACKAGE_VERSION_RELEASE, m4_bregexp(AC_PACKAGE_VERSION, [^[0-9]*\.[0-9]*\.\([0-9]*\).*], [\1]))

  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_VERSION,
    "AC_PACKAGE_VERSION",
    [Version number of project])
  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_VERSION_MAJOR,
    m4_ifnblank(AC_PACKAGE_VERSION_MAJOR,AC_PACKAGE_VERSION_MAJOR,9999),
    [Major version number of project.])
  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_VERSION_MINOR,
    m4_ifnblank(AC_PACKAGE_VERSION_MINOR,AC_PACKAGE_VERSION_MINOR,9999),
    [Minor version number of project.])
  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_VERSION_RELEASE,
    m4_ifnblank(AC_PACKAGE_VERSION_RELEASE,AC_PACKAGE_VERSION_RELEASE,9999),
    [Release version number of project.])

  m4_define(AC_COIN_LIBTOOL_VERSIONINFO,$1)
])


#                          COIN_ENABLE_MSVC                               #

# This macro is invoked by any PROG_compiler macro to establish the
# --enable-msvc option.
# If unset but we have a Windows environment, look for some known C compilers
# and set enable_msvc if (i)cl is picked up (or has been set via CC by user)

AC_DEFUN([AC_COIN_ENABLE_MSVC],
[
  AC_CANONICAL_BUILD

  AC_ARG_ENABLE([msvc],
    [AS_HELP_STRING([--enable-msvc],[look for and allow only Intel/Microsoft compilers on MinGW/MSys/Cygwin])],
    [enable_msvc=$enableval],
    [enable_msvc=no
     case $build in
       *-mingw* | *-cygwin* | *-msys* )
         AC_CHECK_PROGS(CC, [gcc clang icx icl cl])
         case "$CC" in *cl | *icx ) enable_msvc=yes ;; esac
       ;;
     esac])
])


#                        COIN_COMPFLAGS_DEFAULTS                          #

# Overwrite default compiler flags, since the autoconf defaults don't work
# well for cl/icl/icx and we want to take --enable-debug and --enable-msvc into
# account:
# - debugging enabled: enable debug symbols (-g/-Z7)
# - debugging disabled: disable debug code (-DNDEBUG); enable (more) optimization (-O2)
# - enable exceptions for (i)cl
# - enable bugfix to get correct value for __cplusplus for (i)cl

AC_DEFUN([AC_COIN_COMPFLAGS_DEFAULTS],
[
dnl We want --enable-msvc setup and checked
  AC_REQUIRE([AC_COIN_ENABLE_MSVC])

  AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],[build debugging symbols and turn off compiler optimization])],
    [enable_debug=$enableval],
    [enable_debug=no])

dnl This macro should run before the compiler checks (does not seem to be sufficient for CFLAGS)
  AC_BEFORE([$0],[AC_PROG_CXX])
  AC_BEFORE([$0],[AC_PROG_CC])
  AC_BEFORE([$0],[AC_PROG_F77])
  AC_BEFORE([$0],[AC_PROG_FC])

  if test "$enable_debug" = yes ; then
    if test "$enable_msvc" = yes ; then
      : ${FFLAGS:="-nologo -fpp -Z7 -MDd $ADD_FFLAGS"}
      : ${FCFLAGS:="-nologo -fpp -Z7 -MDd $ADD_FCFLAGS"}
      : ${CFLAGS:="-nologo -Z7 -MDd $ADD_CFLAGS"}
      : ${CXXFLAGS:="-nologo -EHs -Z7 -MDd -Zc:__cplusplus $ADD_CXXFLAGS"}
    else
      : ${FFLAGS:="-g $ADD_FFLAGS"}
      : ${FCFLAGS:="-g $ADD_FCFLAGS"}
      : ${CFLAGS:="-g $ADD_CFLAGS"}
      : ${CXXFLAGS:="-g $ADD_CXXFLAGS"}
    fi
  else
    if test "$enable_msvc" = yes ; then
      : ${FFLAGS:="-nologo -fpp -O2 -MD $ADD_FFLAGS"}
      : ${FCFLAGS:="-nologo -fpp -O2 -MD $ADD_FCFLAGS"}
      : ${CFLAGS:="-nologo -DNDEBUG -O2 -MD $ADD_CFLAGS"}
      : ${CXXFLAGS:="-nologo -EHs -DNDEBUG -O2 -MD -Zc:__cplusplus $ADD_CXXFLAGS"}
    else
      : ${FFLAGS:="-O2 $ADD_FFLAGS"}
      : ${FCFLAGS:="-O2 $ADD_FCFLAGS"}
      : ${CFLAGS:="-O2 -DNDEBUG $ADD_CFLAGS"}
      : ${CXXFLAGS:="-O2 -DNDEBUG $ADD_CXXFLAGS"}
    fi
  fi
])


#                            COIN_DEBUGLEVEL                              #

# This macro makes the switches --with-prjct-verbosity and
# --with-prjct-checklevel available, which define the preprocessor macros
# COIN_PRJCT_VERBOSITY and COIN_PRJCT_CHECKLEVEL to the specified value
# (default is 0).

AC_DEFUN([AC_COIN_DEBUGLEVEL],
[
  AC_ARG_WITH(m4_tolower(AC_PACKAGE_NAME)-verbosity,
    AS_HELP_STRING([--with-m4_tolower(AC_PACKAGE_NAME)-verbosity],[specify the debug verbosity level]),
    [if test "$withval" = yes; then withval=1 ; fi
     coin_verbosity=$withval],
    [coin_verbosity=0])
  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_VERBOSITY,
                     $coin_verbosity,
                     [Define to the debug verbosity level (0 is no output)])

  AC_ARG_WITH(m4_tolower(AC_PACKAGE_NAME)-checklevel,
    AS_HELP_STRING([--with-m4_tolower(AC_PACKAGE_NAME)-checklevel],[specify the sanity check level]),
    [if test "$withval" = yes; then withval=1 ; fi
     coin_checklevel=$withval],
    [coin_checklevel=0])
  AC_DEFINE_UNQUOTED(m4_toupper(AC_PACKAGE_NAME)_CHECKLEVEL,
                     $coin_checklevel,
                     [Define to the debug sanity check level (0 is no test)])
])


#                            COIN_INITIALIZE                               #

# AC_COIN_INITIALIZE(version)

# This macro does everything that is required in the early part of the
# configure script, such as defining a few variables.
#   version (optional): the libtool library version
#                       project version number is used if not available
# - enforces the required autoconf version
# - sets the project's version numbers
# - changes the default compiler flags
# - get and patch the build and host types
# - Make silent build rules the default (https://www.gnu.org/software/automake/
#   manual/html_node/Automake-Silent-Rules.html)
# - disable automake maintainer mode
# - Initialize automake
#   - do not be as strict as for GNU projects
#   - don't AC_DEFINE PACKAGE or VERSION (but there're still defined as shell
#     variables in configure, and as make variables).
#   - disable dist target
#   - place objects from sources in subdirs into corresponding subdirs
#   - enable all automake warnings
# - Figure out the path where libraries are installed.
# - Add a configure flag to indicate whether .pc files should be made relocatable.
#   This is off by default, as it creates libtool warnings and doesn't account for
#   cases where --libdir is used.

AC_DEFUN([AC_COIN_INITIALIZE],
[
  AC_PREREQ([2.71])

  AC_COIN_PROJECTVERSION($1)

  AC_REQUIRE([AC_COIN_COMPFLAGS_DEFAULTS])

dnl this needs to used before AC_CANONICAL_BUILD
  AC_CANONICAL_BUILD
  AC_CANONICAL_HOST

  # libtool has some magic for host_os and build_os being mingw, but doesn't know about msys
  if test $host_os = msys ; then
    host_os=mingw
    host=`echo $host | sed -e 's/msys/mingw/'`
  fi

  if test $build_os = msys ; then
    build_os=mingw
    build=`echo $build | sed -e 's/msys/mingw/'`
  fi

dnl this needs to be used before AM_INIT_AUTOMAKE
  AM_SILENT_RULES([yes])

dnl without this, automake adds targets that may try to rerun the autotools
dnl on the users side, which asks for trouble
  AM_MAINTAINER_MODE([disable])

  AM_INIT_AUTOMAKE([foreign no-define no-dist subdir-objects -Wall])

  # Figure out the path where libraries are installed.
  # Unless the user specifies --prefix, prefix is set to NONE until the
  # end of configuration, at which point it will be set to $ac_default_prefix.
  # Unless the user specifies --exec-prefix, exec-prefix is set to NONE until
  # the end of configuration, at which point it's set to '${prefix}'.
  # Sheesh.  So do the expansion, then back it out.
  save_prefix=$prefix
  save_exec_prefix=$exec_prefix
  if test "x$prefix" = xNONE ; then
    prefix=$ac_default_prefix
  fi
  if test "x$exec_prefix" = xNONE ; then
    exec_prefix=$prefix
  fi
  expanded_libdir=$libdir
  while expr "$expanded_libdir" : '.*$.*' >/dev/null 2>&1 ; do
    eval expanded_libdir=$expanded_libdir
  done
  prefix=$save_prefix
  exec_prefix=$save_exec_prefix

  AC_ARG_ENABLE([relocatable],
    [AS_HELP_STRING([--enable-relocatable],[whether prefix in installed .pc files should be setup relative to pcfiledir])],
    [coin_enable_relocatable=$enableval],
    [coin_enable_relocatable=no])
  AM_CONDITIONAL([COIN_RELOCATABLE], [test $coin_enable_relocatable = yes])
])


#                           COIN_PROG_LIBTOOL                             #

# Set up libtool parameters and create and patchup libtool
# (https://www.gnu.org/software/libtool/manual/html_node/LT_005fINIT.html)
# Pass no-win32-dll to omit passing win32-dll to LT_INIT.
# Checks whether ar-lib wrapper needs to be used.
#
# Set up LT_LDFLAGS variable, which user can initialize and configure augments:
# - Add version info flag using the libtool library info, if defined,
#   otherwise use the project version if a full major.minor.release number is available.
# - Add -no-undefined as shared libraries should have no undefined symbols.
#   For Windows DLLs, it is mandatory to add this.
#
# Defines automake conditional COIN_STATIC_BUILD on whether we build
# shared or static. Used in .pc files.
#
# Patch libtool to circumvent some issues when using MSVC and MS lib.
# This needs to be run after config.status has created libtool.
# 1. Relax check which libraries can be used when linking a DLL.
#    libtool's func_mode_link() would reject linking a .lib file when building a DLL,
#    even though this .lib file may just be the one that eventually loads a depending DLL,
#    e.g., mkl_core_dll.lib. Setting deplibs_check_method=pass_all will still print a
#    warning, but the .lib is still passed to the linker.
# 2. Ensure always_export_symbols=no if win32-dll. Even when we pass win32-dll,
#    libtool forces always_export_symbols=yes for --tag=CXX if using MS compiler.
#    This leads to a nm call that collects ALL C-functions from a library
#    and explicitly dll-exporting them, leading to warnings about duplicates
#    regarding those that are properly marked for dll-export in the source.
# 3. Do not add mkl_*.lib to old_deplibs, which can result in trying to unpack and repack
#    the MKL libraries (which are pretty big). Instead, treat them like other -l<...> libs.
# 4. Add MKL libraries to dependency_libs in .la file, which I guess should be
#    the case due to point 5.
#
# Patch libtool also to circumvent some issues when using MinGW (Msys+GCC).
# 1. Relax check which libraries can be used when linking a DLL.
#    libtool's func_mode_link() would reject linking MinGW system libraries,
#    e.g., -lmingw32, when building a DLL, because it does not find this
#    library in the installation path, and then falls back to build only
#    static libraries. Setting deplibs_check_method=pass_all will avoid
#    this faulty check.

AC_DEFUN([AC_COIN_PROG_LIBTOOL],
[
dnl checkout AR and decide whether to use ar-lib wrapper
  AM_PROG_AR

dnl create libtool
  LT_INIT(disable-static pic-only m4_bmatch($1,no-win32-dll,,win32-dll))

  case "$am_cv_ar_interface" in
    lib )
      AC_CONFIG_COMMANDS([libtoolclpatch],
        [sed -e '/^deplibs_check_method/s/.*/deplibs_check_method="pass_all"/g' \
             m4_bmatch($1,no-win32-dll,,[-e 's|always_export_symbols=yes|always_export_symbols=no|g']) \
             -e '/func_append old_deplibs/s/\(.*\)/case $arg in *mkl_*.lib) ;; *) \1 ;; esac/g' \
             -e '/static library .deplib is not portable/a case $deplib in *mkl_*.lib) newdependency_libs="$deplib $newdependency_libs" ;; esac' \
         libtool > libtool.tmp
         mv libtool.tmp libtool
         chmod 755 libtool])
      ;;
    * )
      case $build in
        *-mingw* )
          AC_CONFIG_COMMANDS([libtoolmingwpatch],
            [sed -e '/^deplibs_check_method/s/.*/deplibs_check_method="pass_all"/g' libtool > libtool.tmp
             mv libtool.tmp libtool
             chmod 755 libtool])
        ;;
      esac
      ;;
  esac

  AC_ARG_VAR(LT_LDFLAGS,[Flags passed to libtool when building libraries or executables that are installed])

  m4_ifnblank(AC_COIN_LIBTOOL_VERSIONINFO,
    LT_LDFLAGS="$LT_LDFLAGS -version-info AC_COIN_LIBTOOL_VERSIONINFO"
    AC_MSG_NOTICE(libtool version info: -version-info AC_COIN_LIBTOOL_VERSIONINFO),
    m4_ifnblank(AC_PACKAGE_VERSION_MAJOR,
     m4_ifnblank(AC_PACKAGE_VERSION_MINOR,
      m4_ifnblank(AC_PACKAGE_VERSION_RELEASE,
       LT_LDFLAGS="$LT_LDFLAGS -version-number AC_PACKAGE_VERSION_MAJOR:AC_PACKAGE_VERSION_MINOR:AC_PACKAGE_VERSION_RELEASE"
       AC_MSG_NOTICE(libtool version info: -version-number AC_PACKAGE_VERSION_MAJOR:AC_PACKAGE_VERSION_MINOR:AC_PACKAGE_VERSION_RELEASE)
    ))))

  LT_LDFLAGS="$LT_LDFLAGS -no-undefined"

  AM_CONDITIONAL([COIN_STATIC_BUILD],[test "$enable_shared" = no])
])


#                    COIN_PROG_CC   COIN_PROG_CXX                         #

# Macros to find C and C++ compilers according to specified lists of compiler
# names. For Fortran compilers, see coin_fortran.m4.
#
# If enable-msvc, then test for Intel (on Windows) and MS C/C++ compiler
# explicitly and add the compile wrapper before AC_PROG_CC/CXX. The compile
# wrapper works around issues related to finding MS link.exe. (Unix link.exe
# occurs first in PATH, which causes compile and link checks to fail.) For
# the same reason, set LD to use the compile wrapper. If CC/CXX remains unset
# (neither icl, icx, or cl was found, and CC was not set by the user), stop with
# an error.
#
# Declares ADD_C(XX)FLAGS variables for additional compiler flags.
#
# Note that automake redefines AC_PROG_CC to invoke _AM_PROG_CC_C_O (an
# equivalent to AC_PROG_CC_C_O, plus wrap CC in the compile script if needed)
# and _AM_DEPENDENCIES (`dependency style'). Look in aclocal.m4 to see this.

AC_DEFUN_ONCE([AC_COIN_PROG_CC],
[
  AC_REQUIRE([AC_COIN_ENABLE_MSVC])

dnl Setting up libtool with LT_INIT will AC_REQUIRE AC_PROG_CC, but we want
dnl to invoke it from this macro first so that we can supply a parameter.
  AC_BEFORE([$0],[LT_INIT])

  if test $enable_msvc = yes ; then
    AC_CHECK_PROGS(CC, [icx icl cl])
    if test -n "$CC" ; then
      CC="$am_aux_dir/compile $CC"
      ac_cv_prog_CC="$CC"
      LD="$CC"
      : ${AR:=lib}
    else
      AC_MSG_ERROR([Neither MS nor Intel C compiler found in PATH and CC is unset.])
    fi
  fi

dnl Look for some C compiler and check that it works. If the user has set CC
dnl or we found icl/icx/cl above, this should not overwrite CC. Unlike the macros
dnl that establish C++ or Fortran compilers, PROG_CC also takes care of adding
dnl the compile wrapper if needed.
  AC_PROG_CC([gcc clang cc icx icc icl cl cc xlc xlc_r pgcc])

  AC_ARG_VAR(ADD_CFLAGS,[Additional C compiler options (if not overwriting CFLAGS)])
])

# Note that automake redefines AC_PROG_CXX to invoke _AM_DEPENDENCIES
# (`dependency style') but does not add an equivalent for AC_PROG_CXX_C_O,
# hence we need an explicit call.

AC_DEFUN_ONCE([AC_COIN_PROG_CXX],
[
  AC_REQUIRE([AC_COIN_ENABLE_MSVC])
  AC_REQUIRE([AC_COIN_PROG_CC])

  if test $enable_msvc = yes ; then
    AC_CHECK_PROGS(CXX, [icx icl cl])
    if test -n "$CXX" ; then
      CXX="$am_aux_dir/compile $CXX"
      ac_cv_prog_CXX="$CXX"
      LD="$CXX"
      : ${AR:=lib}
    else
      AC_MSG_ERROR([Neither MS nor Intel C++ compiler found in PATH and CXX is unset.])
    fi
  fi

dnl Look for some C++ compiler and check that it works. If the user has set
dnl CXX or we found icl/icx/cl above, this should not overwrite CXX.
  AC_PROG_CXX([g++ clang++ c++ pgCC icx icpc gpp cxx cc++ icl cl FCC KCC RCC xlC_r aCC CC])

dnl If the compiler does not support -c -o, wrap it with the compile script.
  AC_PROG_CXX_C_O
  if test $ac_cv_prog_cxx_c_o = no ; then
    CXX="$am_aux_dir/compile $CXX"
  fi

  AC_ARG_VAR(ADD_CXXFLAGS,[Additional C++ compiler options (if not overwriting CXXFLAGS)])
])

#                             COIN_CXXLIBS                                #

# Determine the C++ runtime libraries required for linking a C++ library
# with a Fortran or C compiler.  The result is available in CXXLIBS.

AC_DEFUN([AC_COIN_CXXLIBS],
[AC_REQUIRE([AC_COIN_PROG_CXX])dnl
AC_LANG_PUSH(C++)
AC_ARG_VAR(CXXLIBS,[Libraries necessary for linking C++ code with non-C++ compiler])
if test -z "$CXXLIBS"; then
  if test "$GXX" = "yes"; then
    case "$CXX" in
      icpc* | */icpc*)
        CXXLIBS="-lstdc++"
        ;;
      *)
        # clang uses libc++ as the default standard C++ library, not libstdc++
        # this test is supposed to recognize whether the compiler is clang
        AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <ciso646>]], [[
#ifndef _LIBCPP_VERSION
       choke me
#endif
          ]])],
          [CXXLIBS="-lc++"],
          [CXXLIBS="-lstdc++ -lm"])  dnl The -lm works around an issue with libtool removing -lm from the dependency_libs in an .la file (look for postdeps= in libtool)
        ;;
    esac
  else
    case $build in
     *-mingw* | *-cygwin* | *-msys* )
       if test "$enable_msvc" = yes ; then
         CXXLIBS=nothing
       fi;;
     *-linux-*)
      case "$CXX" in
      icpc* | */icpc*)
        CXXLIBS="-lstdc++"
             ;;
      pgCC* | */pgCC*)
        CXXLIBS="-lstd -lC -lc"
             ;;
      esac;;
    *-ibm-*)
      CXXLIBS="-lC -lc"
      ;;
    *-hp-*)
      CXXLIBS="-L/opt/aCC/lib -l++ -lstd_v2 -lCsup_v2 -lm -lcl -lc"
      ;;
    *-*-solaris*)
      CXXLIBS="-lCstd -lCrun"
    esac
  fi
fi
if test -z "$CXXLIBS"; then
  AC_MSG_WARN([Could not automatically determine CXXLIBS (C++ link libraries; necessary if main program is in Fortran or C).])
else
  AC_MSG_NOTICE([Assuming that CXXLIBS is $CXXLIBS.])
fi
if test x"$CXXLIBS" = xnothing; then
  CXXLIBS=
fi
AC_LANG_POP(C++)
]) # AC_COIN_CXXLIBS


#                            COIN_RPATH_FLAGS                             #

# This macro, in case shared objects are used, defines a variable
# RPATH_FLAGS that can be used by the linker to hardwire the library
# search path for the given directories.  This is useful for example
# Makefiles

AC_DEFUN([AC_COIN_RPATH_FLAGS],
[RPATH_FLAGS=

if test $enable_shared = yes; then
  case $build in
    *-linux-*)
      if test "$GCC" = "yes"; then
        RPATH_FLAGS=
        for dir in $1; do
          RPATH_FLAGS="$RPATH_FLAGS -Wl,--rpath -Wl,$dir"
        done
      fi ;;
    *-darwin*)
        RPATH_FLAGS=nothing ;;
    *-ibm-*)
      case "$CC" in
      xlc* | */xlc* | mpxlc* | */mpxlc*)
        RPATH_FLAGS=nothing ;;
      esac ;;
    *-hp-*)
        RPATH_FLAGS=nothing ;;
    *-mingw* | *-msys* )
        RPATH_FLAGS=nothing ;;
    *-*-solaris*)
        RPATH_FLAGS=
        for dir in $1; do
          RPATH_FLAGS="$RPATH_FLAGS -R$dir"
        done
  esac

  if test "$RPATH_FLAGS" = ""; then
    AC_MSG_WARN([Could not automatically determine how to tell the linker about automatic inclusion of the path for shared libraries.  The test examples might not work if you link against shared objects.  You will need to set the LD_LIBRARY_PATH, DYLP_LIBRARY_PATH, or LIBDIR variable manually.])
  fi
  if test "$RPATH_FLAGS" = "nothing"; then
    RPATH_FLAGS=
  fi
fi

AC_SUBST(RPATH_FLAGS)
]) # AC_COIN_RPATH_FLAGS


#                   COIN_DEFINENAMEMANGLING                               #

# COIN_DEFINENAMEMANGLING (name,namemangling)
# -------------------------------------------------------------------------
# Determine C/C++ name mangling to allow linking with header-less libraries.
#  name ($1) an identifier to prefix C macro names
#  namemangling ($2) the name mangling scheme as determined by COIN_NAMEMANGLING
#                    or COIN_TRY_LINK
#
# Defines the C macros $1_FUNC and $1_FUNC_ (in uppercase) to be used for
# declaring functions from library $1.

# -------------------------------------------------------------------------

AC_DEFUN([AC_COIN_DEFINENAMEMANGLING],
[
  AH_TEMPLATE($1_FUNC, [Define to a macro mangling the given C identifier (in lower and upper case).])
  AH_TEMPLATE($1_FUNC_, [As $1_FUNC, but for C identifiers containing underscores.])
  case "$2" in
   "lower case, no underscore, no extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [name])
      AC_DEFINE($1_FUNC_[(name,NAME)], [name]) ;;
   "lower case, no underscore, extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [name])
      AC_DEFINE($1_FUNC_[(name,NAME)], [name [##] _]) ;;
   "lower case, underscore, no extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [name [##] _])
      AC_DEFINE($1_FUNC_[(name,NAME)], [name [##] _]) ;;
   "lower case, underscore, extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [name [##] _])
      AC_DEFINE($1_FUNC_[(name,NAME)], [name [##] __]) ;;
   "upper case, no underscore, no extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [NAME])
      AC_DEFINE($1_FUNC_[(name,NAME)], [NAME]) ;;
   "upper case, no underscore, extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [NAME])
      AC_DEFINE($1_FUNC_[(name,NAME)], [NAME [##] _]) ;;
   "upper case, underscore, no extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [NAME [##] _])
      AC_DEFINE($1_FUNC_[(name,NAME)], [NAME [##] _]) ;;
   "upper case, underscore, extra underscore")
      AC_DEFINE($1_FUNC[(name,NAME)],  [NAME [##] _])
      AC_DEFINE($1_FUNC_[(name,NAME)], [NAME [##] __]) ;;
   *)
      AC_MSG_WARN([Unsupported or unknown name-mangling scheme: $2])
      ;;
  esac
])


#                            COIN_TRY_LINK                                #

# This is a helper macro for checking if a library can be linked based on
# a function name only.
#   COIN_TRY_LINK(func,lflags,pcfiles,action-if-success,action-if-failed,msg)
#  func ($1) the name of the function to try to link
#  lflags ($2) linker flags to use
#  pcfiles ($3) pc files to query for additional linker flags
#  action-if-success ($4) commands to execute if any linking was successful
#  action-if-failed ($5) commands to execute if no linking was successful
#  msg ($6) 'no' to suppress 'checking for' messages
#
# The macro tries different name mangling schemes and expands into
# action-if-success for the first one that succeeds.
# It sets variable func_namemangling according to the found name mangling
# scheme, which can be used as input for COIN_DEFINENAMEMANGLING.

AC_DEFUN([AC_COIN_TRY_LINK],
[
dnl setup LIBS by adding $2 and those from $3
  ac_save_LIBS="$LIBS"
  m4_ifnblank([$2], [LIBS="$2 $LIBS"])
  m4_ifnblank([$3],
  [ AC_REQUIRE([AC_COIN_HAS_PKGCONFIG])
    if test -n "$3" ; then
      temp_LFLAGS=`PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG --libs $pkg_static $3`
      LIBS="$temp_LFLAGS $LIBS"
    fi
  ])

  $1_namemangling=unknown

  for ac_extra in "no extra underscore" "extra underscore" ; do
    for ac_case in "lower case" "upper case" ; do
      for ac_trail in "underscore" "no underscore" ; do
        case $ac_case in
          "lower case")
            ac_name=m4_tolower($1)
            ;;
          "upper case")
            ac_name=m4_toupper($1)
            ;;
        esac
        if test "$ac_trail" = "underscore" ; then
          ac_name=${ac_name}_
        fi
        if test "$ac_extra" = "extra underscore" ; then
          ac_name=${ac_name}_
        fi
        m4_if([$6],[no],[],[AC_MSG_CHECKING([for function $ac_name in $LIBS])])
        AC_TRY_LINK_FUNC([$ac_name],
          [$1_namemangling="${ac_case}, ${ac_trail}, ${ac_extra}"
           ac_success=yes],
          [ac_success=no])
        m4_if([$6],[no],[],[AC_MSG_RESULT([$ac_success])])
        if test $ac_success = yes ; then
          break 3
        fi
      done
    done
  done
  LIBS=$ac_save_LIBS

  if test $ac_success = yes ; then
    m4_ifblank([$4],[:],[$4])
    m4_ifnblank([$5],[else $5])
  fi
])


#                           COIN_HAS_PKGCONFIG                            #

# Check whether a suitable pkg-config tool is available.  pkgconf is the
# up-and-coming thing, replacing pkg-config, so it is preferred.  The default
# minimal version number is 0.16.0 because COIN-OR .pc files include a
# URL field which breaks pkg-config version <= 0.15. A more recent minimum
# version can be specified as a parameter.  Portions of the macro body are
# derived from macros in pkg.m4.

# If a pkg-config tool is found, then the variable PKGCONFIG is set
# to its path, otherwise it is set to "". Further, the AM_CONDITIONAL
# COIN_HAS_PKGCONFIG is set and PKGCONFIG is AC_SUBST'ed.

# Finally, the search path for .pc files is assembled from the value of
# $prefix and $PKG_CONFIG_PATH in a variable COIN_PKG_CONFIG_PATH, which is
# also AC_SUBST'ed.  The search path will include the installation directory
# for .pc files for COIN-OR packages. COIN-OR .pc files are installed
# in ${libdir}/pkgconfig and COIN_INITIALIZE takes care of setting up
# $expanded_libdir based on $libdir.

# Of course, this whole house of cards balances on the shaky assumption that
# the user is sane and has installed all packages in the same place and does
# not change that place when make executes. If not, well, it's their
# responsibility to augment PKG_CONFIG_PATH in the environment.

AC_DEFUN([AC_COIN_HAS_PKGCONFIG],
[
  AC_ARG_VAR([PKG_CONFIG],[path to pkg-config utility])

dnl This is a modified version of PKG_PROG_PKG_CONFIG from pkg.m4.
  if test -z "$PKG_CONFIG" ; then
   AC_CHECK_TOOLS([PKG_CONFIG],[pkgconf pkg-config])
  fi
  if test -n "$PKG_CONFIG" ; then
    pkg_min_version=m4_default([$1],[0.16.0])
    AC_MSG_CHECKING([$PKG_CONFIG is at least version $pkg_min_version])
    if $PKG_CONFIG --atleast-pkgconfig-version $pkg_min_version ; then
      pkg_version=`$PKG_CONFIG --version`
      AC_MSG_RESULT([yes: $pkg_version])
    else
     AC_MSG_RESULT([no])
     PKG_CONFIG=""
   fi
  fi

dnl Check if PKG_CONFIG supports the short-errors flag.
dnl This is a modified version of _PKG_SHORT_ERRORS_SUPPORTED from pkg.m4.
  if test -n "$PKG_CONFIG" &&
     $PKG_CONFIG --atleast-pkgconfig-version 0.20 ; then
    pkg_short_errors=" --short-errors "
  else
    pkg_short_errors=""
  fi

dnl Check whether -static option of pkg-config should be used when requesting
dnl libs
  pkg_static=
  if test -n "$PKG_CONFIG" ; then
    case "$LDFLAGS" in "-static" | "* -static*" ) pkg_static=--static ;; esac
  fi

dnl Create a automake conditional and PKG_CONFIG variable
  AM_CONDITIONAL([COIN_HAS_PKGCONFIG], [test -n "$PKG_CONFIG"])
  AC_SUBST(PKG_CONFIG)

  COIN_PKG_CONFIG_PATH="${PKG_CONFIG_PATH}"
  AC_SUBST(COIN_PKG_CONFIG_PATH)

  COIN_PKG_CONFIG_PATH="${expanded_libdir}/pkgconfig:${COIN_PKG_CONFIG_PATH}"
  if test -n "$PKG_CONFIG"; then
    AC_MSG_NOTICE([$PKG_CONFIG path is "$COIN_PKG_CONFIG_PATH"])
  fi
])  # COIN_HAS_PKGCONFIG


#                      COIN_CHK_MOD_EXISTS                                #

# COIN_CHK_MOD_EXISTS(MODULE, PACKAGES,
#                     [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])

# Check to see whether a particular module (set of packages) exists. Derived
# from PKG_CHECK_MODULES() from pkg.m4, but set only the variables $1_VERSIONS
# and $1_PKG_ERRORS. PACKAGES ($2) is a space-separated list of .pc file names
# (without the .pc suffix). Note that variable values will have one line per
# package (embedded newlines) if more than one package is given in PACKAGES.

AC_DEFUN([AC_COIN_CHK_MOD_EXISTS],
[
  AC_REQUIRE([AC_COIN_HAS_PKGCONFIG])

  if test -n "$PKG_CONFIG" ; then
    if PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG --exists "$2" ; then
      m4_toupper($1)_VERSIONS=`PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG --modversion "$2" 2>/dev/null | tr '\n' ' '`
      $3
    else
      m4_toupper($1)_PKG_ERRORS=`PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG $pkg_short_errors --errors-to-stdout --print-errors "$2"`
      $4
    fi
  else
    AC_MSG_ERROR("Cannot check for existence of module $1 without pkgconf")
  fi
])


#                          COIN_CHK_HERE                                  #

# COIN_CHK_HERE([prim],[client packages],[pcfile])

# Augment the _LFLAGS, _CFLAGS, and _PCFILES variables of the client
# packages with the values from PRIM_LFLAGS_PUB, PRIM_CFLAGS_PUB, and
# PRIM_PCFILES_PUB. This macro is intended for the case where a single project
# builds several objects and one object includes another. For example,
# the various OsiXxxLib solvers, which depend on OsiLib. We can't consult
# osi.pc (it's not installed yet) but the relevant variables are ready at
# hand. The name of prim is often different from the name of the .pc file
# ($3), hence the separate parameter. If $3 is not given, it defaults to
# tolower($1).

# This macro should be called after FINALIZE_FLAGS is invoked for the
# client packages, for two reasons: First, COIN-OR packages tend to use
# .pc files, so we're probably adding a package to _PCFILES that isn't yet
# installed. Also, FINALIZE_FLAGS will have accessed the .pc files already
# in _PCFILES and expanded them into _LIBS and _CFLAGS, saving the original
# _LIBS and _CFLAGS in _LIBS_NOPC and _CFLAGS_NOPC.

AC_DEFUN([AC_COIN_CHK_HERE],
[
dnl Make sure the necessary variables exist for each client package.
  m4_foreach_w([myvar],[$2],
    [AC_SUBST(m4_toupper(myvar)_LFLAGS)
     AC_SUBST(m4_toupper(myvar)_CFLAGS)
     AC_SUBST(m4_toupper(myvar)_PCFILES)
    ])

dnl Add the .pc file and augment LFLAGS and CFLAGS.
dnl From the CFLAGS of $1, remove the -D$1_BUILD, though.
    m4_foreach_w([myvar],[$2],
      [m4_toupper(myvar)_PCFILES="$m4_toupper(myvar)_PCFILES m4_default($3,m4_tolower($1))"
       m4_toupper(myvar)_LFLAGS="$m4_toupper(myvar)_LFLAGS $m4_toupper($1)_LFLAGS"
       m4_toupper(myvar)_CFLAGS="$m4_toupper(myvar)_CFLAGS `echo $m4_toupper($1)_CFLAGS | sed -e s/-D[]m4_toupper($1)_BUILD//`"

       # Define BUILDTOOLS_DEBUG to enable debugging output
       if test "$BUILDTOOLS_DEBUG" = 1 ; then
         AC_MSG_NOTICE([CHK_HERE adding $1 to myvar:])
         AC_MSG_NOTICE([m4_toupper(myvar)_PCFILES: "${m4_toupper(myvar)_PCFILES}"])
         AC_MSG_NOTICE([m4_toupper(myvar)_LFLAGS: "${m4_toupper(myvar)_LFLAGS}"])
         AC_MSG_NOTICE([m4_toupper(myvar)_CFLAGS: "${m4_toupper(myvar)_CFLAGS}"])
       fi
      ])
])   # COIN_CHK_HERE


#                      COIN_DEF_PRIM_ARGS                                 #

# COIN_DEF_PRIM_ARGS([prim],[base],[lflags],[cflags],[dflags],[build])

# This is a utility macro to handle the standard arguments that COIN-OR
# configuration files supply for a component (package or library):
#   --with-prim: use primitive (yes / no / special)
#   --with-prim-lflags: linker flags for the primitive
#   --with-prim-cflags: preprocessor & compiler flags for the primitive
#   --with-prim-data: data directory for the primitive
# These arguments allow the user to override default macro behaviour from the
# configure command line.
# The string prim, lower-cased, is used in the flag name.
# The parameters base, lflags, cflags, and dflags have the value yes or no and
# determine whether a --with option will be defined for prim, lflags, cflags,
# and data, respectively. They must be literals, as the macro uses them to cut
# out unused options. To use the results, construct the name of the shell
# variable as specified in the autoconf doc'n for ARG_WITH.

# Setting the final parameter to 'default_build' will cause the phrase
# "'build' will look for a COIN ThirdParty package" to be inserted in the
# documentation for --with-prim.

AC_DEFUN([AC_COIN_DEF_PRIM_ARGS],
[
  m4_define([extraHelp],[
    m4_normalize(Do not use $1. [If an argument is given to --with-m4_tolower($1), then]
      ['yes' is equivalent to --with-m4_tolower($1),]
      m4_case($6,[default_build],
      ['no' is equivalent to --without-m4_tolower($1)[,]
       'build' will look for a COIN-OR ThirdParty package.],
      ['no' is equivalent to --without-m4_tolower($1)])
      m4_case($3$4$5,nonono,,
        nonoyes,
        and any other argument is applied as for --with-m4_tolower($1)-data,
        noyesno,
        and any other argument is applied as for --with-m4_tolower($1)-cflags,
        noyesyes,
        and any other argument is applied as for --with-m4_tolower($1)-cflags,
        and any other argument is applied as for --with-m4_tolower($1)-lflags)[.])])

  m4_if($2,yes,
    [AC_ARG_WITH([m4_tolower($1)],
       AS_HELP_STRING([--without-m4_tolower($1)],extraHelp))])

  m4_if($3,yes,
    [AC_ARG_WITH([m4_tolower($1)-lflags],
       AS_HELP_STRING([--with-m4_tolower($1)-lflags],
         [Linker flags for $1 appropriate for your environment.
          (Most often, -l specs for libraries.)]))])

  m4_if($4,yes,
    [AC_ARG_WITH([m4_tolower($1)-cflags],
       AS_HELP_STRING([--with-m4_tolower($1)-cflags],
         [Compiler flags for $1 appropriate for your environment.
          (Most often, -I specs for header file directories.)]))])

  m4_if($5,yes,
    [AC_ARG_WITH([m4_tolower($1)-data],
       AS_HELP_STRING([--with-m4_tolower($1)-data],
         [A data directory specification for $1 appropriate for your
          environment.]))])
])   # COIN_DEF_PRIM_ARGS

#                     FIND_PRIM_LIB (obsolete)                            #

# COIN_FIND_PRIM_LIB([prim],[lflgs],[cflgs],[dflgs],
#                    [func],[other libraries],
#                    [dfltaction],[cmdlineopts])

# Determine whether we can use primary library prim ($1) and assemble
# information on the required linker flags (prim_lflags), compiler flags
# (prim_cflags), and data directories (prim_data) as specified by cmdlineopts.
# Run a link check if the user provides [func]. Linker flags for the link are
# the concatenation of [lflgs] and [other libraries].

# cmdlineopts ($8) specifies the set of configure command line options
# defined and processed: 'nodata' produces --with-prim, --with-prim-lflags,
# and --with-prim-cflags; 'dataonly' produces only --with-prim and
# --with-prim-data; anything else ('all' works well) produces all four
# command line options. Shell code produced by the macro is tailored based
# on cmdlineopts. `nodata' is the default.

# --with-prim is interpreted as follows:
#   * --with-prim=no is equivalent to --without-prim.
#   * --with-prim or --with-prim=yes is equivalent to
#       --with-prim-lflags=-lprim
#       --with-prim-data=/usr/local/share
#   * --with-prim=build attempts to invent something that will find a COIN
#     ThirdParty library or data
#       --with-prim-lflags="-L\$(libdir) -lcoinprim"
#       --with-prim-cflgs="-I\$(pkgincludedir)/ThirdParty"
#       --with-prim-data="\$(pkgdatadir)"
#   * Any other value is taken as equivalent to
#       --with-prim-data=value (dataonly) or
#       --with-prim-lflags=value (anything else)

# The algorithm first checks for a user-specified value of --with-prim;
# if this is no, prim is skipped. Next, it looks for user specified values
# given with command line parameters --with-prim-lflags, --with-prim-cflags,
# and --with-prim-data. If none of these are specified, it will use the values
# passed as parameters. It's all or none: any command line options disable all
# parameters.

# Default action ($7) (no, yes, build) is the default value of --with-prim
# if the user offers no guidance via command line parameters. The (hardwired)
# default is yes. `build' doesn't have a hope of working except for COIN
# ThirdParty packages, and even then it's pretty shaky. You should be
# using CHK_PKG, because COIN packaging for ThirdParty software creates a .pc
# file.

# The macro doesn't test that the specified values actually work unless
# [func] is given as a parameter. This is deliberate --- there's no guarantee
# that the specified library can be accessed just yet with the specified
# flags. Except for the link check, all we're doing here is filling in
# variables using a complicated algorithm.

AC_DEFUN([AC_COIN_FIND_PRIM_LIB_OBS],
[
  dflt_action=m4_default([$7],[yes])

  # Initialize variables for the primary library.
  m4_tolower(coin_has_$1)=noInfo
  m4_tolower($1_lflags)=
  m4_tolower($1_cflags)=
  m4_tolower($1_data)=

  # --with-prim is always present.
dnl If the client specified dataonly, its value is assigned to prim_data.
  withval="$m4_tolower(with_$1)"
  if test -n "$withval" ; then
    case "$withval" in
      no )
        m4_tolower(coin_has_$1)=skipping
        ;;
      yes )
        m4_tolower(coin_has_$1)=requested
        ;;
      build )
        m4_tolower(coin_has_$1)=build
        ;;
      * )
        m4_tolower(coin_has_$1)=yes
        m4_if(m4_default($8,nodata),dataonly,
          [m4_tolower($1_data)="$withval"],
          [m4_tolower($1_lflags)="$withval"])
        ;;
    esac
  fi

  m4_if(m4_default($8,nodata),dataonly,[],
    [# --with-prim-lflags and --with-prim-cflags are present
     # Specifying --with-prim=no overrides the individual options for lflags and cflags.
     if test "$m4_tolower(coin_has_$1)" != skipping ; then
       withval="$m4_tolower(with_$1_lflags)"
       if test -n "$withval" ; then
         case "$withval" in
           build | no | yes )
             AC_MSG_ERROR(["$withval" is not valid here; please specify linker flags appropriate for your environment.])
             ;;
           * )
             m4_tolower(coin_has_$1)=yes
             m4_tolower($1_lflags)="$withval"
             ;;
         esac
       fi

       withval="$m4_tolower(with_$1_cflags)"
       if test -n "$withval" ; then
         case "$withval" in
           build | no | yes )
             AC_MSG_ERROR(["$withval" is not valid here; please specify compiler flags appropriate for your environment.])
             ;;
           * )
             m4_tolower(coin_has_$1)=yes
             m4_tolower($1_cflags)="$withval"
             ;;
         esac
       fi
     fi])

  m4_if(m4_default($8,nodata),nodata,[],
    [# --with-prim-data will be present
     # Specifying --with-prim=no overrides the individual option for data.
     if test "$m4_tolower(coin_has_$1)" != skipping ; then
       withval="$m4_tolower(with_$1_data)"
       if test -n "$withval" ; then
         case "$withval" in
           build | no | yes )
             AC_MSG_ERROR(["$withval" is not valid here; please give a data directory specification appropriate for your environment.])
             ;;
           * )
             m4_tolower(coin_has_$1)=yes
             m4_tolower($1_data)="$withval"
             ;;
         esac
       fi
     fi])

  # At this point, coin_has_prim can be one of
  # - noInfo (no user options specified),
  # - skipping (user said no),
  # - requested,
  # - build (user said yes or build and gave no further guidance), or
  # - yes (user specified one or more --with-prim options).
  # If we're already at yes or skipping, we're done looking.

  # If there are no user options (noInfo) and the default is no, we're skipping.
  # Otherwise, the default must be yes or build; consider the package requested.
  # A default action we don't recognise defaults to yes.
  if test "$m4_tolower(coin_has_$1)" = noInfo ; then
    case $dflt_action in
      no )
        m4_tolower(coin_has_$1)=skipping
        ;;
      build )
        m4_tolower(coin_has_$1)=build
        ;;
      * )
        m4_tolower(coin_has_$1)=requested
        ;;
    esac
  fi

  # Now coin_has_prim can be one of skipping, yes, build, or requested.
  # For build or requested, use the parameter values or invent some.
  case $m4_tolower(coin_has_$1) in
    build | requested)
      m4_if(m4_default($8,nodata),dataonly,[],
        [m4_ifnblank([$2],
           [m4_tolower($1_lflags)="$2"],
           [if test "$m4_tolower(coin_has_$1)" = build ; then
              m4_tolower($1_lflags)="-L\$(libdir) -l[]m4_tolower(coin$1)"
            else
              m4_tolower($1_lflags)="-l[]m4_tolower($1)"
            fi])
         m4_ifnblank([$3],
           [m4_tolower($1_cflags)="$3"],
           [if test "$m4_tolower(coin_has_$1)" = build ; then
              m4_tolower($1_cflags)="-I\$(pkgincludedir)/ThirdParty"
            fi])])
      m4_if(m4_default($8,nodata),nodata,[],
        [m4_tolower($1_data)=m4_default([$3],
           [if test "$m4_tolower(coin_has_$1)" = build ; then
              m4_tolower($1_data)="\$(pkgdatadir)"
            else
              m4_tolower($1_data)="/usr/local/share"
            fi])])
dnl FIXME This catches user-supplied as well as made up. And not checking is
dnl a feature. We need to discuss  -lh, 210114-
dnl go to linkcheck if we have a symbol to look for ($5)
dnl otherwise skip project, since we cannot check whether made-up l/cflags work
      m4_ifnblank([$5],m4_tolower(coin_has_$1)=yes,m4_tolower(coin_has_$1)=skipping)
      ;;
    skipping | yes )
      ;;
    * )
      AC_MSG_WARN([Unexpected status "$m4_tolower(coin_has_$1)" in COIN_FIND_PRIM_LIB])
      ;;
  esac

  # At this point, coin_has_prim is yes or skipping.
  m4_ifnblank([$5],
    [if test $m4_tolower(coin_has_$1) != skipping ; then
       # Time to run a link check since we have a function name.
       # Use whatever we've collected for lflags, plus other libraries ($6).
       ac_save_LIBS=$LIBS
       LIBS="$m4_tolower($1_lflags) $6"
       AC_TRY_LINK_FUNC([$5],[],m4_tolower(coin_has_$1)=no)
       LIBS=$ac_save_LIBS
     fi])

  # The final value of coin_has_prim will be yes, no, or skipping. No means that
  # the link check failed. Yes means that we passed the link check, or no link
  # check was performed. Skipping means the user said `don't use.'

  # Define BUILDTOOLS_DEBUG to enable debugging output
  if test "$BUILDTOOLS_DEBUG" = 1 ; then
    AC_MSG_NOTICE([FIND_PRIM_LIB result for $1: "$m4_tolower(coin_has_$1)"])
    AC_MSG_NOTICE([Collected values for package '$1'])
    AC_MSG_NOTICE([m4_tolower($1_lflags) is "$m4_tolower($1_lflags)"])
    AC_MSG_NOTICE([m4_tolower($1_cflags) is "$m4_tolower($1_cflags)"])
    AC_MSG_NOTICE([m4_tolower($1_data) is "$m4_tolower($1_data)"])
    AC_MSG_NOTICE([m4_tolower($1_pcfiles) is "$m4_tolower($1_pcfiles)"])
  fi

])  # COIN_FIND_PRIM_LIB


#                          COIN_CHK_LIB (obsolete)                        #

# COIN_CHK_LIB([prim],[client packages],[lflgs],[cflgs],[dflgs],
#              [func],[other libraries],
#              [dfltaction],[cmdopts])

# Determine whether we can use primary library prim ($1) and assemble
# information on the required linker flags (prim_lflags), compiler flags
# (prim_cflags), and data directories (prim_data). A link check will be
# performed in COIN_FIND_PRIM_LIB if [func] is specified, using link flags
# formed by concatenating the values of [lflgs] and [other libraries].

# The configure command line options offered to the user are controlled
# by cmdopts ($9). 'nodata' offers --with-prim, --with-prim-lflags, and
# --with-prim-cflags; 'dataonly' offers --with-prim and --with-prim-data;
# 'all' offers all four. DEF_PRIM_ARGS and FIND_PRIM_LIB are tailored
# accordingly. The (hardwired) default is 'nodata'.

# Macro parameters lflgs ($3), cflgs ($4), and dflgs ($5) are used for
# --with-prim-lflags, --with-prim-cflags, and --with-prim-data if and only if
# there are no user-supplied values on the command line. It's all or nothing;
# any user-supplied value causes all macro parameters to be ignored.

# Default action ($8) (no, yes, build) is the default action if the user
# offers no guidance via command line parameters. Really, 'build' has no
# hope of working except for COIN ThirdParty packages. Don't use it for
# other COIN packages. You should be using CHK_PKG because COIN packaging
# for ThirdParty software creates a .pc file.

# Define an automake conditional COIN_HAS_PRIM to record the result. If we
# decide to use prim, also define a preprocessor symbol COIN_HAS_PRIM.

# Linker and compiler flag information will be propagated to the space-
# separated list of client packages ($2) using the _LFLAGS and _CFLAGS
# variables of client packages. These variables match Libs.private and
# Cflags.private, respectively, in a .pc file.

# Data directory information is used differently. Typically what's wanted is
# individual variables specifying the data directory for each primitive. Hence
# the macro defines PRIM_DATA for the primitive.

AC_DEFUN([AC_COIN_CHK_LIB_OBS],
[
  AC_MSG_CHECKING(m4_ifnblank($6,[for package $1 with function $6],[for package $1]))

dnl Make sure the necessary variables exist for each client package.
  m4_foreach_w([myvar],[$2],
    [AC_SUBST(m4_toupper(myvar)_LFLAGS)
     AC_SUBST(m4_toupper(myvar)_CFLAGS)
    ])

  # Check to see if the user has overridden configure parameters from the
  # environment.
  m4_tolower(coin_has_$1)=noInfo
  if test x"$COIN_SKIP_PROJECTS" != x ; then
    for pkg in $COIN_SKIP_PROJECTS ; do
      if test "$m4_tolower(pkg)" = "$m4_tolower($1)" ; then
        m4_tolower(coin_has_$1)=skipping
      fi
    done
  fi

dnl If we are not skipping this project, define and process the command line
dnl options according to the cmdopts parameter. Then invoke FIND_PRIM_PKG to do
dnl the heavy lifting.
  if test "$m4_tolower(coin_has_$1)" != skipping ; then
    m4_case(m4_default($9,nodata),
      nodata,[AC_COIN_DEF_PRIM_ARGS([$1],yes,yes,yes,no,$8)],
      dataonly,[AC_COIN_DEF_PRIM_ARGS([$1],yes,no,no,yes,$8)],
      [AC_COIN_DEF_PRIM_ARGS([$1],yes,yes,yes,yes,$8)])
    AC_COIN_FIND_PRIM_LIB(m4_tolower($1),[$3],[$4],[$5],[$6],[$7],[$8],[$9])
    AC_MSG_RESULT([$m4_tolower(coin_has_$1)])
  else
    AC_MSG_RESULT([$m4_tolower(coin_has_$1) due to COIN_SKIP_PROJECTS])
  fi

  # Possibilities are `yes', `no', or `skipping'. Normalise to `yes' or `no'.
  if test "$m4_tolower(coin_has_$1)" != yes ; then
    m4_tolower(coin_has_$1)=no
  fi

dnl Create an automake conditional COIN_HAS_PRIM.
  AM_CONDITIONAL(m4_toupper(COIN_HAS_$1),
                   [test $m4_tolower(coin_has_$1) = yes])

  # If we've located the package, define preprocessor symbol COIN_HAS_PRIM
  # and augment the necessary variables for the client packages.
  if test $m4_tolower(coin_has_$1) = yes ; then
    AC_DEFINE(m4_toupper(AC_PACKAGE_NAME)_HAS_[]m4_toupper($1),[1],
      [Define to 1 if the $1 package is available])
    m4_foreach_w([myvar],[$2],
      [m4_toupper(myvar)_LFLAGS="$m4_tolower($1_lflags) $m4_toupper(myvar)_LFLAGS"
       m4_toupper(myvar)_CFLAGS="$m4_tolower($1_cflags) $m4_toupper(myvar)_CFLAGS"
      ])

dnl Finally, set up PRIM_DATA, unless the user specified nodata.
    m4_if(m4_default([$9],nodata),nodata,[],
      [AC_SUBST(m4_toupper($1)_DATA)
       m4_toupper($1)_DATA=$m4_tolower($1_data)])
  fi
])   # COIN_CHK_LIB


#                           COIN_CHK_LIBM                           #

# COIN_CHK_LIBM([client1 client2 ...])
# Check if a library spec is needed for the math library. If something is
# needed, the macro adds the flags to CLIENT_LFLAGS for each client.

AC_DEFUN([AC_COIN_CHK_LIBM],
[
  AC_REQUIRE([AC_PROG_CC])

  m4_foreach_w([myvar],[$1],[AC_SUBST(m4_toupper(myvar)_LFLAGS)])

  coin_save_LIBS="$LIBS"
  LIBS=
  AC_SEARCH_LIBS([cos],[m],
    [if test "$ac_cv_search_cos" != 'none required' ; then
       m4_foreach_w([myvar],[$1],
         [m4_toupper(myvar)_LFLAGS="$ac_cv_search_cos $m4_toupper(myvar)_LFLAGS"
         ])
     fi])
  LIBS="$coin_save_LIBS"
]) # AC_COIN_CHK_LIBM


#                           COIN_CHK_ZLIB                               #

# COIN_CHK_ZLIB([client1 client2 ...])

# This macro checks for the libz library.  If found, it sets the automake
# conditional COIN_HAS_ZLIB and defines the C preprocessor variable
# COIN_HAS_ZLIB. The default is to use zlib, if it's present.  For each client,
# it adds the linker flags to the variable CLIENT_LFLAGS for each client.

AC_DEFUN([AC_COIN_CHK_ZLIB],
[
  AC_REQUIRE([AC_COIN_PROG_CC])

  m4_foreach_w([myvar],[$1],[AC_SUBST(m4_toupper(myvar)_LFLAGS)])

  coin_has_zlib=no

  AC_ARG_ENABLE([zlib],
    [AS_HELP_STRING([--disable-zlib],[do not compile with compression library zlib])],
    [coin_enable_zlib=$enableval],
    [coin_enable_zlib=yes])

  if test x$coin_enable_zlib = xyes ; then
    AC_CHECK_HEADER([zlib.h],[coin_has_zlib=yes])
    if test x$coin_has_zlib = xyes ; then
      AC_CHECK_LIB([z],[gzopen],[],[coin_has_zlib=no])
    fi
    if test x$coin_has_zlib = xyes ; then
      m4_foreach_w([myvar],[$1],
        [m4_toupper(myvar)_LFLAGS="-lz $m4_toupper(myvar)_LFLAGS"
        ])
      AC_DEFINE(m4_toupper(AC_PACKAGE_NAME)_HAS_ZLIB,[1],[Define to 1 if zlib is available])
    fi
  fi

  AM_CONDITIONAL([COIN_HAS_ZLIB],[test x$coin_has_zlib = xyes])
]) # AC_COIN_CHK_ZLIB


#                            COIN_CHK_BZLIB                             #

# COIN_CHK_BZLIB([client1 client2 ...])

# This macro checks for the libbz2 library.  If found, it defines the
# C preprocessor variable COIN_HAS_BZLIB and the automake conditional
# COIN_HAS_BZLIB.  Further, for a (space separated) list of clients, it adds
# the linker flag to the variable CLIENT_LFLAGS for each client.

AC_DEFUN([AC_COIN_CHK_BZLIB],
[
  AC_REQUIRE([AC_PROG_CC])

  m4_foreach_w([myvar],[$1],[AC_SUBST(m4_toupper(myvar)_LFLAGS)])

  coin_has_bzlib=no

  AC_ARG_ENABLE([bzlib],
    [AS_HELP_STRING([--disable-bzlib],[do not compile with compression library bzlib])],
    [coin_enable_bzlib=$enableval],
    [coin_enable_bzlib=yes])

  if test $coin_enable_bzlib = yes ; then
    AC_CHECK_HEADER([bzlib.h],[coin_has_bzlib=yes])
    if test $coin_has_bzlib = yes ; then
      AC_CHECK_LIB([bz2],[BZ2_bzReadOpen],[],[coin_has_bzlib=no])
    fi
    if test $coin_has_bzlib = yes ; then
      m4_foreach_w([myvar],[$1],
        [m4_toupper(myvar)_LFLAGS="-lbz2 $m4_toupper(myvar)_LFLAGS"
        ])
      AC_DEFINE(m4_toupper(AC_PACKAGE_NAME)_HAS_BZLIB,[1],[Define to 1 if bzlib is available])
    fi
  fi

  AM_CONDITIONAL([COIN_HAS_BZLIB],[test x$coin_has_bzlib = xyes])
]) # AC_COIN_CHK_BZLIB


#                              COIN_CHK_GMP                               #

# COIN_CHK_GMP([client1 client2 ...])

# This macro checks for the gmp library.  If found, it defines the C
# preprocessor variable COIN_HAS_GMP and the automake conditional COIN_HAS_GMP.
# Further, for a (space separated) list of clients, it adds the linker
# flag to the variable CLIENT_LFLAGS for each client.

AC_DEFUN([AC_COIN_CHK_GMP],
[
  AC_REQUIRE([AC_PROG_CC])

  m4_foreach_w([myvar],[$1],[AC_SUBST(m4_toupper(myvar)_LFLAGS)])

  coin_has_gmp=no

  AC_ARG_ENABLE([gmp],
    [AS_HELP_STRING([--disable-gmp],[do not compile with GNU multiple precision library])],
    [coin_enable_gmp=$enableval],
    [coin_enable_gmp=yes])

  if test $coin_enable_gmp = yes ; then
    AC_CHECK_HEADER([gmp.h],
      [AC_CHECK_LIB([gmp],[__gmpz_init],[coin_has_gmp=yes])])
    if test $coin_has_gmp = yes ; then
      m4_foreach_w([myvar],[$1],
        [m4_toupper(myvar)_LFLAGS="-lgmp $m4_toupper(myvar)_LFLAGS"
        ])
      AC_DEFINE(m4_toupper(AC_PACKAGE_NAME)_HAS_GMP,[1],[Define to 1 if GMP is available])
    fi
  fi

  AM_CONDITIONAL(COIN_HAS_GMP,[test x$coin_has_gmp = xyes])
]) # AC_COIN_CHK_GMP


#                         COIN_CHK_GNU_READLINE                         #

# COIN_CHK_GNU_READLINE([client1 client2 ...])

# This macro checks for GNU's readline.  It verifies that the header
# readline/readline.h is available, and that the -lreadline library contains
# "readline".  It is assumed that #include <stdio.h> is included in the source
# file before the #include<readline/readline.h> If found, it defines the C
# preprocessor variable COIN_HAS_READLINE.  Further, for a (space separated)
# list of clients, it adds the linker flag to the variable CLIENT_LFLAGS for
# each client.

AC_DEFUN([AC_COIN_CHK_GNU_READLINE],
[
  AC_REQUIRE([AC_PROG_CC])

  m4_foreach_w([myvar],[$1],[AC_SUBST(m4_toupper(myvar)_LFLAGS)])

  coin_has_readline=no

  AC_ARG_ENABLE([readline],
    [AS_HELP_STRING([--disable-readline],[do not compile with readline library])],
    [coin_enable_readline=$enableval],
    [coin_enable_readline=yes])

  if test $coin_enable_readline = yes ; then
    AC_CHECK_HEADER([readline/readline.h],
      [coin_has_readline=yes],[],[#include <stdio.h>])
    coin_save_LIBS="$LIBS"
    LIBS=
    if test $coin_has_readline = yes ; then
      AC_SEARCH_LIBS([tputs],
        [ncurses termcap curses],[],[coin_has_readline=no])
    fi
    if test $coin_has_readline = yes ; then
      AC_CHECK_LIB([readline],[readline],[],[coin_has_readline=no])
    fi
    if test $coin_has_readline = yes ; then
      m4_foreach_w([myvar],[$1],
        [m4_toupper(myvar)_LFLAGS="$LIBS $m4_toupper(myvar)_LFLAGS"
        ])
      AC_DEFINE(m4_toupper(AC_PACKAGE_NAME)_HAS_READLINE,[1],[Define to 1 if readline is available])
    fi
    LIBS="$coin_save_LIBS"
  fi

  AM_CONDITIONAL(COIN_HAS_READLINE,[test x$coin_has_readline = xyes])
]) # AC_COIN_CHK_GNU_READLINE



#                           COIN_DOXYGEN                                  #

# This macro determines the configuration information for doxygen, the tool
# used to generate online documentation of COIN code. It takes one parameter,
# a list of projects (mixed-case, to match the install directory names) that
# should be processed as external tag files. E.g., COIN_DOXYGEN([Clp Osi]).

# This macro will define the following variables:
#  coin_have_doxygen    Yes if doxygen is found, no otherwise
#  coin_doxy_usedot     Defaults to `yes'; --with-dot will still check to see
#                        if dot is available
#  coin_doxy_tagname    Name of doxygen tag file (placed in doxydoc directory)
#  coin_doxy_logname    Name of doxygen log file (placed in doxydoc directory)
#  coin_doxy_tagfiles   List of doxygen tag files used to reference other
#                       doxygen documentation

# It's not immediately obvious, but the code in this macro, configure-time
# substitions in doxygen.conf.in, and build-time edits of doxygen.conf in
# Makemain.inc combine to hardwire the assumptions that a tag file is named
# proj_doxy.tag, that PKG_TARNAME is coin-or-proj, and that the doxygen
# documentation is in the GNU default location $(docdir)/$PKG_TARNAME. Have
# a look over the complete machinery before you start changing things. The
# point of the build-time edits is to allow the user to redefine docdir at
# build time, as per GNU standards. Failure to use coin-or-proj as PKG_TARNAME
# will surely break linking of documentation with tag files.

AC_DEFUN([AC_COIN_DOXYGEN],
[
  AC_MSG_NOTICE([configuring doxygen documentation options])

  # Check to see if Doxygen and LaTeX are available.
  AC_CHECK_PROG([coin_have_doxygen],[doxygen],[yes],[no])
  AC_CHECK_PROG([coin_have_latex],[latex],[yes],[no])

  AC_ARG_WITH([dot],
    AS_HELP_STRING([--with-dot],[use dot (from graphviz) when creating documentation with doxygen if available; --without-dot to disable]),
    [],[withval=yes])

  # Look for the dot tool from the graphviz package, unless the user has disabled it.
  if test x"$withval" = xno ; then
    coin_doxy_usedot=NO
    AC_MSG_CHECKING([for dot ])
    AC_MSG_RESULT([disabled])
  else
    AC_CHECK_PROG([coin_doxy_usedot],[dot],[YES],[NO])
  fi

  # Generate a tag file name and a log file name.
  AC_SUBST([coin_doxy_tagname],[m4_tolower(AC_PACKAGE_NAME)_doxy.tag])
  AC_SUBST([coin_doxy_logname],[m4_tolower(AC_PACKAGE_NAME)_doxy.log])

  AM_CONDITIONAL(COIN_HAS_DOXYGEN, [test $coin_have_doxygen = yes])
  AM_CONDITIONAL(COIN_HAS_LATEX, [test $coin_have_latex = yes])

dnl Process the list of project names and massage each one into the name of
dnl a tag file. The value of coin_doxy_tagfiles is substituted for TAGFILES
dnl in doxygen.conf.in. Further substitution for @baredocdir_nosub@ will happen
dnl as an edit during make install. See comments in Makemain.inc.
  coin_doxy_tagfiles="m4_foreach_w([myvar],[$1],[@baredocdir_nosub@/coin-or-m4_tolower(myvar)/doxydoc/m4_tolower(myvar)_doxy.tag=@baredocdir_nosub@/coin-or-m4_tolower(myvar)/doxydoc/html ])"
  AC_SUBST([coin_doxy_tagfiles])
]) # AC_COIN_DOXYGEN


#                          COIN_EXAMPLE_FILES                             #

# This macro deals with `example' files. Its original use is for data files
# associated with the Data projects. It allows for uniform handling of files
# that are not processed but should be present in the build directory. Most of
# the work is done by make. This macro just sets up the necessary variables.
# The justification for this approach is to allow the use of file name wild
# cards in the argument. The underlying machinery of autoconf and automake
# kind of breaks down when the file names are not literal text.

# One tradeoff for allowing the use of wildcards is that it's not possible to
# give the created link a name that's different from the source file.

# The variables (file names accumulate over calls):
# EXAMPLE_DIST_FILES: The originals that should be distributed
# EXAMPLE_UNCOMP_FILES: like EXAMPLE_DIST_FILES, but names of compressed files
# are stripped of their .gz suffix.
# EXAMPLE_CLEAN_FILES: contains only the names of files that were stripped of
# .gz. Essential for distclean in a non-VPATH build.
# One of EXAMPLE_DIST_FILES or EXAMPLE_UNCOMP_FILES will be the list of file
# names for install / uninstall. See Makefile.am in a Data project for usage.

AC_DEFUN([AC_COIN_EXAMPLE_FILES],
[
  AC_REQUIRE([AC_COIN_CHECK_VPATH])[]dnl
  AC_REQUIRE([AC_PROG_LN_S])[]dnl
  AM_CONDITIONAL([COIN_VPATH_BUILD],[test x$coin_vpath_config = xyes])

  # Get the names of the files and sort them to the various variables. As
  # a convenient side-effect, the loop will remove newlines from the list
  # of files. Avoid adding duplicate file names. (Allowing wild cards makes
  # duplicates far too likely.)
  # The redirect of stderr on the ls is arguable. It avoids error messages
  # in the configure output if the package maintainer specifies files that
  # don't exist (for example, generic wild card specs that anticipate future
  # growth) but it will hide honest errors.
  files=`cd $srcdir; ls $1 2>/dev/null`
  for file in $files; do
    if expr " $EXAMPLE_DIST_FILES " : '.* '"$file"' .*' >/dev/null 2>&1 ; then
      continue ;
    fi
    EXAMPLE_DIST_FILES="$EXAMPLE_DIST_FILES $file"
    if expr "$file" : '.*\.gz$' >/dev/null 2>&1 ; then
      EXAMPLE_UNCOMP_FILES="$EXAMPLE_UNCOMP_FILES `expr "$file" : '\(.*\)\.gz$'`"
      EXAMPLE_CLEAN_FILES="$EXAMPLE_CLEAN_FILES `expr "$file" : '\(.*\)\.gz$'`"
    else
      EXAMPLE_UNCOMP_FILES="$EXAMPLE_UNCOMP_FILES $file"
    fi
  done

  AC_SUBST(EXAMPLE_DIST_FILES)
  AC_SUBST(EXAMPLE_UNCOMP_FILES)
  AC_SUBST(EXAMPLE_CLEAN_FILES)
]) # AC_COIN_EXAMPLE_FILES


#                           COIN_FINALIZE_FLAGS                           #

# COIN_FINALIZE_FLAGS([prim1 prim2 ...])

# For each PRIM, produce final versions of variables specifying linker and
# compiler flags.  PRIM_LFLAGS_NOPC and PRIM_CFLAGS_NOPC are appropriate for
# use in .pc files together with PRIM_PCFILES. PRIM_LFLAGS and PRIM_CFLAGS
# are augmented to contain flags obtained by invoking PKG_CONFIG on packages
# listed in PRIM_PCFILES and are appropriate for use in Makefile.am files.

AC_DEFUN([AC_COIN_FINALIZE_FLAGS],
[
  m4_foreach_w([myvar],[$1],
    [ if test "$BUILDTOOLS_DEBUG" = 1 ; then
        AC_MSG_NOTICE([FINALIZE_FLAGS for myvar:])
      fi
      AC_SUBST(m4_toupper(myvar)_LFLAGS_NOPC,[$m4_toupper(myvar)_LFLAGS])
      AC_SUBST(m4_toupper(myvar)_CFLAGS_NOPC,[$m4_toupper(myvar)_CFLAGS])
      if test -n "${m4_toupper(myvar)_PCFILES}" ; then
        temp_CFLAGS=`PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG --cflags ${m4_toupper(myvar)_PCFILES}`
        temp_LFLAGS=`PKG_CONFIG_PATH="$COIN_PKG_CONFIG_PATH" $PKG_CONFIG --libs $pkg_static ${m4_toupper(myvar)_PCFILES}`
        m4_toupper(myvar)_CFLAGS="$temp_CFLAGS ${m4_toupper(myvar)_CFLAGS}"
        m4_toupper(myvar)_LFLAGS="$temp_LFLAGS ${m4_toupper(myvar)_LFLAGS}"
      fi

      # setup XYZ_EXPORT symbol for library users
      libexport_attribute=
      if test "$enable_shared" = yes ; then
        case $build_os in
          cygwin* | mingw* | msys* | cegcc* )
            libexport_attribute="__declspec(dllimport)"
            if test "$enable_static" = yes ; then
              AC_MSG_ERROR([Cannot do DLL and static LIB builds simultaneously. Do not add --enable-static without --disable-shared.])
            fi
          ;;
        esac
      fi
      AC_DEFINE_UNQUOTED(m4_toupper(myvar)_EXPORT, [$libexport_attribute], [Library Visibility Attribute])

      # add -DXYZ_BUILD to XYZ_CFLAGS
      m4_toupper(myvar)_CFLAGS="${m4_toupper(myvar)_CFLAGS} -D[]m4_toupper(myvar)_BUILD"

      # Define BUILDTOOLS_DEBUG to enable debugging output
      if test "$BUILDTOOLS_DEBUG" = 1 ; then
        AC_MSG_NOTICE([m4_toupper(myvar)_LFLAGS_NOPC: "${m4_toupper(myvar)_LFLAGS_NOPC}"])
        AC_MSG_NOTICE([m4_toupper(myvar)_CFLAGS_NOPC: "${m4_toupper(myvar)_CFLAGS_NOPC}"])
        AC_MSG_NOTICE([adding "${m4_toupper(myvar)_PCFILES}"])
        AC_MSG_NOTICE([m4_toupper(myvar)_LFLAGS: "${m4_toupper(myvar)_LFLAGS}"])
        AC_MSG_NOTICE([m4_toupper(myvar)_CFLAGS: "${m4_toupper(myvar)_CFLAGS}"])
      fi
    ])
])


#                              COIN_FINALIZE                              #

# This macro should be called at the very end of the configure.ac file.
# It creates the output files (by using AC_OUTPUT), and might do some other
# things (such as generating links to data files in a VPATH configuration).
# It also prints the "success" message.
# Note: If the variable coin_skip_ac_output is set to yes, then no output
# files are written.

AC_DEFUN([AC_COIN_FINALIZE],
[
  AC_OUTPUT

dnl AC_MSG_NOTICE([In case of trouble, first consult the documentation at https://coin-or.github.io/user_introduction])
  AC_MSG_NOTICE([Configuration of $PACKAGE_NAME successful])
])

