//
// Created by P!nk on 01.07.2025.
//

#ifndef EMULATOR_H
#define EMULATOR_H

#include <cmath>
#include <iosfwd>
#include <vector>
#include "CPU.h"
#include "Memory.h"
class Cpu;

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

    void readROM(const std::string &name);
    void loadROMIntoMem(const std::vector<Byte> &rom, Word addr);
    void loadByteIntoMem(Byte instruction, Word addr = 0x0000);

    static void log(int totalCycles, logMode mode, const std::string &message);
    static void log(int totalCycles, logMode mode, const std::string &message, Byte value);
    static void log(int totalCycles, logMode mode, const std::string &message, Word value);
    static void log(int totalCycles, logMode mode, const std::string &message, const std::string &value);
    static void log(int totalCycles, logMode mode, const std::string &message, bool withValue, const std::string &value);

    void showMemory(Word startingAddress = 0x0000, Word endingAddress = 0x00FF) const;
    void showRegisters() const;
    void showFlag(Cpu::flags flag) const;
};

#endif //EMULATOR_H
