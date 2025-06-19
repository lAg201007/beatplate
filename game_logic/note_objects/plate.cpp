#include "plate.h"

Plate::Plate(int offset)
    : Note(offset, "plate"),
      plateObject("assets/sprites/game/objects/plate.png", 0, 0, 200, 200, 0.25f, 0.25f) 
{
    
};

void Plate::start() {

}

void Plate::update(float dt) {

}

void Plate::render(sf::RenderWindow& window) {
    
}