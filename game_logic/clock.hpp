#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include "utils/audio_manager.h"
#include <print>

class GameClock {
public:
    GameClock(int& startTime_ms, int& endTime_ms) : 
        paused(false), 
        currentTime(sf::Time::Zero), 
        startTime_ms(startTime_ms), 
        endTime_ms(endTime_ms) {}

    void update(sf::Time dt, AudioManager& audioManager) {
        if (!paused) {
            if (audioManager.isPlaying()) {
                // Clock segue o tempo real da música
                syncClockToMusic(audioManager);
            } else {
                // Clock avança manualmente (ex: pré-start)
                currentTime += dt;
            }
        }
    }

    void pause() {
        paused = true;
    }

    void unpause() {
        paused = false;
    }

    void reset(AudioManager& audioManager) {
        currentTime = sf::Time::Zero;
        audioManager.setCurrentTime(sf::Time::Zero);
    }


    void setTime(sf::Time time, AudioManager& audioManager, int offset_ms = 0) {
        int clamped = std::clamp(time.asMilliseconds(), startTime_ms, endTime_ms);
        currentTime = sf::milliseconds(clamped);
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

    int getStartTime() const {
        return startTime_ms;
    }

    int getEndTime() const {
        return endTime_ms;
    }

    bool isPaused() const { return paused; }

private:
    bool paused;
    int startTime_ms;
    int endTime_ms;
    sf::Time currentTime;
};
