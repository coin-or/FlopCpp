# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

include $(top_srcdir)/BuildTools/Makemain.inc

if COIN_HAS_CBC

noinst_PROGRAMS = unitTest

unitTest_SOURCES = unitTest.cpp

unitTest_LDADD = ../src/libFlopCpp.la $(FLOPCPPTEST_LFLAGS)

AM_CPPFLAGS = -I$(srcdir)/../src -I../src $(FLOPCPPTEST_CFLAGS)
AM_LDFLAGS = $(LT_LDFLAGS)

test: unitTest$(EXEEXT)
	./unitTest$(EXEEXT)

else

test:
	echo "Need CBC to run test."

endif

.PHONY: test
