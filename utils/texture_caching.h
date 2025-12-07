#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <string>

extern std::unordered_map<std::string, sf::Texture> TextureCache;

sf::Texture& LoadTexture(const std::string& path);
