# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

#############################################################################
#                       Names and other basic things                        #
#############################################################################

AC_INIT([FlopCpp],[devel],[https://github.com/coin-or/FlopCpp/issues/new],[coin-or-flopcpp],[https://github.com/coin-or/FlopCpp])

AC_COPYRIGHT([
Copyright 2006 International Business Machines and others.
All Rights Reserved.
This file is part of the open source package FlopC++ which is distributed
under the Eclipse Public License.])

# List one file in the package so that the configure script can test
# whether the package is actually there
AC_CONFIG_SRCDIR(src/flopc.hpp)

# Do some initialization work (version numbers, change prefix default, ...)
AC_COIN_INITIALIZE

#############################################################################
#                         Standard build tool stuff                         #
#############################################################################

# Get the name of the C++ compiler and appropriate compiler options
AC_COIN_PROG_CXX

# This is a C++ package, set the language accordingly.
AC_LANG_PUSH(C++)

# Initialize libtool
AC_COIN_PROG_LIBTOOL

# set RPATH_FLAGS to the compiler link flags required to hardcode location
# of the shared objects (expanded_libdir is set somewhere in configure before)
# (used in the examples)
AC_COIN_RPATH_FLAGS([$expanded_libdir])

#############################################################################
#                              COIN-OR components                           #
#############################################################################

AC_COIN_CHK_PKG(Osi,[FlopCppLib FlopCppUnitTest FlopCppTest])
if test $coin_has_osi != yes ; then
  AC_MSG_ERROR([Required package Osi not available.])
fi

# Cbc is used by the test, Clp and Cbc by the examples
AC_COIN_CHK_PKG(Cbc,[FlopCppTest],[osi-cbc])
AC_COIN_CHK_PKG(Clp,,[osi-clp])

##############################################################################
#                   VPATH links for example input files                      #
##############################################################################

# In case this is a VPATH configuration we need to make sure that the
# input files for the examples are available in the VPATH directory.

AC_COIN_VPATH_LINK(examples/fixed.dat)
AC_COIN_VPATH_LINK(examples/train.dat)
AC_COIN_VPATH_LINK(examples/aircraft.cpp)
AC_COIN_VPATH_LINK(examples/ampl.cpp)
AC_COIN_VPATH_LINK(examples/bid.cpp)
AC_COIN_VPATH_LINK(examples/CEPFlopCppModel.cpp)
AC_COIN_VPATH_LINK(examples/coex.cpp)
AC_COIN_VPATH_LINK(examples/coexx.cpp)
AC_COIN_VPATH_LINK(examples/cross.cpp)
AC_COIN_VPATH_LINK(examples/cuttingStock.cpp)
AC_COIN_VPATH_LINK(examples/gapmin.cpp)
AC_COIN_VPATH_LINK(examples/magic.cpp)
AC_COIN_VPATH_LINK(examples/mine.cpp)
AC_COIN_VPATH_LINK(examples/multiProduct.cpp)
AC_COIN_VPATH_LINK(examples/stampl2.cpp)
AC_COIN_VPATH_LINK(examples/stampl.cpp)
AC_COIN_VPATH_LINK(examples/stochbenders.cpp)
AC_COIN_VPATH_LINK(examples/sudoku.cpp)
AC_COIN_VPATH_LINK(examples/tap.cpp)
AC_COIN_VPATH_LINK(examples/train.cpp)
AC_COIN_VPATH_LINK(examples/transport.cpp)
AC_COIN_VPATH_LINK(examples/xbsl.cpp)
AC_COIN_VPATH_LINK(examples/runall)
AC_COIN_VPATH_LINK(examples/runone)

##############################################################################
#                   Finishing up by writing all the output                   #
##############################################################################

# Doxygen documentation
AC_COIN_DOXYGEN([CoinUtils Osi Clp Cgl Cbc])

# Finalize libs and cflags.
AC_COIN_FINALIZE_FLAGS([FlopCppLib FlopCppUnitTest FlopCppTest])

# Here list all the files that configure should create (except for the
# configuration header file)
AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 src/Makefile
                 test/Makefile
                 unittest/Makefile
                 flopcpp.pc])

AC_CONFIG_FILES([doxydoc/doxygen.conf])

# Here put the location and name of the configuration header file
AC_CONFIG_HEADERS([src/config.h src/config_flopcpp.h])

# Finally, we let configure write all the output...
AC_COIN_FINALIZE
