#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "utils/SFML_CLASSES.h"
#include "utils/scale_manager.h"
#include "game_states/menu.h"
#include "state_stack.h"

int main()
{
    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode::getDesktopMode(),"beatplate", sf::State::Fullscreen);
    window->setMouseCursorVisible(false);

    ScaleManager::UpdateScale(window->getSize().x, window->getSize().y);
    
    StateStack stack;

    stack.pushState(std::make_unique<MainMenu>(stack, *window));

    sf::Clock clock;
    float accumulator = 0.0f;
    const float minDt = 0.001f; // 1 ms em segundos
    bool inFocus = true;

    while (window->isOpen())
    {
        while (auto event = window->pollEvent())
        {

            if (inFocus) 
                stack.handleEvent(*event);

            if (event->is<sf::Event::Closed>())
                window->close();

            if (event->is<sf::Event::FocusGained>()) {
                window->setMouseCursorVisible(false);
                inFocus = true;
            }

            if (event->is<sf::Event::FocusLost>()) {
                window->setMouseCursorVisible(true);
                inFocus = false;
            }

            if (event->is<sf::Event::Resized>()) {
                const auto& resizeEvent = event->getIf<sf::Event::Resized>();
                ScaleManager::UpdateScale(resizeEvent->size.x, resizeEvent->size.y);
            }
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
