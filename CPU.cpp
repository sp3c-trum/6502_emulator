//
// Created by P!nk on 30.06.2025.
//
#include <iostream>
#include <ostream>
#include "CPU.h"
#include "Memory.h"
#include "Emulator.h"

void Cpu::attachEmulator(Emulator* emu) {
    this->emulator = emu;
}

void Cpu::reset(Memory & memory) {
    PC = memory[0xFFFC] + (memory[0xFFFD] << 8);
    SP = 0x01FF;
    totalCycles = 0;
    A = X = Y = C = Z = I = D = B = V = 0;
}

Cpu::Byte Cpu::fetchByte(int &cycles, Memory &memory) {
    const Byte value = memory[PC];
    cycles--; totalCycles++; PC++;
    return value;
}

Cpu::Word Cpu::fetchWord(int &cycles, Memory &memory) {
    const Byte firstByte = fetchByte(cycles, memory);
    const Byte secondByte = fetchByte(cycles, memory);
    const Word wholeAddress = (secondByte << 8) | firstByte;
    return wholeAddress;
}

Cpu::Byte Cpu::readByte(int &cycles, Memory &memory, Word addr) {
    const Byte value = memory[addr];
    cycles--; totalCycles++;
    return value;
}

Cpu::Word Cpu::readWord(int &cycles, Memory &memory, Word addr) {
    const Byte firstByte = readByte(cycles, memory, addr);
    const Byte secondByte = readByte(cycles, memory, (addr + 1) & 0x00FF);
    const Word wholeAddress = (secondByte << 8) | firstByte;
    return wholeAddress;
}

Cpu::Byte Cpu::getValueFromZP(int &cycles, Memory &memory, Cpu::instructionModes mode) {

    Byte addr = fetchByte(cycles, memory);
    Byte value = 0;
    Word wordAddr;

    switch (mode) {
        case ZP: {
            value = readByte(cycles, memory, addr);
            return value;
        }
        case ZPX: {
            addr += X; cycles--; totalCycles++;
            value = readByte(cycles, memory, addr);
            return value;
        }
        case ZPY: {
            addr += Y; cycles--; totalCycles++;
            value = readByte(cycles, memory, addr);
            return value;
        }
        case INDX: {
            addr += X; cycles--; totalCycles++;
            wordAddr = readWord(cycles, memory, addr);
            value = readByte(cycles, memory, wordAddr);
            return value;
        }
        case INDY: {
            wordAddr = readWord(cycles, memory, addr) + Y;
            cycles--; totalCycles++;
            value = readByte(cycles, memory, wordAddr);
            return value;
        }
        default: {
            Emulator::log(Emulator::ERROR, "Failed to read address from zero page.");
            return 0xFF;
        }
    }
}

Cpu::Word Cpu::getValueFromABS(int &cycles, Memory &memory, instructionModes mode) {

    Word baseAddr = fetchWord(cycles,memory);
    Word addr = 0;
    Word value = 0;

    switch (mode) {
        case ABS: {
            value = readByte(cycles, memory, baseAddr);
            return value;
        }
        case ABX: {
            addr = baseAddr + X;
            value = readByte(cycles, memory, addr);
            if ((baseAddr & 0xFF00) != (addr & 0xFF00)) {
                cycles--; totalCycles++; //Another cycle if the value crosses a memory page
            }
            return value;
        }
        case ABY: {
            addr = baseAddr + Y;
            value = readByte(cycles, memory, addr);
            if ((baseAddr & 0xFF00) != (addr & 0xFF00)) {
                cycles--; totalCycles++; //Another cycle if the value crosses a memory page
            }
            return value;
        }
        default: {
            Emulator::log(Emulator::ERROR, "Failed to read absolute address value.");
            return 0xFF;
        }
    }
}


void Cpu::setReg(registers reg, Byte value) {
    switch (reg) {
        case a:
            A = value;
            break;
        case x:
            X = value;
            break;
        case y:
            Y = value;
            break;
        default:
            Emulator::log(Emulator::ERROR, "Invalid register: ", reg);
            std::cout << "Unknown register: " << reg << std::endl;
    }
}

void Cpu::setZ(Byte value) {
    Z = (value == 0);
}

void Cpu::setN(Byte value) {
    N = (value & 0x80) != 0;
}

Cpu::Byte Cpu::returnReg(registers reg) const {
    switch (reg) {
        case a:
            return A;
        case x:
            return X;
        case y:
            return Y;
        default:
            std::cout << "Unknown register: " << reg << std::endl;
            return 0x00;
    }
}

Cpu::Byte Cpu::returnFlag(flags flag) const {
    switch (flag) {
        case c:
            return C;
        case z:
            return Z;
        case i:
            return I;
        case d:
            return D;
        case b:
            return B;
        case v:
            return V;
        case n:
            return N;
        default:
            std::cout << "Unknown register: " << flag << std::endl;
            return 0x00;
    }
}

void Cpu::execute(int cycles, Memory & memory) {
    while (cycles > 0) {
        switch (Byte instruction = fetchByte(cycles, memory)) {
            case 0x69: //ADC Immediate
                ADC(IM, memory, cycles); break;
            case 0x65: //ADC Zero Page
                ADC(ZP, memory, cycles); break;
            case 0x75: //ADC Zero Page,X
                ADC(ZPX, memory, cycles); break;
            case 0x6D: // ADC Absolute
                ADC(ABS, memory, cycles); break;
            case 0x7D: // ADC Absolute,X
                ADC(ABX, memory, cycles); break;
            case 0x79: // ADC Absolute,Y
                ADC(ABY, memory, cycles); break;
            case 0x61: //ADC (Indirect,X)
                ADC(INDX, memory, cycles); break;
            case 0x71: //ADC (Indirect),Y
                ADC(INDY, memory, cycles); break;
            case 0x29: //AND Immediate
                AND(IM, memory, cycles); break;
            case 0x25: //AND Zero Page
                AND(ZP, memory, cycles); break;
            case 0x35: //AND Zero Page,X
                AND(ZPX, memory, cycles); break;
            case 0x2D: //AND Absolute
                AND(ABS, memory, cycles); break;
            case 0x3D: //AND Absolute,X
                AND(ABX, memory, cycles); break;
            case 0x39: //AND Absolute,Y
                AND(ABY, memory, cycles); break;
            case 0x21: //AND (Indirect,X)
                AND(INDX, memory, cycles); break;
            case 0x31: //AND (Indirect),Y
                AND(INDY, memory, cycles); break;
            case 0xA9: //LDA Immediate
                LDA(IM, memory, cycles); break;
            case 0xA5: //LDA Zero Page
                LDA(ZP, memory, cycles); break;
            case 0xB5: //LDA Zero Page,X
                LDA(ZPX, memory, cycles); break;
            case 0xAD: //LDA Absolute
                LDA(ABS, memory, cycles); break;
            case 0xBD: //LDA Absolute,X
                LDA(ABX, memory, cycles); break;
            case 0xB9: //LDA Absolute,Y
                LDA(ABY, memory, cycles); break;
            case 0xA1: //LDA (Indirect,X)
                LDA(INDX, memory, cycles); break;
            case 0xB1: //LDA (Indirect),Y
                LDA(INDY, memory, cycles); break;
            case 0xA2: //LDX Immediate
                LDX(IM, memory, cycles); break;
            case 0xA6: //LDX Zero Page
                LDX(ZP, memory, cycles); break;
            case 0xB6: //LDX Zero Page,Y
                LDX(ZPY, memory, cycles); break;
            case 0xAE: //LDX Absolute
                LDX(ABS, memory, cycles); break;
            case 0xBE: //LDX Absolute,Y
                LDX(ABY, memory, cycles); break;
            case 0xA0: //LDY Immediate
                LDY(IM, memory, cycles); break;
            case 0xA4: //LDY Zero Page
                LDY(ZP, memory, cycles); break;
            case 0xB4: //LDY Zero Page,X
                LDY(ZPX, memory, cycles); break;
            case 0xAC: //LDY Absolute
                LDY(ABS, memory, cycles); break;
            case 0xBC: //LDY Absolute,X
                LDY(ABX, memory, cycles); break;
            case 0x85: //STA Zero Page
                STA(ZP, memory, cycles); break;
            case 0x95: //STA Zero Page,X
                STA(ZPX, memory, cycles); break;
            case 0x8D: //STA Absolute
                STA(ABS, memory, cycles); break;
            case 0x9D: //STA Absolute,X
                STA(ABX, memory, cycles); break;
            case 0x99: //STA Absolute,Y
                STA(ABY, memory, cycles); break;
            case 0x81: //STA (Indirect,X)
                STA(INDX, memory, cycles); break;
            case 0x91: //STA (Indirect), Y
                STA(INDY, memory, cycles); break;
            case 0x86: //STX Zero Page
                STX(ZP, memory, cycles); break;
            case 0x96: //STX Zero Page,Y
                STX(ZPY, memory, cycles); break;
            case 0x8E: //STX Absolute
                STX(ABS, memory, cycles); break;
            case 0x84: //STY Zero Page
                STY(ZP, memory, cycles); break;
            case 0x94: //STY Zero Page,X
                STY(ZPX, memory, cycles); break;
            case 0x8C: //STY Absolute
                STY(ABS, memory, cycles); break;
            case 0x4C: //JMP Absolute
                JMP(ABS, memory, cycles); break;
            case 0x6C: //JMP Indirect
                JMP(IN, memory, cycles); break;
            case 0xEA: //NOP
                cycles--; totalCycles++; break;
            case 0x78: //SEI
                SEI(memory, cycles);  break;
            case 0xF8: //SED
                SED(memory, cycles); break;
            case 0x38: //SEC
                SEC(memory, cycles); break;
            case 0xAA: //TAX
                TAX(memory, cycles); break;
            case 0xA8: //TAY
                TAY(memory, cycles); break;
            case 0x8A: //TXA
                TXA(memory, cycles); break;
            case 0x98: //TYA
                TYA(memory, cycles); break;
            default:
                Emulator::log(Emulator::ERROR, "Unknown instruction: ", instruction);
                break;
        }
    }
}

Cpu::Word Cpu::getAddress(int &cycles, Memory &memory, instructionModes mode, const std::string& instruction) {

    Word address = 0x00;

    switch (mode) {
        case ZP: {
            address = fetchByte(cycles, memory);
            return address;
        }
        case ZPX: {
            address = fetchByte(cycles, memory) + X;
            cycles--; totalCycles++;
            return address;
        }
        case ZPY: {
            address = fetchByte(cycles, memory) + Y;
            cycles--; totalCycles++;
            return address;
        }
        case ABS: {
            address = fetchWord(cycles, memory);
            return address;
        }
        case ABX: {
            address = fetchWord(cycles, memory) + X;
            cycles--; totalCycles++;
            return address;
        }
        case ABY: {
            address = fetchWord(cycles, memory) + Y;
            cycles--; totalCycles++;
            return address;
        }
        case INDX: {
            address = fetchByte(cycles, memory) + X;
            cycles--; totalCycles++;
            address = readWord(cycles, memory, address);
            return address;
        }
        case INDY: {
            address = fetchByte(cycles, memory);
            address = readWord(cycles, memory, address) + Y;
            return address;
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal instruction: " + instruction);
            return 0x00;
        }
    }
}


Cpu::Word Cpu::getValueFromAddress(int &cycles, Memory &memory, instructionModes mode, std::string instruction) {

    Word value = 0x00;

    switch (mode) {
        case IM:
            value = fetchByte(cycles, memory);
            return value;
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP);
            return value;
        }
        case ZPX: {
            value = getValueFromZP(cycles, memory, ZPX);
            return value;
        }
        case ZPY: {
            value = getValueFromZP(cycles, memory, ZPY);
            return value;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            return value;
        }
        case ABX: {
            value = getValueFromABS(cycles, memory, ABX);
            return value;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY);
            return value;
        }
        case IN: {
            value = readWord(cycles, memory, getValueFromABS(cycles, memory, ABS));
            return value;
        }
        case INDX: {
            value = getValueFromZP(cycles, memory, INDX);
            return value;
        }
        case INDY: {
            value = getValueFromZP(cycles, memory, INDY);
            return value;
        }
        default:
            emulator->log(Emulator::ERROR, "Illegal instruction", true);
            return 0x00;
    }
}

void Cpu::ADC(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = getValueFromAddress(cycles, memory, mode, "ADC");
    Word sum = static_cast<uint16_t>(A) + static_cast<uint16_t>(value) + static_cast<uint16_t>(C);
    Byte result = static_cast<Byte>(sum & 0xFF);

    C = (sum > 0xFF);  // Carry
    V = (~(A ^ value) & (A ^ result) & 0x80) != 0;  // Overflow

    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::INY(Memory &memory, int &cycles) {
    Y++; cycles--; totalCycles++;
    setN(Y);
    setZ(Y);
}


void Cpu::INX(Memory &memory, int &cycles) {
    X++; cycles++; totalCycles++;
    setN(X);
    setZ(X);
}

void Cpu::INC(instructionModes mode, Memory &memory, int &cycles) {
    Word addr = getAddress(cycles, memory, mode, "INC");
    memory[addr]++; cycles--; totalCycles++;
}

void Cpu::AND(instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "AND");
    Byte result = value & A;
    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::LDX(instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDX");
    setReg(x, value);
    setZ(value);
    setN(value);
}

void Cpu::LDY(instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDY");
    setReg(y, value);
    setZ(value);
    setN(value);
}

void Cpu::LDA(instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDA");
    setReg(a, value);
    setZ(value);
    setN(value);
}

void Cpu::STX(instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STX");
    memory.writeByte(address, X);
}

void Cpu::STY(instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STY");
    memory.writeByte(address, Y);
}

void Cpu::STA(instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STA");
    memory.writeByte(address, A);
}

void Cpu::JMP(instructionModes mode, Memory &memory, int &cycles) {
    Word value = getAddress(cycles, memory, mode, "JMP");
    PC = value;
}

void Cpu::SEI(Memory &memory, int &cycles) {
    I = 1; cycles--; totalCycles++;
}

void Cpu::SED(Memory &memory, int &cycles) {
    D = 1; cycles--; totalCycles++;
}

void Cpu::SEC(Memory &memory, int &cycles) {
    C = 1; cycles--; totalCycles++;
}

void Cpu::TAX(Memory &memory, int &cycles) {
    setReg(x, A); cycles--; totalCycles++;
    setZ(x);
    setN(x);
}

void Cpu::TAY(Memory &memory, int &cycles) {
    setReg(y, A); cycles--; totalCycles++;
    setZ(y);
    setN(y);
}

void Cpu::TXA(Memory &memory, int &cycles) {
    setReg(a, X); cycles--; totalCycles++;
    setZ(a);
    setN(a);
}

void Cpu::TYA(Memory &memory, int &cycles) {
    setReg(a, Y); cycles--; totalCycles++;
    setZ(a);
    setN(a);
}

Cpu::Cpu(Memory &mem) {
    reset(mem);
}