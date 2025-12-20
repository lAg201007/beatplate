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
