#pragma once
#include "game_state.h"
#include "../SFML_CLASSES.h"

class MainMenu : public State {
public:
    MainMenu(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    Object Cursor;
    sf::Vector2i mouse_pos;
};