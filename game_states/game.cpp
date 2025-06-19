#include <SFML/Window/Mouse.hpp>
#include "song_select.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"

SongSelect::SongSelect(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window)
{

}

void SongSelect::handleEvent(const sf::Event& event) {

}

void SongSelect::update(sf::Time dt) {

}

void SongSelect::render() {   
    
}
