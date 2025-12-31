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
    std::pair<float,float> lifetime;
    float spread;
    std::pair<int,int> particle_count;
    sf::Vector2f emitterInitialPos;
    std::pair<sf::Vector2f, sf::Vector2f> initialVelocity; 
    sf::Vector2f acceleration; 
    std::vector<std::pair<float, sf::Vector2f>> scaleArray;
    std::vector<std::pair<float, sf::Color>> colorArray;
    std::vector<std::pair<float, sf::Angle>> rotationArray;

    sf::Font debugFont;
    bool fontLoaded = false;

    ParticleEmitter emitter;
};
