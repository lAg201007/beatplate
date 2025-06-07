#pragma once
#include <SFML/Graphics.hpp>

class StateStack;

class State {
public:
    State(StateStack& stack, sf::RenderWindow& window)
        : mStack(stack), mWindow(window) {}

    virtual ~State() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(sf::Time dt) = 0;
    virtual void render() = 0;

protected:
    StateStack& mStack;
    sf::RenderWindow& mWindow;
};
