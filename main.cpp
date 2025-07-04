#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;

    emulator.loadByteIntoMem(0x00, 0xFFFC);
    emulator.loadByteIntoMem(0x80, 0xFFFD);
    emulator.cpu.reset(emulator.mem);
    std::cout << (int) emulator.cpu.PC << std::endl;

    emulator.readROM("test.bin");
    emulator.loadROMIntoMem(emulator.ROM, 0x0000);
    std::cout << (int) emulator.cpu.PC << std::endl;
    emulator.cpu.execute(100000, emulator.mem);
    std::cout << (int) emulator.cpu.PC << std::endl;
    emulator.showRegisters();

}
