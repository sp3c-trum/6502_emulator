//
// Created by P!nk on 01.07.2025.
//

#include "Emulator.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <bits/ostream.tcc>
#include <sstream>

Emulator::Emulator()
    : cpu(mem) {
    cpu.reset(mem);
}

#include <ctime>
#include <string>

void Emulator::log(int totalCycles, logMode mode, const std::string &message, bool withValue, const std::string &value) {
    time_t timestamp;
    time(&timestamp);
    std::string timeStr = std::strtok(ctime(&timestamp), "\n");
    std::cout << "\n[" << totalCycles << "]";

    switch (mode) {
        case INFO:
            std::cout << "[" << timeStr << "][INFO]    ";
            break;
        case ERROR:
            std::cout << "[" << timeStr << "][ERROR]   ";
            break;
        case SUCCESS:
            std::cout << "[" << timeStr << "][SUCCESS] ";
            break;
        case WARNING:
            std::cout << "[" << timeStr << "][WARNING] ";
            break;
        case DEBUG:
            std::cout << "[" << timeStr << "][DEBUG]   ";
            break;
        default:
            std::cout << "[" << timeStr << "][MESSAGE] ";
    }

    std::cout << message;
    if (withValue) {
        std::cout << " " << value;
    }
}

void Emulator::log(int totalCycles, logMode mode, const std::string &message) {
    log(totalCycles, mode, message, false, "");
}

void Emulator::log(int totalCycles, logMode mode, const std::string &message, Byte value) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << static_cast<int>(value);
    log(totalCycles, mode, message, true, ss.str());
}

void Emulator::log(int totalCycles, logMode mode, const std::string &message, Word value) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << value;
    log(totalCycles, mode, message, true, ss.str());
}

void Emulator::log(int totalCycles, logMode mode, const std::string &message, const std::string &value) {
    log(totalCycles, mode, message, true, value);
}

void Emulator::readROM(const std::string &name) {
    char byte;
    std::ifstream file (name, std::ios::binary);

    log(0, INFO, "Reading ROM.");

    if (!file.is_open())
    {
        log(0, ERROR, "Failed to open file");
        return;
    }

    while (file.read(&byte, 1)) {
        ROM.push_back(static_cast<unsigned char>(byte));
    }

    log(0, SUCCESS, "Successfully read ROM");
}


void Emulator::loadROMIntoMem(const Word addr) {
    log(0, INFO, "Loading ROM into memory at address: ", addr);

    for (size_t i = 0; i < ROM.size(); ++i)
        mem[addr + i] = ROM[i];

    // Ustaw reset vector na adres startowy ROM
    mem[0xFFFC] = static_cast<Byte>(addr & 0x00FF);         // Low byte
    mem[0xFFFD] = static_cast<Byte>((addr >> 8) & 0x00FF);  // High byte

    log(0, SUCCESS, "Successfully loaded ROM into memory");
}



void Emulator::loadByteIntoMem(Byte instruction, Word addr) {
    mem.Data[addr] = instruction;
}

void Emulator::showMemory(const Word startingAddress, const Word endingAddress) const {
    log(0, INFO, "Showing memory:\n");
    for (Word i = startingAddress; i < endingAddress; i++) {
        std::cout << std::hex << static_cast<int>(mem.Data[i]) << std::hex << " ";
    }
    std::cout << "\n";
}

void Emulator::showRegisters() const {
    std::cout << "\nRegister A = " << static_cast<int>(cpu.returnReg(Cpu::a))
    << "\nRegister X = " <<  static_cast<int>(cpu.returnReg(Cpu::x))
    << "\nRegister Y = " << static_cast<int>(cpu.returnReg(Cpu::y)) << "\n";
}

void Emulator::showFlag(Cpu::flags flag) const {
    std::cout << "Flag " << flag << ": " << static_cast<int>(cpu.returnFlag(flag)) << "\n";
}

