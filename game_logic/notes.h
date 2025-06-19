#pragma once
#include <string>
#include <SFML/Graphics.hpp>

enum class NoteState {
    Waiting,
    Active,
    Hit,
    Missed
};

enum class HitResult {
    Perfect,
    PerfectEarly,
    PerfectLate,
    TooEarly,
    TooLate,
    None
};

class Note {
public:
    int offset = 0;
    std::string type;
    NoteState state = NoteState::Waiting;
    HitResult hitResult = HitResult::None;

    Note(int offset, const std::string& type)
        : offset(offset), type(type) {}

    virtual ~Note() = default;

    virtual void update(float dt) {}
    virtual void start() {}
    virtual void render(sf::RenderWindow& window) {}
};