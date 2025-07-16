#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../libs/json.hpp"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include "../game_logic/note_objects/plate.h"
#include "../utils/audio_manager.h"
#include <fstream>

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background)
    : State(stack, window),
      Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f),
      songFolder(songFolder),
      background(background) 
{
    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    for (auto& [key, value] : data["notes"].items()) {
        if (value["type"] == "plate") {
                notes.push_back(std::make_shared<Plate>(mWindow,
                static_cast<int>(value["offset"]),
                static_cast<int>(value["xPos"]),
                static_cast<int>(data["metadata"]["AR"]),
                static_cast<int>(data["metadata"]["ACD"]),
                static_cast<int>(data["metadata"]["PS"])
                )
            );
        }
    }

    AudioManager::getInstance().pauseMusic();   
    ResizeSpriteToFitWindow(*background.sprite, mWindow);
}

namespace {
    float getHitScore(HitResult hit) {
        switch (hit) {
            case HitResult::Perfect:       return 1.0f;
            case HitResult::PerfectEarly:
            case HitResult::PerfectLate:   return 0.8f;
            case HitResult::TooEarly:
            case HitResult::TooLate:       return 0.5f;
            default:                       return 0.0f; // Miss, etc.
        }
    }
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
}

void Game::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),300});

    if (!started) {
        startDelay -= dt.asSeconds();
        if (startDelay <= 0.f) {
            started = true;
            AudioManager::getInstance().playMusic(songFolder + "/song.mp3", false, nullptr);
        }
        return;
    }

    elapsedTime += dt.asMilliseconds();  

    for (auto& note : notes) {
        note->update(elapsedTime, dt.asSeconds()); 
    }

    totalScore = 0.0f;
    totalProcessed = 0;

    for (const auto& note : notes) {
        if (note->state == NoteState::Hit || note->state == NoteState::Missed) {
            totalScore += getHitScore(note->hitResult);
            totalProcessed++;
        }
    }
  
    float accuracy = (totalProcessed > 0) ? (100.0f * totalScore / totalProcessed) : 100.0f;
}

void Game::render() {   
    //ShaderUtils::drawVerticalBlurSprite(mWindow, *background.sprite, background.blurredStrength);
    ShaderUtils::drawShaderCompound(mWindow, ShaderUtils::createVerticalBlurCompound(mWindow, *background.sprite, background.blurredStrength));
    for (const auto& note : notes) {
        note->render(mWindow);
    }
    mWindow.draw(*Cursor.sprite);
}
