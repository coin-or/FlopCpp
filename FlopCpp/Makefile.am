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

EXTRA_DIST = 

########################################################################
#                           Extra Targets                              #
########################################################################

test: all
	cd unittest; make test
	cd test; make test

unitTest: test

clean-local:
	if test -r test/Makefile; then cd test; make clean; fi
	if test -r unittest/Makefile; then cd unittest; make clean; fi

distclean-local:
	if test -r test/Makefile; then cd test; make distclean; fi
	if test -r unittest/Makefile; then cd unittest; make distclean; fi

.PHONY: test unitTest

########################################################################
#                  Installation of the addlibs file                    #
########################################################################

pkgconfiglibdir = $(libdir)/pkgconfig
pkgconfiglib_DATA = flopcpp.pc

addlibsdir = $(DESTDIR)$(datadir)/coin/doc/FlopCpp

install-data-hook:
	@$(mkdir_p) "$(addlibsdir)"
if COIN_HAS_PKGCONFIG
	PKG_CONFIG_PATH=@COIN_PKG_CONFIG_PATH@ \
	$(PKG_CONFIG) --libs flopcpp > $(addlibsdir)/flopcpp_addlibs.txt
else
if COIN_CXX_IS_CL
	echo "-libpath:`$(CYGPATH_W) @abs_lib_dir@` libFlopCpp.lib @FLOPCPP_LIBS_INSTALLED@" > $(addlibsdir)/flopcpp_addlibs.txt
else
	echo -L@abs_lib_dir@ -lFlopCpp @FLOPCPP_LIBS_INSTALLED@ > $(addlibsdir)/flopcpp_addlibs.txt
endif
endif

uninstall-hook:
	rm -f $(addlibsdir)/flopcpp_addlibs.txt

########################################################################
#                         Maintainer Stuff                             #
########################################################################

# Files that are generated and should be cleaned with make distclean
DISTCLEANFILES =

include BuildTools/Makemain.inc
