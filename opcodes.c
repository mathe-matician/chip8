#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "SDLRect.h"

uint16_t opcodesInit[OPCODE_SIZE] = {
    0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
    0xA000, 0xB000, 0xC000, 0xD000, 0x8001, 0x8002, 0x8003, 0x8004,
    0x8005, 0x8006, 0x8007, 0x8000, 0x800E, 0xE09E, 0xE0A1, 0xF065,
    0xF055, 0xF033, 0xF029, 0xF01E, 0xF018, 0xF015, 0xF00A, 0xF007,
    0x00E0, 0x00EE
};

//AND uint16_t with 0x0FFF;
uint16_t Get_0xxx(uint16_t a_opcode) {
    return a_opcode & 0x0FFF;
}

//AND uint16_t with 0x0FF0;
uint16_t Get_0xx0(uint16_t a_opcode) {
    return a_opcode & 0X0FF0;
}

//AND uint16_t with 0x0F00;
uint16_t Get_0x00(uint16_t a_opcode) {
    int l_res = a_opcode & 0x0F00;
    l_res = l_res >> 8;
    return l_res;
}

//AND uint16_t with 0x00FF;
uint16_t Get_00xx(uint16_t a_opcode) {
    return a_opcode & 0x00FF;
}

//AND uint16_t with 0x00F0;
uint16_t Get_00x0(uint16_t a_opcode) {
    int l_res = a_opcode & 0x00F0;
    l_res = l_res >> 4;
    return l_res;
}

//AND uint16_t with 0x000F;
uint16_t Get_000x(uint16_t a_opcode) {
    return a_opcode & 0x000F;
}

//0x0nnn: Jump to a machine code routine at nnn.
//This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.
void SYS() {
    //TODO make error handling for this opcode
    printf("SYS not implemented");
}

//0x1nnn: Jump to location nnn.
//The interpreter sets the program counter to nnn    CALL, SNE,  
void JP_1nnn(CHIP8_t* a_chip8) {
    uint16_t l_address = Get_0xxx(a_chip8->opcode);
    pc = l_address;
}

//2nnn: Call subroutine at nnn.
//The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
void CALL(CHIP8_t* a_chip8) {
    sp--; //stack grows down
    a_chip8->memory[STACK_ADDR] = pc;
    pc = Get_0xxx(a_chip8->opcode);
}

//TODO: make sure this is correct with memory fetching
//SE Vx, byte 3xkk: Skip next instruction if Vx = kk.
//The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
void SE_3xkk(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] == a_chip8->memory[Get_00xx(a_chip8->opcode)]) {
        pc += 2;
    }
}
//SNE Vx, byte 4xkk
//Skip next instruction if Vx != kk.
//The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
void SNE(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] != Get_00xx(a_chip8->opcode)) {
        pc += 2;
    }
}

//SE Vx, Vy 5xy0
//Skip next instruction if Vx = Vy.
//The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
void SE_5xy0(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] == a_chip8->V[Get_00x0(a_chip8->opcode)]) {
        pc += 2;
    }
}

//LD Vx, byte 6xkk
//Set Vx = kk.
//The interpreter puts the value kk into register Vx.
void LD_6xkk(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] = Get_00xx(a_chip8->opcode);
}

//ADD Vx, byte 7xkk
//Set Vx = Vx + kk.
//Adds the value kk to the value of register Vx, then stores the result in Vx.
void ADD_7xkk(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] += Get_00xx(a_chip8->opcode);
}

//LD I, addr Ann
//Set I = nnn.
//The value of register I is set to nnn.
void LD_I_Annn(CHIP8_t* a_chip8) {
    a_chip8->I = Get_0xxx(a_chip8->opcode);
}

//JP V0, addr Bnnn
//Jump to location nnn + V0.
//The program counter is set to nnn plus the value of V0.
void JP_V0_Bnnn(CHIP8_t* a_chip8) {
    pc = Get_0xxx(a_chip8->opcode);
}


//RND Vx, byte Cxkk
//Set Vx = random byte AND kk.
//The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. 
//The results are stored in Vx. See instruction 8xy2 for more information on AND.
void RND(CHIP8_t* a_chip8) {
    time_t a_t;
    srand((unsigned)time(&a_t));
    short l_rand = rand() % 256;
    a_chip8->V[Get_0x00(a_chip8->opcode)] = l_rand & Get_00xx(a_chip8->opcode);
}

//DRW Vx, Vy, nibble Dxyn
/*
    Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
    Each row of 8 pixels is read as bit-coded 
    (with the most significant bit of each byte displayed on the left) 
    starting from memory location I; 
    I value doesn't change after the execution of this instruction. 
    As described above, VF is set to 1 if any screen pixels are flipped 
    from set to unset when the sprite is drawn, and to 0 if that doesn't happen.
*/

void DRW(CHIP8_t* a_chip8) {
    uint8_t l_how_many_bytes = Get_000x(a_chip8->opcode);
    uint16_t l_memoryLocation = a_chip8->I;

    int l_xPosition = a_chip8->V[Get_0x00(a_chip8->opcode)];
    int l_yPosition = a_chip8->V[Get_00x0(a_chip8->opcode)];

    uint8_t l_mem_byte = 0;
    uint8_t l_mask = 0;
    uint32_t *l_pixels = (uint32_t*)g_surface->pixels;
    uint32_t l_data = 0;
    int l_yPosition_temp = l_yPosition;
    int l_xPosition_temp = l_xPosition;

    while (l_how_many_bytes--) {
        l_mem_byte = a_chip8->memory[l_memoryLocation++];
        l_mask = BIT7;

        for (int i = 0; i < BYTE; i++) {
            if (l_mem_byte & l_mask) {
                l_data = l_pixels[(l_yPosition_temp * g_surface->w) + l_xPosition_temp];
                if (l_data) {
                    a_chip8->V[0x0F] = 0x01;
                } else {

                    a_chip8->V[0x0F] = 0;
                }

                if (l_xPosition_temp > SCREEN_WIDTH) {
                    l_xPosition_temp -= SCREEN_WIDTH;
                }

                if (l_yPosition_temp > SCREEN_HEIGHT) {
                    l_yPosition_temp -= SCREEN_HEIGHT;
                }

                if (SDL_SetRenderDrawColor(g_renderer, WHITE, WHITE, WHITE, WHITE)) {
                    ERROR_MSG;
                    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
                }
                if (SDL_RenderDrawPoint(g_renderer, l_xPosition_temp++, l_yPosition_temp)) {
                    ERROR_MSG;
                    fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
                }
                SDL_RenderPresent(g_renderer);

                l_mask = l_mask >> 1;
            }
            l_xPosition_temp++;
            l_mask = l_mask >> 1;
        }
        l_xPosition_temp = l_xPosition;
        l_yPosition_temp++;
    }
}

//OR Vx, Vy 8xy1
//Set Vx = Vx OR Vy.
//Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
void OR(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] |= a_chip8->V[Get_00x0(a_chip8->opcode)];
}

//AND Vx, Vy 8xy2
//Set Vx = Vx AND Vy.
//Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
void AND(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] &= a_chip8->V[Get_00x0(a_chip8->opcode)];
}

//XOR Vx, Vy 8xy3
//Set Vx = Vx XOR Vy.
//Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
void XOR(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] ^= a_chip8->V[Get_00x0(a_chip8->opcode)];
}

//ADD Vx, Vy 8xy4
//Set Vx = Vx + Vy, set VF = carry.
//The values of Vx and Vy are added together. 
//If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. 
//Only the lowest 8 bits of the result are kept, and stored in Vx.
//TODO: greater than 255, how to simulate this carry?
void ADD_8xy4(CHIP8_t* a_chip8) {
    if ((a_chip8->V[Get_0x00(a_chip8->opcode)] += a_chip8->V[Get_00x0(a_chip8->opcode)]) > 255) {
        a_chip8->V[0x0F] = 0x01;
    }
}

//SUB Vx, Vy 8xy5
//Set Vx = Vx - Vy, set VF = NOT borrow.
//If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void SUB(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] > a_chip8->V[Get_00x0(a_chip8->opcode)]) {
        a_chip8->V[0x0F] = 0x01;
    } else {
        a_chip8->V[0x0F] = 0;
    }
    a_chip8->V[Get_0x00(a_chip8->opcode)] -= a_chip8->V[Get_00x0(a_chip8->opcode)];
}

//SHR Vx, {, Vy} 8xy6
//Set Vx = Vx SHR 1.
//If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void SHR(CHIP8_t* a_chip8) {
    if ((a_chip8->V[Get_0x00(a_chip8->opcode)] & 0x01) == 0x01) {
        a_chip8->V[0x0F] = 0x01;
    } else {
        a_chip8->V[0x0F] = 0;
    }
    a_chip8->V[Get_0x00(a_chip8->opcode)] = a_chip8->V[Get_0x00(a_chip8->opcode)] >> 1;
}

//SUBN Vx, Vy 8xy7
//Set Vx = Vy - Vx, set VF = NOT borrow.
//If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
void SUBN(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_00x0(a_chip8->opcode)] > a_chip8->V[Get_0x00(a_chip8->opcode)]) {
        a_chip8->V[0x0F] = 0x01;
    } else {
        a_chip8->V[0x0F] = 0;
    }
    a_chip8->V[Get_00x0(a_chip8->opcode)] -= a_chip8->V[Get_0x00(a_chip8->opcode)];
}

//SHL Vx, {, Vy} 8xyE
//Set Vx = Vx SHL 1.
//If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void SHL(CHIP8_t* a_chip8) {
    if (((a_chip8->V[Get_0x00(a_chip8->opcode)] & 0xF0) & 0x01) == 0x01) {
        a_chip8->V[0x0F] = 0x01;
    } else {
        a_chip8->V[0x0F] = 0;
    }

    a_chip8->V[Get_0x00(a_chip8->opcode)] = a_chip8->V[Get_0x00(a_chip8->opcode)] << 1;
}

//LD Vx, Vy 8xy0
//Set Vx = Vy.
//Stores the value of register Vy in register Vx.
void LD_8xy0(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] = a_chip8->V[Get_00x0(a_chip8->opcode)];
}

//SKP Vx Ex9E
//Skip next instruction if key with the value of Vx is pressed.
//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
void SKP(CHIP8_t* a_chip8) {
    const uint8_t *state;
    state = SDL_GetKeyboardState(NULL);
    int l_val = a_chip8->V[Get_0x00(a_chip8->opcode)];
    if (state[l_val]) {
        pc += 2;
    }
}

//SKNP Vx ExA1
//Skip next instruction if key with the value of Vx is not pressed.
//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
void SKNP(CHIP8_t* a_chip8) {
    const uint8_t *state;
    state = SDL_GetKeyboardState(NULL);
    int l_val = a_chip8->V[Get_0x00(a_chip8->opcode)];
    if (!state[l_val]) {
        pc += 2;
    }
}

//LD Vx, [I] Fx65
//Read registers V0 through Vx from memory starting at location I.
//The interpreter reads values from memory starting at location I into registers V0 through Vx.
void LD_Fx65_I(CHIP8_t* a_chip8) {
    int l_endReg = Get_0x00(a_chip8->opcode);
    uint16_t l_memLoc = a_chip8->I;
    for (int i = 0; i < l_endReg+1; i++) {
        a_chip8->V[i] = a_chip8->memory[l_memLoc++];
    }
}

//LD [I], Vx Fx55
//Store registers V0 through Vx in memory starting at location I
//The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I
void LD_I_Fx55(CHIP8_t* a_chip8) {
    int l_endReg = Get_0x00(a_chip8->opcode);
    uint16_t l_memLoc = a_chip8->I;
    for (int i = 0; i < l_endReg+1; i++) {
        a_chip8->memory[l_memLoc++] = a_chip8->V[i];
    }
}

//LD B, Vx Fx33
//Store BCD representation of Vx in memory locations I, I+1, and I+2.
//The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
//the tens digit at location I+1, and the ones digit at location I+2.
void LD_B_Fx33(CHIP8_t* a_chip8) {
    uint16_t l_Vx = a_chip8->V[Get_0x00(a_chip8->opcode)];
    uint16_t l_hund = abs(l_Vx / 100);
    uint16_t l_tens = abs(l_Vx / 10) % 10;
    uint16_t l_ones = l_Vx % 10;

    a_chip8->memory[a_chip8->I] = l_hund;
    a_chip8->memory[a_chip8->I+1] = l_tens;
    a_chip8->memory[a_chip8->I+2] = l_ones;
}

//LD F, Vx Fx29
//Set I = location of sprite for digit Vx.
//The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
//The index register I is set to the address of the hexadecimal character in VX. 
//You probably stored that font somewhere in the first 512 bytes of memory, so now you just need to point I to the right character.
//An 8-bit register can hold two hexadecimal numbers, but this would only point to one character. 
//The original COSMAC VIP interpreter just took the last nibble of VX and used that as the character.
void LD_F_Fx29(CHIP8_t* a_chip8) {
    uint16_t l_hexNum = a_chip8->V[Get_0x00(a_chip8->opcode)];
    uint16_t memLoc = 0x200 + (l_hexNum * 5);
    a_chip8->I = memLoc;
}

//ADD I, Vx Fx1E
//Set I = I + Vx.
//The values of I and Vx are added, and the results are stored in I.
void ADD_I_Fx1E(CHIP8_t* a_chip8) {
    a_chip8->I += a_chip8->V[Get_0x00(a_chip8->opcode)];
}

//LD ST, Vx Fx18
//Set sound timer = Vx.
//ST is set equal to the value of Vx.
void LD_ST_Fx18(CHIP8_t* a_chip8) {
    a_chip8->sound_timer = a_chip8->V[Get_0x00(a_chip8->opcode)];
}

//LD DT, Vx Fx15
//Set delay timer = Vx.
//DT is set equal to the value of Vx.
void LD_DT_Fx15(CHIP8_t* a_chip8) {
    a_chip8->delay_timer = a_chip8->V[Get_0x00(a_chip8->opcode)];
}

//LD Vx, K Fx0A
//Wait for a key press, store the value of the key in Vx.
//All execution stops until a key is pressed, then the value of that key is stored in Vx.
void LD_K_Fx0A(CHIP8_t* a_chip8) {
    const uint8_t *state;
    state = SDL_GetKeyboardState(NULL);
    int l_reg = Get_0x00(a_chip8->opcode);
    int l_key = a_chip8->V[l_reg];
    while (1) {
        if (state[l_key]) {
            a_chip8->V[l_reg] = l_key;
            break;
        }
    }
}

//LD Vx, DT Fx07
//Set Vx = delay timer value.
//The value of DT is placed into Vx.
void LD_DT_Fx07(CHIP8_t* a_chip8) {
    a_chip8->V[Get_0x00(a_chip8->opcode)] = a_chip8->delay_timer;
}

//LD Vx, DT 00E0
void CLS(CHIP8_t* a_chip8) {
    if (SDL_RenderClear(g_renderer)) {
        ERROR_MSG;
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
    }
}

//LD Vx, DT 00EE
//Return from a subroutine.
//The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
void RET(CHIP8_t* a_chip8) {
    pc = *sp;
    sp--;
}

void empty() {}

//to call: (*opcode_execute[index])()
void (*opcode_execute[35])(CHIP8_t* a_chip8) = {
    SYS, SHR, SUBN, LD_Fx65_I, DRW, ADD_I_Fx1E, JP_V0_Bnnn, SHL, SKP,
    LD_F_Fx29, ADD_7xkk, SKNP, SE_5xy0, LD_ST_Fx18, SE_3xkk, LD_K_Fx0A, JP_1nnn,
    LD_DT_Fx07, RET, LD_I_Fx55, LD_B_Fx33, CLS, RND, LD_DT_Fx15, LD_I_Annn,
    empty, LD_8xy0, OR, LD_6xkk, AND, SNE, XOR, CALL, ADD_8xy4, SUB
};

void execute_opcode(int a_index, CHIP8_t* a_chip8) {
    if (a_index > 35) {
        ERROR_MSG;
        fprintf(stderr, "%d is not a valid opcode\n", a_index);
        return; 
    }

    (*opcode_execute[a_index])(a_chip8);
}