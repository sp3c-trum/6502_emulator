#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;
    emulator.loadIntoMem(0x80, 0x1221);
    emulator.loadIntoMem(0xAE, 0x0000); //LDX Absolute
    emulator.loadIntoMem(0x12, 0x0001); //  Address byte 1
    emulator.loadIntoMem(0x21, 0x0002); //  Address byte 2
    emulator.showMemory(0x0000);

    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();
}
