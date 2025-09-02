#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <string>
#include <vector>
#include <utility>
class SongSlot; // forward declaration

class AudioManager {
private:
    struct TempSound {
        std::unique_ptr<sf::SoundBuffer> buffer;
        std::unique_ptr<sf::Sound> sound;
        TempSound(const sf::SoundBuffer& buf) {
            buffer = std::make_unique<sf::SoundBuffer>(buf);
            sound = std::make_unique<sf::Sound>(*buffer);
        }
    };

public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    void playMusic(const std::string& filepath, bool loop = true, std::shared_ptr<SongSlot> slot = nullptr) {
        if (!music.openFromFile(filepath)) return;
        music.setLooping(loop);
        music.play();
        currentPath = filepath; // Salva o caminho da música atual
        currentSlot = slot;
    }

    void stopMusic() {
        music.stop();
    }

    void pauseMusic() {
        music.pause();
    }

    void resumeMusic() {
        music.play();
    }

    bool isPlaying() const {
        return music.getStatus() == sf::SoundSource::Status::Playing;
    }

    // Retorna o tempo atual da música
    float getCurrentTime() const {
        return music.getPlayingOffset().asSeconds();
    }

    // Retorna o caminho do arquivo da música atual
    const std::string& getCurrentMusicPath() const {
        return currentPath;
    }

    // Retorna o slot atual
    std::shared_ptr<SongSlot> getCurrentSlot() const {
        return currentSlot;
    }

    void playTemporarySound(const sf::SoundBuffer& buffer, float volume = 100.f) {
        tempSounds.emplace_back(buffer);
        tempSounds.back().sound->setVolume(volume);
        tempSounds.back().sound->play();
    }

    void updateTempSounds() {
        tempSounds.erase(
            std::remove_if(tempSounds.begin(), tempSounds.end(),
                [](const TempSound& temp) {
                    return temp.sound->getStatus() == sf::Sound::Status::Stopped;
                }
            ),
            tempSounds.end()
        );
    }

private:
    AudioManager() = default;
    sf::Music music;
    std::string currentPath; // Caminho da música atual
    std::shared_ptr<SongSlot> currentSlot; // agora é shared_ptr
    std::vector<TempSound> tempSounds;
};