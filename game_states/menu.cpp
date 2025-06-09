
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
    BackgroundBlurTween(0.0f,5.0f,3.0f,Tween::linear)
{
    TitleTween.initScale(1.0f,1.1f);
    TitleTween.play();
    TitleTransparencyTween.initTransparency(0.0f,1.0f);
    TitleTransparencyTween.play();
    BackgroundBlurTween.play();

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
    BackgroundBlurTween.update(dt.asSeconds());

    if (!TitleTween.isActive()) {
        TitleTween.reset();
        TitleTween.play();
    }

    if (Title.DetectButtonClick(mWindow)) {
        if (menu_step == 1) {
            menu_step = 2;
            std::cout << "passo 2" << std::endl;
        }
        else if (menu_step == 2) {
            menu_step = 1;
            std::cout << "passo 1" << std::endl;
        }
    } 
}

void MainMenu::render() {   
    mWindow.clear(sf::Color::Transparent);

    ShaderUtils::drawVerticalBlurSprite(mWindow,background, BackgroundBlurTween.getValue());
    mWindow.draw(*Title.sprite);
    mWindow.draw(*Cursor.sprite);
}
