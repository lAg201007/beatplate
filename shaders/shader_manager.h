#pragma once

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"
#include <SFML/Graphics.hpp>
#include <variant>
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include <unordered_map>

using UniformValue = std::variant<
    int,
    float,
    bool,
    sf::Vector2f,
    std::reference_wrapper<const sf::Texture>
>;

struct ShaderCompound {
    std::optional<sf::Sprite> renderSprite;
    std::shared_ptr<sf::Shader> shader;
    std::shared_ptr<sf::RenderTexture> textureHolder; 

    ShaderCompound()
        : shader(nullptr), textureHolder(nullptr) {}
    ShaderCompound(sf::Sprite& s, std::shared_ptr<sf::Shader> sh)
        : renderSprite(s), shader(sh), textureHolder(nullptr) {}
};

namespace ShaderUtils {
    ShaderCompound createGenericShaderCompound(sf::RenderWindow& window, ShaderSprite& sprite, std::string shaderPath, const std::vector<std::pair<std::string, UniformValue>>& uniforms);
    ShaderCompound createVerticalBlurCompound(sf::RenderWindow& mWindow, ShaderSprite& sprite, float BlurStrength);
    ShaderCompound createWhiteMaskCompound(sf::RenderWindow& window, ShaderSprite& sprite, int WhiteIntensity);
    ShaderCompound createBlackOutCompound(sf::RenderWindow& window, ShaderSprite& sprite, int BlackIntensity);
    void drawShaderCompound(sf::RenderWindow& window, const ShaderCompound& compound);
    void drawCompoundVector(sf::RenderWindow& window, const std::vector<ShaderCompound>& compounds);

    void loadShaders();
}
