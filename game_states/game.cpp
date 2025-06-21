#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../libs/json.hpp"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include "../game_logic/note_objects/plate.h"
#include <fstream>

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder)
    : State(stack, window)
{
    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    for (auto& [key, value] : data["notes"].items()) {
        if (value["type"] == "plate") {
            notes.push_back(std::make_shared<Plate>(static_cast<int>(value["offset"]), static_cast<int>(value["xPos"]),static_cast<float>(data["metadata"]["AR"])));
        }
    }
}

void Game::handleEvent(const sf::Event& event) {

}

void Game::update(sf::Time dt) {
    elapsedTime += dt.asMilliseconds(); // agora é float, em segundos

    for (auto& note : notes) {
        note->update(elapsedTime);
    }
}

void Game::render() {   
    for (const auto& note : notes) {
        note->render(mWindow);
    }
}
