// StateStack.cpp
#include "state_stack.h"

void StateStack::pushState(std::unique_ptr<State> state) {
    mStack.push(std::move(state));
}

void StateStack::popState() {
    if (!mStack.empty())
        mStack.pop();
}

State* StateStack::getCurrentState() {
    return mStack.empty() ? nullptr : mStack.top().get();
}

void StateStack::handleEvent(const sf::Event& event) {
    if (auto* state = getCurrentState())
        state->handleEvent(event);
}

void StateStack::update(sf::Time dt) {
    if (auto* state = getCurrentState())
        state->update(dt);
}

void StateStack::render() {
    if (auto* state = getCurrentState())
        state->render();
}
