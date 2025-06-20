#pragma once
#include "../notes.h"
#include "../../utils/SFML_CLASSES.h"

class Plate : public Note {
public:
    Object plateObject;    

    Plate(int offset, int xPos, const float AR);

    void update(float elapsed) override;
    void start() override;
    void render(sf::RenderWindow& window) override;

private:
    int TargetYPos = 300;
    int StartYPos = 600;
    float startTimeMs = 0.0f;
    float approachMs = 1200.0f;
};