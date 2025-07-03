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
    SP = 0x0100;
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
    const Byte secondByte = readByte(cycles, memory, addr++);
    const Word wholeAddress = (secondByte << 8) | firstByte;
    return wholeAddress;
}

Cpu::Byte Cpu::getValueFromZP(int &cycles, Memory &memory, Cpu::instructionModes mode) {

    Byte addr = fetchByte(cycles, memory);
    Byte value = 0;

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
        default: {
            emulator->log(Emulator::ERROR, "Failed to read address from zero page.");
            return 0xFF;
        }
    }
}

Cpu::Byte Cpu::getValueFromABS(int &cycles, Memory &memory, instructionModes mode) {

    Word baseAddr = fetchWord(cycles,memory);
    Word addr = 0;
    Byte value = 0;

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
            emulator->log(Emulator::ERROR, "Failed to read absolute address value.");
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
            emulator->log(Emulator::ERROR, "Invalid register: ", reg);
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
                ADC(IM, memory, cycles);
                break;
            case 0x65: //ADC Zero Page
                ADC(ZP, memory, cycles);
                break;
            case 0x75: //ADC Zero Page,X
                ADC(ZPX, memory, cycles);
                break;
            case 0x6D: // ADC Absolute
                ADC(ABS, memory, cycles);
                break;
            case 0x7D: // ADC Absolute,X
                ADC(ABX, memory, cycles);
                break;
            case 0x79: // ADC Absolute,Y
                ADC(ABY, memory, cycles);
                break;
            case 0x29: //AND Immediate
                AND(IM, memory, cycles);
                break;
            case 0x25: //AND Zero Page
                AND(ZP, memory, cycles);
                break;
            case 0x35: //AND Zero Page,X
                AND(ZPX, memory, cycles);
                break;
            case 0x2D: //AND Absolute
                AND(ABS, memory, cycles);
                break;
            case 0x3D: //AND Absolute,X
                AND(ABX, memory, cycles);
                break;
            case 0x39: //AND Absolute,Y
                AND(ABY, memory, cycles);
                break;
            case 0x21: //AND (Indirect,X)
                AND(INX, memory, cycles);
                break;
            case 0x31: //AND (Indirect),Y
                AND(INY, memory, cycles);
                break;
            case 0xA9: //LDA Immediate
                LDA(IM, memory, cycles);
                break;
            case 0xA5: //LDA Zero Page
                LDA(ZP, memory, cycles);
                break;
            case 0xB5: //LDA Zero Page,X
                LDA(ZPX, memory, cycles);
                break;
            case 0xA2: //LDX Immediate
                LDX(IM, memory, cycles);
                break;
            case 0xA6: //LDX Zero Page
                LDX(ZP, memory, cycles);
                break;
            case 0xB6: //LDX Zero Page,Y
                LDX(ZPY, memory, cycles);
                break;
            case 0xAE: //LDX Absolute
                LDX(ABS, memory, cycles);
                break;
            case 0xBE: //LDX Absolute,Y
                LDX(ABY, memory, cycles);
                break;
            case 0xA0: //LDY Immediate
                LDY(IM, memory, cycles);
                break;
            case 0xA4: //LDY Zero Page
                LDY(ZP, memory, cycles);
                break;
            case 0xB4: //LDY Zero Page,X
                LDY(ZPX, memory, cycles);
                break;
            case 0xAC: //LDY Absolute
                LDY(ABS, memory, cycles);
                break;
            case 0xBC: //LDY Absolute,X
                LDY(ABX, memory, cycles);
                break;
            case 0x4A: //Logical Shift Right Accumulator
                //TODO
                break;
            case 0xEA: //NOP
                break;
            default:
                emulator->log(Emulator::ERROR, "Unknown instruction: ", instruction);
                break;
        }
    }
}

void Cpu::ADC(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = 0;

    switch (mode) {
        case IM:
            value = fetchByte(cycles, memory);
            break;
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP);
            break;
        }
        case ZPX: {
            value = getValueFromZP(cycles, memory, ZPX);
            break;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            break;
        }
        case ABX: {
            value = getValueFromABS(cycles, memory, ABX);
            break;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY);
            break;
        }
        default:
            emulator->log(Emulator::ERROR, "Illegal ADC", false);
            return;
    }

    uint16_t carry = (C != 0) ? 1 : 0;

    uint16_t sum = static_cast<uint16_t>(A) + static_cast<uint16_t>(value) + carry;
    Byte result = static_cast<Byte>(sum & 0xFF);

    C = (sum > 0xFF);  // Carry
    V = (~(A ^ value) & (A ^ result) & 0x80) != 0;  // Overflow

    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::AND(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = 0;

    switch (mode) {
        case IM: {
            value = fetchByte(cycles, memory) & A;
            break;
        }
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP) & A;
            break;
        }
        case ZPX: {
            value = getValueFromZP(cycles, memory, ZPX) & A;
            break;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS) & A;
            break;
        }
        case ABX: {
            value = getValueFromABS(cycles, memory, ABX) & A;
            break;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY) & A;
            break;
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal AND");
        }
    }

    setReg(a, value);
    setZ(value);
    setN(value);

}


void Cpu::LDX(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = 0;

    switch (mode) {
        case IM: {
            value = fetchByte(cycles, memory);
            break;
        }
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP);
            break;
        }
        case ZPY: {
            value = getValueFromZP(cycles, memory, ZPY);
            break;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            break;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY);
            break;
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal LDX");
            break;
        }
    }

    setReg(x, value);
    setZ(value);
    setN(value);

}

void Cpu::LDY(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = 0;

    switch (mode) {
        case IM: {
            value = fetchByte(cycles, memory);
            break;
        }
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP);
            break;
        }
        case ZPY: {
            value = getValueFromZP(cycles, memory, ZPY);
            break;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            break;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY);
            break;
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal LDY");
            break;
        }
    }

    setReg(y, value);
    setZ(value);
    setN(value);

}

void Cpu::LDA(instructionModes mode, Memory &memory, int &cycles) {

    Byte value = 0;

    switch (mode) {
        case IM: {
            value = fetchByte(cycles, memory);
            break;
        }
        case ZP: {
            value = getValueFromZP(cycles, memory, ZP);
            break;
        }
        case ZPX: {
            value = getValueFromZP(cycles, memory, ZPX);
            break;
        }
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            break;
        }
        case ABX: {
            value = getValueFromABS(cycles, memory, ABX);
            break;
        }
        case ABY: {
            value = getValueFromABS(cycles, memory, ABY);
            break;
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal LDA");
            break;
        }
    }

    setReg(a, value);
    setZ(value);
    setN(value);

}

void Cpu::JMP(instructionModes mode, Memory &memory, int &cycles) {

    Word value = 0;

    switch (mode) {
        case ABS: {
            value = getValueFromABS(cycles, memory, ABS);
            break;
        }
        case IN: {
            value = readWord(cycles, memory, getValueFromABS(cycles, memory, ABS));
        }
        default: {
            emulator->log(Emulator::ERROR, "Illegal JMP");
        }
    }

    PC = value;

}


Cpu::Cpu(Memory &mem) {
    reset(mem);
}

Cpu::Cpu() {
    PC = 0x0000;
    SP = 0x0100;
    totalCycles = 0;
    A = X = Y = C = Z = I = D = B = V = N = 0;
}