//
// Created by P!nk on 30.06.2025.
//

#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>

class Memory {
private:
    using Byte = unsigned char;
    using Word = unsigned short;

    static constexpr uint32_t MAXMEM = 65536;

public:
    Byte Data[MAXMEM]{};
    void clear();
    Byte operator[](Word byte) const;
    Byte &operator[](Word byte);
    Byte readByte(const Word &addr, int &cycles) const;
    void writeByte(const Word &addr, Byte value);
    Memory();
};

#endif //MEMORY_H
