// StateStack.h
#pragma once
#include "game_states/game_state.h"
#include <memory>
#include <stack>

class StateStack {
public:
    void pushState(std::unique_ptr<State> state);
    void popState();

    State* getCurrentState();

    void handleEvent(const sf::Event& event);
    void update(sf::Time dt);
    void render();

private:
    std::stack<std::unique_ptr<State>> mStack;
};
