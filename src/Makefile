CC      = gcc
CFLAGS  = -Wall -g3 -O -D_DEFAULT_SOURCE -std=c99 -pedantic -Werror
BUILD   = emulate assemble
LDFLAGS = -lm

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

emulate: emulate.o computer.o bit_implementation.o fetch.o decode.o execute.o

assemble: assemble.o computer.o bit_implementation.o fetch.o decode.o execute.o \
	symbol_table.o tokenizer.o transform.o transform_utils.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
