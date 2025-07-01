//
// Created by P!nk on 30.06.2025.
//

#include "Memory.h"
using Byte = unsigned char;
using Word = unsigned short;

void Memory::clear() {
    for (int i = 0; i < MAXMEM; i++) {
        Data[i] = 0x69;
    }
}


Byte Memory::operator[](Word byte) const {
    return Data[byte];
}

Byte &Memory::operator[](Word byte) {
    return Data[byte];
}

Byte Memory::readByte(Word &addr, int &cycles) {
    cycles--;
    return Data[addr];
}

Byte Memory::writeByte(Word &addr, int &cycles, Byte value) {
    cycles--;
    Data[addr] = value;
    return Data[addr];
};

void Memory::writeByte(Word &addr, Byte value) {
    Data[addr] = value;
};

Memory::Memory() {
    clear();
}
