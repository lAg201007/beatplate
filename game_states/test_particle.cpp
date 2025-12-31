#include "../utils/particle_system.hpp"
#include "test_particle.hpp"
#include "game_state.h"
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
  scaleArray({{1,1}}),
  spread(300),
  colorArray({ {0.f, {10,230,1,255}}, {1.f, {50, 120, 255, 0}} }),
  rotationArray({ {0.f, {sf::degrees(0)}  }, {1.f, {sf::degrees(360)}  } }),

  emitter(texturePath, lifetime, spread, particle_count, emitterInitialPos, initialVelocity, acceleration, scaleArray, colorArray, rotationArray)
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
    }
  }
}

void ParticleTest::update(sf::Time dt) {
  updateAllParticles(dt.asSeconds());
}

void ParticleTest::render() { 
  mWindow.clear(sf::Color::Black);

  renderAllParticles(mWindow);

  if (fontLoaded) {
    sf::Text debugText(debugFont);
    debugText.setCharacterSize(16);
    debugText.setFillColor(sf::Color::White);
    debugText.setOutlineColor(sf::Color::Black);
    debugText.setOutlineThickness(1.0f);
    
    float currentY = 10.0f; // Posição Y inicial para o texto

    // --- Parte 1: FreeSlots (uint32_t IDs) ---
    std::string freeSlotsInfo = "free slots: ";
    int freeSlotCounter = 0; // Contador de slots por linha

    for (uint32_t id : FreeSlots) {
        freeSlotsInfo += std::to_string(id);
        
        freeSlotCounter++;
        
        if (freeSlotCounter % 20 == 0) {
            // Nova linha a cada 20 slots
            freeSlotsInfo += "\n";
        } else {
            // Espaço para separar IDs na mesma linha
            freeSlotsInfo += " ";
        }
    }
    // Adiciona uma quebra de linha final se o último ID não completou o bloco de 20
    if (freeSlotCounter % 20 != 0 && freeSlotCounter > 0) {
        freeSlotsInfo += "\n";
    }

    debugText.setString(freeSlotsInfo);
    debugText.setPosition({10.0f, currentY}); // Posição X fixa, Y inicial
    mWindow.draw(debugText);
    
    // Atualiza a posição Y para a próxima seção de texto
    currentY += debugText.getGlobalBounds().size.y + 20.0f; 

    // --- Parte 2: GlobalParticleArray (Índices de partículas ativas) ---
    std::string aliveParticlesInfo = "alive particles: ";
    int aliveParticleCounter = 0; // Contador de partículas ativas por linha
    
    // Looping pelo GlobalParticleArray para obter os índices
    for (size_t i = 0; i < GlobalParticleArray.size(); ++i) {
        // Supondo que o tipo de partícula tenha o membro 'alive'
        if (GlobalParticleArray[i].alive) {
            // Usa o índice (i) como o ID da partícula
            aliveParticlesInfo += std::to_string(i);

            aliveParticleCounter++;
            
            if (aliveParticleCounter % 20 == 0) {
                // Nova linha a cada 20 IDs ativos
                aliveParticlesInfo += "\n";
            } else {
                // Espaço para separar IDs na mesma linha
                aliveParticlesInfo += " ";
            }
        }
    }
    // Adiciona uma quebra de linha final se o último ID não completou o bloco de 20
    if (aliveParticleCounter % 20 != 0 && aliveParticleCounter > 0) {
        aliveParticlesInfo += "\n";
    }

    debugText.setString(aliveParticlesInfo);
    debugText.setPosition({10.0f, currentY}); // Posição X fixa, Y atualizada
    mWindow.draw(debugText);
  }
}
