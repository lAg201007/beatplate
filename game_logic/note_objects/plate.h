#pragma once
#include "../notes.h"
#include "../../utils/SFML_CLASSES.h"

class Plate : public Note {
public:
    Object plateObject;    

    Plate(int offset);

    void update(float dt) override;
    void start() override;
    void render(sf::RenderWindow& window) override;
};