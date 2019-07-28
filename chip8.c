#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// #ifdef _WIN32
// #include <windows.h>
// #endif // _WIN32

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

    uint8_t screen[64][32];
    uint8_t memory[4096];

} chip8;

uint8_t sprites[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0x50, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

int exec(chip8* c8, opcode inst)
{
    // byte_n is an opcode nibble stored as a byte
    // cbyte_n is an opcode byte
    uint8_t op_id = (inst & 0xF000) >> 12; // byte_0
    uint8_t byte_1 = (inst & 0x0F00) >> 8;
    uint8_t byte_2 = (inst & 0x00F0) >> 4;
    uint8_t cbyte_1 = inst & 0x00FF; // the full second byte is used more often than byte_3

    switch (op_id) {
    case 0x0:
        if (inst == 0x00E0) { // 0x00E0: cls
            clear_screen(c8);
        } else if (inst == 0x00EE) { // 0x00EE: ret
            // return
        }
        break;

    case 0x1: // 0x1NNN: jp addr
        c8->PC = inst & 0x0FFF;
        break;

    case 0x2: // 0x2NNN: call nnn
        c8->stack[c8->SP] = c8->PC;
        c8->SP++;
        c8->PC = inst & 0x0FFF;
        break;

    case 0x3: // 0x3XNN: se VX, byte
        if (c8->V[byte_1] == (cbyte_1))
            c8->PC += 2;
        break;

    case 0x4: // 0x4XNN: sne VX, byte
        if (c8->V[byte_1] != (cbyte_1))
            c8->PC += 2;
        break;

    case 0x5: // 0x5XY0: se VX, VY
        if (c8->V[byte_1] == c8->V[byte_2])
            c8->PC += 2;
        break;

    case 0x6: // 0x6XNN: ld VX, byte
        c8->V[byte_1] = (cbyte_1);
        break;

    case 0x7: // 0x7XNN: add VX, byte
        c8->V[byte_1] += (cbyte_1);
        break;

    case 0x8:
        switch (inst & 0x000F) {
        case 0x0: // 0x8XY0: ld VX, VY
            c8->V[byte_1] = c8->V[byte_2];
            break;

        case 0x1: // 0x8XY1: or VX, VY
            c8->V[byte_1] |= c8->V[byte_2];
            break;

        case 0x2: // 0x8XY2: and VX, VY
            c8->V[byte_1] &= c8->V[byte_2];
            break;

        case 0x3: // 0x8XY3: xor VX, VY
            c8->V[byte_1] ^= c8->V[byte_2];
            break;

        case 0x4: // 0x8XY4: add VX, VY
            uint16_t result = c8->V[byte_1] + c8->V[byte_2];
            c8->V[0xF] = (result >> 8) & 0x1;
            c8->V[byte_1] = (uint8_t)result;
            break;

        case 0x5: // 0x8XY5: sub VX, VY
            int16_t result = c8->V[byte_2] - c8->V[byte_1];
            c8->V[0xF] = (result >> 8) & 0x1;
            c8->V[byte_1] = (uint8_t)result;
            break;

        case 0x6: // 0x8XY6: shr VX {, VY}
            c8->V[0xF] = c8->V[byte_1] & 0x0001;
            c8->V[byte_1] >>= 1;
            break;

        case 0x7: // 0x8XY7: subn VX, VY
            int16_t result = c8->V[byte_1] - c8->V[byte_2];
            c8->V[0xF] = (result >> 8) & 0x1;
            c8->V[byte_1] = (uint8_t)result;
            break;

        case 0xE: // 0x8XYE: shl VX {, VY}
            c8->V[0xF] = (c8->V[byte_1] >> 7) & 0x1;
            c8->V[byte_1] <<= 1;
            break;
        }
        break;

    case 0x9: // 9XY0: sne VX, VY
        if (c8->V[byte_1] != c8->V[byte_2])
            c8->PC += 2;
        break;

    case 0xA: // ANNN: ld I, addr
        c8->I = (inst & 0x0FFF);
        break;

    case 0xB: // BNNN: jp V0, addr
        c8->PC = (inst & 0x0FFF) + c8->V[0];
        break;

    case 0xC: // CXNN: rnd VX, byte
        c8->V[byte_1] = (rand() % 0x100) & (cbyte_1);
        break;

    case 0xD: // DXYN: drw VX, VY, nibble
        // display(Vx, Vy n);

    case 0xE:
        if (cbyte_1 == 0x009E) { // EX9E: skp Vx
            if (c8->keyState[c8->V[byte_1]])
                c8->PC += 2;
        } else if (cbyte_1 == 0x00A1) { // EXA1: skp Vx
            if (!c8->keyState[c8->V[byte_1]])
                c8->PC += 2;
        }
        break;

    case 0xF:
        switch (cbyte_1) {
        case (0x07): // FX07: ld VX, DT
            c8->V[byte_1] = c8->DT;
            break;

        case (0x0A): // FX0A: kld VX
            c8->V[byte_1] = readKey(c8);
            break;

        case (0x15): // FX15: ld DT, VX
            c8->DT = c8->V[byte_1];
            break;

        case (0x18): // FX18: ld ST, VX
            c8->ST = c8->V[byte_1];
            break;

        case (0x1E): // FX1E: add I, VX
            c8->I += c8->V[byte_1];
            break;

        case (0x29): // FX29: ld F, VX
            c8->I = c8->V[byte_1] * 5;
            break;

        case (0x33): // FX55: bcd Vx
            c8->memory[c8->I] = byte_1 / 100;
            c8->memory[c8->I + 1] = byte_1 % 100;
            c8->memory[c8->I + 2] = byte_1 % 10;
            break;

        case (0x55): // FX55: rdmp VX
            for (uint8_t i = 0; i < byte_1; i++) {
                c8->memory[c8->I + i] = c8->V[i];
            }
            break;

        case (0x65): // FX65: rrcl VX
            for (uint8_t i = 0; i < byte_1; i++) {
                c8->V[i] = c8->memory[c8->I + i];
            }
            break;
        }
    }
    c8->PC += 2;
}

int load(chip8* c8)
{
    for (size_t i = 0; i < 80; i++) { // load sprites into [0x0 - 0x50]
        c8->memory[i] = sprites[i];
    }
}

int clear_screen(chip8* c8);

int update_keys(chip8* c8);

uint8_t readKey(chip8* c8);

int main(int argc, char const* argv[])
{
    chip8 machine;
    opcode inst;
    FILE* ptr = fopen(argv[1], "rb");
    for (;;) {
        refresh();
        if (fread(&inst, sizeof(opcode), 1, ptr) != 1)
            break;

        int status = exec(&machine, inst);
        if (status == 0)
            break;
    }
    return 0;
}