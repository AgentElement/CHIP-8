#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef uint16_t opcode;

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

    uint8_t screen[2048];
    uint8_t memory[4096];

} chip8;

int exec(chip8* c8, opcode inst);
int load(chip8* c8, FILE* file);

#endif // CHIP8_H