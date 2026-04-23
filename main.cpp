#include <functional>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "CPU.h"
#include "Emulator.h"
#include "Gui.h"

namespace {
using Byte = unsigned char;
using Word = unsigned short;
constexpr Word kProgramStart = static_cast<Word>(0x8000);

bool runTestCase(const std::string &name, const std::vector<Byte> &program, int cycles, const std::function<bool(Emulator &)> &check) {
    Emulator emulator;
    emulator.mem.clear();
    for (std::size_t i = 0; i < program.size(); ++i) {
        emulator.mem.writeByte(static_cast<Word>(kProgramStart + i), program[i]);
    }

    emulator.mem.writeByte(0xFFFC, static_cast<Byte>(kProgramStart & 0x00FF));
    emulator.mem.writeByte(0xFFFD, static_cast<Byte>((kProgramStart >> 8) & 0x00FF));
    emulator.cpu.reset(emulator.mem);
    emulator.cpu.execute(cycles, emulator.mem);

    const bool passed = check(emulator);
    std::cout << "\n[TEST] " << name << " -> " << (passed ? "PASS" : "FAIL") << '\n';
    if (!passed) {
        emulator.showRegisters();
        emulator.showFlag(Cpu::z);
        emulator.showFlag(Cpu::c);
        emulator.showFlag(Cpu::n);
        emulator.showMemory(0x0200, 0x0204);
    }
    return passed;
}

bool runSelfTests() {
    int failures = 0;

    failures += !runTestCase(
        "branch + ADC + store",
        {
            0xA9, 0x00,       // LDA #$00
            0xF0, 0x02,       // BEQ +2
            0xA9, 0xFF,       // skipped
            0xA9, 0x42,       // LDA #$42
            0x18,             // CLC
            0x69, 0x20,       // ADC #$20
            0x8D, 0x00, 0x02, // STA $0200
            0xFF              // HALT
        },
        40,
        [](Emulator &emulator) {
            return emulator.cpu.returnReg(Cpu::a) == 0x62 &&
                   emulator.mem[0x0200] == 0x62 &&
                   emulator.cpu.returnFlag(Cpu::z) == 0 &&
                   emulator.cpu.returnFlag(Cpu::c) == 0;
        }
    );

    failures += !runTestCase(
        "TSX / TXS transfer",
        {
            0xBA,
            0x8E, 0x01, 0x02,
            0xA2, 0x20,
            0x9A,
            0xBA,
            0x8E, 0x02, 0x02,
            0xFF
        },
        30,
        [](Emulator &emulator) {
            return emulator.mem[0x0201] == 0xFF &&
                   emulator.mem[0x0202] == 0x20 &&
                   emulator.cpu.returnReg(Cpu::x) == 0x20;
        }
    );

    failures += !runTestCase(
        "INC / DEC flags",
        {
            0xA9, 0x00,
            0x8D, 0x03, 0x02,
            0xEE, 0x03, 0x02,
            0xCE, 0x03, 0x02,
            0xFF
        },
        30,
        [](Emulator &emulator) {
            return emulator.mem[0x0203] == 0x00 &&
                   emulator.cpu.returnFlag(Cpu::z) == 1 &&
                   emulator.cpu.returnFlag(Cpu::n) == 0;
        }
    );

    std::cout << "\nSelf-test summary: " << (failures == 0 ? "ALL PASS" : std::to_string(failures) + " FAIL") << '\n';
    return failures == 0;
}
} // namespace

int main(int argc, char *argv[]) {

    const std::string romPath = argv[1];
    Word loadAddress = 0x8000;
    if (argc >= 3) {
        loadAddress = static_cast<Word>(std::stoul(argv[2], nullptr, 0));
    }

    Emulator emulator;

    if (argc >= 2) {
        const std::string firstArg = argv[1];
        if (firstArg == "--gui" || firstArg == "--window") {
            return runSfmlWindowDemo(emulator);
        }
    }

    if (argc <= 1 || std::string(argv[1]) == "--self-test") {
        return runSelfTests() ? 0 : 1;
    }

    if (!emulator.readROM(romPath)) {
        return 1;
    }

    if (!emulator.loadROMIntoMem(loadAddress)) {
        return 1;
    }

    emulator.cpu.reset(emulator.mem);

    std::cout << "Reset vector PC: 0x" << std::hex << emulator.cpu.PC << std::endl;
    std::cout << "First opcode: 0x" << std::hex << static_cast<int>(emulator.mem[emulator.cpu.PC]) << std::endl;

    emulator.cpu.execute(100000, emulator.mem);

    emulator.showRegisters();
    emulator.showFlag(Cpu::z);
    emulator.showFlag(Cpu::c);
    emulator.showFlag(Cpu::n);
    emulator.showMemory(0x0200, 0x0204);

    return 0;
}
