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

all: querier fuzzquery

querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# indexer source dependencies
querier.o:  $C/word.h $C/index.h $L/mem.h $L/webpage.h $L/file.h

# indexer source dependencies
fuzzquery.o:  $L/mem.h

# expects crawler directories ../data/letters-10 ../data/toscrape-2
# obtained through seeds as described in /crawler at depths 10 and 2 respectively
test: querier fuzzquery testing.sh testing.out
	bash -v testing.sh >& testing.out

# expects directory ../data/letters-10
valgrind: querier
	$(VALGRIND) ./querier ../data/letters-10 ../data/letters-10/index.ndx

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f fuzzquery
	rm -f core
