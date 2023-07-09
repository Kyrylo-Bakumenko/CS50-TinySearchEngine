# Makefile for 'common' module
#
# Kyrylo Bakuemnko,	21 April 2023

.PHONY: all test valgrind clean

C = ../common
L = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
LLIBS = $C/common.a $L/libcs50-given.a
LIBS =

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all -s

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$C
CC = gcc
MAKE = make

all: indexer indextest

indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# indexer source dependencies
indexer.o:  $C/index.h $L/mem.h $L/webpage.h
indextest.o: $C/index.h $L/file.h indexer.c

# expects a directories ../data/letters-1 ../data/letters-2 ../data/letters-3
test: indexer indextest testing.sh testing.out
	bash -v testing.sh >& testing.out

# expects directory ../data/letters-3
valgrind: indexer indextest
	$(VALGRIND) ./indexer ../data/letters-3 ../data/letters-3/index.ndx
	$(VALGRIND) ./indextest ../data/letters-3/index.ndx ../data/letters-3/index_new.ndx

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f indextest
	rm -f core
