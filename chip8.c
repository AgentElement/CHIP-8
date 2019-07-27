#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// #ifdef _WIN32
// #include <windows.h>
// #endif // _WIN32

typedef uint16_t opcode;

uint8_t memory[4096];
uint8_t screen[64][32];
uint16_t stack[16];
uint8_t keyState[16];

/*
 * General-purpose registers: V0 to VF.
 * VF is used as a flag by some operations.
 */
uint8_t V[16];

uint16_t I; // Instruction pointer
uint8_t SP; // Stack pointer
uint16_t PC; // Program counter
uint8_t DT; // Delay timer
uint8_t ST; // Sound timer

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

int exec(opcode inst)
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
            clear_screen();
        } else if (inst == 0x00EE) { // 0x00EE: ret
            // return
        }

    case 0x1: // 0x1NNN: jp addr
        PC = inst & 0x0FFF;

    case 0x2: // 0x2NNN: ret
        // call NNN

    case 0x3: // 0x3XNN: se VX, byte
        if (V[byte_1] == (cbyte_1))
            PC += 2;

    case 0x4: // 0x4XNN: sne VX, byte
        if (V[byte_1] != (cbyte_1))
            PC += 2;

    case 0x5: // 0x5XY0: se VX, VY
        if (V[byte_1] == V[byte_2])
            PC += 2;

    case 0x6: // 0x6XNN: ld VX, byte
        V[byte_1] = (cbyte_1);

    case 0x7: // 0x7XNN: add VX, byte
        V[byte_1] += (cbyte_1);

    case 0x8:
        switch (inst & 0x000F) {
        case 0x0: // 0x8XY0: ld VX, VY
            V[byte_1] = V[byte_2];

        case 0x1: // 0x8XY1: or VX, VY
            V[byte_1] |= V[byte_2];

        case 0x2: // 0x8XY2: and VX, VY
            V[byte_1] &= V[byte_2];

        case 0x3: // 0x8XY3: xor VX, VY
            V[byte_1] ^= V[byte_2];

        case 0x4: // 0x8XY4: add VX, VY
            uint16_t result = V[byte_1] + V[byte_2];
            V[16] = (result >> 8) & 0x1;
            V[byte_1] = (uint8_t)result;

        case 0x5: // 0x8XY5: sub VX, VY
            int16_t result = V[byte_2] - V[byte_1];
            V[16] = (result >> 8) & 0x1;
            V[byte_1] = (uint8_t)result;

        case 0x6: // 0x8XY6: shr VX {, VY}
            V[16] = V[byte_1] & 0x0001;
            V[byte_1] >>= 1;

        case 0x7: // 0x8XY7: subn VX, VY
            int16_t result = V[byte_1] - V[byte_2];
            V[16] = (result >> 8) & 0x1;
            V[byte_1] = (uint8_t)result;

        case 0xE: // 0x8XYE: shl VX {, VY}
            V[16] = (V[byte_1] >> 7) & 0x1;
            V[byte_1] <<= 1;
        }

    case 0x9: // 9XY0: sne VX, VY
        if (V[byte_1] != V[byte_2])
            PC += 2;

    case 0xA: // ANNN: ld I, addr
        I = (inst & 0x0FFF);

    case 0xB: // BNNN: jp V0, addr
        PC = (inst & 0x0FFF) + V[0];

    case 0xC: // CXNN: rnd VX, byte
        V[byte_1] = (rand() % 0x100) & (cbyte_1);

    case 0xD: // DXYN: drw VX, VY, nibble
        // display(Vx, Vy n);

    case 0xE:
        if ((cbyte_1) == 0x009E) { // EX9E: skp Vx
            if (keyState[V[byte_1]])
                PC += 2;
        } else if ((cbyte_1) == 0x00A1) { // EXA1: skp Vx
            if (!keyState[V[byte_1]])
                PC += 2;
        }

    case 0xF:
        switch (cbyte_1) {
        case (0x07): // FX07: ld VX, DT
            V[byte_1] = DT;

        case (0x0A): // FX0A: kld VX
            V[byte_1] = readKey();

        case (0x15): // FX15: ld DT, VX
            DT = V[byte_1];

        case (0x18): // FX18: ld ST, VX
            ST = V[byte_1];

        case (0x1E): // FX1E: add I, VX
            I += V[byte_1];

        case (0x29): // FX29: ld F, VX
            I = V[byte_1] * 5;
        
        case (0x33): // FX55: bcd Vx

        case (0x55): // FX55: rdmp VX

        case (0x65): // FX65: rrcl VX

        }
    }
    PC += 2;
}

int load()
{
    for (size_t i = 0; i < 80; i++) { // load sprites into [0x0 - 0x50]
        memory[i] = sprites[i];
    }
}

int clear_screen();

int update_keys();

uint8_t readKey();

int main(int argc, char const* argv[])
{
    opcode inst;
    FILE* ptr = fopen(argv[1], "rb");
    for (;;) {
        refresh();
        if (fread(&inst, sizeof(opcode), 1, ptr) != 1)
            break;

        int status = exec(inst);
        if (status == 0)
            break;
    }
    return 0;
}