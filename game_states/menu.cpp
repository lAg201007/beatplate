#include "menu.h"
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"

std::vector<std::function<void(float)>> tweens;
float elapsed = 0.0f;

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window), Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    Title("assets/sprites/main_menu/title.png",600,360,225,104),
    TitleTween(*Title.sprite,5.0f,Tween::linear)
{
    TitleTween.initScale(1.0f,1.1f);
    TitleTween.play();
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

    TitleTween.update(dt.asSeconds());

    if (!TitleTween.isActive()) {
        TitleTween.reset();
        TitleTween.play();
    }
}

void MainMenu::render() {
    mWindow.clear(sf::Color::White);
    mWindow.draw(*Cursor.sprite);
    mWindow.draw(*Title.sprite);
}
