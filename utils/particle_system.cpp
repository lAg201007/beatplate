#include "../utils/particle_system.hpp"

std::vector<ParticleSlot> GlobalParticleArray;
std::vector<uint32_t> FreeSlots;

uint32_t alocateParticle(Particle& newParticle) {
  if (!FreeSlots.empty()) {
    int id = FreeSlots.back();
    FreeSlots.pop_back();

    ParticleSlot newSlot(std::make_unique<Particle>(newParticle),true);
    GlobalParticleArray[id] = std::move(newSlot);
    return id;
  }

  ParticleSlot newSlot(std::make_unique<Particle>(newParticle), true);
  GlobalParticleArray.push_back(std::move(newSlot));
  return GlobalParticleArray.size();
}

void dealocateParticle(uint32_t id) {
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
    dealocateParticle(id);
    return;
  }

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
  std::pair<float, sf::Color> ColorPointA;
  std::pair<float, sf::Color> ColorPointB;

  for (std::pair<float, sf::Color> ColorPoint : ParticleInstance->ColorArray) {
    
  }

  // Apply Rotation

  // Apply Scale
  
}

