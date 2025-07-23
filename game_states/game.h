#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../game_logic/notes.h"
#include "../shaders/shader_manager.h"

class Game : public State {
public:
    Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, ShaderObject& background);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    Object Cursor;
    std::vector<std::shared_ptr<Note>> notes;
    std::vector<ShaderCompound> backgroundCompounds;
    sf::Vector2i mouse_pos;
    float elapsedTime = 0.0f;
    float totalScore = 0.0f;
    int totalProcessed = 0;
    float startDelay = 2.0f; 
    bool started = false;
    std::string songFolder;
    ShaderObject background;
};
