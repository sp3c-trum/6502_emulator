//
// Created by P!nk on 30.06.2025.
//
#include "CPU.h"

#include <iostream>
#include <ostream>

#include "Memory.h"

void Cpu::reset(Memory & memory) {
    PC = 0x0000;
    SP = 0x0100;
    A = X = Y = C = Z = I = D = B = V = 0;
    memory.clear();
}

Cpu::Byte Cpu::fetchByte(int &cycles, Memory &memory) {
    const Byte value = memory[PC];
    PC++;
    cycles--;
    return value;
}

Cpu::Word Cpu::getAbsoluteAddr(int &cycles, Memory &memory) {
    Byte firstByte = fetchByte(cycles, memory);
    Byte secondByte = fetchByte(cycles, memory);
    Word wholeAddress = (firstByte << 8) | secondByte;
    return wholeAddress;
}

Cpu::Byte Cpu::readByte(int &cycles, Memory &memory, Byte addr) {
    const Byte value = memory[addr];
    cycles--;
    return value;
}

Cpu::Byte Cpu::readByte(int &cycles, Memory &memory, Word addr) {
    const Byte value = memory[addr];
    cycles--;
    return value;
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
            std::cout << "Unknown register: " << reg << std::endl;
    }
}

Cpu::Byte Cpu::returnReg(registers reg) {
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

Cpu::Byte Cpu::returnFlag(flags flag) {
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
                PC++;
                break;
            default:
                std::cout << "Unknown instruction: " << std::hex << (int) instruction << std::hex << std::endl;
                break;
        }
    }
}

void Cpu::ADC(instructionModes mode, Memory &memory, int &cycles) {

    Byte operand = 0;

    switch (mode) {
        case IM:
            operand = fetchByte(cycles, memory);
            break;
        case ZP: {
            Byte addr = fetchByte(cycles, memory);
            operand = readByte(cycles, memory, addr);
            break;
        }
        case ZPX: {
            Byte addr = fetchByte(cycles, memory);
            addr += X; cycles--;
            operand = readByte(cycles, memory, addr);
            break;
        }
        case ABS: {
            Word addr = getAbsoluteAddr(cycles, memory);
            operand = readByte(cycles, memory, addr);
            break;
        }
        case ABX: {
            Word addr = getAbsoluteAddr(cycles, memory) + X;
            operand = readByte(cycles, memory, addr);
            break;
        }
        case ABY: {
            Word addr = getAbsoluteAddr(cycles, memory) + Y;
            operand = readByte(cycles, memory, addr);
            break;
        }
        default:
            std::cout << "Illegal ADC\n";
            return;
    }

    uint16_t sum = A + operand + C;
    C = (sum > 0xFF);
    V = (~(A ^ operand) & (A ^ sum) & 0x80) != 0;    // Jeśli operand i A miały ten sam znak, ale wynik ma inny znak to ustaw overflow

    setReg(a, static_cast<Byte>(sum & 0xFF));

    Z = A == 0;
    N = (A & 0x80) != 0;
}

void Cpu::AND(instructionModes mode, Memory &memory, int &cycles) {

    Byte memoryValue = 0;
    Byte finalValue = 0;
    Word address = 0;

    switch (mode) {
        case IM: {
            finalValue = fetchByte(cycles, memory) & A;
            setReg(a, finalValue);
            break;
        }
        case ZP: {
            finalValue = readByte(cycles, memory, fetchByte(cycles, memory)) & A;
            setReg(a, finalValue);
            break;
        }
        case ZPX: {
            memoryValue = fetchByte(cycles, memory);
            finalValue = readByte(cycles, memory, static_cast<Byte>(memoryValue + X)) & A;
            setReg(a, finalValue);
            cycles--;
            break;
        }
        case ABS: {
            address = getAbsoluteAddr(cycles, memory);
            finalValue = readByte(cycles, memory, address) & A;
            setReg(a, finalValue);
            break;
        }
        case ABX: {
            address = getAbsoluteAddr(cycles, memory) + X;
            finalValue = readByte(cycles, memory, address) & A;
            setReg(a, finalValue);
            break;
        }
        case ABY: {
            address = getAbsoluteAddr(cycles, memory) + Y;
            finalValue = readByte(cycles, memory, address) & A;
            setReg(a, finalValue);
            break;
        }
            default: {
            std::cout << "Illegal AND\n";
        }
    }

    Z = A == 0;
    N = (A & 0x80) != 0;
}


void Cpu::LDX(instructionModes mode, Memory &memory, int &cycles) {

    Byte memoryValue = 0;
    Byte finalValue = 0;
    Word address = 0;

    switch (mode) {
        case IM: {
            setReg(x, fetchByte(cycles, memory));
            break;
        }
        case ZP: {
            memoryValue = fetchByte(cycles, memory);
            setReg(x, readByte(cycles,memory, memoryValue));
            break;
        }
        case ZPY: {
            memoryValue = fetchByte(cycles, memory);
            finalValue = memoryValue + y; cycles--;
            setReg(x, readByte(cycles,memory, finalValue));
            break;
        }
        case ABS: {
            address = getAbsoluteAddr(cycles, memory);
            finalValue = readByte(cycles, memory, address);
            setReg(x, finalValue);
            break;
        }
        case ABY: {
            address = getAbsoluteAddr(cycles, memory) + Y;
            finalValue = readByte(cycles, memory, address);
            setReg(x, finalValue);
            break;
        }
        default: {
            std::cout << "Illegal LDX\n";
            break;
        }
    }

    Z = A == 0;
    N = (A & 0x80) != 0;
}

void Cpu::LDY(instructionModes mode, Memory &memory, int &cycles) {

    Byte memoryValue = 0;
    Byte finalValue = 0;
    Word address = 0;

    switch (mode) {
        case IM: {
            setReg(y, fetchByte(cycles, memory));
            break;
        }
        case ZP: {
            memoryValue = fetchByte(cycles, memory);
            setReg(y, readByte(cycles, memory, memoryValue));
            break;
        }
        case ZPX: {
            memoryValue = fetchByte(cycles, memory);
            finalValue = memoryValue + x; cycles--;
            setReg(y, readByte(cycles, memory, finalValue));
            break;
        }
        case ABS: {
            address = getAbsoluteAddr(cycles, memory);
            finalValue = readByte(cycles, memory, address);
            setReg(y, finalValue);
            break;
        }
        case ABX: {
            address = getAbsoluteAddr(cycles, memory) + X;
            finalValue = readByte(cycles, memory, address);
            setReg(y, finalValue);
            break;
        }
        default: {
            std::cout << "Illegal LDY\n";
            break;
        }
    }

    Z = A == 0;
    N = (A & 0x80) != 0;
}

void Cpu::LDA(instructionModes mode, Memory &memory, int &cycles) {
    switch (mode) {
        case IM: {
            setReg(a, fetchByte(cycles, memory));
            break;
        }
        case ZP: {
            Byte ZPAddress = fetchByte(cycles, memory);
            setReg(a, readByte(cycles,memory, ZPAddress));
            break;
        }
        case ZPX: {
            Byte ZPXAddress = fetchByte(cycles, memory);
            Byte ZPXDest = ZPXAddress + x; cycles--;
            setReg(a, readByte(cycles,memory, ZPXDest));
            break;
        }
        default: {
            std::cout << "Illegal LDA\n";
            break;
        }
    }

    Z = A == 0;
    N = (A & 0x80) != 0;
}

Cpu::Cpu(Memory &mem) {
    reset(mem);
}

Cpu::Cpu() {
    //Stub
}