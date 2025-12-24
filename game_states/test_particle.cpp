#include "../utils/particle_system.hpp"
#include "test_particle.hpp"
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"

ParticleTest::ParticleTest(StateStack& stack, sf::RenderWindow& window) :
  State(stack, window),
  texturePath("assets/sprites/cursor.png"),
  lifetime({1.5,3}),
  particle_count({20,40}),
  emitterInitialPos({500, 500}),
  initialVelocity({45, -45}, {65, -65}),
  acceleration({20, -20}),
  scaleArray({{1,1}}),
  spread(300),
  colorArray({ {0.f, {10,230,1,255}}, {3.f, {50, 120, 255, 255}} }),
  rotationArray({ {0.f, {sf::degrees(0)}  }, {3.f, {sf::degrees(360)}  } }),

  testEmmiter(texturePath, lifetime, spread, particle_count, emitterInitialPos, initialVelocity, acceleration, scaleArray, colorArray, rotationArray)
{
    
}

void ParticleTest::handleEvent(const sf::Event& event) {
}

void ParticleTest::update(sf::Time dt) {
  testEmmiter.update(dt, mWindow);
}

void ParticleTest::render() { 
  mWindow.clear(sf::Color::Black);
  testEmmiter.render(mWindow);
}
