#pragma once 

#include "../utils/SFML_CLASSES.h"
#include "../utils/rng.hpp"
#include <iostream>
#include <memory>
#include <vector>

struct Particle {
  Object object;

  // the first value of the pair will aways be the time in lifetime that the value in the array will be placed
  std::vector<std::pair<float, sf::Color>> ColorArray;
  std::vector<std::pair<float, sf::Angle>> RotationArray;
  std::vector<std::pair<float, sf::Vector2f>> ScaleArray;

  sf::Vector2f Velocity;
  sf::Vector2f Acceleration;
  sf::Angle Direction;
  float Lifetime;
  float Elapsed = 0.f;

  void reset() {
    object = Object();
    ColorArray = {{0,sf::Color::White}};
    RotationArray = {{0, sf::degrees(0)}};
    ScaleArray = {{0, {0,0}}};

    Velocity = {0,0};
    Acceleration = {0,0};
    Direction = sf::degrees(0);
    Elapsed = 0;
  }
};

struct ParticleSlot {
  Particle particle;
  bool alive = false;

  void reset() {
    alive = false;
    particle.reset();
  }
};  

// Global Particle Array
extern std::vector<ParticleSlot> GlobalParticleArray;
extern std::vector<uint32_t> FreeSlots;

template<typename T>
std::pair<std::pair<float, T>, std::pair<float, T>> returnNeighborPointsInArray(const std::vector<std::pair<float, T>>& array, float Elapsed);

uint32_t allocateParticle(Particle& newParticle);
void deallocateParticle(uint32_t id);

uint8_t lerpColor(float ColorA, float ColorB, float t);
sf::Angle lerpRotation(sf::Angle RotationA, sf::Angle RotationB, float t);
sf::Vector2f lerpScale(sf::Vector2f ScaleA, sf::Vector2f ScaleB, float t);

void drawParticle(uint32_t id, sf::RenderWindow& window);
void updateParticle(uint32_t id, float dt);

void updateAllParticles(float dt);
void renderAllParticles(sf::RenderWindow& window);

class ParticleEmitter {
public:
  //std::pair<x,y>
  //scaleArray -> std::pair<lifeTimePoint, scale>
  ParticleEmitter(std::string texturePath,
                                  std::pair<float,float> lifetime, 
                                  float spread,
                                  std::pair<int,int> particle_count,
                                  sf::Vector2f emitterInitialPos,    
                                  std::pair<sf::Vector2f, sf::Vector2f> initialVelocity,        
                                  sf::Vector2f acceleration,
                                  std::vector<std::pair<float, sf::Vector2f>> scaleArray,
                                  std::vector<std::pair<float, sf::Color>> colorArray,
                                  std::vector<std::pair<float, sf::Angle>> rotationArray,
                                  sf::Vector2i objectOrigin = {0,0})
      : m_texturePath(texturePath),
        m_lifetime(lifetime),
        m_spread(spread),
        m_particleCount(particle_count),
        m_emitterInitialPos(emitterInitialPos),
        m_initialVelocity(initialVelocity),
        m_acceleration(acceleration),
        m_scaleArray(std::move(scaleArray)),
        m_colorArray(std::move(colorArray)),
        m_rotationArray(std::move(rotationArray)),
        m_objectOrigin(objectOrigin)
  { 
  }

  ~ParticleEmitter() 
  {
      for (uint32_t& particleId : m_particleIds) {
          deallocateParticle(particleId); 
      }
      m_particleIds.clear();
  }

  void spawn() {
    int RandomParticleCount = Random::rangeInt(m_particleCount.first, m_particleCount.second);

    for (int i = 0; i < RandomParticleCount;  i++) {
      Object newObject(m_texturePath, m_emitterInitialPos.x, m_emitterInitialPos.y, m_objectOrigin.x, m_objectOrigin.y);

      float halfSpread = m_spread * 0.5f;
      float offsetDegrees = Random::rangeFloat(-halfSpread, halfSpread);

      sf::Vector2f RandomVel({Random::rangeFloat(m_initialVelocity.first.x, m_initialVelocity.second.x),
                               Random::rangeFloat(m_initialVelocity.first.y, m_initialVelocity.second.y)});
      
      Particle newParticle;

      newParticle.Elapsed = 0.f;
      newParticle.Direction = sf::degrees(offsetDegrees);
      newParticle.object = newObject;
      newParticle.ColorArray = m_colorArray; 
      newParticle.RotationArray = m_rotationArray; 
      newParticle.ScaleArray = m_scaleArray; 
      newParticle.Velocity = RandomVel;
      newParticle.Acceleration = m_acceleration;
      newParticle.Lifetime = Random::rangeFloat(m_lifetime.first, m_lifetime.second);
      
      m_particleIds.push_back(allocateParticle(newParticle));
    }
  }

  void move(sf::Vector2f position) {
    m_emitterInitialPos = position;
  }
  
private:
    std::string m_texturePath;
    std::pair<float,float> m_lifetime; 
    float m_spread;
    std::pair<int,int> m_particleCount;
    sf::Vector2f m_emitterInitialPos;    
    std::pair<sf::Vector2f, sf::Vector2f> m_initialVelocity;        
    sf::Vector2f m_acceleration;
    std::vector<std::pair<float, sf::Vector2f>> m_scaleArray;
    std::vector<std::pair<float, sf::Color>> m_colorArray;
    std::vector<std::pair<float, sf::Angle>> m_rotationArray;
    sf::Vector2i m_objectOrigin;

    std::vector<uint32_t> m_particleIds; 
};
