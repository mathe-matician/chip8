CC=gcc
CFLAGS= -Wall -g
LIBS= -lSDL2
SRC=main.c chip8defs.c opcodes.c
OUTPUT=-o chip8
.PHONY: chip8

chip8:
	$(CC) $(SRC) $(CFLAGS) $(LIBS) $(OUTPUT)