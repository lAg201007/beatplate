// StateStack.h
#pragma once
#include "game_states/game_state.h"
#include <memory>
#include <stack>

struct SharedState {
    float musicTime = 0.f;
    std::string musicPath;
};

class StateStack {
public:
    void pushState(std::unique_ptr<State> state);
    void popState();

    State* getCurrentState();

    void handleEvent(const sf::Event& event);
    void update(sf::Time dt);
    void render();

    SharedState sharedState;

private:
    std::stack<std::unique_ptr<State>> mStack;
};
