#include "utilities.h"
#include "SFML_CLASSES.h"
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>

void taskDelay(int milliseconds, std::function<void()> callback) {
    std::thread([milliseconds, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        callback();
    }).detach();
}

int fitTextToWidth(sf::Text& text, float maxWidth, unsigned int minSize) {
    unsigned int size = text.getCharacterSize();
    float currentWidth = text.getLocalBounds().size.x;

    // Cria uma cópia temporária para simular o ajuste
    sf::Text temp = text;

    while (currentWidth > maxWidth && size > minSize) {
        size--;
        temp.setCharacterSize(size);
        currentWidth = temp.getLocalBounds().size.x;
    }
    return size;
}

void ResizeSpriteToFitWindow(sf::Sprite& sprite, sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();                
    sf::Vector2u textureSize = sprite.getTexture().getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    sprite.setScale({scaleX, scaleY});
}

