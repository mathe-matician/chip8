CC=gcc
.PHONY: chip8

chip8:
	$(CC) main.c chip8defs.c opcodes.c -o chip8 -I include -L lib -l SDL2-2.0.0
