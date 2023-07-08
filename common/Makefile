# Makefile for 'common' module
#
# Kyrylo Bakuemnko,	21 April 2023

OBJS = word.o index.o pagedir.o ../libcs50/mem.o
LIB = common.a
L = ../libcs50

# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L
CC = gcc
MAKE = make

all: $(LIB)

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o: pagedir.h $L/mem.h
index.o: index.h 
word.o: word.h
../libcs50/mem.o: $L/mem.h

.PHONY: all clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f $(LIB) *~ *.o
	rm -f core
