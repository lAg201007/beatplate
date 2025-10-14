#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"

class Game : public State {
public:
    Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    Object Cursor;
    sf::Vector2i mouse_pos;
    std::string songFolder;
    Object background;
};
