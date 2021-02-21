#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <errno.h>
#define OPCODE_SIZE 35
#define MEMORY_SIZE 0x0FFF
#define REG_SIZE 16
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 32
#define ERROR_MSG\
  fprintf(stderr, "Error(%d): %s\nLine: %d\nFile: %s\nC Version: %ld\nDate: %s\n", errno,__func__,__LINE__,__FILE__,__STDC_VERSION__,__DATE__)

extern const unsigned char fontset[80];
typedef void (*emulateCycle)();
typedef void (*initSystem)();
typedef void (*loadProgram)();
typedef void (*setKeys)();
typedef void (*executeOpcode)();
extern void (*opcode_execute[OPCODE_SIZE])();

struct opCode {
    uint16_t opcode;
};
//TODO: make stack a linked list - pop, push etc
struct stack {
    uint16_t address;
    struct stack* next;
};

struct opCode* opcode_hash[OPCODE_SIZE];
extern uint16_t opcodesInit[OPCODE_SIZE];
struct stack* stack_head;

typedef struct chip8 {
    uint8_t memory[MEMORY_SIZE]; //4k of memory
    uint8_t gfx[SCREEN_HEIGHT][SCREEN_WIDTH]; //screen size
    uint8_t V[REG_SIZE]; //16 V 8-bit registers
    struct stack* stack_full;
    uint16_t sp; //stack pointer
    uint16_t opcode; //16bit opcode variable
    void (*opcode_execute[OPCODE_SIZE])(struct chip8* a_chip8); //function pointer to the opcode to execute
    uint16_t I; //Index register
    uint16_t pc; //program counter
    uint8_t delay_timer; //60hz timer
    uint8_t sound_timer; //60hz timer
    uint8_t key[REG_SIZE]; //CHIP8 has a hex keypad 0 - F
    const unsigned char* font;
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

//opcode hashtable
extern void init_hash();

//stack
extern struct stack* push(uint16_t a_address);
extern void pop();

#endif //CHIP8_H