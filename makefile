##
##	PDS1 Makefile 
##
FILE_EXT = 
##
XGPATH = /usr/local/lib/xgrafix
##
EXEC=	 xpds1$(FILE_EXT)
##
CC =	 gcc
##
##      C compiler.  Normally cc; is scc on Unicos.
##
CFLAGS=-g -I$(XGPATH) -DPDP1PATH=\"$(HOME)/Plazmach/xpds1\" 
##
##      Flags used for compiling.  -O for optimization, which is optional.
##	If the X11 include files are not located in /usr/include/X11, you
##	must specify the directory as a flag prefixed by -I.  For instance, 
##	if the include files for X11 are located in /usr/local/include, the 
##	flag -I/usr/local/include must also be passed.
##
LIBS  = -L$(XGPATH) -L/usr/local/lib -L/usr/X11R6/lib -lXGC250 -ltk -ltcl -lXpm -lX11 -lm -ldl 
##
##      Libraries and their directories used in loading.  Normal is -lm and -lX11
##	for the Math and X11 libraries, which is NOT optional.  On Unicos, -lnet
##	is also used.  If the X11 libraries are not located in /usr/lib/X11, you
##	must also specify the directory as a flag prefixed by -L.  For instance,
##	if the include files for X11 are located in /usr/local/lib, the flag
##	-L/usr/local/lib must also be passed.  On Sun, use -Bstatic to avoid
##	problems with differing versions of OS.  Man cc for more information on flags.
##
##
PDS1OBJ=	fft.o pds1.o prests.o pflmvs.o padjus.o mcc.o initwin.o 

all:	$(PDS1OBJ) $(EXEC)

.c.o:	pds1.h
	$(CC) -Wall -c $(CFLAGS) $*.c

$(EXEC):	$(PDS1OBJ)
		$(CC) $(CFLAGS) -o $(EXEC) $(PDS1OBJ) $(LIBS)

clean:
	@rm *.o *~
