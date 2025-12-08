#include "../utils/scale_manager.h"

// Definição das variáveis estáticas
float ScaleManager::scaleFactorX = 1.0f;
float ScaleManager::scaleFactorY = 1.0f;
const float ScaleManager::BASE_WIDTH = 1280.0f;
const float ScaleManager::BASE_HEIGHT = 720.0f;

void ScaleManager::UpdateScale(float windowWidth, float windowHeight) {
    scaleFactorX = windowWidth / BASE_WIDTH;
    scaleFactorY = windowHeight / BASE_HEIGHT;
}

float ScaleManager::GetScaleX() { 
    return scaleFactorX; 
}

float ScaleManager::GetScaleY() { 
    return scaleFactorY; 
}

sf::Vector2f ScaleManager::ScalePosition(float x, float y) {
    return sf::Vector2f({x * scaleFactorX, y * scaleFactorY});
}

sf::Vector2f ScaleManager::ScaleSize(float x, float y) {
    return sf::Vector2f({x * scaleFactorX, y * scaleFactorY});
}