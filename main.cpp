#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    emulator.readROM("test.bin");

    // Załaduj ROM do 0x0400 (to typowa lokalizacja testu Klausa)
    emulator.loadROMIntoMem(emulator.ROM, 0x0400);

    // Reset ustawi PC na mem[0xFFFC/FFFD], czyli 0x0400
    emulator.cpu.reset(emulator.mem);

    std::cout << "Reset vector PC: 0x" << std::hex << emulator.cpu.PC << std::endl;
    std::cout << "First opcode: 0x" << std::hex << static_cast<int>(emulator.mem[emulator.cpu.PC]) << std::endl;
    std::cout << "ROM size: " << emulator.ROM.size() << std::endl;
    std::cout << "ROM[0]: " << std::hex << static_cast<int>(emulator.ROM[0]) << std::endl;

    emulator.cpu.execute(100, emulator.mem);
    emulator.showRegisters();

    return 0;
}