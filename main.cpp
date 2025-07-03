#include <iostream>

#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;
    Memory memory;

    // ADC Immediate (2 cykle)
    emulator.log(Emulator::DEBUG, "ADC Immediate:\n");
    emulator.loadByteIntoMem(0x00, 0xFFFC);
    emulator.loadByteIntoMem(0x80, 0xFFFD);
    emulator.cpu.reset(emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.loadByteIntoMem(0x69, 0x8000); // ADC #$05
    emulator.loadByteIntoMem(0x05, 0x8001);
    emulator.cpu.execute(2, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);

    // ADC Zero Page (3 cykle)
    emulator.log(Emulator::DEBUG, "ADC Zero Page:\n");
    emulator.loadByteIntoMem(0x65, 0x8002); // ADC $10
    emulator.loadByteIntoMem(0x10, 0x8003);
    emulator.loadByteIntoMem(0x05, 0x0010); // Wartość w pamięci $10
    emulator.cpu.execute(3, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);

    // ADC Zero Page,X (4 cykle)
    emulator.log(Emulator::DEBUG, "ADC Zero Page,X:\n");
    emulator.cpu.setReg(Cpu::x, 0x01);
    emulator.loadByteIntoMem(0x75, 0x8004); // ADC $10,X
    emulator.loadByteIntoMem(0x10, 0x8005);
    emulator.loadByteIntoMem(0x05, 0x0011); // $10 + X = $11
    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);

    // ADC Absolute (4 cykle)
    emulator.log(Emulator::DEBUG, "ADC Absolute:\n");
    emulator.loadByteIntoMem(0x6D, 0x8006); // ADC $1234
    emulator.loadByteIntoMem(0x34, 0x8007);
    emulator.loadByteIntoMem(0x12, 0x8008);
    emulator.loadByteIntoMem(0x05, 0x1234);
    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);

    // ADC Absolute,X (4/5 cykli — tu bez page crossing)
    emulator.log(Emulator::DEBUG, "ADC Absolute,X:\n");
    emulator.cpu.setReg(Cpu::x, 0x01);
    emulator.loadByteIntoMem(0x7D, 0x8009); // ADC $1234,X
    emulator.loadByteIntoMem(0x34, 0x800A);
    emulator.loadByteIntoMem(0x12, 0x800B);
    emulator.loadByteIntoMem(0x05, 0x1235); // $1234 + X
    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);

    // ADC Absolute,Y (4/5 cykli — tu bez page crossing)
    emulator.log(Emulator::DEBUG, "ADC Absolute,Y:\n");
    emulator.cpu.setReg(Cpu::y, 0x01);
    emulator.loadByteIntoMem(0x79, 0x800C); // ADC $1234,Y
    emulator.loadByteIntoMem(0x34, 0x800D);
    emulator.loadByteIntoMem(0x12, 0x800E);
    emulator.loadByteIntoMem(0x05, 0x1235); // $1234 + Y
    emulator.cpu.execute(4, emulator.mem);
    emulator.showRegisters();
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::v);


    // emulator.readROM("test.bin");
    // emulator.loadROMIntoMem(emulator.ROM, 0x00);
    // emulator.cpu.execute(100, emulator.mem);

    emulator.cpu.reset(emulator.mem);

    //emulator.showMemory(0x0000, 0x00FF);
    //emulator.showRegisters();
    //emulator.showFlag(Cpu::c);
    //emulator.showFlag(Cpu::v);

    unsigned char byte1 = 0x02;
    unsigned char byte2 = byte1 << 1;
    bool what = (bool) (byte1 >= 128);
    std::cout << (int) byte2 << " " << what << std::endl;

}
