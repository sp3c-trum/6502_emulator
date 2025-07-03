//
// Created by P!nk on 30.06.2025.
//

#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <string>
#include "Memory.h"

class Emulator;

class Cpu {
private:
    using Byte = unsigned char;
    using Word = unsigned short;

    Word SP; //Stack pointer
    Byte A, X, Y; //Registers
    Byte C : 1; //Carry flag
    Byte Z : 1; //Zero flag
    Byte I : 1; //Interrupt disable
    Byte D : 1; //Decimal mode
    Byte B : 1; //Break command
    Byte V : 1; //Overflow flag
    Byte N : 1; //Negative flag

    enum instructionModes {ACC, IM, ZP, ZPX, ZPY, REL, ABS, ABX, ABY, INX, INY};
    Emulator* emulator = nullptr;

public:
    Word PC; //Program counter                  (out of private for debug purposes)
    enum registers {a, x, y}; //Register names  (out of private for debug purposes)
    enum flags {c, z, i, d, b, v, n}; //        (out of private for debug purposes)
    int totalCycles;

    Cpu(Memory & mem);
    Cpu();
    void attachEmulator(Emulator* emu);
    void reset(Memory & memory);
    void execute(int cycles, Memory & memory);

    Byte fetchByte(int &cycles, Memory & memory);
    Byte readByte(int &cycles, Memory & memory, Byte addr);
    Byte readByte(int &cycles, Memory & memory, Word addr);
    Word readWord(int &cycles, Memory &memory);
    Byte getValueFromZP(int &cycles, Memory &memory, instructionModes mode);
    Byte getValueFromABS(int &cycles, Memory &memory, instructionModes mode);

    void setReg(registers reg, Byte value);
    void setZ(Byte value);
    void setN(Byte value);
    Byte returnReg(registers reg);
    Byte returnFlag(flags flag);

    //Instructions:
    void ADC(instructionModes mode, Memory &memory, int &cycles);
    void AND(instructionModes mode, Memory &memory, int &cycles);
    // void ASL(instructionModes mode, Memory &memory, int &cycles);
    // void BCC(instructionModes mode, Memory &memory, int &cycles);
    // void BCS(instructionModes mode, Memory &memory, int &cycles);
    // void BEQ(instructionModes mode, Memory &memory, int &cycles);
    // void BIT(instructionModes mode, Memory &memory, int &cycles);
    void LDX(instructionModes mode, Memory &memory, int &cycles);
    void LDY(instructionModes mode, Memory &memory, int &cycles);
    void LDA(instructionModes mode, Memory &memory, int &cycles);

};

#endif //CPU_H
