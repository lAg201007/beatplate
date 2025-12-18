#ifndef SCALE_MANAGER_H
#define SCALE_MANAGER_H

#include <SFML/System/Vector2.hpp>

class ScaleManager {
private:
    static float scaleFactorX;
    static float scaleFactorY;
    static const float BASE_WIDTH;
    static const float BASE_HEIGHT;

public:
    static void UpdateScale(float windowWidth, float windowHeight);
    static float GetScaleX();
    static float GetScaleY();
    static float GetBaseWidth();
    static float GetBaseHeight();
    static sf::Vector2f ScalePosition(float x, float y);
    static sf::Vector2f ScaleSize(float x, float y);
    static sf::Vector2f UnscalePosition(float x, float y);
    static sf::Vector2f GetBaseCenterPosition();
};
#endif
