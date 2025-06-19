#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../game_logic/notes.h"

class Game : public State {
public:
    Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    std::vector<std::shared_ptr<Note>> notes;
};
