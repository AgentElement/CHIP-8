#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdio.h>

typedef uint16_t opcode;

#define MAX_X 64
#define MAX_Y 32

typedef struct {
    /*
    * General-purpose registers: V0 to VF.
    * VF is used as a flag by some operations.
    */
    uint8_t V[16];

    uint16_t stack[16];
    uint8_t keyState[16];

    uint16_t I; // Instruction pointer
    uint8_t SP; // Stack pointer
    uint16_t PC; // Program counter
    uint8_t DT; // Delay timer
    uint8_t ST; // Sound timer
    uint8_t DF; // Draw flag

    uint8_t screen[MAX_X * MAX_Y];
    uint8_t memory[4096];

} chip8;

int exec(chip8* c8, opcode inst);
int load(chip8* c8, FILE* file);
void update_timers(chip8* c8);

#endif // CHIP8_H