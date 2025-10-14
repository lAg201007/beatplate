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

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background)
    : State(stack, window),
      Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f),
      songFolder(songFolder),
      background(background) 
{
    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    AudioManager::getInstance().pauseMusic();   
    ResizeSpriteToFitWindow(*background.sprite, window);

    std::ifstream configFile("config.json");
    nlohmann::json config;
    configFile >> config;

    background = ShaderUtils::applyBlurToObject(window, background, background.blurredStrength);
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
}

void Game::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),300});
}

void Game::render() {   
    mWindow.draw(*background.sprite);
    mWindow.draw(*Cursor.sprite);
}
