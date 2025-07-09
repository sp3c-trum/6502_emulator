//
// Created by P!nk on 30.06.2025.
//

#ifndef CPU_H
#define CPU_H

#include "Memory.h"
class Emulator;

class Cpu {
private:
    using Byte = unsigned char;
    using Word = unsigned short;

    Word SP{}; //Stack pointer
    Byte A{}, X{}, Y{}; //Registers
    Byte C : 1{}; //Carry flag
    Byte Z : 1{}; //Zero flag
    Byte I : 1{}; //Interrupt disable
    Byte D : 1{}; //Decimal mode
    Byte B : 1{}; //Break command
    Byte V : 1{}; //Overflow flag
    Byte N : 1{}; //Negative flag

    enum instructionModes {ACC, IM, ZP, ZPX, ZPY, REL, ABS, ABX, ABY, INDX, INDY, IN};
    static std::string toString(instructionModes mode);

    [[nodiscard]] Byte encodeFlags() const;
    void decodeFlags(Byte status);

    Emulator* emulator = nullptr;

public:
    Word PC{}; //Program counter                (out of private for debug purposes)
    enum registers {a, x, y}; //Register names  (out of private for debug purposes)
    enum flags {c, z, i, d, b, v, n}; //        (out of private for debug purposes)
    int totalCycles{};

    explicit Cpu(Memory & mem);

    void attachEmulator(Emulator* emu);
    void reset(Memory &memory);
    void execute(int cycles, Memory &memory);

    Byte fetchByte(int &cycles, Memory &memory);
    Byte readByte(int &cycles, Memory &memory, Word addr);
    Word fetchWord(int &cycles, Memory &memory);
    Word readWord(int &cycles, Memory &memory, Word addr);

    void writeToStack(int &cycles, Memory &memory, Byte value);
    Byte fetchFromStack(int &cycles, Memory &memory);

    Byte getValueFromZP(int &cycles, Memory &memory, instructionModes mode);
    Word getValueFromABS(int &cycles, Memory &memory, instructionModes mode);

    void setReg(registers reg, Byte value);
    void setZ(Byte value);
    void setN(Byte value);
    [[nodiscard]] Byte returnReg(registers reg) const;
    [[nodiscard]] Byte returnFlag(flags flag) const;

    void branch(int &cycles, Memory &memory, Byte offset);
    Word getValueFromAddress(int &cycles, Memory &memory, instructionModes mode, const std::string &instruction);
    Word getAddress(int &cycles, Memory &memory, instructionModes mode, const std::string& instruction);

    //Processor Opcodes:
    void ADC(instructionModes mode, Memory &memory, int &cycles);
    void SBC(instructionModes mode, Memory &memory, int &cycles);
    void CMP(instructionModes mode, Memory &memory, int &cycles);

    void AND(instructionModes mode, Memory &memory, int &cycles);
    void EOR(instructionModes mode, Memory &memory, int &cycles);
    void ORA(instructionModes mode, Memory &memory, int &cycles);
    void CPY(instructionModes mode, Memory &memory, int &cycles);
    void CPX(instructionModes mode, Memory &memory, int &cycles);

    void BCC(Memory &memory, int &cycles);
    void BCS(Memory &memory, int &cycles);
    void BEQ(Memory &memory, int &cycles);
    void BMI(Memory &memory, int &cycles);
    void BNE(Memory &memory, int &cycles);
    void BPL(Memory &memory, int &cycles);
    void BVC(Memory &memory, int &cycles);
    void BVS(Memory &memory, int &cycles);

    void INY(Memory &memory, int &cycles);
    void INX(Memory &memory, int &cycles);
    void INC(instructionModes mode, Memory &memory, int &cycles);
    void DEY(Memory &memory, int &cycles);
    void DEX(Memory &memory, int &cycles);
    void DEC(instructionModes mode, Memory &memory, int &cycles);

    void LDX(instructionModes mode, Memory &memory, int &cycles);
    void LDY(instructionModes mode, Memory &memory, int &cycles);
    void LDA(instructionModes mode, Memory &memory, int &cycles);

    void STX(instructionModes mode, Memory &memory, int &cycles);
    void STY(instructionModes mode, Memory &memory, int &cycles);
    void STA(instructionModes mode, Memory &memory, int &cycles);

    void TAX(Memory &memory, int &cycles);
    void TAY(Memory &memory, int &cycles);
    void TXA(Memory &memory, int &cycles);
    void TYA(Memory &memory, int &cycles);

    void JMP(instructionModes mode, Memory &memory, int &cycles);

    void SEI(Memory &memory, int &cycles);
    void SED(Memory &memory, int &cycles);
    void SEC(Memory &memory, int &cycles);
    void CLC(Memory &memory, int &cycles);
    void CLD(Memory &memory, int &cycles);
    void CLI(Memory &memory, int &cycles);
    void CLV(Memory &memory, int &cycles);

    void PHA(Memory &memory, int &cycles);
    void PLA(Memory &memory, int &cycles);
    void PHP(Memory &memory, int &cycles);
    void PLP(Memory &memory, int &cycles);

    void TSX(Memory &memory, int &cycles);
    void TXS(Memory &memory, int &cycles);

    void ROR(instructionModes mode, Memory &memory, int &cycles);
    void ROL(instructionModes mode, Memory &memory, int &cycles);

};

#endif //CPU_H
