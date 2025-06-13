#include <SFML/Window/Mouse.hpp>
#include "song_select.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"

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
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                mStack.popState();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                List.scrollListUpByOne();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                List.scrollListDownByOne();
            }
        }
    }

    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
}

void SongSelect::update(sf::Time dt) {
    List.updateSlotTweens(dt.asSeconds());
}

void SongSelect::render() {   
    mWindow.clear(sf::Color::Transparent);
    List.RenderList(mWindow);
}
