#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include "utils/audio_manager.h"

class GameClock {
public:
    GameClock() : paused(false), currentTime(sf::Time::Zero) {}

    void update(sf::Time dt) {
        if (!paused)
            currentTime += dt;
    }

    void pause() {
        paused = true;
    }

    void unpause() {
        paused = false;
    }

    void setTime(sf::Time time, AudioManager& audioManager, int offset_ms = 0) {
        currentTime = (time.asMilliseconds() >= 0) ? time : sf::Time::Zero;
        syncMusicToClock(audioManager, offset_ms);
    }

    void syncMusicToClock(AudioManager& audioManager, int offset_ms = 0) {
        if (!audioManager.isPlaying())
            return;

        sf::Time musicTime = audioManager.getCurrentTime();
        sf::Time offset = sf::milliseconds(offset_ms);

        // Se já estiver sincronizado, não faz nada
        if (std::abs((currentTime - (musicTime - offset)).asMilliseconds()) < 1)
            return;

        audioManager.setCurrentTime(currentTime + offset);
    }

    void syncClockToMusic(AudioManager& audioManager, int offset_ms = 0) {
        if (!audioManager.isPlaying())
            return;

        sf::Time musicTime = audioManager.getCurrentTime();
        sf::Time offset = sf::milliseconds(offset_ms);

        // Se já estiver sincronizado, não faz nada
        if (std::abs(((musicTime - offset) - currentTime).asMilliseconds()) < 1)
            return;

        currentTime = musicTime - offset;
    }

    sf::Time getTime() const {
        return currentTime;
    }

private:
    bool paused;
    sf::Time currentTime;
};
