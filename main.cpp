#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;

    emulator.loadByteIntoMem(0x00, 0xFFFC);
    emulator.loadByteIntoMem(0x00, 0xFFFD);
    emulator.loadByteIntoMem(0x69, 0x0000);
    emulator.loadByteIntoMem(0x00, 0x0001);

    emulator.readROM("test.bin");
    emulator.loadROMIntoMem(emulator.ROM, 0x00);
    emulator.cpu.execute(100, emulator.mem);

    // emulator.showMemory(0x0000, 0x00FF);
    // emulator.cpu.execute(2, emulator.mem);
    // emulator.showRegisters();
    // emulator.showFlag(Cpu::c);
    // emulator.showFlag(Cpu::v);

    // unsigned char byte1 = 0x02;
    // unsigned char byte2 = byte1 << 1;
    // bool what = (bool) (byte1 >= 128);
    // std::cout << (int) byte2 << " " << what << std::endl;

}
