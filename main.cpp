#include <iostream>
#include "CPU.h"
#include "Emulator.h"
#include "Memory.h"

int main() {
    Emulator emulator;

    std::vector<uint8_t> testProgram = {
        0xA9, 0x10,       // LDA #$10
        0xAA,             // TAX
        0xE8,             // INX
        0xCA,             // DEX

        0xA0, 0x05,       // LDY #$05
        0xC8,             // INY
        0x88,             // DEY

        0x8D, 0x00, 0x02, // STA $0200

        0xA9, 0x22,       // LDA #$22
        0x69, 0x20,       // ADC #$20

        0x29, 0xF0,       // AND #$F0
        0x09, 0x02,       // ORA #$02
        0x49, 0x01,       // EOR #$01

        0x4A,             // LSR
        0x0A,             // ASL
        0x2A,             // ROL
        0x6A,             // ROR

        0xC9, 0x42,       // CMP #$42
        0xE0, 0x10,       // CPX #$10
        0xC0, 0x05,       // CPY #$05

        0xF0, 0x03,       // BEQ +3 (skocz jeśli Z=1)
        0xA9, 0x00,       // LDA #$00 (pomijane)
        0x4C, 0x22, 0x00, // JMP $0022 (jump to halt)

        // label_eq:
        0xA9, 0x42,       // LDA #$42

        // halt:
        0xFF              // HALT opcode
    };
    for (size_t i = 0; i < testProgram.size(); ++i)
        emulator.mem[0x0000 + i] = testProgram[i];
    // Reset vector (adresy 0xFFFC-0xFFFD = 0x0000)
    emulator.loadByteIntoMem(0x00, 0xFFFC);
    emulator.loadByteIntoMem(0x00, 0xFFFD);
    emulator.cpu.reset(emulator.mem);

    // Informacje początkowe
    std::cout << "Reset vector PC: 0x" << std::hex << emulator.cpu.PC << std::endl;
    std::cout << "First opcode: 0x" << std::hex << static_cast<int>(emulator.mem[emulator.cpu.PC]) << std::endl;

    // Uruchom program
    emulator.cpu.execute(100, emulator.mem);

    // Pokaż rejestry i flagi
    emulator.showRegisters();
    std::cout << "\nZawartosc $0200: 0x" << std::hex << static_cast<int>(emulator.mem[0x0200]) << std::endl;

    emulator.showFlag(Cpu::c);  // Carry
    emulator.showFlag(Cpu::z);  // Zero
    emulator.showFlag(Cpu::n);  // Negative
    emulator.showFlag(Cpu::v);  // Overflow (sprawdzany przez BIT)

    emulator.showMemory(0x0000, 0x000D);

    return 0;
}
