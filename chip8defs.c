#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

/*
MEMORY MAP
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/

const uint16_t _CLS = 0x00E0;
const uint16_t _RET = 0x00EE;

const unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int hash_code(uint16_t a_opcode) {
   return a_opcode % OPCODE_SIZE;
}

//clean up for opcodes?
//automatic reference counter for freeing memory?
void insert(uint16_t a_opcode) {
    int l_opcodeIndex = hash_code(a_opcode);
    struct opCode* l_opcodeStruct = (struct opCode*)malloc(sizeof(struct opCode));
    l_opcodeStruct->opcode = a_opcode;
    while(opcode_hash[l_opcodeIndex] != NULL) {
        ++l_opcodeIndex;
        l_opcodeIndex %= OPCODE_SIZE;
    }
    opcode_hash[l_opcodeIndex] = l_opcodeStruct;
}

void init_hash() {
    for (int i = 0; i < OPCODE_SIZE; i++) {
        insert(opcodesInit[i]);
    }
}

int Execute(uint16_t a_opcode) {
    printf("Execute");
    return 0;
}

uint16_t Decode(uint16_t a_opcode) {
    int l_code = a_opcode & 0xF0FF;
    //figure out what opcode it is and error if not in hash table.
    int test = hash_code(l_code);
    struct opCode* l_temp = opcode_hash[test];
    uint16_t l_opcode = l_temp->opcode;
    if (!l_opcode) {
        fprintf(stderr, "Error: Not a valid opcode.\n");
        return -1;
    }
    //TODO: Parse the opcode for particulars.
    return l_opcode;
}

void emulateCycleImp(CHIP8_t* a_chip8) {
    //fetch
    uint8_t l_msb = a_chip8->memory[a_chip8->pc];
    uint8_t l_lsb = a_chip8->memory[a_chip8->pc + 1];
    //decode
    a_chip8->opcode = l_msb << 8 | l_lsb;
    uint16_t l_decodedOpcode = Decode(a_chip8->opcode);
    if (l_decodedOpcode) {
        fprintf(stderr, "Error: There was an error decoding the opcode");
        return;
    }
    //execute
    a_chip8->opcode_execute[l_decodedOpcode](a_chip8);
}

void initSystemImp(CHIP8_t* a_chip8) {
    init_hash();
    a_chip8->stack_full = stack_head;
    memset(a_chip8->memory, 0, MEMORY_SIZE);
    a_chip8->pc = 0x0200;
    a_chip8->delay_timer = 0;
    a_chip8->sound_timer = 0;
    a_chip8->font = fontset;
    a_chip8->opcode = 0x0000;
    a_chip8->sp = 0;
}

void loadProgramImp(CHIP8_t* a_chip8, char* a_program) {
    printf("Loading %s\n", a_program);
}

void setKeysImp(CHIP8_t* a_chip8) {
    printf("Storing key press state\n");
}

struct stack* push(uint16_t a_address) {
    struct stack* node = (struct stack*)malloc(sizeof(struct stack));
    node->address = a_address;
    node->next = stack_head;
    stack_head = node;
    return node;
}

void pop() {
    struct stack* temp = stack_head->next;
    free(stack_head);
    stack_head = temp;
}