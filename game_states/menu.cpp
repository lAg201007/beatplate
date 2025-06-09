
#include "menu.h"
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"

std::vector<std::function<void(float)>> tweens;
float elapsed = 0.0f;

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    Title("assets/sprites/main_menu/title.png",600,360,225,104),
    background("assets/sprites/main_menu/background.png",0,0),

    TitleTween(*Title.sprite,0.5f,Tween::linear),
    TitleTransparencyTween(*Title.sprite,3.0f,Tween::linear),
    TitleMenuClickPositionTween(*Title.sprite,0.5f,Tween::easeOutQuad),
    TitleBackClickPositionTween(*Title.sprite,1.0f,Tween::easeOutQuad)
{
    TitlePosition = Title.sprite->getPosition();

    TitleTween.initScale(1.0f,1.1f);
    TitleTransparencyTween.initTransparency(0.0f,1.0f);

    TitleMenuClickPositionTween.initPosition(Title.sprite->getPosition(),TitlePosition + sf::Vector2f({0,-150}));
    TitleBackClickPositionTween.initPosition(Title.sprite->getPosition(),TitlePosition);

    TitleTransparencyTween.play();

    sf::Vector2u windowSize = window.getSize();                
    sf::Vector2u textureSize = background.sprite->getTexture().getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.sprite->setScale({scaleX, scaleY});

    menu_step = 1;
}

void MainMenu::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                mWindow.close();
            }
        }
    }

    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }

}

void MainMenu::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);

    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});

    TitleTween.update(dt.asSeconds());
    TitleTransparencyTween.update(dt.asSeconds());
    TitleMenuClickPositionTween.update(dt.asSeconds());
    TitleBackClickPositionTween.update(dt.asSeconds());

    if (!TitleTween.isActive()) {
        TitleTween.reset();
        TitleTween.play();
    }

    if (Title.DetectButtonClick(mWindow)) {
        if (!TitleMenuClickPositionTween.isActive() && !TitleBackClickPositionTween.isActive()) {
            if (menu_step == 1) {
                TitleMenuClickPositionTween.initPosition(Title.sprite->getPosition(), TitlePosition + sf::Vector2f(0, -150));
                TitleMenuClickPositionTween.reset();
                TitleMenuClickPositionTween.play();

                menu_step = 2;
            }
            else if (menu_step == 2) {
                TitleBackClickPositionTween.initPosition(Title.sprite->getPosition(), TitlePosition);
                TitleBackClickPositionTween.reset();
                TitleBackClickPositionTween.play();

                menu_step = 1;
            } 
        }   
    } 
}

void MainMenu::render() {   
    mWindow.clear(sf::Color::Transparent);

    ShaderUtils::drawVerticalBlurSprite(mWindow,background, 5.0f);
    mWindow.draw(*Title.sprite);
    mWindow.draw(*Cursor.sprite);
}
