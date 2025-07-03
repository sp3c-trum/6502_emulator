//
// Created by P!nk on 01.07.2025.
//

#include "Emulator.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <bits/ostream.tcc>

Emulator::Emulator() {
    cpu.reset(mem);
}

void Emulator::log(logMode mode, std::string message, Word value) {

    time_t timestamp;
    time(&timestamp);
    std::string time = std::strtok(ctime(&timestamp), "\n");

    switch (mode) {
        case INFO:
            std::cout << "\n[" << time << "][INFO]    ";
            break;
        case ERROR:
            std::cout << "\n[" << time << "][ERROR]   ";
            break;
        case SUCCESS:
            std::cout << "\n[" << time << "][SUCCESS] ";
            break;
        case WARNING:
            std::cout << "\n[" << time << "][WARNING] ";
            break;
        case DEBUG:
            std::cout << "\n[" << time << "][DEBUG]   ";
            break;
        default:
            std::cout << "\n[" << time << "][MESSAGE] ";
    }
    std::cout << message << value;
}


void Emulator::readROM(std::string name) {

    log(INFO, "Reading ROM.");

    std::ifstream file (name, std::ios::binary);
    if (!file.is_open())
    {
        log(ERROR, "Failed to open file");
        return;
    }

    char byte;
    while (file.read(&byte, 1)) {
        //std::cout << std::hex << (unsigned int)(unsigned char)byte << ' ';
        ROM.push_back(static_cast<unsigned char>(byte));
    }

    log(SUCCESS, "Successfully read ROM");
}


void Emulator::loadROMIntoMem(std::vector<Byte>, Word addr) { //TODO Dodać ładowanie romu do konkretnego adresu w pamięci zgodnie z nagłówkiem
    log(INFO, "Loading ROM into memory.");

    for (Word i = addr; i < 0xFFFF; i++) {
        mem.Data[i] = ROM[i];
    }

    log(SUCCESS, "Successfully loaded ROM into memory");
    cpu.reset(mem);
}

void Emulator::loadByteIntoMem(Byte instruction, Word addr) {
    mem.Data[addr] = instruction;
}

void Emulator::showMemory(Word startingAddress, Word endingAddress) {
    log(INFO, "Showing memory:\n");
    for (Word i = startingAddress; i < endingAddress; i++) {
        std::cout << std::hex << (int) mem.Data[i] << std::hex << " ";
    }
    std::cout << "\n";
}

void Emulator::showRegisters() {
    std::cout << "Register A = " << (int) cpu.returnReg(Cpu::a) << std::endl
    << "Register X = " <<  (int) cpu.returnReg(Cpu::x) << std::endl
    << "Register Y = " << (int) cpu.returnReg(Cpu::y) << std::endl;
}

void Emulator::showFlag(Cpu::flags flag) {
    std::cout << "Flag " << flag << ": " << (int) cpu.returnFlag(flag) << std::endl;
}

