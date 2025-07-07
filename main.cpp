#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;

    emulator.readROM("test.bin");
    emulator.loadROMIntoMem(emulator.ROM, 0x0000);
    emulator.cpu.reset(emulator.mem);
    std::cout << (int) emulator.cpu.PC << std::endl;
    emulator.cpu.execute(65335, emulator.mem);
    emulator.showRegisters();
    std::cout << (int) emulator.cpu.PC << std::endl;

}
