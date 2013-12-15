# Copyright (C) 2006 International Business Machines.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

## $Id: configure.ac 369 2006-06-14 01:19:26Z andreasw $

# Author:  Andreas Waechter            IBM    2006-04-13

#############################################################################
#                       Names and other basic things                        #
#############################################################################

AC_PREREQ(2.59)

AC_INIT([FlopCpp],[trunk],[flopcpp@list.coin-or.org])

AC_COPYRIGHT([
Copyright 2006 International Business Machines and others.
All Rights Reserved.
This file is part of the open source package FlopC++ which is distributed
under the Common Public License.])

# List one file in the package so that the configure script can test
# whether the package is actually there
AC_CONFIG_SRCDIR(src/flopc.hpp)

# Where should everything be installed by default?  Here, we want it
# to be installed directly in 'bin', 'lib', 'include' subdirectories
# of the directory where configure is run.  The default would be
# /usr/local.
AC_PREFIX_DEFAULT([`pwd`])

#############################################################################
#                         Standard build tool stuff                         #
#############################################################################

# Get the system type
AC_CANONICAL_BUILD

# If this project depends on external projects, the Externals file in
# the source root directory contains definition of where to find those
# externals.  The following macro ensures that those externals are
# retrieved by svn if they are not there yet.
AC_COIN_PROJECTDIR_INIT(FlopCpp)

# Check if user wants to produce debugging code
AC_COIN_DEBUG_COMPILE(FlopCpp)

# Get the name of the C++ compiler and appropriate compiler options
AC_COIN_PROG_CXX

# Initialize automake and libtool
AC_COIN_INIT_AUTO_TOOLS

#############################################################################
#                              COIN components                              #
#############################################################################

AC_COIN_CHECK_PACKAGE(Osi, [osi], [FlopCpp])
if test $coin_has_osi != yes ; then
  AC_MSG_ERROR([Required package Osi not available.])
fi
AC_COIN_CHECK_PACKAGE(Cbc, [osi-cbc])
AC_COIN_CHECK_PACKAGE(Clp, [osi-clp])

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

# Here list all the files that configure should create (except for the
# configuration header file)
AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 src/Makefile
                 test/Makefile
                 unittest/Makefile
                 flopcpp.pc
                 flopcpp-uninstalled.pc])

# Here put the location and name of the configuration header file
AC_CONFIG_HEADER([src/config.h src/config_flopcpp.h])

# Finally, we let configure write all the output...
AC_COIN_FINALIZE
