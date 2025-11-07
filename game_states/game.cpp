#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../libs/json.hpp"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include "../utils/audio_manager.h"
#include "../utils/SFML_CLASSES.h"
#include <fstream>
#include <print>

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background)
    : State(stack, window),
      Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f),
      songFolder(songFolder),
      background(background),
      gameClock(startTime_ms, endTime_ms)
{
    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    startTime_ms = data["metadata"]["StartsAt"].get<int>();
    endTime_ms = data["metadata"]["EndsAt"].get<int>();

    AudioManager::getInstance().pauseMusic();   
    ResizeSpriteToFitWindow(*background.sprite, window);

    std::ifstream configFile("config.json");
    nlohmann::json config;
    configFile >> config;

    offset_ms = config["settings"]["music_offset_ms"].get<int>();

    auto bindArray = config["settings"]["binds"]["game_click"].get<std::vector<std::string>>();
    std::pair<std::string, std::string> binds = {bindArray[0], bindArray[1]};

    for (auto& note : data["notes"]) {
        if (note["type"] == "plate") {
            int offset = note["offset"].get<int>();
            int xPos = note["initialXPos"].get<int>();
            int finalXPos = note["finalXPos"].get<int>();
            int plateNumber = note["PlateNumber"].get<int>();
            float velocity = note["Vel"].get<float>();

            std::unique_ptr<Plate> newPlate = std::make_unique<Plate>(
                offset,
                binds,
                xPos,
                0,
                data["metadata"]["yPos"].get<int>(),
                finalXPos,
                plateNumber,
                data["metadata"]["PS"].get<int>(),
                data["metadata"]["ACD"].get<int>(),
                data["metadata"]["AR"].get<float>(),
                velocity
            );

            notes.push_back(std::move(newPlate));
        }
    }

    background = ShaderUtils::applyBlurToObject(window, background, background.blurredStrength);

    AudioManager::getInstance().playMusic(songFolder + "/song.mp3", false, nullptr);
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                if (paused) {
                    resume();
                } else {
                    pause();
                }
            }

            // Debug key to rewind time by 5 seconds
            if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                gameClock.setTime(gameClock.getTime() - sf::milliseconds(5000), AudioManager::getInstance(), offset_ms);
            }
        }
    }
}

void Game::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),300});
    gameClock.update(dt, AudioManager::getInstance());

    for (auto& note : notes) {
        note->update(gameClock.getTime(), mWindow);
    }

    //std::println("Clock Time: {} ms", gameClock.getTime().asMilliseconds());
    //std::println("Music Time: {} ms", AudioManager::getInstance().getCurrentTime().asMilliseconds());
}

void Game::render() {   
    mWindow.draw(*background.sprite);

    for (auto& note : notes) {
        note->render(mWindow);
    }

    mWindow.draw(*Cursor.sprite);
}
