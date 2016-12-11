#
# Project: IFJ16, a programming language interpreter
# FIT VUT Brno
# Authors: xzaryb00 - Zarybnický Jakub
#          xtamas01 - Tamaškovič Marek
#          xvasko12 - Vaško Martin
#          xvasko14 - Vaško Michal
#          xzales12 - Záleský Jiří
#

CC=gcc
CPPFLAGS=-pedantic -Wall -Wextra
CFLAGS=-std=c99 -g
LDFLAGS=-L.

DOC_LOG=./dokumentace.log ./dokumentace.aux ./dokumentace.toc
EXE=ifj16 ifj16.exe test/parser test/parser.exe ifj16.o test/parser.o test/symbol-table.o test/symbol-table test/symbol-table.exe

LIBOBJ=ial.o ir.o parser.o scanner.o stringology.o sanity.o interpret.o int_memory_management.o type.o expr.o

.PHONY: all doc test val clean

all: ifj16

debug: CFLAGS += -g3
debug: all

debugInt: CFLAGS += -DDEBUG
debugInt: debug

test: ifj16 test/parser test/symbol-table test/stack
	bash testsuite.sh

#valgrind
val: ifj16 test/parser
	bash valgrind.sh

#documentation + pack
dist: $(wildcard *.c) $(wildcard *.h) Makefile rozdeleni rozsireni dokumentace.pdf
	tar cvzf xzaryb00.tgz $^

doc: dokumentace.pdf

clean:
	$(RM) $(EXE) xzaryb00.tgz $(LIBOBJ) dokumentace.pdf

ifj16: ifj16.o $(LIBOBJ)
test/parser: test/parser.o $(LIBOBJ)
test/symbol-table: test/symbol-table.o $(LIBOBJ)
test/stack: test/stack.o $(LIBOBJ)

dokumentace.pdf: doc/dokumentace.tex
	cd doc && pdflatex $(notdir $^) # musi to tu byt dva krat ten preklad nevymazavajte to !
	cd doc && pdflatex $(notdir $^)
	mv doc/dokumentace.pdf .
	cd doc && rm $(DOC_LOG)
