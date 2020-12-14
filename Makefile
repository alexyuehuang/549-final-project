# makefile for Cache Simulator and testing tasks
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

all: csim test-trans tracegen test-sort tracegen-sort tracegen-mm test-mm

csim: csim.c
	$(CC) $(CFLAGS) -o csim csim.c -lm 

test-trans: test-trans.c trans.o
	$(CC) $(CFLAGS) -o test-trans test-trans.c trans.o

tracegen: tracegen.c trans.o
	$(CC) $(CFLAGS) -O0 -o tracegen tracegen.c trans.o

trans.o: trans.c
	$(CC) $(CFLAGS) -O0 -c trans.c

test-sort: test-sort.c sort.o
	$(CC) $(CFLAGS) -o test-sort test-sort.c sort.o -lm

tracegen-sort: tracegen-sort.c sort.o
	$(CC) $(CFLAGS) -O0 -o tracegen-sort tracegen-sort.c sort.o -lm

sort.o: sort.c
	$(CC) $(CFLAGS) -O0 -c sort.c

mm.o: mm.c
	$(CC) $(CFLAGS) -O0 -c $^

tracegen-mm: tracegen-mm.c mm.o
	$(CC) $(CFLAGS) -O0 -o $@ $^ -lm

test-mm: test-mm.c mm.o
	$(CC) $(CFLAGS) -O0 -o $@ $^ -lm

#
# Clean the src dirctory
#
clean:
	rm -rf *.o
	rm -f csim
	rm -f test-trans tracegen test-sort tracegen-sort
	rm -f trace.tmp trace.f*
	rm -f csim_results
	rm -f marker
