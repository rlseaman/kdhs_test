#///////////////////////////////////////////////////////////////////////////////
#//
#//
#///////////////////////////////////////////////////////////////////////////////

# primary dependencies

NAME     =  pxf
VERSION  = 1.0
HERE    := $(shell /bin/pwd)


# secondary dependencies

LIBBASE     = lib$(NAME)
STATICLIB   = $(HERE)/$(LIBBASE).a
SHAREDLIB   = $(HERE)/$(LIBBASE).so.$(VERSION)
MONSOON_INC = $(MONSOON_HOME)/inc

#PLATFORM    = MACOSX
#PVM_ROOT    = /usr/local/pvm/pvm3/
#PVM_ARCH    = DARWIN

PLATFORM    = LINUX
PVM_ROOT    = ../lib/pvm3/
PVM_ARCH    = LINUXI386


# stuff that's precious to keep

.PRECIOUS:
.KEEP_STATE:


# includes, flags and libraries

CINCS   = -I./ -I../include -I/MNSN/soft_dev/inc -I../lib/cfitsio
CFLAGS  = -O2 -m32 -g $(CINCS)


# list of source and include files


SRCS  = pxf.c pxfAux.c pxfSendMetaData.c pxfSendPixelData.c pxfFileOpen.c
OBJS  = pxf.o pxfAux.o pxfSendMetaData.o pxfSendPixelData.o pxfFileOpen.o
INCS  = smCache.h mbus.h

LFLAGS = -L../lib -L$(PVM_ROOT)/lib/$(PVM_ARCH) 
LIBS = ../lib/cfitsio/libcfitsio.a -L../smc -lsmCache -lmbus -lpvm3 -lgpvm3 -lm



# targets

all:  pxf

clean:
	/bin/rm -rf pxf *.o
	/bin/rm -rf .make.state .nse_depinfo

realclean:
	/bin/rm -rf pxf *.o
	/bin/rm -rf .archive/* *~
	/bin/rm -rf .make.state .nse_depinfo

everything:
	make clean
	make all

help: HELP


# Unit test programs to be built.

pxf: $(OBJS)
	cc -o pxf -w $(CFLAGS) $(CINCS) $(OBJS) $(LFLAGS) $(LIBS)
	cp pxf ../bin

%.o: %.c $(INCS)
	gcc $(CINCS) $(CFLAGS) -c $< -o $@


