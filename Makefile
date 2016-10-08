CC=gcc
CPPFLAGS=-pedantic -Wall -Wextra -g
CFLAGS=-std=c99
LDFLAGS :=  -L.

#It should be enough to add object files here...
LIBOBJ := interpreter.o ial.o

.ONESHELL:
.PHONY: test clean

all: interpreter

interpreter: $(LIBOBJ)

test: interpreter
	bash testsuite.sh

clean:
	$(RM) interpreter $(LIBOBJ)