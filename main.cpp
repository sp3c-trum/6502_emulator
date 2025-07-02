#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;
    emulator.loadIntoMem(0x80, 0x1221);
    emulator.loadIntoMem(0x25, 0x1001);
    emulator.loadIntoMem(0x01, 0x1005);
    emulator.loadIntoMem(0xAE, 0x0000); //LDX Absolute
    emulator.loadIntoMem(0x10, 0x0001); //  Address byte 1
    emulator.loadIntoMem(0x05, 0x0002); //  Address byte 2
    emulator.loadIntoMem(0xBC, 0x0003); //LDY Absolute
    emulator.loadIntoMem(0x10, 0x0004);
    emulator.loadIntoMem(0x00, 0x0005);
    emulator.showMemory(0x0000);

    emulator.cpu.execute(4, emulator.mem);
    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();


    unsigned char byte1 = 0x02;
    unsigned char byte2 = byte1 << 1;
    bool what = (bool) (byte1 >= 128);
    std::cout << (int) byte2 << " " << what << std::endl;

}
