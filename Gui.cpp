#include "Gui.h"

#ifdef HAS_SFML
#include <iostream>
#include <SFML/Graphics.hpp>
#include <optional>

#include "Emulator.h"

int runSfmlWindowDemo(const Emulator& emulator) {
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "NES Emulator", sf::State::Windowed);
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.openFromFile("vcr.ttf"))
    {
        std::cerr << "Could not load font.\n";
    }
    sf::Text text(font);
    text.setString("test");
    text.setFillColor(sf::Color::White);


    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>() ||
                (event->is<sf::Event::KeyPressed>() &&
                 event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }

        window.clear(sf::Color(28, 34, 48));
        window.draw(text);
        window.display();
    }

    return 0;
}

#else

#include <iostream>

int runSfmlWindowDemo() {
    std::cerr << "SFML 3 window mode requested, but SFML 3 was not found at build time.\n";
    std::cerr << "Install SFML 3 and reconfigure CMake, then run with --gui.\n";
    return 1;
}

#endif

