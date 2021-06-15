#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"
#include "SDLRect.h"

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

const unsigned int g_keys[KEYSIZE] = {
    SDL_SCANCODE_0, SDL_SCANCODE_1, SDL_SCANCODE_2,
    SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5,
    SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
    SDL_SCANCODE_9, SDL_SCANCODE_A, SDL_SCANCODE_B,
    SDL_SCANCODE_C, SDL_SCANCODE_D, SDL_SCANCODE_E,
    SDL_SCANCODE_F
};

const unsigned int* g_keys_p = g_keys;

//Store fontset in memory locations 0x000 - 0x079
void init_font(CHIP8_t* a_chip8) {
    uint16_t memFont = 0x0000;
    for (int i = 0; i < 80; i++) {
        a_chip8->memory[memFont++] = fontset[i];
    }
}
uint16_t hash_code(uint16_t a_opcode) {
   return a_opcode % OPCODE_SIZE;
}

//automatic reference counter for freeing memory?
void insert(uint16_t a_opcode) {
    struct opCode* l_opcodeStruct = (struct opCode*)malloc(sizeof(struct opCode));
    l_opcodeStruct->opcode = a_opcode;
    uint16_t l_opcodeIndex = hash_code(a_opcode);
    while(opcode_hash[l_opcodeIndex] != NULL) {
        if (l_opcodeIndex++ > OPCODE_SIZE) {
            l_opcodeIndex = 0;
        }
    }
    opcode_hash[l_opcodeIndex] = l_opcodeStruct;
}

void init_hash() {
    for (int i = 0; i < OPCODE_SIZE; i++) {
        insert(opcodesInit[i]);
    }
}

void Execute(uint16_t a_opcode) {

}

uint16_t Decode(uint16_t a_opcode) {
    uint16_t l_tempcode = a_opcode & 0xF000;
    uint16_t l_code = hash_code(l_tempcode);
    return l_code;
}

void emulateCycleImp(CHIP8_t* a_chip8) {
    //fetch
    uint16_t l_msb = a_chip8->memory[pc] << 8;
    uint16_t l_lsb = a_chip8->memory[pc+1];
    pc += 2;
    //decode
    uint16_t l_opcode = l_msb | l_lsb;
    uint16_t l_decodedOpcode = Decode(l_opcode);
    a_chip8->opcode = l_opcode;
    //execute
    execute_opcode(l_decodedOpcode, a_chip8);
}

void initSystemImp(CHIP8_t* a_chip8) {
    init_hash();
    memset(a_chip8->memory, 0, MEMORY_SIZE);
    sp = &a_chip8->memory[STACK_ADDR];
    pc = 0x200;
    a_chip8->delay_timer = 0;
    a_chip8->sound_timer = 0;
    init_font(a_chip8);
    a_chip8->I = 0;
}

void getProgram(CHIP8_t* a_chip8, const char* a_program) {
    FILE* l_fp = fopen(a_program, "rb");
    if (!l_fp) {
        ERROR_MSG;
        fprintf(stderr, "File failed to open: Errno (%d)\n", errno);
    }

    fseek(l_fp, 0L, SEEK_END);
    int l_file_size = ftell(l_fp);
    rewind(l_fp);

    unsigned char l_buffer[l_file_size];

    fread(l_buffer, sizeof(l_buffer), sizeof(*l_buffer), l_fp);
    fclose(l_fp);

    unsigned char* l_buffer_p = l_buffer;
    int l_prog_start = 0x200;
    while (l_buffer_p && l_prog_start != MEMORY_SIZE+1) {
        a_chip8->memory[l_prog_start] = *l_buffer_p;
        l_prog_start++;
        l_buffer_p++;
    }
}

void loadProgramImp(CHIP8_t* a_chip8, char* a_program) {
    printf("Loading %s\n", a_program);
    getProgram(a_chip8, a_program);
}

void setKeysImp(CHIP8_t* a_chip8) {
    const uint8_t *state;
    state = SDL_GetKeyboardState(NULL);
    
}