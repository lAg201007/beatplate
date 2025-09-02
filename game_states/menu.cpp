    #include "menu.h"
    #include "song_select.h"
    #include "../state_stack.h"        
    #include <SFML/Window/Mouse.hpp>
    #include "../utils/tween_service.h"
    #include "../shaders/shader_manager.h"
    #include "../utils/utilities.h"
    #include "../utils/tween_storage.h"
    #include "../utils/audio_manager.h"
    #include <filesystem>
    #include <fstream>
    #include "../libs/json.hpp"
    #include "print"

    //TODO: FAZER O TWEEN BRANCO NO TITLE QUANDO DA O BEAT, O TWEEN BRANCO NO TITLE QUANDO CLICA
    // E O TWEEN DE HOVER NO SONG SLOT

    int MainMenu::ActualMusicBpm = 0;

    MainMenu::MainMenu(StateStack& stack, sf::RenderWindow& window)
        : State(stack, window),
        Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
        Title("assets/sprites/main_menu/title.png", 0, 0, 640, 360), // x e y serão definidos depois
        background("assets/sprites/main_menu/background.png",0,0),
        start_buffer("assets/sounds/menu/start.wav"),
        circle_beat_buffer("assets/sounds/menu/circle_beat.wav"),
        start_sound(start_buffer),

        TitleTween(std::make_unique<Tween>(*Title.sprite,0.5f,Tween::linear)),
        TitleTransparencyTween(*Title.sprite,3.0f,Tween::linear),
        StartTextTransparencyTween(0.0f,1.0f,3.0f),
        StartGameText(Arial),
        textColor(StartGameText.getFillColor())
    {
        start_sound.setVolume(130.f);
        Title.sprite->setPosition({window.getSize().x / 2.f, window.getSize().y / 2.f});
        TitlePosition = Title.sprite->getPosition();

        TitleTween->initScale(1.0f,1.1f);
        TitleTransparencyTween.initTransparency(0.0f,1.0f);
        TitleTransparencyTween.play();

        if (!Arial.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
            std::cerr << "não foi possivel carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
        }

        StartGameText.setString("Press the Title Button to Play");
        StartGameText.setCharacterSize(20);
        StartGameText.setPosition(TitlePosition + sf::Vector2f({-150,80}));

        ResizeSpriteToFitWindow(*background.sprite, window);

        taskDelay(3500, [this]() {
            StartTextTransparencyTween.play();
        });

        background = ShaderUtils::applyBlurToObject(mWindow, background, 2.f);
    }

    void MainMenu::handleEvent(const sf::Event& event) {
        if (event.is<sf::Event::KeyPressed>()) {
            if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    mWindow.close();
                }
            }
        }

        if (event.is<sf::Event::Closed>()) {
            mWindow.close();
        }

    }

    void MainMenu::update(sf::Time dt) {
        mouse_pos = sf::Mouse::getPosition(mWindow);
        Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});

        textTransparencyValue = StartTextTransparencyTween.getValue();
        textColor.a = static_cast<int>(std::round(textTransparencyValue * 255.0f));
        StartGameText.setFillColor(textColor);

        if (TitleTween) {
            TitleTween->update(dt.asSeconds());
            if (!TitleTween->isActive()) {
                TitleTween->reset();
                TitleTween->play();
                AudioManager::getInstance().playTemporarySound(circle_beat_buffer, 10.f);
            }
        }

        TitleTransparencyTween.update(dt.asSeconds());
        StartTextTransparencyTween.update(dt.asSeconds());

        if (Title.DetectButtonClick(mWindow)) {
            start_sound.play();
            mStack.pushState(std::make_unique<SongSelect>(mStack,mWindow)); 
        } 

        // Detecta se tem música tocando
        if (AudioManager::getInstance().isPlaying()) {
            std::string musicPath = AudioManager::getInstance().getCurrentMusicPath();
            if (!musicPath.empty() && musicPath != lastCheckedMusicPath) {
                lastCheckedMusicPath = musicPath;
                std::filesystem::path path(musicPath);
                std::string folder = path.parent_path().string();

                // Tenta abrir map.json, se não existir tenta data.json
                std::ifstream jsonFile(folder + "/map.json");
                if (jsonFile) {
                    nlohmann::json data;
                    jsonFile >> data;
                    int bpm = data["metadata"].value("BPM", 0); // 0 se não existir

                    if (bpm != ActualMusicBpm) {
                        ActualMusicBpm = bpm;
                        float tweenTime = (bpm > 0) ? (60.0f / bpm) : 0.5f;
                        TitleTween = std::make_unique<Tween>(*Title.sprite, tweenTime, Tween::linear);
                        TitleTween->initScale(1.0f, 1.1f);
                        TitleTween->play();
                    }
                }
            }
        } else {
            // Se não tem música, volta para padrão
            if (ActualMusicBpm != 0) {
                ActualMusicBpm = 0;
                TitleTween = std::make_unique<Tween>(*Title.sprite, 0.5f, Tween::linear);
                TitleTween->initScale(1.0f, 1.1f);
                TitleTween->play();
            }
        }
    }

    void MainMenu::render() {  
        mWindow.clear(sf::Color::Transparent);

        mWindow.draw(*background.sprite);
        mWindow.draw(*Title.sprite);
        mWindow.draw(StartGameText);
        mWindow.draw(*Cursor.sprite);
    }
