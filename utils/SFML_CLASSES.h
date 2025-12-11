
// ==================== NO SEU ARQUIVO PRINCIPAL (objects.h ou similar) ====================
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/texture_caching.h"
#include "../utils/scale_manager.h"  // <--- Inclua o scale_manager

extern uint32_t uniqueIdCounter;

// ==================== SHADER SPRITE ====================
#ifndef SCALED_TEXT
#define SCALED_TEXT

class ScaledText : public sf::Text {
private:
    sf::Vector2f basePosition{0, 0};
    unsigned int baseCharacterSize = 30;

public:
    using sf::Text::Text;

    void setPosition(float x, float y) {
        basePosition = {x, y};
        sf::Vector2f scaledPos = ScaleManager::ScalePosition(x, y);
        sf::Text::setPosition(scaledPos);
    }

    void setPosition(const sf::Vector2f& position) {
        setPosition(position.x, position.y);
    }

    void setCharacterSize(unsigned int size) {
        baseCharacterSize = size;
        unsigned int scaledSize = static_cast<unsigned int>(size * ScaleManager::GetScaleX());
        sf::Text::setCharacterSize(scaledSize);
    }

    unsigned int getBaseCharacterSize() const {
        return baseCharacterSize;
    }

    void updateScale() {
        setPosition(basePosition);
        setCharacterSize(baseCharacterSize);
    }
};

#endif

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

    uint32_t uniqueId = uniqueIdCounter++;

    void setPosition(float x, float y) {
        sf::Vector2f scaledPos = ScaleManager::ScalePosition(x, y);
        sf::Sprite::setPosition(scaledPos);
    }

    void setPosition(const sf::Vector2f& position) {
        setPosition(position.x, position.y);
    }

    void setScale(float factorX, float factorY) {
        float scaledX = factorX * ScaleManager::GetScaleX();
        float scaledY = factorY * ScaleManager::GetScaleY();
        sf::Sprite::setScale({scaledX, scaledY});
    }

    void setScale(const sf::Vector2f& factors) {
        setScale({factors.x, factors.y});
    }

    void setOrigin(float x, float y) {
        sf::Sprite::setOrigin({x, y});
    }

    void setOrigin(const sf::Vector2f& origin) {
        sf::Sprite::setOrigin(origin);
    }
};

#endif

// ==================== SCALED SPRITE ====================
#ifndef SCALED_SPRITE
#define SCALED_SPRITE

class ScaledSprite : public sf::Sprite {
public:
    ScaledSprite(const sf::Texture& texture)
        : sf::Sprite(texture) {}

    ScaledSprite(const sf::Texture& texture, const sf::IntRect& rectangle)
        : sf::Sprite(texture, rectangle) {}

    ScaledSprite(const ScaledSprite&) = default;
    ScaledSprite(ScaledSprite&&) noexcept = default;
    ScaledSprite& operator=(const ScaledSprite&) = default;
    ScaledSprite& operator=(ScaledSprite&&) noexcept = default;

    void setPosition(float x, float y) {
        sf::Vector2f scaledPos = ScaleManager::ScalePosition(x, y);
        sf::Sprite::setPosition(scaledPos);
    }

    void setPosition(const sf::Vector2f& position) {
        setPosition(position.x, position.y);
    }

    void setScale(float factorX, float factorY) {
        float scaledX = factorX * ScaleManager::GetScaleX();
        float scaledY = factorY * ScaleManager::GetScaleY();
        sf::Sprite::setScale({scaledX, scaledY});
    }

    void setScale(const sf::Vector2f& factors) {
        setScale(factors.x, factors.y);
    }

    void setOrigin(float x, float y) {
        sf::Sprite::setOrigin({x, y});
    }

    void setOrigin(const sf::Vector2f& origin) {
        sf::Sprite::setOrigin(origin);
    }
};

#endif

// ==================== OBJECT ====================
#ifndef OBJECT
#define OBJECT

class Object {
public:
    sf::Texture* spriteTexture;
    std::shared_ptr<ScaledSprite> sprite;
    std::string filePath;
    float blurredStrength = 0.0f;

    Object() : spriteTexture(&LoadTexture("assets/sprites/no_texture.jpg")),
               filePath("assets/sprites/no_texture.jpg"),
               sprite(std::make_shared<ScaledSprite>(*spriteTexture)),
               blurredStrength(0.0f)
    {
        sprite->setPosition(0,0);
    }

    Object(std::string imgfile, float startXpos, float startYpos,
           int originX = 0, int originY = 0,
           float scaleX = 1, float scaleY = 1)
        : filePath(imgfile) 
    {
        spriteTexture = &LoadTexture(imgfile);
        sprite = std::make_shared<ScaledSprite>(*spriteTexture);
        
        sprite->setPosition(startXpos, startYpos);

        if (originX && originY)
            sprite->setOrigin(sf::Vector2f((float)originX, (float)originY));

        sprite->setScale(scaleX, scaleY);
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

// ==================== SHADER OBJECT ====================
#ifndef SHADER_OBJECT
#define SHADER_OBJECT

class ShaderObject {
public:
    sf::Texture* spriteTexture;
    std::shared_ptr<ShaderSprite> sprite;
    std::string filePath;
    float blurredStrength = 0.0f;

    ShaderObject(std::string imgfile, float startXpos, float startYpos,
                 int originX = 0, int originY = 0,
                 float scaleX = 1, float scaleY = 1)
        : filePath(imgfile)
    {
        spriteTexture = &LoadTexture(imgfile);
        sprite = std::make_shared<ShaderSprite>(*spriteTexture);
        
        sprite->setPosition(startXpos, startYpos);

        if (originX && originY)
            sprite->setOrigin(sf::Vector2f((float)originX, (float)originY));

        sprite->setScale(scaleX, scaleY);
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

// ==================== TEXTURE ====================
#ifndef TEXTURE
#define TEXTURE

class Texture {
public:
    std::shared_ptr<sf::Texture> texture;

    Texture(std::string imgfile) {
        texture = std::make_shared<sf::Texture>();

        if (!texture->loadFromFile(imgfile)) {
            std::cerr << "Não foi possível carregar a imagem: " << imgfile << std::endl;
        }
    }
};

#endif 

// ==================== SOUND ====================
#ifndef SOUND
#define SOUND

class Sound {
public:
    sf::SoundBuffer Buffer;
    std::unique_ptr<sf::Sound> sound;
    
    Sound(std::string soundfile, int soundVolume = 100) {
        if (!Buffer.loadFromFile(soundfile)) {
            std::cerr << "Não foi possível carregar som para o arquivo: " << soundfile << std::endl;
        }

        sound = std::make_unique<sf::Sound>(Buffer);
        sound->setVolume(soundVolume);
    }
};

#endif 

// ==================== BASE BUTTON ====================
#ifndef BASE_BUTTON
#define BASE_BUTTON

class BaseButton {
protected:
    bool PressedLastFrame = false;

public:
    virtual sf::FloatRect getBounds() const = 0;

    bool isHovered(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = getBounds();
        
        // Simplesmente checa se o mouse (em coordenadas reais) está dentro dos bounds (já escalados)
        return bounds.contains(static_cast<sf::Vector2f>(mousePos));
    }

    bool DetectButtonClick(sf::RenderWindow &window) {
        bool mousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        bool hovered = isHovered(window);

        if (hovered && mousePressed && !PressedLastFrame && window.hasFocus()) {
            PressedLastFrame = true;
            return true;
        }

        PressedLastFrame = mousePressed;
        return false;
    }
};

#endif


// ==================== BUTTON ====================
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

// ==================== SHADER BUTTON ====================
#ifndef SHADER_BUTTON
#define SHADER_BUTTON

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
