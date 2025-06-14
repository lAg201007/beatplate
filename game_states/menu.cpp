#include "menu.h"
#include "song_select.h"
#include "../state_stack.h"        
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    Title("assets/sprites/main_menu/title.png",600,360,225,104),
    background("assets/sprites/main_menu/background.png",0,0),

    TitleTween(*Title.sprite,0.5f,Tween::linear),
    TitleTransparencyTween(*Title.sprite,3.0f,Tween::linear),
    StartTextTransparencyTween(0.0f,1.0f,3.0f),
    StartGameText(Arial),
    textColor(StartGameText.getFillColor())
{
    TitlePosition = Title.sprite->getPosition();
    TitleTween.initScale(1.0f,1.1f);
    TitleTransparencyTween.initTransparency(0.0f,1.0f);
    TitleTransparencyTween.play();

    if (!Arial.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
        std::cerr << "não foi possivel carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
    }

    StartGameText.setString("Press the Title Button to Play");
    StartGameText.setCharacterSize(20);
    StartGameText.setPosition(TitlePosition + sf::Vector2f({-100,80}));

    sf::Vector2u windowSize = window.getSize();                
    sf::Vector2u textureSize = background.sprite->getTexture().getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.sprite->setScale({scaleX, scaleY});

    taskDelay(3500, [this]() {
        StartTextTransparencyTween.play();
    });

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

    textTransparencyValue = StartTextTransparencyTween.getValue();
    textColor.a = static_cast<int>(std::round(textTransparencyValue * 255.0f));
    StartGameText.setFillColor(textColor);

    TitleTween.update(dt.asSeconds());
    TitleTransparencyTween.update(dt.asSeconds());
    StartTextTransparencyTween.update(dt.asSeconds());

    if (!TitleTween.isActive()) {
        TitleTween.reset();
        TitleTween.play();
    }

    if (Title.DetectButtonClick(mWindow)) {
        mStack.pushState(std::make_unique<SongSelect>(mStack,mWindow));
    } 
}

void MainMenu::render() {   
    mWindow.clear(sf::Color::Transparent);

    ShaderUtils::drawVerticalBlurSprite(mWindow,*background.sprite, 5.0f);
    mWindow.draw(*Title.sprite);
    mWindow.draw(StartGameText);
    mWindow.draw(*Cursor.sprite);
}
