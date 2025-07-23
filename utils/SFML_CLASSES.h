#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

extern uint32_t uniqueIdCounter;

#ifndef SHADER_SPRITE
#define SHADER_SPRITE

class ShaderSprite : public sf::Sprite {
public:
    ShaderSprite(const sf::Texture& texture)
        : sf::Sprite(texture) {}

    ShaderSprite(const sf::Texture& texture, const sf::IntRect& rectangle)
        : sf::Sprite(texture, rectangle) {}

    ShaderSprite(const ShaderSprite&) = default;
    ShaderSprite(ShaderSprite&&) noexcept = default;
    ShaderSprite& operator=(const ShaderSprite&) = default;
    ShaderSprite& operator=(ShaderSprite&&) noexcept = default;

    uint32_t uniqueId = uniqueIdCounter++; // Unique ID for each ShaderSprite instance
    
};

#endif

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

    void debug_DrawRect(sf::RenderWindow& window) {
        sf::FloatRect bounds = sprite->getGlobalBounds();

        sf::RectangleShape debugRect;
        debugRect.setPosition(bounds.position); 
        debugRect.setSize(bounds.size);       

        debugRect.setFillColor(sf::Color::Transparent);
        debugRect.setOutlineColor(sf::Color::Red);
        debugRect.setOutlineThickness(1.0f);

        window.draw(debugRect);
    }  
};

#endif 

#ifndef SHADER_OBJECT
#define SHADER_OBJECT

class ShaderObject {
public:
    std::shared_ptr<sf::Texture> spriteTexture;
    std::shared_ptr<ShaderSprite> sprite;
    std::string filePath;
    float blurredStrength = 0.0f;

    ShaderObject(std::string imgfile, float startXpos, float startYpos, int originX = 0, int originY = 0, float scaleX = 1, float scaleY = 1) : filePath(imgfile) {
        spriteTexture = std::make_shared<sf::Texture>();

        if (!spriteTexture->loadFromFile(imgfile)) {
            std::cerr << "N�o foi poss�vel carregar a imagem: " << imgfile << std::endl;
        }

        sprite = std::make_shared<ShaderSprite>(*spriteTexture);

        sprite->setPosition({ startXpos, startYpos });


        if (originX && originY) {
            sprite->setOrigin(sf::Vector2f(static_cast<float>(originX), static_cast<float>(originY)));
        }

        sprite->setScale({ scaleX, scaleY });
    }

    void debug_DrawRect(sf::RenderWindow& window) {
        sf::FloatRect bounds = sprite->getGlobalBounds();

        sf::RectangleShape debugRect;
        debugRect.setPosition(bounds.position); 
        debugRect.setSize(bounds.size);       

        debugRect.setFillColor(sf::Color::Transparent);
        debugRect.setOutlineColor(sf::Color::Red);
        debugRect.setOutlineThickness(1.0f);

        window.draw(debugRect);
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

#ifndef BASE_BUTTON
#define BASE_BUTTON

class BaseButton {
protected:
    bool PressedLastFrame = false;

public:
    virtual sf::FloatRect getBounds() const = 0; // classe abstrata

    bool isHovered(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return getBounds().contains(static_cast<sf::Vector2f>(mousePos));
    }

    bool DetectButtonClick(sf::RenderWindow &window) {
        bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        bool hovered = isHovered(window);

        if (hovered && mousePressed && !PressedLastFrame) {
            PressedLastFrame = true;
            return true;
        }

        PressedLastFrame = mousePressed;
        return false;
    }
};

#endif

#ifndef BUTTON
#define BUTTON

class Button : public Object, public BaseButton {
public:
    Button(std::string imgfile, float startXpos, float startYpos,
           int originX = 0, int originY = 0,
           float scaleX = 1, float scaleY = 1)
        : Object(imgfile, startXpos, startYpos, originX, originY, scaleX, scaleY) {}

    
    sf::FloatRect getBounds() const override {
        return sprite->getGlobalBounds();
    }
};

#endif

#ifndef SHADER_BUTTON
#define SHADER_BUTTON

// eu n queria ter que copiar e colar a função :(

class ShaderButton : public ShaderObject, public BaseButton {
public:
    ShaderButton(std::string imgfile, float startXpos, float startYpos,
           int originX = 0, int originY = 0,
           float scaleX = 1, float scaleY = 1)
        : ShaderObject(imgfile, startXpos, startYpos, originX, originY, scaleX, scaleY) {}

    sf::FloatRect getBounds() const override {
        return sprite->getGlobalBounds();
    }
};

#endif