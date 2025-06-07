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
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), "game",sf::Style::Default,sf::State::Fullscreen);
    window->setMouseCursorVisible(false);
    
    StateStack stack;

    stack.pushState(std::make_unique<MainMenu>(stack, *window));

    sf::Clock clock;

    while (window->isOpen())
    {
        while (auto event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window->close();

            if (event->is<sf::Event::FocusGained>())
                window->setMouseCursorVisible(false);

            if (event->is<sf::Event::FocusLost>())
                window->setMouseCursorVisible(true);
        }

        sf::Time dt = clock.restart();
        stack.update(dt);
        
        window->clear(sf::Color::White);
        stack.render();
        window->display();
    }

    return 0;
}
