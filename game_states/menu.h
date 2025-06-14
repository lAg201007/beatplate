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
    sf::Vector2i mouse_pos;
    sf::Shader blurShader;
    Tween TitleTween;
    Tween TitleTransparencyTween;
    ValueTween StartTextTransparencyTween;
    sf::Vector2f TitlePosition;
    sf::Font Arial;
    sf::Text StartGameText;
    sf::Color textColor;
    float textTransparencyValue;

};
