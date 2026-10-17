# Copyright (C) 2006 International Business Machines and others.
# All Rights Reserved.
# This file is distributed under the Eclipse Public License.

include $(top_srcdir)/BuildTools/Makemain.inc

noinst_PROGRAMS = unitTest

unitTest_SOURCES = booleanTest.cpp constantTest.cpp indexTest.cpp setTest.cpp TestBed.cpp TestItem.cpp unitTest.cpp

unitTest_LDADD = ../src/libFlopCpp.la $(FLOPCPPUNITTEST_LFLAGS)

AM_CPPFLAGS = -I$(srcdir)/../src -I../src $(FLOPCPPUNITTEST_CFLAGS)
AM_LDFLAGS = $(LT_LDFLAGS)

test: unitTest$(EXEEXT)
	./unitTest$(EXEEXT)

.PHONY: test
