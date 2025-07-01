//
// Created by P!nk on 01.07.2025.
//

#ifndef EMULATOR_H
#define EMULATOR_H

#include "CPU.h"
#include "Memory.h"


class Emulator {
private:
    using Byte = unsigned char;
    using Word = unsigned short;
public:
    Memory mem;
    Cpu cpu;

    Emulator();
    void loadIntoMem(Byte instruction, Word addr = 0x0000);
    void showMemory(Word startingAddress);
    void showRegisters();
    void showFlag(Cpu::flags flag);
    //oid showZeroPage(Memory &memory);
    //void showStack(Memory &memory);
};



#endif //EMULATOR_H
