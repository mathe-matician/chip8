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
    printf("SYS not implemented");
}

//0x1nnn: Jump to location nnn.
//The interpreter sets the program counter to nnn.
void JP_1nnn(CHIP8_t* a_chip8) {
    uint16_t l_address = Get_0xxx(a_chip8->opcode);
    a_chip8->pc = l_address;
}

//2nnn: Call subroutine at nnn.
//The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
void CALL(CHIP8_t* a_chip8) {
    a_chip8->sp++;
    a_chip8->stack_full = push(a_chip8->pc);
    a_chip8->pc = Get_0xxx(a_chip8->opcode);
}

//TODO: make sure this is correct with memory fetching
//SE Vx, byte 3xkk: Skip next instruction if Vx = kk.
//The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
void SE_3xkk(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] == a_chip8->memory[Get_00xx(a_chip8->opcode)]) {
        a_chip8->pc+=0x0002;
    }
}
//SNE Vx, byte 4xkk
//Skip next instruction if Vx != kk.
//The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
void SNE(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] != Get_00xx(a_chip8->opcode)) {
        a_chip8->pc+=0x0002;
    }
}

//SE Vx, Vy 5xy0
//Skip next instruction if Vx = Vy.
//The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
void SE_5xy0(CHIP8_t* a_chip8) {
    if (a_chip8->V[Get_0x00(a_chip8->opcode)] == a_chip8->V[Get_00x0(a_chip8->opcode)]) {
        a_chip8->pc+=0x0002;
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
    a_chip8->pc = Get_0xxx(a_chip8->opcode);
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
    int l_xPosition = Get_0x00(a_chip8->opcode);
    int l_yPosition = Get_00x0(a_chip8->opcode);
    printf("how many = %d\n", l_how_many_bytes);

    //read n bytes starting at location I
    while ((int*)l_how_many_bytes--) {
        uint8_t l_mem_byte = a_chip8->memory[l_memoryLocation];
        
        //Collision
        //XOR Vx,Vy with screen x,y, 
        //if any pixels are erased by this above XOR, run for collision flag:
        uint32_t *pixels = (uint32_t*)g_surface->pixels;
        uint32_t data = pixels[(l_yPosition * g_surface->w) + l_xPosition];

        //temp collision test
        if (data) {
            printf("YOU HIT A PIXEL");
            a_chip8->V[0x0F] = 0x01;
        } else {
            a_chip8->V[0x0F] = 0;
        }

        //Figure out drawing location
        //test for drawing screen location and screen boundaries
        if (l_xPosition > SCREEN_WIDTH-1) {
            l_xPosition -= SCREEN_WIDTH+1;
        }

        if (l_yPosition > SCREEN_HEIGHT-1) {
            l_yPosition -= SCREEN_HEIGHT+1;
        }

        SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderDrawPoint(g_renderer, l_xPosition, l_yPosition);
        SDL_RenderPresent(g_renderer);
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
    //check keyboard input
}

//SKNP Vx ExA1
//Skip next instruction if key with the value of Vx is not pressed.
//Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
void SKNP(CHIP8_t* a_chip8) {
    printf("SKNP");
}

//LD Vx, [I] Fx65
//Read registers V0 through Vx from memory starting at location I.
//The interpreter reads values from memory starting at location I into registers V0 through Vx.
void LD_Fx65_I(CHIP8_t* a_chip8) {
    int l_endRegister = Get_0x00(a_chip8->opcode);
    for (int i = 0; i != l_endRegister; i++) {
        //a_chip8->memory[]
        printf("LD\n");
    }
}

//LD [I], Vx Fx55
void LD_I_Fx55(CHIP8_t* a_chip8) {
    printf("LD_I_Fx55");
}

//LD B, Vx Fx33
void LD_B_Fx33(CHIP8_t* a_chip8) {
    printf("LD_B_Fx33");
}

//LD F, Vx Fx29
void LD_F_Fx29(CHIP8_t* a_chip8) {
    printf("LD_F_Fx29");
}

//ADD I, Vx Fx1E
void ADD_I_Fx1E(CHIP8_t* a_chip8) {
    printf("ADD_I_Fx1E");
}

//LD ST, Vx Fx1E
void LD_ST_Fx1E(CHIP8_t* a_chip8) {
    printf("LD_ST_Fx1E");
}

//LD DT, Vx Fx15
void LD_DT_Fx15(CHIP8_t* a_chip8) {
    printf("LD_DT_Fx15");
}

//LD Vx, K Fx0A
void LD_K_Fx0A(CHIP8_t* a_chip8) {
    printf("LD_K_Fx0A");
}

//LD Vx, DT Fx07
void LD_DT_Fx07(CHIP8_t* a_chip8) {
    printf("LD_DT_Fx07");
}

//LD Vx, DT Fx07
void CLS(CHIP8_t* a_chip8) {
    printf("CLS");
}

//LD Vx, DT Fx07
void RET(CHIP8_t* a_chip8) {
    printf("RET");
}

//to call: (*opcode_execute[index])()
void (*opcode_execute[OPCODE_SIZE])() = {
    SYS, JP_1nnn, CALL, SE_3xkk, SNE, SE_5xy0, LD_6xkk, ADD_7xkk,
    LD_I_Annn, JP_V0_Bnnn, RND, DRW, OR, AND, XOR, ADD_8xy4,
    SUB, SHR, SUBN, SHL, LD_8xy0, SKP, SKNP, LD_Fx65_I,
    LD_I_Fx55, LD_B_Fx33, LD_F_Fx29, ADD_I_Fx1E, LD_ST_Fx1E, LD_DT_Fx15, LD_K_Fx0A, LD_DT_Fx07,
    CLS, RET
};