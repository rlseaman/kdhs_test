#///////////////////////////////////////////////////////////////////////////////
#//
#//  MONSOON Makefile.Linux
#//  Copyright © 2002 AURA Inc. All rights reserved.
#//
#///////////////////////////////////////////////////////////////////////////////

#///////////////////////////////////////////////////////////////////////////////
#// primary target(s)
#///////////////////////////////////////////////////////////////////////////////
DNAME=_netw
NAME = dhsNETW
TCLP = dhsTcl
UTIL = dhsUtil
DCA = dca
HERE := $(shell pwd)

#///////////////////////////////////////////////////////////////////////////////
#// secondary dependencies
#///////////////////////////////////////////////////////////////////////////////
LIBNAME = lib$(NAME)
LIBTCLP = lib$(TCLP)
LIBUTIL = lib$(UTIL)

TCLP_VERSION := $(shell grep 'define _$(TCLP)_H_' $(HERE)/$(TCLP).h | awk '{print $$3}')
UTIL_VERSION := $(shell grep 'define _dhsImpl_H_' $(HERE)/dhsImplementationSpecifics.h | awk '{print $$3}')

SHAREDTCLPLIB = $(LIBNAME)Tcl.so.$(TCLP_VERSION)
SHAREDUTILLIB = $(LIBNAME).so.$(UTIL_VERSION)
STATICTCLPLIB = $(LIBNAME)Tcl.a.$(TCLP_VERSION)
STATICUTILLIB = $(LIBNAME).a.$(UTIL_VERSION)

#///////////////////////////////////////////////////////////////////////////////
#// dependencies
#///////////////////////////////////////////////////////////////////////////////
MONSOON_HOME  := $(shell echo $$MONSOON_HOME)
MONSOON_BIN    = $(MONSOON_HOME)/bin
MONSOON_CFG    = $(MONSOON_HOME)/cfg
MONSOON_DATA   = $(MONSOON_HOME)/data
MONSOON_ETC    = $(MONSOON_HOME)/etc
MONSOON_INC    = $(MONSOON_HOME)/inc
MONSOON_LIB    = $(MONSOON_HOME)/lib
MONSOON_SBIN   = $(MONSOON_HOME)/sbin
MONSOON_SRC    = $(MONSOON_HOME)/src
MONSOON_TCL    = $(MONSOON_HOME)/tcl
MONSOON_DHSSRC = $(MONSOON_SRC)/Util/dhsLibraries
MONSOON_DHSLIB = $(MONSOON_LIB)/dhsLibs

#///////////////////////////////////////////////////////////////////////////////
#// includes, flags and libraries
#///////////////////////////////////////////////////////////////////////////////
CINCS  = -I$(HERE) -I$(MONSOON_INC) -I/usr/local/include
CFLAGS = -O2 -Wall -DDEBUGTCL -DXLONG=int -D__PTIME__ -DDEBUG -g

PLEV := $(shell grep TCL_MINOR_VERSION /usr/include/tcl.h | awk '{print $$3}')
ifeq ($(PLEV), 3)
 CFLAGS += -DTCL83
endif
ifeq ($(PLEV), 4)
 CFLAGS += -DTCL84
endif

OSTYP := $(shell uname -m)
ifeq ($(OSTYP),x86_64)
 CLIBS = -L/usr/lib64 -lc -ldl
else
 CLIBS = -L/usr/lib -lc -ldl
endif

#///////////////////////////////////////////////////////////////////////////////
#// list of source and include files
#///////////////////////////////////////////////////////////////////////////////
TCLPSRC := $(shell ls $(UTIL)*.c $(DCA)*.c $(TCLP)*.c)
UTILSRC := $(shell ls $(UTIL)*.c $(DCA)*.c)
TCLPINC := $(shell ls $(TCLP)*.h)
UTILINC := $(shell ls ../$(UTIL)*.h)

#///////////////////////////////////////////////////////////////////////////////
#// help: target(s)
#///////////////////////////////////////////////////////////////////////////////
help: HELP
HELP:
	clear
	@echo ""
	@echo "$(LIBUTIL) $(UTIL_VERSION) Makefile.Linux"
	@echo "(c) 2002, AURA Inc. All rights reserved."
	@echo ""
	@echo "The following targets are available:"
	@echo ""
	@echo "   all clean help install"
	@echo ""
	@echo "To re-build the library, use: % make everything"
	@echo ""

#///////////////////////////////////////////////////////////////////////////////
#// clean: target(s)
#///////////////////////////////////////////////////////////////////////////////
clean_shared:
	rm -rf SharedTclp SharedUtil $(HERE)/*.o $(HERE)/*.so*
	rm -rf $(SHAREDTCLPLIB) $(SHAREDUTILLIB)
	rm -rf $(MONSOON_DHSLIB)/$(LIBTCLP)* $(MONSOON_DHSLIB)/$(LIBUTIL)*
clean_static:
	rm -rf StaticTclp StaticUtil $(HERE)/*.o $(HERE)/*.a*
	rm -rf $(STATICTCLPLIB) $(STATICUTILLIB)
	rm -rf $(MONSOON_DHSLIB)/$(LIBTCLP)* $(MONSOON_DHSLIB)/$(LIBUTIL)*
clean: clean_static clean_shared

#///////////////////////////////////////////////////////////////////////////////
#// compile: target(s)
#///////////////////////////////////////////////////////////////////////////////
compile_shared: clean_shared SharedUtil SharedTclp $(TCLPSRC:%.c=SharedTclp/%.o) $(UTILSRC:%.c=SharedUtil/%.o)
compile_static: clean_static StaticUtil StaticTclp $(TCLPSRC:%.c=StaticTclp/%.o) $(UTILSRC:%.c=StaticUtil/%.o)
compile: compile_static compile_shared

#///////////////////////////////////////////////////////////////////////////////
#// all: target(s)
#///////////////////////////////////////////////////////////////////////////////
all_shared: compile_shared $(SHAREDUTILLIB) $(SHAREDTCLPLIB)
all_static: compile_static $(STATICUTILLIB) $(STATICTCLPLIB)
all: all_static all_shared

#///////////////////////////////////////////////////////////////////////////////
#// everything: target(s)
#///////////////////////////////////////////////////////////////////////////////
everything:
	@/bin/echo -e "\n****\n****   making ${LIBNAME}  ****\n****\n"
	gmake clean
	gmake all
	gmake install
	gmake dcalib32.a dhslib32.a

#///////////////////////////////////////////////////////////////////////////////
#// install: target(s)
#///////////////////////////////////////////////////////////////////////////////
install_shared: all_shared
	rm -rf $(MONSOON_LIB)/$(LIBTCLP).so* $(MONSOON_LIB)/$(LIBUTIL).so*
	cp $(SHAREDTCLPLIB) $(SHAREDUTILLIB) $(MONSOON_DHSLIB)/
	ln -sf ../dhsLibs/$(SHAREDTCLPLIB) $(MONSOON_DHSLIB)/$(LIBNAME)Tcl.so
	ln -sf ../dhsLibs/$(SHAREDUTILLIB) $(MONSOON_DHSLIB)/$(LIBNAME).so
	ln -sf ../lib/dhsLibs/$(LIBNAME)Tcl.so $(MONSOON_LIB)/$(LIBTCLP).so
	ln -sf ../lib/dhsLibs/$(LIBNAME).so $(MONSOON_LIB)/$(LIBUTIL).so
install_static: all_static
	rm -rf $(MONSOON_LIB)/$(LIBTCLP).a* $(MONSOON_LIB)/$(LIBUTIL).a*
	cp $(STATICTCLPLIB) $(STATICUTILLIB) $(MONSOON_DHSLIB)/
	ln -sf ../dhsLibs/$(STATICTCLPLIB) $(MONSOON_DHSLIB)/$(LIBNAME)Tcl.a
	ln -sf ../dhsLibs/$(STATICUTILLIB) $(MONSOON_DHSLIB)/$(LIBNAME).a
	ln -sf ../lib/dhsLibs/$(LIBNAME)Tcl.a $(MONSOON_LIB)/$(LIBTCLP).a
	ln -sf ../lib/dhsLibs/$(LIBNAME).a $(MONSOON_LIB)/$(LIBUTIL).a
install_include: $(MONSOON_INC) $(MONSOON_DHSLIB)
	rm -rf $(MONSOON_INC)/$(TCLP).h $(MONSOON_INC)/$(UTIL).h
	/bin/ln -sf ../src/Util/dhsLibraries/$(DNAME)/$(TCLP).h $(MONSOON_INC)/$(TCLP).h
	/bin/ln -sf ../src/Util/dhsLibraries/$(UTIL).h $(MONSOON_INC)/$(UTIL).h
	/bin/ln -sf ../src/Util/dhsLibraries/$(DNAME)/$(IMPL).h $(MONSOON_INC)/$(IMPL).h
install: install_include install_static install_shared
	@touch ../$(shell /bin/date +%Y%m%d)$(DNAME)

#///////////////////////////////////////////////////////////////////////////////
#// leave this stuff alone
#///////////////////////////////////////////////////////////////////////////////
$(MONSOON_DHSLIB):
	mkdir $(MONSOON_DHSLIB)

$(UTIL).h:
	ln -sf ../$(UTIL).h ./$(UTIL).h

$(SHAREDTCLPLIB): $(TCLPSRC:%.c=SharedTclp/%.o)
	ld -shared -o $@ $? $(CLIBS)
SharedTclp/%.o: %.c $(TCLPINC)
	gcc $(CINCS) $(CFLAGS) -fpic -shared -c $< -o $@
SharedTclp:
	mkdir $@
	chmod 777 $@

$(SHAREDUTILLIB): $(UTILSRC:%.c=SharedUtil/%.o)
	ld -shared -o $@ $? $(CLIBS)
SharedUtil/%.o: %.c $(UTILINC)
	gcc $(CINCS) $(CFLAGS) -fpic -shared -c $< -o $@
SharedUtil:
	mkdir $@
	chmod 777 $@

$(STATICTCLPLIB): $(TCLPSRC:%.c=StaticTclp/%.o)
	ar rv $@ $?
StaticTclp/%.o: %.c $(TCLPINC)
	gcc $(CINCS) $(CFLAGS) -c $< -o $@
StaticTclp:
	mkdir $@
	chmod 777 $@

$(STATICUTILLIB): $(UTILSRC:%.c=StaticUtil/%.o)
	ar rv $@ $?
StaticUtil/%.o: %.c $(UTILINC)
	gcc $(CINCS) $(CFLAGS) -c $< -o $@
StaticUtil:
	mkdir $@
	chmod 777 $@

#///////////////////////////////////////////////////////////////////////////////
#// DCA build(s)
#///////////////////////////////////////////////////////////////////////////////
DCAFLAGS = -Wall -DDEBUG -g -c -D__MNSNERR__ -D_MNSN_STD_HDR_ -D_shmUtil_H_ -D__USE_MISC -DXLONG=int
DCAINCS  = -I.. -I$(HERE) -I$(MONSOON_INC) -I/home/ndhs/include
DCASRCS := $(shell ls $(DCA)*.c)
dcalib.a: $(UTILINC) $(DCASRCS)
	gcc $(DCAFLAGS) $(DCAINCS) $(DCASRCS)
	ar -r $@ $(DCA)*.o
dcalib32.a: $(UTILINC) $(DCASRCS)
	gcc -m32 $(DCAFLAGS) $(DCAINCS) $(DCASRCS)
	ar -r $@ $(DCA)*.o

#///////////////////////////////////////////////////////////////////////////////
#// DHS build(s)
#///////////////////////////////////////////////////////////////////////////////
DHSFLAGS = -Wall -DDEBUG -g -c -D__USE_MISC -DXLONG=int
DHSINCS  = -I.. -I$(HERE) -I$(MONSOON_INC) -I/home/ndhs/include
DHSSRCS := $(shell ls $(UTIL)*.c)
dhslib.a: $(UTILINC) $(DHSSRCS)
	gcc $(DHSFLAGS) $(DHSINCS) $(DHSSRCS)
	ar -r $@ $(UTIL)*.o
dhslib32.a: $(UTILINC) $(DHSSRCS)
	gcc -m32 $(DHSFLAGS) $(DHSINCS) $(DHSSRCS)
	ar -r $@ $(UTIL)*.o
