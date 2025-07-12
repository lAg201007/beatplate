#pragma once

#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <SFML/Graphics.hpp>

void taskDelay(int milliseconds, std::function<void()> callback);

int fitTextToWidth(sf::Text& text, float maxWidth, unsigned int minSize = 10);

void ResizeSpriteToFitWindow(sf::Sprite& sprite, sf::RenderWindow& window);

#endif