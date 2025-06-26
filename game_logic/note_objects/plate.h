#pragma once
#include "../notes.h"
#include "../../utils/SFML_CLASSES.h"
#include <cctype>
#include "../../utils/tween_service.h"

class Plate : public Note {
public:
    Button plateObject;  
    sf::RenderWindow& window;
    std::vector<std::string> binds;
    int AR;
    int ACD;
    int PS;

    Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS);

    void update(float elapsed, float dt) override;
    void start() override;
    void render(sf::RenderWindow& window) override;

    bool DetectHoverX(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = plateObject.sprite->getGlobalBounds();
        return bounds.contains(sf::Vector2f({static_cast<float>(mousePos.x), 300.0f}));
    }

    bool DetectClickWithBind(sf::RenderWindow& window) {
        bool bindPressed = false;
        for (const auto& bind : binds) {
            sf::Keyboard::Key key = sf::Keyboard::Key::Unknown;
            if (bind.size() == 1 && std::isalpha(bind[0])) {
                key = static_cast<sf::Keyboard::Key>(
                    static_cast<int>(sf::Keyboard::Key::A) + (std::toupper(bind[0]) - 'A')
                );
            }
            if (key != sf::Keyboard::Key::Unknown && sf::Keyboard::isKeyPressed(key)) {
                bindPressed = true;
                break;
            }
        }

        bool hovered = DetectHoverX(window);
       
        if (hovered && bindPressed && !PressedLastFrame) {
            PressedLastFrame = true;
            return true;
        }
        PressedLastFrame = bindPressed;
        return false;
    }
private:
    int TargetYPos = 300;
    int StartYPos = 600;
    float startTimeMs = 0.0f;
    float approachMs = 1200.0f;
    float hitWindow = 0.0f;
    bool PressedLastFrame = false;
    Object approachCircle;
    Tween ApproachCircleScaleTween;
    Tween HitTransparencyTween;
    Tween HitScaleTween;
    Tween MissScaleTween;
    Tween MissTransparencyTween;
};