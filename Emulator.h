//
// Created by P!nk on 01.07.2025.
//

#ifndef EMULATOR_H
#define EMULATOR_H

#include <iosfwd>
#include <vector>

#include "CPU.h"
#include "Memory.h"


class Emulator {

private:
    using Byte = unsigned char;
    using Word = unsigned short;


public:
    enum logMode {INFO, ERROR, SUCCESS, WARNING, DEBUG};

    Memory mem;
    Cpu cpu;
    std::vector<Byte> ROM;

    Emulator();

    void readROM(std::string name);
    void loadROMIntoMem(std::vector<Byte>, Word addr);
    void loadByteIntoMem(Byte instruction, Word addr = 0x0000);

    void log(logMode mode, std::string message);
    void showMemory(Word startingAddress = 0x0000, Word endingAddress = 0x00FF);
    void showRegisters();
    void showFlag(Cpu::flags flag);
};

#endif //EMULATOR_H
