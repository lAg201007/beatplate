
#include "song_select.h"
#include <SFML/Window/Mouse.hpp>
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"

sf::Font SongSlot::Montserrat;

SongSelect::SongSelect(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    List("assets/songs", {974,232},mWindow)
{
    if (!SongSlot::Montserrat.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
        std::cerr << "não foi possível carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
    }
}

void SongSelect::handleEvent(const sf::Event& event) {

}

void SongSelect::update(sf::Time dt) {
    
}

void SongSelect::render() {   
    mWindow.clear(sf::Color::Transparent);
    List.RenderList(mWindow);
}
