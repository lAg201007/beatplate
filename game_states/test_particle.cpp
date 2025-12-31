#include "../utils/particle_system.hpp"
#include "test_particle.hpp"
#include "game_state.h"
#include "state_stack.h"
#include "../utils/SFML_CLASSES.h"
#include <print>

ParticleTest::ParticleTest(StateStack& stack, sf::RenderWindow& window) :
  State(stack, window),
  texturePath("assets/sprites/cursor.png"),
  lifetime({1.5,3}),
  particle_count({20,40}),
  emitterInitialPos({500, 500}),
  initialVelocity({45, -45}, {65, -65}),
  acceleration({20, -20}),
  scaleArray({ {0.f, {1,1}}, {1.f, {2,2}}  }),
  spread(300),
  colorArray({ {0.f, {10,230,1,255}}, {1.f, {50, 120, 255, 0}} }),
  rotationArray({ {0.f, {sf::degrees(0)}  }, {1.f, {sf::degrees(360)}  } }),

  emitter(texturePath, lifetime, spread, particle_count, emitterInitialPos, initialVelocity, acceleration, scaleArray, colorArray, rotationArray),
  Cursor("assets/sprites/cursor.png", 400, 300, 0, 0)
{
  if (debugFont.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
    fontLoaded = true;
  } else {
      std::cerr << "ERRO: Não foi possível carregar a fonte para debug." << std::endl;
  }
}

void ParticleTest::handleEvent(const sf::Event& event) {
  if (event.is<sf::Event::KeyPressed>()) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
      if (keyPressed->scancode == sf::Keyboard::Scancode::G) {
        emitter.spawn();
      }

      if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
        mStack.popState();
      }
    }
  }
}

void ParticleTest::update(sf::Time dt) {
  sf::Vector2i mouse_pos = sf::Mouse::getPosition(mWindow);
  emitter.move({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});
  Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});

  updateAllParticles(dt.asSeconds());
}

void ParticleTest::render() { 
  mWindow.clear(sf::Color::Black);
  renderAllParticles(mWindow);
  mWindow.draw(*Cursor.sprite);
}
