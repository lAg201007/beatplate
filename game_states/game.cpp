#include <SFML/Window/Mouse.hpp>
#include "game.h"
#include "../libs/json.hpp"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include "../utils/audio_manager.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/scale_manager.h"
#include <fstream>
#include <print>

// inicializando os sons do plate
sf::SoundBuffer bufhit1;
sf::SoundBuffer bufhit2;
sf::SoundBuffer bufhit3;
sf::Sound hit1(bufhit1);
sf::Sound hit2(bufhit2);
sf::Sound hit3(bufhit3);
// game.cpp ou arquivo onde a função initializeKeyMap está definida

// game.cpp ou arquivo onde a função initializeKeyMap está definida

std::map<std::string, sf::Keyboard::Scan> initializeKeyMap() {
    std::map<std::string, sf::Keyboard::Scan> keyMap;

    // 1. Letras (A-Z) - Mapeamento explícito
    keyMap["A"] = sf::Keyboard::Scancode::A;
    keyMap["B"] = sf::Keyboard::Scancode::B;
    keyMap["C"] = sf::Keyboard::Scancode::C;
    keyMap["D"] = sf::Keyboard::Scancode::D;
    keyMap["E"] = sf::Keyboard::Scancode::E;
    keyMap["F"] = sf::Keyboard::Scancode::F;
    keyMap["G"] = sf::Keyboard::Scancode::G;
    keyMap["H"] = sf::Keyboard::Scancode::H;
    keyMap["I"] = sf::Keyboard::Scancode::I;
    keyMap["J"] = sf::Keyboard::Scancode::J;
    keyMap["K"] = sf::Keyboard::Scancode::K;
    keyMap["L"] = sf::Keyboard::Scancode::L;
    keyMap["M"] = sf::Keyboard::Scancode::M;
    keyMap["N"] = sf::Keyboard::Scancode::N;
    keyMap["O"] = sf::Keyboard::Scancode::O;
    keyMap["P"] = sf::Keyboard::Scancode::P;
    keyMap["Q"] = sf::Keyboard::Scancode::Q;
    keyMap["R"] = sf::Keyboard::Scancode::R;
    keyMap["S"] = sf::Keyboard::Scancode::S;
    keyMap["T"] = sf::Keyboard::Scancode::T;
    keyMap["U"] = sf::Keyboard::Scancode::U;
    keyMap["V"] = sf::Keyboard::Scancode::V;
    keyMap["W"] = sf::Keyboard::Scancode::W;
    keyMap["X"] = sf::Keyboard::Scancode::X;
    keyMap["Y"] = sf::Keyboard::Scancode::Y;
    keyMap["Z"] = sf::Keyboard::Scancode::Z;

    // 2. Números (0-9)
    keyMap["0"] = sf::Keyboard::Scancode::Num0;
    keyMap["1"] = sf::Keyboard::Scancode::Num1;
    keyMap["2"] = sf::Keyboard::Scancode::Num2;
    keyMap["3"] = sf::Keyboard::Scancode::Num3;
    keyMap["4"] = sf::Keyboard::Scancode::Num4;
    keyMap["5"] = sf::Keyboard::Scancode::Num5;
    keyMap["6"] = sf::Keyboard::Scancode::Num6;
    keyMap["7"] = sf::Keyboard::Scancode::Num7;
    keyMap["8"] = sf::Keyboard::Scancode::Num8;
    keyMap["9"] = sf::Keyboard::Scancode::Num9;
    
    // 3. Teclado Numérico (Numpad)
    keyMap["Numpad0"] = sf::Keyboard::Scancode::Numpad0;
    keyMap["Numpad1"] = sf::Keyboard::Scancode::Numpad1;
    keyMap["Numpad2"] = sf::Keyboard::Scancode::Numpad2;
    keyMap["Numpad3"] = sf::Keyboard::Scancode::Numpad3;
    keyMap["Numpad4"] = sf::Keyboard::Scancode::Numpad4;
    keyMap["Numpad5"] = sf::Keyboard::Scancode::Numpad5;
    keyMap["Numpad6"] = sf::Keyboard::Scancode::Numpad6;
    keyMap["Numpad7"] = sf::Keyboard::Scancode::Numpad7;
    keyMap["Numpad8"] = sf::Keyboard::Scancode::Numpad8;
    keyMap["Numpad9"] = sf::Keyboard::Scancode::Numpad9;

    // 4. Teclas de Função (F1 - F15)
    keyMap["F1"] = sf::Keyboard::Scancode::F1;
    keyMap["F2"] = sf::Keyboard::Scancode::F2;
    keyMap["F3"] = sf::Keyboard::Scancode::F3;
    keyMap["F4"] = sf::Keyboard::Scancode::F4;
    keyMap["F5"] = sf::Keyboard::Scancode::F5;
    keyMap["F6"] = sf::Keyboard::Scancode::F6;
    keyMap["F7"] = sf::Keyboard::Scancode::F7;
    keyMap["F8"] = sf::Keyboard::Scancode::F8;
    keyMap["F9"] = sf::Keyboard::Scancode::F9;
    keyMap["F10"] = sf::Keyboard::Scancode::F10;
    keyMap["F11"] = sf::Keyboard::Scancode::F11;
    keyMap["F12"] = sf::Keyboard::Scancode::F12;
    keyMap["F13"] = sf::Keyboard::Scancode::F13;
    keyMap["F14"] = sf::Keyboard::Scancode::F14;
    keyMap["F15"] = sf::Keyboard::Scancode::F15;


    // 5. Teclas de Pontuação e Símbolos Comuns
    keyMap["Slash"] = sf::Keyboard::Scancode::Slash;
    keyMap["Equal"] = sf::Keyboard::Scancode::Equal;
    keyMap["LBracket"] = sf::Keyboard::Scancode::LBracket;
    keyMap["RBracket"] = sf::Keyboard::Scancode::RBracket;
    keyMap["Comma"] = sf::Keyboard::Scancode::Comma;
    keyMap["Period"] = sf::Keyboard::Scancode::Period;

    // 6. Teclas Especiais (Membros corrigidos)
    keyMap["Escape"] = sf::Keyboard::Scancode::Escape;
    keyMap["Enter"] = sf::Keyboard::Scancode::Enter;
    keyMap["Space"] = sf::Keyboard::Scancode::Space;
    keyMap["Up"] = sf::Keyboard::Scancode::Up;
    keyMap["Down"] = sf::Keyboard::Scancode::Down;
    keyMap["Left"] = sf::Keyboard::Scancode::Left;
    keyMap["Right"] = sf::Keyboard::Scancode::Right;
    keyMap["LControl"] = sf::Keyboard::Scancode::LControl;
    keyMap["RControl"] = sf::Keyboard::Scancode::RControl;
    keyMap["LShift"] = sf::Keyboard::Scancode::LShift;
    keyMap["RShift"] = sf::Keyboard::Scancode::RShift;
    keyMap["LAlt"] = sf::Keyboard::Scancode::LAlt;
    keyMap["RAlt"] = sf::Keyboard::Scancode::RAlt;
    keyMap["Tab"] = sf::Keyboard::Scancode::Tab;
    keyMap["Backspace"] = sf::Keyboard::Scancode::Backspace;
    keyMap["Delete"] = sf::Keyboard::Scancode::Delete;
    keyMap["Insert"] = sf::Keyboard::Scancode::Insert;
    keyMap["Unknown"] = sf::Keyboard::Scancode::Unknown; // Necessário para stringToKeycode

    return keyMap;
}

// Função para converter string para keycode
sf::Keyboard::Scan stringToKeycode(const std::string& keyString) {
    // Declara o mapa como static const para inicializá-lo apenas uma vez
    static const std::map<std::string, sf::Keyboard::Scan> keyMap = initializeKeyMap();

    // Cria uma cópia da string para conversão para maiúsculas/uppercase para garantir
    // que "a" e "A" retornem a mesma tecla.    
    std::string upperKeyString = keyString;
    std::transform(upperKeyString.begin(), upperKeyString.end(), upperKeyString.begin(), ::toupper);

    auto it = keyMap.find(upperKeyString);
    if (it != keyMap.end()) {
        return it->second;
    } else {
        // Trata strings desconhecidas
        return sf::Keyboard::Scancode::Unknown;
    }
}

Game::Game(StateStack& stack, sf::RenderWindow& window, const std::string& songFolder, Object& background)
    : State(stack, window),
      Cursor("assets/sprites/CursorInGame.png", 400, 300, 67, 67),
      songFolder(songFolder),
      background(background),
      gameClock(startTime_ms, endTime_ms)
{
    loadHitSounds();

    std::ifstream dataFile(songFolder + "/map.json");
    nlohmann::json data;
    dataFile >> data;

    startTime_ms = data["metadata"]["StartsAt"].get<int>();
    endTime_ms = data["metadata"]["EndsAt"].get<int>();

    AudioManager::getInstance().pauseMusic();   
    ResizeSpriteToFitWindow(*background.sprite, window);

    std::ifstream configFile("config.json");
    nlohmann::json config;
    configFile >> config;

    offset_ms = config["settings"]["music_offset_ms"].get<int>();
    bool debug_mode = config["settings"]["debug_mode"].get<bool>();
    std::println("DEBUG: DEBUG_MODE IS: {}", debug_mode);

    auto bindArray = config["settings"]["binds"]["game_click"].get<std::vector<std::string>>();
    binds = std::make_pair(stringToKeycode(bindArray[0]), stringToKeycode(bindArray[1]));

    float trajectoryDotMaxTransparency = config["settings"]["trajectory_dot_max_transparency"].get<float>();

    for (auto& note : data["notes"]) {
        if (note["type"] == "plate") {
            int offset = note["offset"].get<int>();
            int xPos = note["initialXPos"].get<int>();
            int finalXPos = note["finalXPos"].get<int>();
            int plateNumber = note["PlateNumber"].get<int>();
            int finalYPos = note["finalYPos"].get<int>();
            int hitNum = note["hitsound"].get<int>();

            std::unique_ptr<Plate> newPlate = std::make_unique<Plate>(
                offset,
                trajectoryDotMaxTransparency,
                xPos,
                window.getSize().y + 100,
                finalYPos,
                finalXPos,
                plateNumber,
                hitNum,
                data["metadata"]["PS"].get<int>(),
                data["metadata"]["ACD"].get<int>(),
                data["metadata"]["AR"].get<float>(),
                debug_mode
            );

            notes.push_back(std::move(newPlate));
        }
    }

    background = ShaderUtils::applyBlurToObject(window, background, background.blurredStrength);
    backgroundShader = ShaderUtils::createDarkOutShader(config["settings"]["background_dark_intensity"].get<float>(), window);

    AudioManager::getInstance().playMusic(songFolder + "/song.mp3", false, nullptr);
}

void Game::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                if (paused) {
                    resume();
                } else {
                    pause();
                }
            }

            // Debug key to rewind time by 5 seconds
            if (keyPressed->scancode == sf::Keyboard::Scancode::R) {
                gameClock.setTime(gameClock.getTime() - sf::milliseconds(5000), AudioManager::getInstance(), offset_ms);
            }

            if (keyPressed->scancode == binds.first || keyPressed->scancode == binds.second) {
                for (auto& note : notes) {
                    note->click(gameClock.getTime(), mWindow);
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x) / ScaleManager::GetScaleX(),static_cast<float>(mouse_pos.y) / ScaleManager::GetScaleY()});
    gameClock.update(dt, AudioManager::getInstance());

    for (auto& note : notes) {
        note->update(gameClock.getTime(), mWindow);
    }

    //std::println("Clock Time: {} ms", gameClock.getTime().asMilliseconds());
    //std::println("Music Time: {} ms", AudioManager::getInstance().getCurrentTime().asMilliseconds());
}

void Game::render() {   
    mWindow.draw(*background.sprite, &backgroundShader);

    for (auto& note : notes) {
        note->render(mWindow);
    }

    mWindow.draw(*Cursor.sprite);
}
