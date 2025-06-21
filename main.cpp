#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "utils/SFML_CLASSES.h"

#include "game_states/menu.h"
#include "state_stack.h"

int main()
{
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(),"beatplate", sf::State::Fullscreen);
    window->setMouseCursorVisible(false);
    
    StateStack stack;

    stack.pushState(std::make_unique<MainMenu>(stack, *window));

    sf::Clock clock;
    float accumulator = 0.0f;
    const float minDt = 0.001f; // 1 ms em segundos

    while (window->isOpen())
    {
        while (auto event = window->pollEvent())
        {

            stack.handleEvent(*event);

            if (event->is<sf::Event::Closed>())
                window->close();

            if (event->is<sf::Event::FocusGained>())
                window->setMouseCursorVisible(false);

            if (event->is<sf::Event::FocusLost>())
                window->setMouseCursorVisible(true);
        }

        sf::Time dt = clock.restart();
        accumulator += dt.asSeconds();

        while (accumulator >= minDt) {
            stack.update(sf::seconds(minDt));
            accumulator -= minDt;
        }

        window->clear(sf::Color::White);
        stack.render();
        window->display();
    }

    return 0;
}
