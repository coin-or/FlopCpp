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

# This is for libtool
libFlopCpp_la_LDFLAGS = $(LT_LDFLAGS)

########################################################################
#                            Additional flags                          #
########################################################################

# Here list all include flags, relative to this "srcdir" directory.  This
# "cygpath" stuff is necessary to compile with native compilers on Windows.
# "top_srcdir" refers to the basic directory for the main package that is
# being compiled.
AM_CPPFLAGS = $(FLOPCPP_CFLAGS)

# This line is necessary to allow VPATH compilation
DEFAULT_INCLUDES = -I. -I`$(CYGPATH_W) $(srcdir)` 

########################################################################
#                Headers that need to be installed                     #
########################################################################

# Here list all the header files that are required by a user of the library,
# and that therefore should be installed in 'include/coin'
includecoindir = $(includedir)/coin

includecoin_HEADERS = \
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

install-exec-local:
	$(install_sh_DATA) config_flopcpp.h $(DESTDIR)$(includecoindir)/FlopCppConfig.h

uninstall-local:
	rm -f $(DESTDIR)$(includecoindir)/FlopCppConfig.h
