#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

all: csim test-trans tracegen

csim: csim.c
	$(CC) $(CFLAGS) -o csim csim.c -lm 

test-trans: test-trans.c trans.o
	$(CC) $(CFLAGS) -o test-trans test-trans.c trans.o

tracegen: tracegen.c trans.o
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.tmp trace.f*
	rm -f csim_results
	rm -f marker
