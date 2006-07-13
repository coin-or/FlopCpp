# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Common Public License.

## $Id$

# Author:  Andreas Waechter           IBM    2006-04-13

AUTOMAKE_OPTIONS = foreign

########################################################################
#                          Subdirectories                              #
########################################################################

SUBDIRS = src

# We don't want to compile the test subdirectory, unless the test target is
# specified.  But we need to list it as subdirectory to make sure that it is
# included in the tarball

if ALWAYS_FALSE
  SUBDIRS += test
endif

########################################################################
#             Additional files to be included in tarball               #
########################################################################

# Here we need include all files that are not mentioned in other Makefiles

EXTRA_DIST = \
    examples/cross.cpp \
    examples/multiProduct.cpp \
    examples/train.cpp \
    examples/aircraft.cpp \
    examples/cuttingStock.cpp \
    examples/train.dat \
    examples/ampl.cpp   \
    examples/fixed.dat  \
    examples/transport.cpp \
    examples/bid.cpp    \
    examples/gapmin.cpp   \
    examples/sodoku.cpp   \
    examples/xbsl.cpp \
    examples/coex.cpp   \
    examples/magic.cpp  \
    examples/stochbenders.cpp \
    examples/coexx.cpp   \
    examples/mine.cpp  \
    examples/tap.cpp \
	examples/Makefile.in

########################################################################
#                           Extra Targets                              #
########################################################################

test: all
	cd test; make test

unitTest: test

clean-local:
	if test -r test/Makefile; then cd test; make clean; fi

distclean-local:
	if test -r test/Makefile; then cd test; make distclean; fi

.PHONY: test unitTest

########################################################################
#                  Installation of the addlibs file                    #
########################################################################

addlibsfile = flopcpp_addlibs.txt

install-exec-local:
	$(install_sh_DATA) $(addlibsfile) $(DESTDIR)$(libdir)/$(addlibsfile)

uninstall-local:
	rm -f $(DESTDIR)$(libdir)/$(addlibsfile)

########################################################################
#                         Maintainer Stuff                             #
########################################################################

# Files that are generated and should be cleaned with make distclean
DISTCLEANFILES =

include ../BuildTools/Makemain.inc