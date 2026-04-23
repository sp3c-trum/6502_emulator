//
// Created by P!nk on 01.07.2025.
//

#include "Emulator.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

Emulator::Emulator()
    : cpu(mem) {
    cpu.reset(mem);
}

#include <ctime>
#include <string>

void Emulator::log(int totalCycles, logMode mode, const std::string &message, bool withValue, const std::string &value) {
    time_t timestamp;
    time(&timestamp);
    std::string timeStr = std::ctime(&timestamp);
    if (!timeStr.empty() && timeStr.back() == '\n') {
        timeStr.pop_back();
    }
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

bool Emulator::readROM(const std::string &name) {
    ROM.clear();
    char byte;
    std::ifstream file (name, std::ios::binary);

    log(0, INFO, "Reading ROM.");

    if (!file.is_open())
    {
        log(0, ERROR, "Failed to open file");
        return false;
    }

    std::vector<Byte> raw;
    while (file.read(&byte, 1)) {
        raw.push_back(static_cast<Byte>(byte));
    }

    if (raw.size() >= 16 && raw[0] == 'N' && raw[1] == 'E' && raw[2] == 'S' && raw[3] == 0x1A) {
        size_t offset = 16;
        if ((raw[6] & 0x04) != 0 && raw.size() >= offset + 512) {
            offset += 512;
        }

        const size_t available = raw.size() - offset;
        const size_t prgBanks = raw[4] == 0 ? 1 : raw[4];
        const size_t prgSize = prgBanks * 16384;
        const size_t copySize = std::min(prgSize, available);
        ROM.assign(raw.begin() + static_cast<std::vector<Byte>::difference_type>(offset), raw.begin() + static_cast<std::vector<Byte>::difference_type>(offset + copySize));
        log(0, INFO, "Detected iNES header; loaded PRG data bytes: ", static_cast<Word>(ROM.size()));
    } else {
        ROM = std::move(raw);
    }

    if (ROM.empty()) {
        log(0, ERROR, "ROM is empty after loading.");
        return false;
    }

    log(0, SUCCESS, "Successfully read ROM");
    return true;
}


bool Emulator::loadROMIntoMem(const Word addr) {
    log(0, INFO, "Loading ROM into memory at address: ", addr);

    if (ROM.empty()) {
        log(0, ERROR, "No ROM data loaded.");
        return false;
    }

    if (static_cast<size_t>(addr) + ROM.size() > 0x10000) {
        log(0, ERROR, "ROM does not fit into memory at the requested address.");
        return false;
    }

    for (size_t i = 0; i < ROM.size(); ++i)
        mem.writeByte(static_cast<Word>(addr + i), ROM[i]);

    // Set the reset vector to the loaded code start.
    mem[0xFFFC] = static_cast<Byte>(addr & 0x00FF);         // Low byte
    mem[0xFFFD] = static_cast<Byte>((addr >> 8) & 0x00FF);  // High byte

    log(0, SUCCESS, "Successfully loaded ROM into memory");
    return true;
}



void Emulator::loadByteIntoMem(Byte instruction, Word addr) {
    mem.writeByte(addr, instruction);
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

