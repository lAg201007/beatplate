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

uint32_t allocateParticle(Particle&& newParticle) {
    if (!FreeSlots.empty()) {
        uint32_t id = FreeSlots.back();
        FreeSlots.pop_back();
        
        GlobalParticleArray[id].particle = std::make_unique<Particle>(std::move(newParticle));
        GlobalParticleArray[id].alive = true;

        return id;
    }

    ParticleSlot newSlot(std::make_unique<Particle>(std::move(newParticle)), true);
    GlobalParticleArray.push_back(std::move(newSlot));
    return GlobalParticleArray.size() - 1;
}

void deallocateParticle(uint32_t id) {
  if (!GlobalParticleArray[id].alive) {return;}
  GlobalParticleArray[id].reset();
  FreeSlots.push_back(id);
}

uint8_t lerpColor(float ColorA, float ColorB, float t) {
  uint8_t newColor = static_cast<uint8_t>(std::lerp(ColorA, ColorB, t));
  return newColor;
}

sf::Angle lerpRotation(sf::Angle RotationA, sf::Angle RotationB, float t) {
  sf::Angle NewAngle = sf::degrees(std::clamp(std::lerp(RotationA.asDegrees(), RotationB.asDegrees(), t), 0.f, 360.f));
  return NewAngle;
}

sf::Vector2f lerpScale(sf::Vector2f ScaleA, sf::Vector2f ScaleB, float t) {
  sf::Vector2f newScale = {std::lerp(ScaleA.x, ScaleB.x, t), std::lerp(ScaleA.y, ScaleB.y, t)};
  return newScale;
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

  float SegmentDuration = ColorPointB.first - ColorPointA.first;
  float ElapsedInSegment = NormalizedTime - ColorPointA.first;
  float col_t_segment = (SegmentDuration > 0.0001f) ? (ElapsedInSegment / SegmentDuration) : 0.f; 
  col_t_segment = std::clamp(col_t_segment, 0.f, 1.f);
    
  uint8_t NewColorR = lerpColor(static_cast<float>(ColorPointA.second.r),static_cast<float>(ColorPointB.second.r), col_t_segment);
  uint8_t NewColorG = lerpColor(static_cast<float>(ColorPointA.second.g),static_cast<float>(ColorPointB.second.g), col_t_segment);
  uint8_t NewColorB = lerpColor(static_cast<float>(ColorPointA.second.b),static_cast<float>(ColorPointB.second.b), col_t_segment);
  uint8_t NewColorA = lerpColor(static_cast<float>(ColorPointA.second.a),static_cast<float>(ColorPointB.second.a), col_t_segment);

  sf::Color NewColor({NewColorR, NewColorG, NewColorB, NewColorA});
  ParticleInstance->object.sprite->setColor(NewColor);

  // Apply Rotation
  // Getting the points
  auto RotationLoopResult = returnNeighborPointsInArray(ParticleInstance->RotationArray, NormalizedTime);

  auto& RotationPointA = RotationLoopResult.first;
  auto& RotationPointB = RotationLoopResult.second;

  float RotSegmentDuration = RotationPointB.first - RotationPointA.first;
  float RotElapsedInSegment = NormalizedTime - RotationPointA.first;
  float rot_t_segment = (RotSegmentDuration > 0.0001f) ? (RotElapsedInSegment / RotSegmentDuration) : 0.f; 
  rot_t_segment = std::clamp(rot_t_segment, 0.f, 1.f);

  sf::Angle NewAngle = lerpRotation(RotationPointA.second, RotationPointB.second, rot_t_segment);

  ParticleInstance->object.sprite->setRotation(NewAngle);
  
  // Apply Scale
  auto ScaleLoopResult = returnNeighborPointsInArray(ParticleInstance->ScaleArray, NormalizedTime);

  auto& ScalePointA = ScaleLoopResult.first;
  auto& ScalePointB = ScaleLoopResult.second;

  float ScaleSegmentDuration = ScalePointB.first - ScalePointA.first;
  float ScaleElapsedInSegment = NormalizedTime - RotationPointA.first;
  float scale_t_segment = (ScaleSegmentDuration > 0.0001f) ? (ScaleElapsedInSegment / ScaleSegmentDuration) : 0.f; 
  scale_t_segment = std::clamp(scale_t_segment, 0.f, 1.f);

  sf::Vector2f newScale = lerpScale(ScalePointA.second, ScalePointB.second, scale_t_segment);

  ParticleInstance->object.sprite->setScale(newScale);
}

void updateAllParticles(float dt) {
  for (int id = 0; id < GlobalParticleArray.size(); id++) {
    if (!GlobalParticleArray[id].alive) {continue;}
    updateParticle(id, dt);
  }
}

void renderAllParticles(sf::RenderWindow& window) {
  for (int id = 0; id < GlobalParticleArray.size(); id++) {
    if (!GlobalParticleArray[id].alive) {continue;}
    drawParticle(id, window);
  }
}

