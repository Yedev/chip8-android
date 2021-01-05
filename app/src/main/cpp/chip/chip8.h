#ifndef VM_CHIP8
#define VM_CHIP8
#define PROGRAM_START_INDEX 0x200
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define TIMER_HZ 60

class Chip8;

typedef void (*BEEP)();

typedef void (Chip8::*OPCODE)(int, int, int, int);

/**
 * Chip8 vm
 * rest -> init -> loadGame -> loop
 *
 */
class Chip8 {
public:
    Chip8();

    void rest(); //清除所有信息

    void init(); //初始化

    void loop(); //解析操作码

    void timeTick();//触发timer

    bool loadGame(const char *filePath);//加载游戏文件

    void keyEvent(int, bool);//按键控制

    unsigned char gfx[SCREEN_WIDTH * SCREEN_HEIGHT]; // Total amount of pixels: 2048
    bool drawFlag;
    BEEP beep;//play sound callback


private:
    unsigned short pc;          // Program counter
    unsigned short opcode;      // Current opcode
    unsigned short I;           // Index register
    unsigned short sp;          // Stack pointer
    unsigned char V[16];        // V-regs (V0-VF)
    unsigned short stack[16];   // Stack (16 levels)
    unsigned char memory[4096]; // Memory (size = 4k)
    unsigned char delayTimer;   // Delay timer
    unsigned char soundTimer;   // Sound timer
    bool key[16];
    clock_t nextTick;
    int timerCap;
    OPCODE ops[16];

    //Clears the screen.
    //Returns from a subroutine.
    void op0(int X, int Y, int NN, int NNN);

    //Jumps to address NNN.
    void op1(int X, int Y, int NN, int NNN);

    //Calls subroutine at NNN.
    void op2(int X, int Y, int NN, int NNN);

    //Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
    void op3(int X, int Y, int NN, int NNN);

    //Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
    void op4(int X, int Y, int NN, int NNN);

    //Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
    void op5(int X, int Y, int NN, int NNN);

    //Sets VX to NN.
    void op6(int X, int Y, int NN, int NNN);

    //Adds NN to VX. (Carry flag is not changed)
    void op7(int X, int Y, int NN, int NNN);

    //8XY0 Sets VX to the value of VY.
    //8XY1 Sets VX to VX or VY. (Bitwise OR operation)
    //8XY2 Sets VX to VX and VY. (Bitwise AND operation)
    //8XY3 Sets VX to VX xor VY.
    //8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
    //8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    //8XY6 Stores the least significant bit of VX in VF and then shifts VX to the right by 1
    //8XY7 Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    //8XYE Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[b]
    void op8(int X, int Y, int NN, int NNN);

    //Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
    void op9(int X, int Y, int NN, int NNN);

    //Sets I to the address NNN.
    void opA(int X, int Y, int NN, int NNN);

    //Jumps to the address NNN plus V0.
    void opB(int X, int Y, int NN, int NNN);

    //Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    void opC(int X, int Y, int NN, int NNN);

    //Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels.
    // Each row of 8 pixels is read as bit-coded starting from memory location I;
    // I value doesn’t change after the execution of this instruction.
    // As described above, VF is set to 1 if any screen pixels are flipped from set to unset
    // when the sprite is drawn, and to 0 if that doesn’t happen
    void opD(int X, int Y, int NN, int NNN);

    //EX9E Skips the next instruction if the key stored in VX is pressed.
    // (Usually the next instruction is a jump to skip a code block)
    //EXA1 Skips the next instruction if the key stored in VX isn't pressed.
    // (Usually the next instruction is a jump to skip a code block)
    void opE(int X, int Y, int NN, int NNN);

    //FX07 Sets VX to the value of the delay timer.
    //FX0A A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
    //FX15 Sets the delay timer to VX.
    //FX18 Sets the sound timer to VX.
    //FX1E Adds VX to I. VF is not affected.[c]
    //FX29 Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
    //FX33 Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I,
    // the middle digit at I plus 1, and the least significant digit at I plus 2.
    // (In other words, take the decimal representation of VX,
    // place the hundreds digit in memory at location in I,
    // the tens digit at location I+1, and the ones digit at location I+2.)
    //FX55 Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
    //FX65 Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d]
    void opF(int X, int Y, int NN, int NNN);
};

#endif