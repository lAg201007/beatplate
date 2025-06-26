#pragma once
#include <string>
#include <SFML/Graphics.hpp>

enum class NoteState {
    Waiting,
    Active,
    Hitting,
    Hit,
    Missed,
    Missing
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
    int xPos = 0; 
    const float AR;
    std::string type;
    NoteState state = NoteState::Waiting;
    HitResult hitResult = HitResult::None;

    Note(int offset, const std::string& type, int xPos = 0, float AR = 0.0f)
        : offset(offset), type(type), xPos(xPos), AR(AR) {}

    virtual ~Note() = default;

    virtual void update(float elapsed, float dt) {}
    virtual void start() {}
    virtual void render(sf::RenderWindow& window) {}
};