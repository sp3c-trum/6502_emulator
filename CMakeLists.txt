cmake_minimum_required(VERSION 3.31)
project(6502_emulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_SFML_WINDOW "Enable basic SFML 3 window integration" ON)

add_executable(6502_emulator main.cpp
        CPU.h
        Memory.h
        Memory.cpp
        CPU.cpp
        Emulator.cpp
        Emulator.h
        Gui.h
        Gui.cpp
        PPU.cpp
        PPU.h
)

if (ENABLE_SFML_WINDOW)
    # Explicitly set SFML search path to vcpkg to avoid MinGW conflicts
    if (DEFINED CMAKE_TOOLCHAIN_FILE)
        # When using vcpkg toolchain, prioritize vcpkg's SFML
        set(SFML_DIR "C:/vcpkg/installed/x64-windows/share/sfml" CACHE PATH "SFML config path")
    endif ()

    # SFML 3 only.
    find_package(SFML 3 QUIET COMPONENTS Graphics Window System)
    if (SFML_FOUND)
        if (TARGET SFML::Graphics)
            set(_sfml_graphics_binary "")
            foreach(_prop IMPORTED_IMPLIB_DEBUG IMPORTED_IMPLIB_RELEASE IMPORTED_IMPLIB
                          IMPORTED_LOCATION_DEBUG IMPORTED_LOCATION_RELEASE IMPORTED_LOCATION)
                get_target_property(_candidate SFML::Graphics ${_prop})
                if (_candidate AND NOT _candidate STREQUAL "_candidate-NOTFOUND")
                    set(_sfml_graphics_binary "${_candidate}")
                    break()
                endif ()
            endforeach ()

            # Reject MinGW .a binaries with MSVC compiler
            if (_sfml_graphics_binary MATCHES [[\.a$]])
                message(FATAL_ERROR "MinGW SFML binaries (.a) detected (${_sfml_graphics_binary}). "
                    "CLion/MSVC cannot link these. "
                    "Use vcpkg SFML instead. "
                    "Ensure CMake options include: -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows")
            else ()
                target_compile_definitions(6502_emulator PRIVATE HAS_SFML)
                target_link_libraries(6502_emulator PRIVATE SFML::Graphics SFML::Window SFML::System)
                message(STATUS "SFML 3 found (MSVC-compatible): enabling --gui window mode")
            endif ()
        else ()
            message(WARNING "SFML 3 was found, but imported targets (SFML::Graphics/SFML::Window/SFML::System) are missing. Building without --gui support.")
        endif ()
    else ()
        message(WARNING "SFML 3 not found: building without --gui support")
    endif ()
endif ()

