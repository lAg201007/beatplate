#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/particle_system.hpp"

class ParticleTest : public State {
public:
    ParticleTest(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    std::string texturePath;
    float lifetime;
    float spread;
    int particle_count;
    sf::Vector2f emitterInitialPos;
    sf::Vector2f initialVelocity;
    sf::Vector2f acceleration;
    std::vector<sf::Vector2f> scaleArray;
    std::vector<std::pair<float, sf::Color>> colorArray;
    std::vector<std::pair<float, sf::Angle>> rotationArray;

    ParticleEmitter testEmmiter;
};
