#include "menu.h"
#include <SFML/Window/Mouse.hpp>

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window), Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f)
{
}

void MainMenu::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                mWindow.close();
            }
        }

    }
    else if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
}

void MainMenu::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({mouse_pos.x,mouse_pos.y});
}

void MainMenu::render() {
    mWindow.clear(sf::Color::White);
    mWindow.draw(*Cursor.sprite);
}
