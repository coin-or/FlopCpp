CoinDir		= ..
FlopcDir	= .

CXX     =  g++
LD  	=  g++ -o
OPTFLAG	=  -g

###############################################################################
LIBLDFLAGS = -shared
LIBEXT = .so
SHFLAG = -fpic
SHLINKPREFIX = -Wl,-rpath,

CXXFLAGS += $(SHFLAG)

ifeq ($(notdir $(CXX)),g++)
	CXXFLAGS += -pedantic 
	CXXFLAGS += -Wall
	CXXFLAGS += -Wpointer-arith
	CXXFLAGS += -Wcast-qual
	CXXFLAGS += -Wcast-align
	CXXFLAGS += -Wwrite-strings
	CXXFLAGS += -Wconversion
endif

CXXFLAGS += $(OPTFLAG)  

DEPFLAGS += -I$(FlopcDir)/include
DEPFLAGS += -I$(CoinDir)/include

###############################################################################

TARGETDIR = $(FlopcDir)/lib
DEPDIR = $(FlopcDir)/dep

###############################################################################

LIBSRC = $(filter-out %Test.cpp  %flux.cpp, $(shell /bin/ls *.cpp))
LIBOBJ = $(addprefix $(TARGETDIR)/, $(LIBSRC:.cpp=.o))
LIBDEP = $(addprefix $(DEPDIR)/, $(LIBSRC:.cpp=.d))

###############################################################################

.PHONY: default libflopc doc clean

default: libflopc

###############################################################################

$(TARGETDIR)/%.o : %.cpp ${DEPDIR}/%.d
	@echo Compiling $*.cpp
	@if test ! -e ${DEPDIR}/$*.d ; then \
	    echo ; \
	    echo "   ${DEPDIR}/$*.d is missing."; \
	    echo "   Probably a header file was not found when make examined";\
	    echo "   $*.cpp in an attempt to create that dependency."; \
	    echo ; \
	    exit 1; \
	fi
	@mkdir -p $(TARGETDIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@

${DEPDIR}/%.d : %.cpp
	@echo Creating dependency $*.d
	@mkdir -p ${DEPDIR}
	@rm -f $*.d $*.dd
	g++ -MM $(DEPFLAGS) $< > $*.dd
	sed -e "s|$*.o|$(DEPDIR)/$*.d $(TARGETDIR)/$*.o|g" $*.dd > $@
	@rm -f $*.dd

libflopc :  $(TARGETDIR)/libflopc$(LIBEXT)

$(TARGETDIR)/libflopc$(LIBEXT): $(LIBOBJ)
	@echo Creating Library   $(notdir $@)
	@mkdir -p $(TARGETDIR)
	@rm -f $@
	$(LD) $@ $(LIBLDFLAGS) $(LIBOBJ)
	@echo "Library done!"

doc :
	@echo Running doxygen to create documentation
	doxygen doxygenconfig

clean :
	@rm -rf $(DEPDIR)
	@rm -rf $(FlopcDir)/*.o

test :
	cd Samples
	make
	Samples/runall

###############################################################################

-include $(LIBDEP) $(TESTDEP)
