#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../libs/json.hpp"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include <fstream>

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder)
    : State(stack, window)
{
    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    for (auto& [key, value] : data["notes"].items()) {
        std::cout << key << ": " << value << "\n";  
    }
}

void Game::handleEvent(const sf::Event& event) {

}

void Game::update(sf::Time dt) {

}

void Game::render() {   

}
