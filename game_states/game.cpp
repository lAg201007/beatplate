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
      gameClock(startTime_ms, endTime_ms),
      testPlate(3000, {"K", "L"}, 1000, -100, 300, 1, 8, 7.f)
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
    testPlate.update(gameClock.getTime(), mWindow);

    //std::println("Clock Time: {} ms", gameClock.getTime().asMilliseconds());
    //std::println("Music Time: {} ms", AudioManager::getInstance().getCurrentTime().asMilliseconds());
}

void Game::render() {   
    mWindow.draw(*background.sprite);
    mWindow.draw(*Cursor.sprite);
    testPlate.render(mWindow);
}
