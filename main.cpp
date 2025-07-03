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


    unsigned char byte1 = 0x02;
    unsigned char byte2 = byte1 << 1;
    bool what = (bool) (byte1 >= 128);
    std::cout << (int) byte2 << " " << what << std::endl;

}
