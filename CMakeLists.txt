cmake_minimum_required(VERSION 3.31)
project(6502_emulator)

set(CMAKE_CXX_STANDARD 20)

# Ścieżka do SDL2
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/external/SDL2)

# Dołączanie headerów SDL2
include_directories(${SDL2_DIR}/include)

# Ścieżka do bibliotek .lib/.a
link_directories(${SDL2_DIR}/lib)

add_executable(6502_emulator main.cpp
        CPU.h
        Memory.h
        Memory.cpp
        CPU.cpp
        Emulator.cpp
        Emulator.h
)

# Linkowanie SDL2
target_link_libraries(6502_emulator SDL2)

# Skopiowanie SDL2.dll po buildzie
add_custom_command(TARGET 6502_emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DIR}/bin/SDL2.dll
        $<TARGET_FILE_DIR:6502_emulator>)