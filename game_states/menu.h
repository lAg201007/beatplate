#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"

class MainMenu : public State {
public:
    MainMenu(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    Object Cursor;
    Object background;
    Button Title;
    sf::SoundBuffer start_buffer;
    sf::SoundBuffer circle_beat_buffer;
    sf::Sound start_sound;
    sf::Vector2i mouse_pos;
    sf::Shader blurShader;
    std::unique_ptr<Tween> TitleTween; // <-- altere para ponteiro
    Tween TitleTransparencyTween;
    ValueTween StartTextTransparencyTween;
    ValueTween TitleWhiteMaskTween;
    sf::Vector2f TitlePosition;
    sf::Font Arial;
    sf::Text StartGameText;
    sf::Color textColor;
    float textTransparencyValue;
    static int ActualMusicBpm;
    std::string lastCheckedMusicPath;
};
