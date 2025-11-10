#pragma once

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include "../utils/SFML_CLASSES.h"
#include <variant>
#include <optional>

namespace ShaderUtils {
    sf::Shader createShader(const std::string& fragmentShaderPath, std::optional<std::string> vertexShaderPath, std::vector<std::pair<std::string, std::variant<int, float, bool, sf::Vector2f, sf::Texture, sf::Shader::CurrentTextureType>>> uniforms);
    sf::Shader createWhiteMaskShader(float WhiteMultiplier, sf::RenderWindow& window);
    sf::Shader createDarkOutShader(float DarkMultiplier, sf::RenderWindow& window);
    sf::Texture applyBlur(sf::RenderWindow& window, sf::Sprite& sprite, float blurStrength);
    Object applyBlurToObject(sf::RenderWindow& window, Object& object, float blurStrength);
    void drawShader(sf::RenderWindow& window, sf::Sprite& sprite, sf::Shader& shader);
};
