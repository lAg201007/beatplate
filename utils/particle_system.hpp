#pragma once 

#include "../utils/SFML_CLASSES.h"
#include <iostream>
#include <memory>
#include <vector>

struct Particle {
  Object object;

  // the first value of the pair will aways be the time in lifetime that the value in the array will be placed
  std::vector<std::pair<float, sf::Color>> ColorArray;
  std::vector<std::pair<float, sf::Angle>> RotationArray;
  std::vector<sf::Vector2f> ScaleArray;
  
  sf::Vector2f Velocity;
  sf::Vector2f Acceleration;
  float Lifetime;
  float Elapsed = 0.f;
};

struct ParticleSlot {
  std::unique_ptr<Particle> particle;
  bool alive = false;

  void reset() {
    alive = false;
    particle.reset();
  }
};  

// Global Particle Array
extern std::vector<ParticleSlot> GlobalParticleArray;
extern std::vector<uint32_t> FreeSlots;

uint32_t alocateParticle(Particle& newParticle);
void dealocateParticle(uint32_t id);
void drawParticle(uint32_t id, sf::RenderWindow& window);
void updateParticle(uint32_t id, float dt);

class ParticleEmitter {
public:
  //std::pair<x,y>
  //scaleArray -> std::pair<lifeTimePoint, scale>
  ParticleEmitter(std::string texturePath,
                  float lifetime, 
                  int particle_count,
                  sf::Vector2f emitterInitialPos,
                  sf::Vector2f initialVelocity,             
                  sf::Vector2f acceleration,
                  std::vector<sf::Vector2f> scaleArray,
                  std::pair<sf::Angle, sf::Angle> spread,
                  std::vector<std::pair<float, sf::Color>> colorArray,
                  std::vector<std::pair<float, sf::Angle>> rotationArray,

                  sf::Vector2i objectOrigin = {0,0})
  {
    for (int i = GlobalParticleArray.size() + 1; i < particle_count;  i++) {
      Object newObject(texturePath, emitterInitialPos.x, emitterInitialPos.y, objectOrigin.x, objectOrigin.y);
        
      Particle newParticle;

      newParticle.object = newObject;
      newParticle.ColorArray = colorArray;
      newParticle.RotationArray = rotationArray;
      newParticle.ScaleArray = scaleArray;
      newParticle.Velocity = initialVelocity;
      newParticle.Acceleration = acceleration;
      newParticle.Lifetime = lifetime;

      particleIds.push_back(alocateParticle(newParticle));
    }
  }

  ~ParticleEmitter() 
  {
    for (uint32_t& particleId : particleIds) {
      dealocateParticle(particleId);
    }
    particleIds.clear();
  }

  void render(sf::RenderWindow& window) {
    for (uint32_t& particleId : particleIds) {
      drawParticle(particleId, window);
    }
  }

  void update(sf::Time dt, sf::RenderWindow& window) {
    float TimeInSeconds = dt.asSeconds();

    for (uint32_t id : particleIds) {
      updateParticle(id, TimeInSeconds);
    }
  }
  
private:
  std::vector<uint32_t> particleIds;
};
