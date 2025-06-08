#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#ifndef OBJECT
#define OBJECT

class Object {
public:
    std::shared_ptr<sf::Texture> spriteTexture;
    std::shared_ptr<sf::Sprite> sprite;
    std::string filePath;
    float blurredStrength = 0.0f;

    Object(std::string imgfile, float startXpos, float startYpos, int originX = 0, int originY = 0, float scaleX = 1, float scaleY = 1) : filePath(imgfile) {
        spriteTexture = std::make_shared<sf::Texture>();

        if (!spriteTexture->loadFromFile(imgfile)) {
            std::cerr << "N�o foi poss�vel carregar a imagem: " << imgfile << std::endl;
        }

        sprite = std::make_shared<sf::Sprite>(*spriteTexture);

        sprite->setPosition({ startXpos, startYpos });


        if (originX && originY) {
            sprite->setOrigin(sf::Vector2f(static_cast<float>(originX), static_cast<float>(originY)));
        }

        sprite->setScale({ scaleX, scaleY });
    }
};

#endif 

#ifndef TEXTURE
#define TEXTURE

class Texture {
public:
    std::shared_ptr<sf::Texture> texture;

    Texture(std::string imgfile) {
        texture = std::make_shared<sf::Texture>();

        if (!texture->loadFromFile(imgfile)) {
            std::cerr << "N�o foi poss�vel carregar a imagem: " << imgfile << std::endl;
        }
    }
};

#endif 

#ifndef SOUND
#define SOUND

class Sound {
public:
    sf::SoundBuffer Buffer;
    std::unique_ptr<sf::Sound> sound;
    Sound(std::string soundfile, int soundVolume = 100) {

        if (!Buffer.loadFromFile(soundfile)) {
            std::cerr << "N�o foi possivel carregar som para o arquivo: " << soundfile << std::endl;
        }

        sound = std::make_unique<sf::Sound>(Buffer);
        sound->setVolume(soundVolume);
    }
};

#endif 
#ifndef BUTTON
#define BUTTON

class Button : public Object {
public:
    Button(std::string imgfile, float startXpos, float startYpos,
           int originX = 0, int originY = 0,
           float scaleX = 1, float scaleY = 1)
        : Object(imgfile, startXpos, startYpos, originX, originY, scaleX, scaleY) {}

    bool isHovered(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = sprite->getGlobalBounds();
        return bounds.contains(static_cast<sf::Vector2f>(mousePos));
    }

    bool DetectButtonClick(sf::RenderWindow& window, const sf::Event& event) {
        if (auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left && isHovered(window)) {
                return true;
            }
        }
        return false;
    }
};

#endif
