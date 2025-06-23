#pragma once
#include "../notes.h"
#include "../../utils/SFML_CLASSES.h"

class Plate : public Note {
public:
    Button plateObject;  
    sf::RenderWindow& window;
    int AR;
    int ACD;
    int PS;

    Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS);

    void update(float elapsed) override;
    void start() override;
    void render(sf::RenderWindow& window) override;

private:
    int TargetYPos = 300;
    int StartYPos = 600;
    float startTimeMs = 0.0f;
    float approachMs = 1200.0f;
};