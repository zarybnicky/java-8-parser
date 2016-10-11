CC=gcc
CPPFLAGS=-pedantic -Wall -Wextra -g
CFLAGS=-std=c99
LDFLAGS :=  -L.

#It should be enough to add object files here...
LIBOBJ := main.o ial.o Stringology.o parser.o #scanner.o

.ONESHELL:
.PHONY: test clean

all: main

main: $(LIBOBJ)

test: main
	bash testsuite.sh

clean:
	$(RM) interpreter $(LIBOBJ)
