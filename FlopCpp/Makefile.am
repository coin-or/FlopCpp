# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

include BuildTools/Makemain.inc

########################################################################
#                          Subdirectories                              #
########################################################################

SUBDIRS = src

########################################################################
#                           Extra Targets                              #
########################################################################

test: all
	cd unittest; $(MAKE) test
	cd test; $(MAKE) test

unitTest: test

# Doxygen documentation

clean-local: clean-doxygen-docs
	if test -r test/Makefile; then cd test; $(MAKE) clean; fi
	if test -r unittest/Makefile; then cd unittest; $(MAKE) clean; fi

distclean-local:
	if test -r test/Makefile; then cd test; $(MAKE) distclean; fi
	if test -r unittest/Makefile; then cd unittest; $(MAKE) distclean; fi

install-exec-local: install-doc

uninstall-local: uninstall-doc uninstall-doxygen-docs

.PHONY: test unitTest

########################################################################
#                         Installation of .pc files                    #
########################################################################

pkgconfiglib_DATA = flopcpp.pc
