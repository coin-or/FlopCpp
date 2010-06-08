# Copyright (C) 2006 International Business Machines.
# All Rights Reserved.
# This file is distributed under the Common Public License.

## $Id$

# Author:  Andreas Waechter            IBM    2006-04-13

#############################################################################
#                       Names and other basic things                        #
#############################################################################

AC_PREREQ(2.59)

AC_INIT([FlopCpp],[1.0stable],[flopcpp@list.coin-or.org])

AC_COPYRIGHT([
Copyright 2006 International Business Machines and others.
All Rights Reserved.
This file is part of the open source package Coin which is distributed
under the Common Public License.])

AC_REVISION(0.9)

# List one file in the package so that the configure script can test
# whether the package is actually there
AC_CONFIG_SRCDIR(configure.ac)

# Where should everything be installed by default?  Here, we want it
# to be installed directly in 'bin', 'lib', 'include' subdirectories
# of the directory where configure is run.  The default would be
# /usr/local.
AC_PREFIX_DEFAULT([`pwd`])

#############################################################################
#     We only need automake to generate Makefiles for the distribution      #
#############################################################################

# Initialize automake
AC_COIN_INIT_AUTOMAKE

#############################################################################
#             Check which ThirdParty sources are available                  #
#############################################################################

AC_COIN_THIRDPARTY_SUBDIRS([ASL],[ThirdParty/ASL],[solvers/asl.h])
#AC_COIN_THIRDPARTY_SUBDIRS([Boost],[ThirdParty/boost],[boost/shared_ptr.hpp])
#AC_COIN_THIRDPARTY_SUBDIRS([gtest],[ThirdParty/gtest],[include/gtest/gtest.h])
## User has to give third party librarys
AC_SUBST(BOOSTINCDIR)
AC_SUBST(BOOSTLIB)
BOOSTINCDIR=`cd $srcdir/ThirdParty/boost; pwd`
BOOSTLIB=`cd $srcdir/ThirdParty/boost; pwd`
AC_COIN_HAS_USER_LIBRARY([boost],[BOOST],[boost/shared_ptr.hpp])
AC_SUBST(GTESTINCDIR)
AC_SUBST(GTESTLIB)
GTESTINCDIR=`cd $srcdir/ThirdParty/gtest/include; pwd`
GTESTLIB=`pwd`/ThirdParty/gtest/lib
AC_COIN_THIRDPARTY_SUBDIRS([gtest],[ThirdParty/gtest],[include/gtest/gtest.h])
AC_SUBST(GLOGINCDIR)
AC_SUBST(GLOGLIB)
GLOGINCDIR=`cd $srcdir/ThirdParty/glog/src; pwd`
GLOGLIB=`pwd`/ThirdParty/glog
AC_COIN_THIRDPARTY_SUBDIRS([glog],[ThirdParty/glog],[src/googletest.h])

#############################################################################
#                  Check which subprojects are there                        #
#############################################################################

AC_COIN_MAIN_SUBDIRS(CoinUtils)
AC_COIN_MAIN_SUBDIRS(Clp)
AC_COIN_MAIN_SUBDIRS(Osi)
AC_COIN_MAIN_SUBDIRS(Cgl)
AC_COIN_MAIN_SUBDIRS(Cbc)
AC_COIN_MAIN_SUBDIRS(Smi)
AC_COIN_MAIN_SUBDIRS(FlopCpp)

##############################################################################
#                   Finishing up by writing all the output                   #
##############################################################################

# Here list all the files that configure should create (except for the
# configuration header file)
AC_CONFIG_FILES([Makefile])

# Finally, we let configure write all the output...
AC_COIN_FINALIZE
