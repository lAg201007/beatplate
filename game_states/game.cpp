#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"

Game::Game(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window)
{

}

void Game::handleEvent(const sf::Event& event) {

}

void Game::update(sf::Time dt) {

}

void Game::render() {   

}
