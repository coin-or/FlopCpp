# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Common Public License.

## $Id$

# Author:  Andreas Waechter           IBM    2006-04-13

AUTOMAKE_OPTIONS = foreign

########################################################################
#                               libFlopCpp                             #
########################################################################

# Name of the library compiled in this directory.  We want it to be installed
# in the 'lib' directory
lib_LTLIBRARIES = libFlopCpp.la

# List all source files for this library, including headers
libFlopCpp_la_SOURCES = \
	flopc.hpp \
	FlopCppConfig.h \
	MP_boolean.cpp MP_boolean.hpp \
	MP_constant.cpp MP_constant.hpp \
	MP_constraint.cpp MP_constraint.hpp \
	MP_data.cpp MP_data.hpp \
	MP_domain.cpp MP_domain.hpp \
	MP_expression.cpp MP_expression.hpp \
	MP_index.cpp MP_index.hpp \
	MP_model.cpp MP_model.hpp \
	MP_set.cpp MP_set.hpp \
	MP_utilities.cpp MP_utilities.hpp \
	MP_variable.cpp MP_variable.hpp 

# This is for libtool (on Windows)
#libFlopCpp_la_LDFLAGS = $(LT_LDFLAGS)

########################################################################
#                        cbc-generic program                           #
########################################################################
AM_CPPFLAGS =

# Name of the executable compiled in this directory.  We want it to be
# installed in the 'bin' directory
#bin_PROGRAMS = cbc-generic

# List all source files for this executable, including headers
#cbc_generic_SOURCES = \
#	CbcMain.cpp \
#	CbcParam.cpp CbcParam.hpp \
#	unitTest.cpp

# The following file is not to be distributed, since it is "borrowed" from Clp
#nodist_cbc_generic_SOURCES = CbcOrClpParam.cpp

#cbc_generic_LDADD = libCbc.la \
#	$(CGLOBJDIR)/src/libCgl.la \
#	$(OSIOBJDIR)/src/OsiCbc/libOsiCbc.la
#AM_CPPFLAGS = -I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiCbc`

# List all additionally required solver and Osi libraries
#if COIN_HAS_CLP
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(CLPSRCDIR)/src` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiClp` \
#	-I`$(CYGPATH_W) $(CLPOBJDIR)/inc`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiClp/libOsiClp.la \
#	$(CLPOBJDIR)/src/libClp.la
#  LIBS  += `cat $(CLPOBJDIR)/clp_addlibs.txt`
#endif

#if COIN_HAS_CPX
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(CPXINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiCpx`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiCpx/libOsiCpx.la
#endif

#if COIN_HAS_DYLP
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(DYLPSRCDIR)/src/DylpStdLib` \
#        -I`$(CYGPATH_W) $(DYLPSRCDIR)/src/Dylp` \
#        -I`$(CYGPATH_W) $(DYLPOBJDIR)/inc` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiDylp`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiDylp/libOsiDylp.la \
#	$(DYLPOBJDIR)/src/Dylp/libDylp.la
#  LIBS += `cat $(DYLPOBJDIR)/dylp_addlibs.txt`
#endif

#if COIN_HAS_FMP
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiFmp`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiFmp/libOsiFmp.la
#endif

#if COIN_HAS_GLPK
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(GLPKINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiGlpk`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiGlpk/libOsiGlpk.la
#endif

#if COIN_HAS_MSK
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(MSKINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiMsk`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiMsk/libOsiMsk.la
#endif

#if COIN_HAS_OSL
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(OSLINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiOsl`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiOsl/libOsiOsl.la
#endif

#if COIN_HAS_SPX
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(SPXINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiSpx`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiSpx/libOsiSpx.la
#endif

#if COIN_HAS_SYM
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(SYMSRCDIR)/src` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiSym`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiSym/libOsiSym.la \
#	$(SYMOBJDIR)/src/libSym.la
#  LIBS += `cat $(SYMOBJDIR)/sym_addlibs.txt`
#endif

#if COIN_HAS_VOL
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(VOLSRCDIR)/src` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiVol`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiVol/libOsiVol.la \
#	$(VOLOBJDIR)/src/libVol.la
#  LIBS += `cat $(VOLOBJDIR)/vol_addlibs.txt`
#endif

#if COIN_HAS_XPR
#  AM_CPPFLAGS += -I`$(CYGPATH_W) $(XPRINCDIR)` \
#	-I`$(CYGPATH_W) $(OSISRCDIR)/src/OsiXpr`
#  cbc_generic_LDADD += $(OSIOBJDIR)/src/OsiXpr/libOsiXpr.la
#endif

# Finally the Osi Coin library (many of the above need it)
#cbc_generic_LDADD += $(OSIOBJDIR)/src/libOsi.la \
#	$(COINUTILSOBJDIR)/src/libCoinUtils.la
#LIBS += `cat $(OSIOBJDIR)/osi_addlibs.txt` \
#	`cat $(COINUTILSOBJDIR)/coinutils_addlibs.txt` $(ADDLIBS)

# Finally, the -rpath flag is used by libtool to make sure that the shared
# library is found (in the lib install directory) when we are using dynamic
# libraries.
#cbc_generic_LDFLAGS = -rpath $(libdir)

########################################################################
#                            cbc program                               #
########################################################################

# Name of the executable compiled in this directory.  We want it to be
# installed in the 'bin' directory
#bin_PROGRAMS += cbc

# List all source files for this executable, including headers
#cbc_SOURCES = \
#	CoinSolve.cpp \
#	unitTestClp.cpp

# The following file is not to be distributed, since it is "borrowed" from Clp
#nodist_cbc_SOURCES = CbcOrClpParam.cpp

# Additional COIN libraries
#cbc_LDADD = libCbc.la \
#	$(CGLOBJDIR)/src/libCgl.la \
#	$(OSIOBJDIR)/src/OsiClp/libOsiClp.la \
#	$(OSIOBJDIR)/src/libOsi.la \
#	$(CLPOBJDIR)/src/libClp.la \
#	$(COINUTILSOBJDIR)/src/libCoinUtils.la

# Finally, the -rpath flag is used by libtool to make sure that the shared
# library is found (in the lib install directory) when we are using dynamic
# libraries.
#cbc_LDFLAGS = -rpath $(libdir)

# If we have the Ampl solver library, we need to add additional things
#if COIN_HAS_ASL
#cbc_SOURCES += Cbc_ampl.cpp Cbc_ampl.h
#cbc_LDADD += $(ASLLIB)
#AM_CPPFLAGS += $(ASL_CPPFLAGS)
#endif

########################################################################
#                            Additional flags                          #
########################################################################

# Here list all include flags, relative to this "srcdir" directory.  This
# "cygpath" stuff is necessary to compile with native compilers on Windows.
# "top_srcdir" refers to the basic directory for the main package that is
# being compiled.
AM_CPPFLAGS += \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglClique` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglDuplicateRow` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglFlowCover` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglGomory` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglKnapsackCover` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglMixedIntegerRounding` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglMixedIntegerRounding2` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglOddHole` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglPreProcess` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglProbing` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglRedSplit` \
	-I`$(CYGPATH_W) $(CGLSRCDIR)/src/CglTwomir` \
	-I`$(CYGPATH_W) $(COINUTILSSRCDIR)/src` \
	-I`$(CYGPATH_W) $(COINUTILSOBJDIR)/inc` \
	-I`$(CYGPATH_W) $(OSISRCDIR)/src` \
	-I`$(CYGPATH_W) $(OSIOBJDIR)/inc`

# List additional defines
AM_CPPFLAGS += -DCOIN_NO_CLP_MESSAGE -DUSE_FLOPCPPCONFIG

# This line is necessary to allow VPATH compilation with MS compilers
# on Cygwin
DEFAULT_INCLUDES = -I. -I`$(CYGPATH_W) $(srcdir)` -I$(top_builddir)/inc

########################################################################
#                Headers that need to be installed                     #
########################################################################

# Here list all the header files that are required by a user of the library,
# and that therefore should be installed in 'install'
include_HEADERS = \
	../inc/config_flopcpp.h \
	flopc.hpp \
	MP_boolean.hpp \
	MP_constant.hpp \
	MP_constraint.hpp \
	MP_data.hpp \
	MP_domain.hpp \
	MP_expression.hpp \
	MP_index.hpp \
	MP_model.hpp \
	MP_set.hpp \
	MP_utilities.hpp \
	MP_variable.hpp

#############################################################################
# Create the Config.h file that always defines HAVE_CONFIG_H and install it #
#############################################################################

# You only need to adapt the following line
ConfigHeader = FlopCppConfig.h

install-exec-local:
	echo "#ifndef HAVE_CONFIG_H" >bla
	echo "#define HAVE_CONFIG_H" >>bla
	echo "#endif" >> bla
	cat $(srcdir)/$(ConfigHeader) >> bla
	$(install_sh_DATA) bla $(DESTDIR)$(includedir)/$(ConfigHeader)
	rm -f bla

uninstall-local:
	rm -f $(DESTDIR)$(includedir)/$(ConfigHeader)
