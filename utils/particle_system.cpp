#include "../utils/particle_system.hpp"
#include <cmath>

std::vector<ParticleSlot> GlobalParticleArray;
std::vector<uint32_t> FreeSlots;

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
  ParticleInstance->Velocity.x += ParticleInstance->Acceleration.x;
  ParticleInstance->Velocity.y += ParticleInstance->Acceleration.y;

  // Apply Velocity
  ParticleInstance->object.sprite->setPosition(
    {
      ParticleInstance->object.sprite->getPosition().x + ParticleInstance->Velocity.x,
      ParticleInstance->object.sprite->getPosition().y + ParticleInstance->Velocity.y
    }
  );
  
  // Apply Color

  // ColorPointA is the point just before the Elapsed time, and B is the just after one
  std::pair<float, sf::Color> ColorPointA;
  std::pair<float, sf::Color> ColorPointB;

  // Getting the points
  for (std::pair<float, sf::Color> ColorPoint : ParticleInstance->ColorArray) {
    if (ColorPoint.first <= ParticleInstance->Elapsed) {
      ColorPointA = ColorPoint;
    }
    else {
      ColorPointB = ColorPoint;
      break;
    }
  }  

  uint8_t NewColorR = lerpColor(static_cast<float>(ColorPointA.second.r),static_cast<float>(ColorPointB.second.r), NormalizedTime);
  uint8_t NewColorG = lerpColor(static_cast<float>(ColorPointA.second.g),static_cast<float>(ColorPointB.second.g), NormalizedTime);
  uint8_t NewColorB = lerpColor(static_cast<float>(ColorPointA.second.b),static_cast<float>(ColorPointB.second.b), NormalizedTime);
  uint8_t NewColorA = lerpColor(static_cast<float>(ColorPointA.second.a),static_cast<float>(ColorPointB.second.a), NormalizedTime);

  sf::Color NewColor({NewColorR, NewColorG, NewColorB, NewColorA});
  ParticleInstance->object.sprite->setColor(NewColor);

  // Apply Rotation

  // Apply Scale
  
}

