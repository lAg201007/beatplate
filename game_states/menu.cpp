
#include "menu.h"
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"

std::vector<std::function<void(float)>> tweens;
float elapsed = 0.0f;

MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    Title("assets/sprites/main_menu/title.png",600,360,225,104),
    background("assets/sprites/main_menu/background.png",0,0),

    TitleTween(*Title.sprite,0.5f,Tween::linear),
    TitleTransparencyTween(*Title.sprite,3.0f,Tween::linear)
{
    if (!blurShader.loadFromFile("shaders/blur.frag",sf::Shader::Type::Fragment)) {
        std::cerr << "Erro ao carregar shader!" << std::endl;
    }

    TitleTween.initScale(1.0f,1.1f);
    TitleTween.play();
    TitleTransparencyTween.initTransparency(0.0f,1.0f);
    TitleTransparencyTween.play();

    sf::Vector2u windowSize = window.getSize();                
    sf::Vector2u textureSize = background.sprite->getTexture().getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    background.sprite->setScale({scaleX, scaleY});
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
    const sf::Vector2u winSize = mWindow.getSize();

    sf::RenderTexture sceneRT({winSize.x, winSize.y});
    sceneRT.clear(sf::Color::White);
    sceneRT.draw(*background.sprite);
    sceneRT.display();

    sf::RenderTexture horizontalBlurRT({winSize.x, winSize.y});
    blurShader.setUniform("u_texture", sceneRT.getTexture());
    blurShader.setUniform("radius", 100.0f);
    blurShader.setUniform("resolution", static_cast<float>(winSize.x)); 
    blurShader.setUniform("dir", sf::Vector2f(1.0f, 0.0f));
    sf::Sprite horizontalPass(sceneRT.getTexture());

    horizontalBlurRT.clear();
    horizontalBlurRT.draw(horizontalPass, &blurShader);
    horizontalBlurRT.display();

    blurShader.setUniform("u_texture", horizontalBlurRT.getTexture());
    blurShader.setUniform("resolution", static_cast<float>(winSize.y));
    blurShader.setUniform("dir", sf::Vector2f(0.0f, 1.0f));
    sf::Sprite finalBlur(horizontalBlurRT.getTexture());

    mWindow.clear(sf::Color::White);
    mWindow.draw(finalBlur, &blurShader); 

    mWindow.draw(*Title.sprite);
    mWindow.draw(*Cursor.sprite);
}
