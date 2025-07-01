//
// Created by P!nk on 01.07.2025.
//

#include "Emulator.h"

#include <iostream>
#include <bits/ostream.tcc>

Emulator::Emulator() {

}

void Emulator::loadIntoMem(Byte instruction, Word addr) {
    mem.Data[addr] = instruction;
}

void Emulator::showMemory(Word startingAddress) {
    for (Byte i = 0x00; i < 0xFF; i++) {
        std::cout << std::hex << (int) mem.Data[i] << std::hex << " ";
    }
    std::cout << std::endl;
}

void Emulator::showRegisters() {
    std::cout << "Register A = " << (int) cpu.returnReg(Cpu::a) << std::endl
    << "Register X = " <<  (int) cpu.returnReg(Cpu::x) << std::endl
    << "Register Y = " << (int) cpu.returnReg(Cpu::y) << std::endl;
}

void Emulator::showFlag(Cpu::flags flag) {
    std::cout << "Flaga " << flag << ": " << (int) cpu.returnFlag(flag) << std::endl;
}

