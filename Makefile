CC=gcc
CPPFLAGS=-pedantic -Wall -Wextra -g
CFLAGS=-std=c99
LDFLAGS=-L.

LIBOBJ=ial.o interpret.o ir.o parser.o scanner.o stringology.o sanity.o

.PHONY: all test clean

all: ifj16

test: all test/parser
	bash testsuite.sh

clean:
	$(RM) ifj16 ifj16.exe test/parser test/parser.exe ifj16.o test/parser.o xzaryb00.tgz $(LIBOBJ)

ifj16: ifj16.o $(LIBOBJ)
main: main.o $(LIBOBJ)

test/parser: test/parser.o $(LIBOBJ)

dist: $(wildcard *.c) $(wildcard *.h) Makefile rozdeleni rozsireni dokumentace.pdf
	tar cvzf xzaryb00.tgz $^
