#pragma once
#include "game_state.h"
#include "../game_logic/clock.hpp"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../game_logic/notes/plate.hpp"

class Game : public State {
public:
    Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

    void pause() {
        gameClock.pause();
        AudioManager::getInstance().pauseMusic();
        paused = true; 
    };

    void resume() {
        gameClock.unpause();
        AudioManager::getInstance().resumeMusic();
        gameClock.syncMusicToClock(AudioManager::getInstance(), offset_ms);
        paused = false;
    }

private:
    Object Cursor;
    Object background;
    sf::Vector2i mouse_pos;
    std::string songFolder;
    GameClock gameClock;

    std::vector<std::unique_ptr<Note>> notes;

    bool paused = false;
    int offset_ms = 0;

    int startTime_ms = 0;
    int endTime_ms = 0;
};
