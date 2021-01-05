#include <iostream>
#include <algorithm>
#include <ctime>
#include "chip8.h"

using namespace std;
unsigned char fontSet[80] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, //0
                0x20, 0x60, 0x20, 0x20, 0x70, //1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
                0x90, 0x90, 0xF0, 0x10, 0x10, //4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
                0xF0, 0x10, 0x20, 0x40, 0x40, //7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
                0xF0, 0x90, 0xF0, 0x90, 0x90, //A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
                0xF0, 0x80, 0x80, 0x80, 0xF0, //C
                0xE0, 0x90, 0x90, 0x90, 0xE0, //D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
                0xF0, 0x80, 0xF0, 0x80, 0x80  //F
        };

Chip8::Chip8() {
    timerCap = (int) ((1.0 / TIMER_HZ) * CLOCKS_PER_SEC);
    nextTick = clock();
    ops[0] = &Chip8::op0;
    ops[1] = &Chip8::op1;
    ops[2] = &Chip8::op2;
    ops[3] = &Chip8::op3;
    ops[4] = &Chip8::op4;
    ops[5] = &Chip8::op5;
    ops[6] = &Chip8::op6;
    ops[7] = &Chip8::op7;
    ops[8] = &Chip8::op8;
    ops[9] = &Chip8::op9;
    ops[0xA] = &Chip8::opA;
    ops[0xB] = &Chip8::opB;
    ops[0xC] = &Chip8::opC;
    ops[0xD] = &Chip8::opD;
    ops[0xE] = &Chip8::opE;
    ops[0xF] = &Chip8::opF;
}

void Chip8::rest() {
    pc = PROGRAM_START_INDEX;
    opcode = 0;
    I = 0;
    sp = 0;
    fill(begin(V), end(V), 0);
    fill(begin(stack), end(stack), 0);
    fill(begin(memory), end(memory), 0);
    fill(begin(gfx), end(gfx), 0);
    fill(begin(key), end(key), false);
    delayTimer = 0;
    soundTimer = 0;
    drawFlag = true;
}

void Chip8::init() {
    int size = sizeof(fontSet);
    for (int i = 0; i < size; i++) {
        memory[i] = fontSet[i];
    }

}

bool Chip8::loadGame(const char *filePath) {
    // Open file
    FILE *pFile = fopen(filePath, "rb");
    if (pFile == NULL) {
        fputs("File error", stderr);
        return false;
    }

    // Check file size
    fseek(pFile, 0, SEEK_END);
    long fsize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int) fsize);

    // Allocate memory to contain the whole file
    unsigned char *buffer = (unsigned char *) malloc(sizeof(char) * fsize);
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        return false;
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, fsize, pFile);
    if (result != fsize) {
        fputs("Reading error", stderr);
        return false;
    }

    // Copy buffer to Chip8 memory
    if ((sizeof(memory) - PROGRAM_START_INDEX) > fsize) {
        for (int i = 0; i < fsize; ++i)
            memory[i + PROGRAM_START_INDEX] = buffer[i];
    } else
        printf("Error: ROM too big for memory");

    // Close file, free buffer
    fclose(pFile);
    free(buffer);

    return true;
}

void Chip8::loop() {
    opcode = memory[pc] << 8 | memory[pc + 1];
    int X = (opcode & 0x0F00) >> 8;
    int Y = (opcode & 0x00F0) >> 4;
    int NN = opcode & 0x00FF;
    int NNN = opcode & 0x0FFF;
    int index = opcode & 0xF000;
    index >>= 12;
    if (index >= 0 && index < 16) {
        (this->*ops[index])(X, Y, NN, NNN);
    }
}

void Chip8::op0(int X, int Y, int NN, int NNN) {
    switch (opcode & 0x000F) {
        case 0x0000:
            for (int i = 0; i < 2048; ++i)
                gfx[i] = 0x0;
            drawFlag = true;
            pc += 2;
            break;

        case 0x000E:
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
    }
}

void Chip8::op1(int X, int Y, int NN, int NNN) {
    pc = NNN;
}

void Chip8::op2(int X, int Y, int NN, int NNN) {
    stack[sp] = pc;
    ++sp;
    pc = NNN;
}

void Chip8::op3(int X, int Y, int NN, int NNN) {
    if (V[X] == (NN))
        pc += 4;
    else
        pc += 2;
}

void Chip8::op4(int X, int Y, int NN, int NNN) {
    if (V[X] != (NN))
        pc += 4;
    else
        pc += 2;
}

void Chip8::op5(int X, int Y, int NN, int NNN) {
    if (V[X] == V[Y])
        pc += 4;
    else
        pc += 2;
}

void Chip8::op6(int X, int Y, int NN, int NNN) {
    V[X] = NN;
    pc += 2;
}

void Chip8::op7(int X, int Y, int NN, int NNN) {
    V[X] += NN;
    pc += 2;
}

void Chip8::op8(int X, int Y, int NN, int NNN) {
    switch (opcode & 0x000F) {
        case 0x0000:
            V[X] = V[Y];
            pc += 2;
            break;

        case 0x0001:
            V[X] |= V[Y];
            pc += 2;
            break;

        case 0x0002:
            V[X] &= V[Y];
            pc += 2;
            break;

        case 0x0003:
            V[X] ^= V[Y];
            pc += 2;
            break;

        case 0x0004:
            if (V[Y] > (0xFF - V[X]))
                V[0xF] = 1; //carry
            else
                V[0xF] = 0;
            V[X] += V[Y];
            pc += 2;
            break;

        case 0x0005:
            if (V[Y] > V[X])
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[X] -= V[Y];
            pc += 2;
            break;

        case 0x0006:
            V[0xF] = V[X] & 0x1;
            V[X] >>= 1;
            pc += 2;
            break;

        case 0x0007:
            if (V[X] > V[Y])
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[X] = V[Y] - V[X];
            pc += 2;
            break;
        case 0x000E:
            V[0xF] = V[X] >> 7;
            V[X] <<= 1;
            pc += 2;
            break;
    }
}

void Chip8::op9(int X, int Y, int NN, int NNN) {
    if (V[X] != V[Y])
        pc += 4;
    else
        pc += 2;
}

void Chip8::opA(int X, int Y, int NN, int NNN) {
    I = NNN;
    pc += 2;
}

void Chip8::opB(int X, int Y, int NN, int NNN) { pc = (NNN) + V[0]; }

void Chip8::opC(int X, int Y, int NN, int NNN) {
    V[X] = (rand() % 0xFF) & (NN);
    pc += 2;
}

void Chip8::opD(int X, int Y, int NN, int NNN) {
    unsigned short x = V[X];
    unsigned short y = V[Y];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;
    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
        pixel = memory[I + yline];
        for (int xline = 0; xline < 8; xline++) {
            if ((pixel & (0x80 >> xline)) != 0) {
                int pos = ((x + xline) % SCREEN_WIDTH) +
                          ((y + yline) % SCREEN_HEIGHT) * SCREEN_WIDTH;
                if (gfx[pos] == 1) {
                    V[0xF] = 1;
                }
                gfx[pos] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
}

void Chip8::opE(int X, int Y, int NN, int NNN) {
    switch (NN) {
        case 0x009E:
            if (key[V[X]])
                pc += 4;
            else
                pc += 2;
            break;

        case 0x00A1:
            if (!key[V[X]])
                pc += 4;
            else
                pc += 2;
            break;
    }
}

void Chip8::opF(int X, int Y, int NN, int NNN) {
    switch (NN) {
        case 0x0007:
            V[X] = delayTimer;
            pc += 2;
            break;

        case 0x000A: {
            bool keyPress = false;
            for (int i = 0; i < 16; ++i) {
                if (key[i]) {
                    V[X] = i;
                    keyPress = true;
                }
            }
            if (!keyPress)
                return;

            pc += 2;
        }
            break;

        case 0x0015:
            delayTimer = V[X];
            pc += 2;
            break;

        case 0x0018:
            soundTimer = V[X];
            pc += 2;
            break;

        case 0x001E:
            if (I + V[X] > 0xFFF)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            I += V[X];
            pc += 2;
            break;

        case 0x0029:
            I = V[X] * 0x5;
            pc += 2;
            break;

        case 0x0033:
            memory[I] = V[X] / 100;
            memory[I + 1] = (V[X] / 10) % 10;
            memory[I + 2] = (V[X] % 100) % 10;
            pc += 2;
            break;

        case 0x0055:
            for (int i = 0; i <= (X); ++i)
                memory[I + i] = V[i];
            I += (X) + 1;
            pc += 2;
            break;

        case 0x0065:
            for (int i = 0; i <= (X); ++i)
                V[i] = memory[I + i];
            I += (X) + 1;
            pc += 2;
            break;
    }
}

void Chip8::timeTick() {
    clock_t now = clock();
    if (now >= nextTick) {
        if (delayTimer > 0)
            --delayTimer;
        if (soundTimer > 0) {
            if (soundTimer == 1 && beep) {
                beep();
            }
            --soundTimer;
        }
        nextTick += timerCap;
    }
}

void Chip8::keyEvent(int index, bool pressed) {
    key[index] = pressed;
};