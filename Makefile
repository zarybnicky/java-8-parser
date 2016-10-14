CC=gcc
CPPFLAGS=-pedantic -Wall -Wextra -g
CFLAGS=-std=c99
LDFLAGS :=  -L.

EXECUTABLE := ifj16

#It should suffice to add object files here...
LIBOBJ := ifj16.o ial.o interpret.o ir.o parser.o scanner.o stringology.o

.ONESHELL:
.PHONY: test clean

all: $(EXECUTABLE)

test: all
	bash testsuite.sh

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE).exe ${EXECUTABLE}.o $(LIBOBJ)

$(EXECUTABLE): $(LIBOBJ)
