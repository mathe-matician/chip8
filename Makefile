CC=gcc
CFLAGS= -Wall
INCLUDES= -I ./include
LFLAGS= -L ./lib
LIBS= -lSDL2-2.0.0
SRC=main.c chip8defs.c opcodes.c
OUTPUT=chip8
.PHONY: chip8

chip8:
	$(CC) $(SRC) -o $(OUTPUT) $(INCLUDES) $(LFLAGS) $(LIBS)