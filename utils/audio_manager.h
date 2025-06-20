#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <string>
class SongSlot; // forward declaration

class AudioManager {
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

    // Retorna o caminho do arquivo da música atual
    const std::string& getCurrentMusicPath() const {
        return currentPath;
    }

    // Retorna o slot atual
    std::shared_ptr<SongSlot> getCurrentSlot() const {
        return currentSlot;
    }

private:
    AudioManager() = default;
    sf::Music music;
    std::string currentPath; // Caminho da música atual
    std::shared_ptr<SongSlot> currentSlot; // agora é shared_ptr
};