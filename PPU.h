//
// Created by Mikołaj on 2026-04-23.
//

#ifndef INC_6502_EMULATOR_PPU_H
#define INC_6502_EMULATOR_PPU_H
#include <vector>


class PPU {
public:
    std::vector<uint8_t> CHR;
    std::vector<uint8_t> RAM;
    uint16_t address;
    uint8_t data;
    uint8_t OAMAddress;
    uint8_t OAMData;
    uint8_t DMA;
    uint8_t control;
    uint8_t mask;
    uint16_t scroll;
    uint8_t status;
};


#endif //INC_6502_EMULATOR_PPU_H