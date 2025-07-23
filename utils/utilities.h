#pragma once

#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <SFML/Graphics.hpp>
#include "SFML_CLASSES.h"

void taskDelay(int milliseconds, std::function<void()> callback);

int fitTextToWidth(sf::Text& text, float maxWidth, unsigned int minSize = 10);

template<typename SpriteT>
void ResizeSpriteToFitWindow(SpriteT& sprite, sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = sprite.getTexture().getSize();
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
    sprite.setScale({ scaleX, scaleY });
}

#endif