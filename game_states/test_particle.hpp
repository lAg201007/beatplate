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
    std::pair<sf::Vector2f, sf::Vector2f> initialVelocity; //TODO: fazer a parte float dos arrays serem normalizadas
    sf::Vector2f acceleration; // 0 sendo inicio do lifetime da particula, 1 sendo a morte dela
    std::vector<sf::Vector2f> scaleArray;// pra deixar as os arrays certos pra todos os tempos randomizados
    std::vector<std::pair<float, sf::Color>> colorArray;
    std::vector<std::pair<float, sf::Angle>> rotationArray;

    ParticleEmitter testEmmiter;
};
