cmake_minimum_required(VERSION 3.31)
project(6502_emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(6502_emulator main.cpp
        CPU.h
        Memory.h
        Memory.cpp
        CPU.cpp
        Emulator.cpp
        Emulator.h
)
