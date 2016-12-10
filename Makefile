CC=gcc
#Werror add?
CPPFLAGS=-pedantic -Wall -Wextra
CFLAGS=-std=c99 -g
LDFLAGS=-L.

DOC_SRC=./dokumentace.tex
PACK_CONT=Makefile rozdeleni rozsireni

#Removed files after compilation
DOC_LOG=./dokumentace.log ./dokumentace.aux ./dokumentace.toc
EXE=ifj16 ifj16.exe test/parser test/parser.exe ifj16.o test/parser.o test/symbol-table.o test/symbol-table test/symbol-table.exe

LIBOBJ=ial.o ir.o parser.o scanner.o stringology.o sanity.o interpret.o int_memory_management.o type.o

.PHONY: all doc test val clean

all: ifj16

debug: CFLAGS += -g3
debug: all

debugInt: CFLAGS += -DDEBUG
debugInt: debug

#doc
test: ifj16 test/parser test/symbol-table
	bash testsuite.sh

#valgrind
val: ifj16 test/parser
	bash valgrind.sh

clean:
	$(RM) $(EXE) xzaryb00.tgz $(LIBOBJ) $(DOC_LOG) dokumentace.pdf

ifj16: ifj16.o $(LIBOBJ)

test/parser: test/parser.o $(LIBOBJ)
test/symbol-table: test/symbol-table.o $(LIBOBJ)

#doc
dist: $(wildcard *.c) $(wildcard *.h) $(PACK_CONT)
	make document
	tar cvzf xzaryb00.tgz $^ dokumentace.pdf

document: $(DOC_OUT)
	cd doc && pdflatex $(DOC_SRC)
	mv ./doc/dokumentace.pdf ./dokumentace.pdf
	cd doc && rm $(DOC_LOG)
	cd ../
