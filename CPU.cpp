//
// Created by P!nk on 30.06.2025.
//
#include <iostream>
#include <ostream>
#include "CPU.h"
#include "Memory.h"
#include "Emulator.h"

std::string Cpu::toString(const instructionModes mode) {
    switch (mode) {
        case IM:   return "IM";
        case ZP:   return "ZP";
        case ZPX:  return "ZPX";
        case ZPY:  return "ZPY";
        case ABS:  return "ABS";
        case ABX:  return "ABX";
        case ABY:  return "ABY";
        case IN:   return "IN";
        case INDX: return "INDX";
        case INDY: return "INDY";
        default:   return "UNKNOWN";
    }
}

Cpu::Byte Cpu::encodeFlags() const {
    Byte status = 0;

    status |= (N << 7);
    status |= (V << 6);
    status |= (1 << 5);            // Unused bit zawsze ustawiony na 1
    status |= (B << 4);
    status |= (D << 3);
    status |= (I << 2);
    status |= (Z << 1);
    status |= C;

    return status;
}

void Cpu::decodeFlags(const Byte status) {
    N = (status >> 7) & 1;
    V = (status >> 6) & 1;
    B = (status >> 4) & 1;
    D = (status >> 3) & 1;
    I = (status >> 2) & 1;
    Z = (status >> 1) & 1;
    C = status & 1;
}

void Cpu::attachEmulator(Emulator *emu) {
    this->emulator = emu;
}

void Cpu::reset(Memory &memory) {
    PC = memory[0xFFFC] + (memory[0xFFFD] << 8);
    SP = 0xFF;
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

Cpu::Byte Cpu::readByte(int &cycles, Memory &memory, const Word addr) {
    const Byte value = memory[addr];
    cycles--; totalCycles++;
    return value;
}

Cpu::Word Cpu::readWord(int &cycles, Memory &memory, const Word addr) {
    const Byte firstByte = readByte(cycles, memory, addr);
    const Byte secondByte = readByte(cycles, memory, (addr + 1) & 0x00FF);
    const Word wholeAddress = (secondByte << 8) | firstByte;
    return wholeAddress;
}

void Cpu::writeToStack(int &cycles, Memory &memory, Byte value) {
    memory[0x0100 + SP] = value;
    SP--; totalCycles++; cycles--;
}

void Cpu::writeWordToStack(int &cycles, Memory &memory, Word value) {
    const Byte high = (value >> 8) & 0xFF;
    const Byte low  = value & 0xFF;

    writeToStack(cycles, memory, high);
    writeToStack(cycles, memory, low);
}

Cpu::Byte Cpu::fetchFromStack(int &cycles, Memory &memory) {
    SP++; totalCycles++; cycles--;
    return memory[0x0100 + SP];
}


Cpu::Word Cpu::fetchWordFromStack(int &cycles, Memory &memory) {
    const Byte high = fetchFromStack(cycles, memory);
    const Byte low = fetchFromStack(cycles, memory);
    return (high << 8) | low;
}

void Cpu::branch(int &cycles, const Byte offset) {
    const Word oldPC = PC;
    const auto signedOffset = static_cast<int8_t>(offset);
    PC += signedOffset - 1;
    cycles--; totalCycles++;

    if ((oldPC & 0xFF00) != (PC & 0xFF00)) {
        cycles--; totalCycles++;
    }
}

Cpu::Byte Cpu::getValueFromZP(int &cycles, Memory &memory, const instructionModes mode) {

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
            Emulator::log(totalCycles, Emulator::ERROR, "Failed to read address from zero page.");
            return 0xFF;
        }
    }
}

Cpu::Word Cpu::getValueFromABS(int &cycles, Memory &memory, const instructionModes mode) {

    const Word baseAddr = fetchWord(cycles,memory);
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
            Emulator::log(totalCycles, Emulator::ERROR, "Failed to read absolute address value.");
            return 0xFF;
        }
    }
}

void Cpu::setReg(const registers reg, const Byte value) {
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
            Emulator::log(totalCycles, Emulator::ERROR, "Invalid register: ", static_cast<Byte>(reg));
            std::cout << "Unknown register: " << reg << std::endl;
    }
}

void Cpu::setZ(const Byte value) {
    Z = (value == 0);
}

void Cpu::setN(const Byte value) {
    N = (value & 0x80) != 0;
}

Cpu::Byte Cpu::returnReg(const registers reg) const {
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

Cpu::Byte Cpu::returnFlag(const flags flag) const {
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
            std::cout << "Unknown flag: " << flag << std::endl;
            return 0x00;
    }
}

void Cpu::execute(int cycles, Memory &memory) {
    while (cycles > 0) {
        const Byte instruction = fetchByte(cycles, memory);
        std::cout << "\nCykl: " << totalCycles
            << ", Instrukcja: " << static_cast<int>(instruction);
            // << "\nRegister A = " << static_cast<int>(A)
            // << "\nRegister X = " <<  static_cast<int>(X)
            // << "\nRegister Y = " << static_cast<int>(Y) << "\n";
        switch (instruction) {
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
            case 0xE9: //SBC
                SBC(IM, memory, cycles); break;
            case 0xE5: //SBC Zero Page
                SBC(ZP, memory, cycles); break;
            case 0xF5: //SBC Zero Page,X
                SBC(ZPX, memory, cycles); break;
            case 0xED: //SBC Absolute
                SBC(ABS, memory, cycles); break;
            case 0xFD: //SBC Absolute,X
                SBC(ABX, memory, cycles); break;
            case 0xF9: //SBC Absolute,Y
                SBC(ABY, memory, cycles); break;
            case 0xE1: //SBC (Indirect,X)
                SBC(INDX, memory, cycles); break;
            case 0xF1: //SBC (Indirect),Y
                SBC(INDY, memory, cycles); break;
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
            case 0x18: //CLC
                CLC(memory, cycles); break;
            case 0xD8: //CLD
                CLD(memory, cycles); break;
            case 0x58: //CLI
                CLI(memory, cycles); break;
            case 0xB8: //CLV
                CLV(memory, cycles); break;
            case 0xAA: //TAX
                TAX(memory, cycles); break;
            case 0xA8: //TAY
                TAY(memory, cycles); break;
            case 0x8A: //TXA
                TXA(memory, cycles); break;
            case 0x98: //TYA
                TYA(memory, cycles); break;
            case 0xE6: //INC Zero Page
                INC(ZP, memory, cycles); break;
            case 0xF6: //INC ZeroPage,X
                INC(ZPX, memory, cycles); break;
            case 0xEE: //INC Absolute
                INC(ABS, memory, cycles); break;
            case 0xFE: //INC Absolute,X
                INC(ABX, memory, cycles); break;
            case 0xE8: //INX
                INX(memory, cycles); break;
            case 0xC8: //INY
                INY(memory, cycles); break;
            case 0xC6: //DEX Zero Page
                DEC(ZP, memory, cycles); break;
            case 0xD6: //DEX Zero Page,X
                DEC(ZPX, memory, cycles); break;
            case 0xCE: //DEX Absolute
                DEC(ABS, memory, cycles); break;
            case 0xDE: //DEX Absolute,X
                DEC(ABX, memory, cycles); break;
            case 0xCA: //DEX
                DEX(memory, cycles); break;
            case 0x88: //DEY
                DEY(memory, cycles); break;
            case 0x49: //EOR
                EOR(IM, memory, cycles); break;
            case 0x45: //EOR Zero Page
                EOR(ZP, memory, cycles); break;
            case 0x55: //EOR Zero Page,X
                EOR(ZPX, memory, cycles); break;
            case 0x4D: //EOR Absolute
                EOR(ABS, memory, cycles); break;
            case 0x5D: //EOR Absolute,X
                EOR(ABX, memory, cycles); break;
            case 0x59: //EOR Absolute,Y
                EOR(ABY, memory, cycles); break;
            case 0x41: //EOR (Indirect,X)
                EOR(INDX, memory, cycles); break;
            case 0x51: //EOR (Indirect),Y
                EOR(INDY, memory, cycles); break;
            case 0x09: //ORA
                ORA(IM, memory, cycles); break;
            case 0x05: //ORA Zero Page
                ORA(ZP, memory, cycles); break;
            case 0x15: //ORA Zero Page,X
                ORA(ZPX, memory, cycles); break;
            case 0x0D: //ORA Absolute
                ORA(ABS, memory, cycles); break;
            case 0x1D: //ORA Absolute,X
                ORA(ABX, memory, cycles); break;
            case 0x19: //ORA Absolute,Y
                ORA(ABY, memory, cycles); break;
            case 0x01: //ORA (Indirect,X)
                ORA(INDX, memory, cycles); break;
            case 0x11: //ORA (Indirect),Y
                ORA(INDY, memory, cycles); break;
            case 0x70: //BVS
                BVS(memory, cycles); break;
            case 0x50: //BVC
                BVC(memory, cycles); break;
            case 0x10: //BPL
                BPL(memory, cycles); break;
            case 0xD0: //BNE
                BNE(memory, cycles); break;
            case 0x30: //BMI
                BMI(memory, cycles); break;
            case 0xF0: //BEQ
                BEQ(memory, cycles); break;
            case 0xB0: //BCS
                BCS(memory, cycles); break;
            case 0x90: //BCC
                BCC(memory, cycles); break;
            case 0xC9: //CMP
                CMP(IM, memory, cycles); break;
            case 0xC5: //CMP Zero Page
                CMP(ZP, memory, cycles); break;
            case 0xD5: //CMP Zero Page,X
                CMP(ZPX, memory, cycles); break;
            case 0xCD: //CMP Absolute
                CMP(ABS, memory, cycles); break;
            case 0xDD: //CMP Absolute,X
                CMP(ABX, memory, cycles); break;
            case 0xD9: //CMP Absolute,Y
                CMP(ABY, memory, cycles); break;
            case 0xC1: //CMP (Indirect,X)
                CMP(INDX, memory, cycles); break;
            case 0xD1: //CMP (Indirect),Y
                CMP(INDY, memory, cycles); break;
            case 0x48: //PHA
                PHA(memory, cycles); break;
            case 0x08: //PHP
                PHP(memory, cycles); break;
            case 0x68: //PLA
                PLA(memory, cycles); break;
            case 0x28: //PLP
                PLP(memory, cycles); break;
            case 0xBA: //TSX
                TSX(memory, cycles); break;
            case 0x9A: //TXS
                TXS(memory, cycles); break;
            case 0x2A: //ROL Accumulator
                ROL(ACC, memory, cycles); break;
            case 0x26: //ROL Zero Page
                ROL(ZP, memory, cycles); break;
            case 0x36: //ROL Zero Page,X
                ROL(ZPX, memory, cycles); break;
            case 0x2E: //ROL Absolute
                ROL(ABS, memory, cycles); break;
            case 0x3E: //ROL Absolute,X
                ROL(ABX, memory, cycles); break;
            case 0x6A: //ROR Accumulator
                ROR(ACC, memory, cycles); break;
            case 0x66: //ROR Zero Page
                ROR(ZP, memory, cycles); break;
            case 0x76: //ROR Zero Page,X
                ROR(ZPX, memory, cycles); break;
            case 0x6E: //ROR Absolute
                ROR(ABS, memory, cycles); break;
            case 0x7E: //ROR Absolute,X
                ROR(ABX, memory, cycles); break;
            case 0xE0: //CPX
                CPX(IM, memory, cycles); break;
            case 0xE4: //CPX Zero Page
                CPX(ZP, memory, cycles); break;
            case 0xEC: //CPX Absolute
                CPX(ABS, memory, cycles); break;
            case 0xC0: //CPY
                CPY(IM, memory, cycles); break;
            case 0xC4: //CPY Zero Page
                CPY(ZP, memory, cycles); break;
            case 0xCC: //CPY Absolute
                CPY(ABS, memory, cycles); break;
            case 0x20: //JSR
                JSR(memory, cycles); break;
            case 0x60: //RTS
                RTS(memory, cycles); break;
            case 0x00: //BRK
                BRK(memory, cycles); break;
            case 0x40: //RTI
                RTI(memory, cycles); break;
            case 0x24: //BIT Zero Page
                BIT(ZP, memory, cycles); break;
            case 0x2C: //BIT Absolute
                BIT(ABS, memory, cycles); break;
            case 0x4A: //LSR Accumulator
                LSR(ACC, memory, cycles); break;
            case 0x46: //LSR Zero Page
                LSR(ZP, memory, cycles); break;
            case 0x56: //LSR Zero Page,X
                LSR(ZPX, memory, cycles); break;
            case 0x4E: //LSR Absolute
                LSR(ABS, memory, cycles); break;
            case 0x5E: //LSR Absolute,X
                LSR(ABX, memory, cycles); break;
            case 0x0A: //ASL Accumulator
                ASL(ACC, memory, cycles); break;
            case 0x06: //ASL Zero Page
                ASL(ZP, memory, cycles); break;
            case 0x16: //ASL Zero Page,X
                ASL(ZPX, memory, cycles); break;
            case 0x0E: //ASL Absolute
                ASL(ABS, memory, cycles); break;
            case 0x1E: //ASL Absolute,X
                ASL(ABX, memory, cycles); break;
            case 0xFF: // CUSTOM OPCODE - Halt CPU.
                std::cout << "\nHalting CPU - encountered 0xFF";
                cycles = 0;
                break;
            default:
                Emulator::log(totalCycles, Emulator::ERROR, "Unknown instruction: ", instruction);
                break;
        }
    }
}

Cpu::Word Cpu::getAddress(int &cycles, Memory &memory, const instructionModes mode, const std::string& instruction) {

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
        case IN: {
            address = fetchWord(cycles, memory);
            Word indirectAddr;
            if ((address & 0x00FF) == 0x00FF) {
                // 6502 Bug - Page boundary wrap around
                const Byte low = memory[address];
                const Byte high = memory[address & 0xFF00];
                indirectAddr = (high << 8) | low;
            } else {
                indirectAddr = readWord(cycles, memory, address);
            }
            return indirectAddr;
        }
        default: {
            Emulator::log(totalCycles, Emulator::ERROR, "Illegal instruction: " + static_cast<std::string>(instruction));
            return 0x00;
        }
    }
}

Cpu::Word Cpu::getValueFromAddress(int &cycles, Memory &memory, const instructionModes mode, const std::string &instruction) {

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
            Emulator::log(totalCycles, Emulator::ERROR, "Illegal instruction", instruction );
            return 0x00;
    }
}

void Cpu::ADC(const instructionModes mode, Memory &memory, int &cycles) {
    const Byte value = getValueFromAddress(cycles, memory, mode, "ADC");
    const Word sum = static_cast<uint16_t>(A) + static_cast<uint16_t>(value) + static_cast<uint16_t>(C);
    const Byte result = static_cast<Byte>(sum & 0xFF);

    C = sum > 0xFF;  // Carry
    V = (~(A ^ value) & (A ^ result) & 0x80) != 0;  // Overflow

    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::SBC(const instructionModes mode, Memory &memory, int &cycles) {
    const Byte value = getValueFromAddress(cycles, memory, mode, "SBC");
    const uint16_t result = static_cast<uint16_t>(A) - static_cast<uint16_t>(value) - (1 - C);
    const Byte final = result & 0xFF;

    C = result < 0x100;
    V = ((A ^ value) & (A ^ final) & 0x80) != 0;

    setReg(a, final);
    setZ(final);
    setN(final);
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

void Cpu::DEY(Memory &memory, int &cycles) {
    Y--; cycles--; totalCycles++;
    setN(Y);
    setZ(Y);
}

void Cpu::DEX(Memory &memory, int &cycles) {
    X--; cycles++; totalCycles++;
    setN(X);
    setZ(X);
}

void Cpu::INC(const instructionModes mode, Memory &memory, int &cycles) {
    Word addr = getAddress(cycles, memory, mode, "INC");
    memory[addr]++; cycles--; totalCycles++;
}

void Cpu::DEC(const instructionModes mode, Memory &memory, int &cycles) {
    Word addr = getAddress(cycles, memory, mode, "DEC");
    memory[addr]--; cycles--; totalCycles++;
}

void Cpu::AND(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "AND");
    Byte result = value & A;
    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::EOR(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "EOR");
    Byte result = value ^ A;
    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::ORA(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "ORA");
    Byte result = value | A;
    setReg(a, result);
    setZ(result);
    setN(result);
}

void Cpu::CMP(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "CMP");
    Word sum = static_cast<uint16_t>(A) - static_cast<uint16_t>(value);
    Byte result = static_cast<Byte>(sum & 0xFF);

    C = A >= value;
    Z = A == value;
    setN(result);
}

void Cpu::LDX(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDX");
    setReg(x, value);
    setZ(value);
    setN(value);
}

void Cpu::LDY(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDY");
    setReg(y, value);
    setZ(value);
    setN(value);
}

void Cpu::LDA(const instructionModes mode, Memory &memory, int &cycles) {
    Byte value = getValueFromAddress(cycles, memory, mode, "LDA");
    setReg(a, value);
    setZ(value);
    setN(value);
}

void Cpu::STX(const instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STX");
    memory.writeByte(address, X);
}

void Cpu::STY(const instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STY");
    memory.writeByte(address, Y);
}

void Cpu::STA(const instructionModes mode, Memory &memory, int &cycles) {
    Word address = getAddress(cycles, memory, mode, "STA");
    memory.writeByte(address, A);
}

void Cpu::JMP(const instructionModes mode, Memory &memory, int &cycles) {
    const Word value = getAddress(cycles, memory, mode, "JMP");
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

void Cpu::CLC(Memory &memory, int &cycles) {
    C = 0; cycles--; totalCycles++;
}

void Cpu::CLD(Memory &memory, int &cycles) {
    D = 0; cycles--; totalCycles++;
}

void Cpu::CLI(Memory &memory, int &cycles) {
    I = 0; cycles--; totalCycles++;
}

void Cpu::CLV(Memory &memory, int &cycles) {
    V = 0; cycles--; totalCycles++;
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

void Cpu::BCC(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (C == 0) {
        branch(cycles, offset);
    }
}

void Cpu::BCS(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (C == 1) {
        branch(cycles, offset);
    }
}

void Cpu::BEQ(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (Z == 1) {
        branch(cycles, offset);
    }
}

void Cpu::BMI(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (N == 1) {
        branch(cycles, offset);
    }
}

void Cpu::BNE(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (Z == 0) {
        branch(cycles, offset);
    }
}

void Cpu::BPL(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (N == 0) {
        branch(cycles, offset);
    }
}

void Cpu::BVC(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (V == 0) {
        branch(cycles, offset);
    }
}

void Cpu::BVS(Memory &memory, int &cycles) {
    const Byte offset = fetchByte(cycles, memory);
    if (V == 1) {
        branch(cycles, offset);
    }
}

void Cpu::PHA(Memory &memory, int &cycles) {
    writeToStack(cycles, memory, A);
    totalCycles++; cycles--;
}

void Cpu::PLA(Memory &memory, int &cycles) {
    const Byte value = fetchFromStack(cycles, memory);
    totalCycles++; cycles--;
    setReg(a, value);
    setZ(a);
    setN(a);
}

void Cpu::PHP(Memory &memory, int &cycles) {
    const Byte value = encodeFlags();
    writeToStack(cycles, memory, value);
    totalCycles++; cycles--;
}

void Cpu::PLP(Memory &memory, int &cycles) {
    const Byte value = fetchFromStack(cycles, memory);
    decodeFlags(value);
    totalCycles++; cycles--;
}

void Cpu::TSX(Memory &memory, int &cycles) {
    const Byte value = fetchFromStack(cycles, memory);
    setReg(x, value);
    setZ(x);
    setN(x);
}

void Cpu::TXS(Memory &memory, int &cycles) {
    writeToStack(cycles, memory, X);
}

void Cpu::ROL(const instructionModes mode, Memory &memory, int &cycles) {
    if (mode == ACC) {
        const Byte oldCarry = C;
        const Byte oldValue = A;
        C = (oldValue >> 7) & 1;
        A = (A << 1) | oldCarry;
        setZ(A);
        setN(A);
        cycles--; totalCycles++;
    } else {
        Byte address = getAddress(cycles, memory, mode, "ROL");
        Byte oldValue = memory[address];
        Byte oldCarry = C;
        C = (oldValue >> 7) & 1;
        Byte result = (oldValue << 1) | oldCarry;
        memory[address] = result;
        setZ(result);
        setN(result);
        cycles--; totalCycles++;
    }
}

void Cpu::ROR(const instructionModes mode, Memory &memory, int &cycles) {
    if (mode == ACC) {
        const Byte oldCarry = C;
        const Byte oldValue = A;
        C = oldValue & 1;
        A = (A >> 1) | (oldCarry << 7);
        setZ(A);
        setN(A);
        cycles--; totalCycles++;
    } else {
        Word address = getAddress(cycles, memory, mode, "ROR");
        Byte oldValue = memory[address];
        Byte oldCarry = C;
        C = oldValue & 1;
        Byte result = (oldValue >> 1) | (oldCarry << 7);
        memory[address] = result;
        setZ(result);
        setN(result);
        cycles--; totalCycles++;
    }
}

void Cpu::CPX(const instructionModes mode, Memory &memory, int &cycles) {
    const Byte value = getValueFromAddress(cycles, memory, mode, "CPX");
    const Word sum = static_cast<uint16_t>(X) - static_cast<uint16_t>(value);
    const Byte result = static_cast<Byte>(sum & 0xFF);

    C = X >= value;
    Z = X == value;
    setN(result);
}

void Cpu::CPY(const instructionModes mode, Memory &memory, int &cycles) {
    const Byte value = getValueFromAddress(cycles, memory, mode, "CPY");
    const Word sum = static_cast<uint16_t>(Y) - static_cast<uint16_t>(value);
    const Byte result = static_cast<Byte>(sum & 0xFF);

    C = Y >= value;
    Z = Y == value;
    setN(result);
}

void Cpu::JSR(Memory &memory, int &cycles) {
    Word returnAddress = PC - 1;
    writeWordToStack(cycles, memory, returnAddress);
    PC = fetchWord(cycles, memory);
    totalCycles++; cycles--;
}

void Cpu::RTS(Memory &memory, int &cycles) {
    Word returnAddress = fetchWordFromStack(cycles, memory);
    PC = returnAddress + 1;
    totalCycles++; cycles--;
}

void Cpu::BRK(Memory &memory, int &cycles) {
    writeWordToStack(cycles, memory, PC);
    writeToStack(cycles, memory, encodeFlags());
    B = 1;
    PC = memory[0xFFFE] + (memory[0xFFFF] << 8);
}

void Cpu::RTI(Memory &memory, int &cycles) {
    decodeFlags(fetchFromStack(cycles, memory));
    PC = fetchWordFromStack(cycles, memory);
}

void Cpu::BIT(const instructionModes mode, Memory &memory, int &cycles) {
    const Byte value = getValueFromAddress(cycles, memory, mode, "BIT");
    const Byte result = A & value;

    V = (value >> 6) & 1;
    setZ(result);
    setN(value);
}

void Cpu::LSR(const instructionModes mode, Memory &memory, int &cycles) {
    if (mode == ACC) {
        C = A & 0x01;
        A >>= 1;
        setZ(A);
        setN(A);
        cycles--; totalCycles++;
    } else {
        const Word address = getAddress(cycles, memory, mode, "LSR");
        Byte value = memory[address];
        C = value & 0x01;
        value >>= 1;
        memory[address] = value;
        setZ(value);
        setN(value);
        cycles--; totalCycles++;
    }
}

void Cpu::ASL(const instructionModes mode, Memory &memory, int &cycles) {
    if (mode == ACC) {
        C = (A >> 7) & 1;
        A <<= 1;
        setZ(A);
        setN(A);
        cycles--; totalCycles++;
    } else {
        const Word address = getAddress(cycles, memory, mode, "ASL");
        Byte value = memory[address];
        C = (value >> 7) & 1;
        value <<= 1;
        memory[address] = value;
        setZ(value);
        setN(value);
        cycles--; totalCycles++;
    }
}

Cpu::Cpu(Memory &mem) {
    reset(mem);
}