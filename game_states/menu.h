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
    int TitleBeatWhiteMultiplier;
    float textTransparencyValue;
    static int ActualMusicBpm;
    Object Cursor;
    Object background;
    Button Title;
    sf::SoundBuffer start_buffer;
    sf::SoundBuffer circle_beat_buffer;
    sf::Sound start_sound;
    sf::Vector2i mouse_pos;
    sf::Shader blurShader;
    sf::Shader whiteMaskShader;
    std::unique_ptr<Tween> TitleTween;
    std::unique_ptr<ValueTween> TitleWhiteMaskTween;
    Tween TitleTransparencyTween;
    ValueTween StartTextTransparencyTween;
    sf::Vector2f TitlePosition;
    sf::Font Arial;
    sf::Text StartGameText;
    sf::Color textColor;
    std::string lastCheckedMusicPath;
    bool isPendingTweenChange = false;
    bool isPendingStateChange = false;
};
