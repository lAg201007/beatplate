#pragma once

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"

struct ShaderCompound {
    std::optional<sf::Sprite> renderSprite;
    std::shared_ptr<sf::Shader> shader;
    std::shared_ptr<sf::RenderTexture> textureHolder; 

    ShaderCompound()
        : shader(nullptr), textureHolder(nullptr) {}
    ShaderCompound(sf::Sprite& s, std::shared_ptr<sf::Shader> sh)
        : renderSprite(s), shader(sh), textureHolder(nullptr) {}
};
// TODO: FIX THIS

namespace ShaderUtils {
    ShaderCompound createVerticalBlurCompound(sf::RenderWindow& mWindow, sf::Sprite& sprite, float BlurStrength);
    ShaderCompound createWhiteMaskCompound(sf::RenderWindow& window, sf::Sprite& sprite, int WhiteIntensity);
    ShaderCompound createBlackOutCompound(sf::RenderWindow& window, sf::Sprite& sprite, int BlackIntensity);
    void drawShaderCompound(sf::RenderWindow& window, const ShaderCompound& compound);
    void drawCompoundVector(sf::RenderWindow& window, const std::vector<ShaderCompound>& compounds);

    extern std::shared_ptr<sf::Shader> blurShaderPtr;
    extern std::shared_ptr<sf::Shader> whiteMaskShaderPtr;
    void loadShaders();
}
