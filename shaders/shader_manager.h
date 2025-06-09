#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"
namespace ShaderUtils {
    void drawVerticalBlurSprite(sf::RenderWindow& mWindow, Object obj, float BlurStrength);
}
