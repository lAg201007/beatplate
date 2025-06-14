#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"
namespace ShaderUtils {
    void drawVerticalBlurSprite(sf::RenderWindow& mWindow, sf::Sprite sprite, float BlurStrength);
    void drawSpriteWithWhiteMaskShader(sf::RenderWindow& window, sf::Sprite sprite, int WhiteIntensity);
}
