#include "../game_logic/clock.hpp"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../game_logic/clock.hpp"

#pragma once

enum class NoteState {
    Waiting,
    Active,
    Judging,
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

int getAproachRateMs(float AR) {
    if (AR < 5.f) {
        return 1200 + 120 * (5 - AR);
    }
    else if (AR == 5.f) {
        return 1200;
    }
    else {
        return 1200 - 150 * (AR - 5);
    }
}

int getAppearTimeMs(float AR, int hitTime) {
    return hitTime - getAproachRateMs(AR);
}

int getPerfectWindowMs(float ACD) {
    return 80 - 6 * ACD;
}

int getEarlyLateWindowMs(float ACD) {
    return 140 - 8 * ACD;
}

int getTooEarlyLateWindowMs(float ACD) {
    return 200 - 10 * ACD;
}

class Note {
public:
    Note(int offset, const std::string& type, int xPos = 0, float AR = 0.0f)
        : offset(offset), type(type), xPos(xPos), AR(AR) {}

    virtual ~Note() = default;

    virtual void update(sf::Time elapsed, sf::RenderWindow& window) {}
    virtual void render(sf::RenderWindow& window) {}

    void setState(NoteState newState) {
        state = newState;
    }

    void setHitResult(HitResult result) {
        hitResult = result;
    }

    void getHitResult() const {
        return hitResult;
    }

    void getState() const {
        return state;
    }
private:
    const int offset = 0;
    const float AR;
    const std::string type;

    int xPos = 0; 
    NoteState state = NoteState::Waiting;
    HitResult hitResult = HitResult::None;
};