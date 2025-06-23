#pragma once
#include "../notes.h"
#include "../../utils/SFML_CLASSES.h"

class Plate : public Note {
public:
    Object plateObject;    
    float AR;

    Plate(int offset, int xPos, float AR);

    void update(float elapsed, float AR) override;
    void start() override;
    void render(sf::RenderWindow& window) override;

private:
    int TargetYPos = 300;
    int StartYPos = 600;
    float startTimeMs = 0.0f;
    float approachMs = 1200.0f;
};