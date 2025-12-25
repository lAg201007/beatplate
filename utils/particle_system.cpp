#include "../utils/particle_system.hpp"
#include <cmath>

std::vector<ParticleSlot> GlobalParticleArray;
std::vector<uint32_t> FreeSlots;

// Loop through the array and returns the neighbor points in reference to elapsed
template<typename T>
std::pair<std::pair<float, T>, std::pair<float, T>> returnNeighborPointsInArray(const std::vector<std::pair<float, T>>& array, float NormalizedTime) {
  std::pair<float, T> T1;
  std::pair<float, T> T2;

  for (std::pair<float, T> Point : array) {
    if (Point.first <= NormalizedTime) {
      T1 = Point;
    }
    else {
      T2 = Point;
      break;
    }
  } 

  return {T1, T2};
}

uint32_t allocateParticle(Particle& newParticle) {
  if (!FreeSlots.empty()) {
    int id = FreeSlots.back();
    FreeSlots.pop_back();

    ParticleSlot newSlot(std::make_unique<Particle>(newParticle),true);
    GlobalParticleArray[id] = std::move(newSlot);
    return id;
  }

  ParticleSlot newSlot(std::make_unique<Particle>(newParticle), true);
  GlobalParticleArray.push_back(std::move(newSlot));
  return GlobalParticleArray.size() - 1;
}

uint8_t lerpColor(float ColorA, float ColorB, float t) {
    uint8_t newColor = static_cast<uint8_t>(std::lerp(ColorA, ColorB, t));
    return newColor;
}

sf::Angle lerpRotation(sf::Angle RotationA, sf::Angle RotationB, float t) {
  sf::Angle NewAngle = sf::degrees(std::clamp(std::lerp(RotationA.asDegrees(), RotationB.asDegrees(), t), 0.f, 360.f));
  return NewAngle;
}

void deallocateParticle(uint32_t id) {
  GlobalParticleArray[id].reset();
  FreeSlots.push_back(id);
}

void drawParticle(uint32_t id, sf::RenderWindow& window) {
  if (!GlobalParticleArray[id].alive) {return;}
  window.draw(*GlobalParticleArray[id].particle->object.sprite);
}

// dt is in seconds
void updateParticle(uint32_t id, float dt) {
  ParticleSlot& Slot = GlobalParticleArray[id];
  std::unique_ptr<Particle>& ParticleInstance = Slot.particle;
  
  if (!Slot.alive) {return;}

  ParticleInstance->Elapsed += dt;

  if (ParticleInstance->Elapsed >= ParticleInstance->Lifetime) {
    deallocateParticle(id);
    return;
  }

  float NormalizedTime = ParticleInstance->Elapsed / ParticleInstance->Lifetime;
  NormalizedTime = std::clamp(NormalizedTime, 0.f, 1.f);

  // Apply Acceleration
  ParticleInstance->Velocity.x += ParticleInstance->Acceleration.x * dt;
  ParticleInstance->Velocity.y += ParticleInstance->Acceleration.y * dt;

  // Apply Velocity
  // Here we apply velocity not in the Global X and Y, but in the relative X and Y of the particle
  // The Relative X and Y is given by the ParticleInstance's Direction Value
  float DirectionDegrees = ParticleInstance->Direction.asRadians();

  float ConvertedXVelocity = cos(DirectionDegrees) * ParticleInstance->Velocity.x - sin(DirectionDegrees) * ParticleInstance->Velocity.y;
  float ConvertedYVelocity = sin(DirectionDegrees) * ParticleInstance->Velocity.x + cos(DirectionDegrees) * ParticleInstance->Velocity.y;
  
  ParticleInstance->object.sprite->setPosition(
    {
      ParticleInstance->object.sprite->getPosition().x + ConvertedXVelocity * dt,
      ParticleInstance->object.sprite->getPosition().y + ConvertedYVelocity * dt
    }
  );
  
  // Apply Color
  // Getting the points
  
  auto ColorLoopResult = returnNeighborPointsInArray(ParticleInstance->ColorArray,NormalizedTime);
                                
  // ColorPointA is the point just before the Elapsed time, and B is the just after one
  auto& ColorPointA = ColorLoopResult.first;
  auto& ColorPointB = ColorLoopResult.second;
    
  uint8_t NewColorR = lerpColor(static_cast<float>(ColorPointA.second.r),static_cast<float>(ColorPointB.second.r), NormalizedTime);
  uint8_t NewColorG = lerpColor(static_cast<float>(ColorPointA.second.g),static_cast<float>(ColorPointB.second.g), NormalizedTime);
  uint8_t NewColorB = lerpColor(static_cast<float>(ColorPointA.second.b),static_cast<float>(ColorPointB.second.b), NormalizedTime);
  uint8_t NewColorA = lerpColor(static_cast<float>(ColorPointA.second.a),static_cast<float>(ColorPointB.second.a), NormalizedTime);

  sf::Color NewColor({NewColorR, NewColorG, NewColorB, NewColorA});
  ParticleInstance->object.sprite->setColor(NewColor);

  // Apply Rotation
  // Getting the points
  auto RotationLoopResult = returnNeighborPointsInArray(ParticleInstance->RotationArray, NormalizedTime);

  auto& RotationPointA = RotationLoopResult.first;
  auto& RotationPointB = RotationLoopResult.second;

  sf::Angle NewAngle = lerpRotation(RotationPointA.second, RotationPointB.second, NormalizedTime);

  ParticleInstance->object.sprite->setRotation(NewAngle);
  
  // Apply Scale
  
}

