#include <iostream>
#include "Emulator.h"
#include "CPU.h"
#include "Memory.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>

int main() {
    Emulator emulator;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Tworzenie okna
    SDL_Window* window = SDL_CreateWindow(
        "6502 Debugger",         // tytuł okna
        SDL_WINDOWPOS_CENTERED,  // pozycja X
        SDL_WINDOWPOS_CENTERED,  // pozycja Y
        800,                     // szerokość
        600,                     // wysokość
        SDL_WINDOW_SHOWN         // flaga (pokaż okno)
    );

    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Główna pętla zdarzeń
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Można tu dodać renderowanie
        SDL_Delay(16); // ok. 60 FPS
    }

    // Sprzątanie
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
