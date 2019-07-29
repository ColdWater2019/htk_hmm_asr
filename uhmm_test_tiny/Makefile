#
#
# Make file for compiling HMM code in this directory.
# Author: Nyx Zheng
# Date: 19 December 2016
# $Id: Makefile,v 1.3 2016/12/19 15:12:35 Nyx Zheng $
# 
#
CFLAGS= -g
INCS=
# use the following line to "Purify" the code
#CC=purify gcc
CC=gcc
SRCS=viterbi.c hmmutils.c mfcc.c nrutil.c testvit.c 
all: testvit       
testvit: testvit.o viterbi.o nrutil.o hmmutils.o mfcc.o 
	 $(CC) -o testvit testvit.o viterbi.o nrutil.o hmmutils.o mfcc.o -lm
shar:
	shar *.c *.h Makefile README TODO COPYING *.seq *.hmm
depend:
	makedepend -- $(INCS) -- $(SRCS) 
clean:
	rm *.o  
# DO NOT DELETE THIS LINE -- make depend depends on it.

viterbi.o: /usr/include/math.h /usr/include/stdio.h hmm.h nrutil.h
hmmutils.o: /usr/include/stdio.h
hmmutils.o: /usr/include/stdlib.h /usr/include/math.h nrutil.h hmm.h
nrutil.o: /usr/include/malloc.h /usr/include/stdio.h
testvit.o: /usr/include/stdio.h
testvit.o: /usr/include/stdlib.h /usr/include/math.h /usr/include/string.h
testvit.o: nrutil.h hmm.h
mfcc.o: /usr/include/stdio.h
mfcc.o: /usr/include/stdlib.h /usr/include/math.h /usr/include/string.h
mfcc.o: mfcc.h
