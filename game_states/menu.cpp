#include "menu.h"
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"

std::vector<std::function<void(float)>> tweens;
float elapsed = 0.0f;

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    Title("assets/sprites/main_menu/title.png",600,360,225,104),
    background("assets/sprites/main_menu/background.png",100,100,364,205),

    TitleTween(*Title.sprite,0.5f,Tween::linear),
    TitleTransparencyTween(*Title.sprite,3.0f,Tween::linear)
{
    TitleTween.initScale(1.0f,1.1f);
    TitleTween.play();
    TitleTransparencyTween.initTransparency(0.0f,1.0f);
    TitleTransparencyTween.play();
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
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});

    TitleTween.update(dt.asSeconds());
    TitleTransparencyTween.update(dt.asSeconds());

    if (!TitleTween.isActive()) {
        TitleTween.reset();
        TitleTween.play();
    }
}

void MainMenu::render() {
    mWindow.clear(sf::Color::White);
    mWindow.draw(*background.sprite);
    mWindow.draw(*Title.sprite);
    mWindow.draw(*Cursor.sprite);
}
