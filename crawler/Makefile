# Makefile for 'crawler' module
#
# Kyrylo Bakuemnko,	21 April 2023

# uncomment the following to turn on vesrbose memory logging
#TESTING=-DMEMTEST

.PHONY: all test valgrind clean

C = ../common
L = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
LLIBS = $C/common.a $L/libcs50-given.a
LIBS =
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: crawler

crawler: crawler.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# crawler source dependencies
crawler.o: $L/webpage.h $C/pagedir.h $L/mem.h $L/bag.h $L/hashtable.h

# expects a file `testing.sh` to exist
test: crawler testing.sh
	bash -v testing.sh >& testing.out

# expects ____
valgrind: crawler
	mkdir -p ../data/letters-3
	$(VALGRIND) ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters-3 10

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core
