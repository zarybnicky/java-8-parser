CC=gcc
#Werror add?
CPPFLAGS=-pedantic -Wall -Wextra
CFLAGS=-std=c99 -g
LDFLAGS=-L.
DOC_SRC=./doc/doc.tex
DOC_OUT=./dokumentace.pdf

LIBOBJ=ial.o ir.o parser.o scanner.o stringology.o sanity.o interpret.o int_memory_management.o type.o

.PHONY: all test clean

all: ifj16

debug: CFLAGS += -g3
debug: all

debugInt: CFLAGS += -DDEBUG
debugInt: debug

test: ifj16 test/parser test/symbol-table
	bash testsuite.sh

clean:
	$(RM) ifj16 ifj16.exe test/parser test/parser.exe ifj16.o test/parser.o test/symbol-table.o test/symbol-table test/symbol-table.exe xzaryb00.tgz $(LIBOBJ) ./doc.*

ifj16: ifj16.o $(LIBOBJ)

test/parser: test/parser.o $(LIBOBJ)
test/symbol-table: test/symbol-table.o $(LIBOBJ)

dist: $(wildcard *.c) $(wildcard *.h) Makefile rozdeleni rozsireni dokumentace.pdf
	tar cvzf xzaryb00.tgz $^

doc: $(DOC_SRC)
	pdflatex $(DOC_SRC) $(DOC_OUT)
	rm doc.aux doc.log doc.toc
