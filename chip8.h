#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <errno.h>
#define STACK_ADDR 97 // stack size = [97] - [81]. 16 8bit areas
#define OPCODE_SIZE 34
#define MEMORY_SIZE 0x0FFF
#define REG_SIZE 16
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define NEXT_PIXEL_LINE 28
#define BYTE 8
#define BIT7 0x80
#define WHITE 0xFF
#define KEYSIZE 16
#define ERROR_MSG\
  fprintf(stderr, "Error(%d): %s\nLine: %d\nFile: %s\nC Version: %ld\nDate: %s\n", errno,__func__,__LINE__,__FILE__,__STDC_VERSION__,__DATE__)

extern const unsigned char fontset[80];
typedef void (*emulateCycle)();
typedef void (*initSystem)();
typedef void (*loadProgram)();
typedef void (*setKeys)();
typedef void (*executeOpcode)();

struct opCode {
    uint16_t opcode;
};

struct opCode* opcode_hash[OPCODE_SIZE];
extern uint16_t opcodesInit[OPCODE_SIZE];
uint8_t* sp;
uint16_t pc;

typedef struct chip8 {
    uint8_t memory[MEMORY_SIZE]; //4k of memory 0x000 - 0xFFF
    uint8_t V[REG_SIZE]; //16 V 8-bit registers
    uint16_t opcode; //16bit opcode
    uint16_t I; //Index register
    uint8_t delay_timer; //60hz timer
    uint8_t sound_timer; //60hz timer
    uint8_t key[REG_SIZE]; //CHIP8 has a hex keypad 0 - F
    emulateCycle EmulateCycle; //fetch, decode, execute
    initSystem init; //init memory
    loadProgram LoadProgram;
    setKeys SetKeys;
} CHIP8_t;

//chip8 functions
extern void EmulateCycle(struct chip8* a_chip8);
extern void emulateCycleImp(struct chip8* a_chip8);
extern void initSystemImp(struct chip8* a_chip8);
extern void loadProgramImp(struct chip8* a_chip8, char* a_program);
extern void setKeysImp(struct chip8* a_chip8);
extern void getKey(struct chip8* a_chip8, uint8_t* state);

extern void execute_opcode(int a_index, struct chip8* a_chip8);
extern void init_hash();

extern uint16_t Get_0xxx(uint16_t a_opcode);
extern uint16_t Get_0xx0(uint16_t a_opcode);
extern uint16_t Get_0x00(uint16_t a_opcode);
extern uint16_t Get_00xx(uint16_t a_opcode);
extern uint16_t Get_00x0(uint16_t a_opcode);
extern uint16_t Get_000x(uint16_t a_opcode);

#endif //CHIP8_H