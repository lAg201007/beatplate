#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../libs/json.hpp"
#include "../shaders/shader_manager.h"
#include "../utils/tween_storage.h"
#include "../utils/utilities.h"
#include "../utils/audio_manager.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <cstdint>
#include <thread>
#include <future>

// Data structure for loading slots
struct SongSlotData {
    std::string SongName;
    std::string Artist;
    std::string Mapper;
    int Difficulty;
    std::string FolderLocation;
};

// Useful Functions
namespace {
    SongSlotData load_and_return_data(std::string data_folder) {
        std::ifstream dataFile(data_folder + "/data.json");
        if (!dataFile.is_open()) {
            std::cerr << "Não foi possível abrir " << data_folder << "/data.json" << std::endl;
            return SongSlotData{};
        }
        nlohmann::json jsonData;
        dataFile >> jsonData;

        SongSlotData data;
        data.FolderLocation = data_folder;
        data.SongName = jsonData.value("SongName", "");
        data.Artist = jsonData.value("Artist", "");
        data.Mapper = jsonData.value("Mapper", "");
        data.Difficulty = jsonData.value("Difficulty", 0);

        return data;
    }

    std::vector<SongSlotData> load_song_slot_data(std::string path) {
        std::vector<SongSlotData> slotDataList;
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_directory()) {
                std::string folder = entry.path().string();

                std::future<SongSlotData> data = std::async(std::launch::async, load_and_return_data, folder);
                
                slotDataList.push_back(data.get());
            }
        }
        return slotDataList;
    }

    std::vector<std::shared_ptr<SongSlot>> createSongSlots(const std::vector<SongSlotData>& slotDataList, sf::Vector2f list_position) {
        std::vector<std::shared_ptr<SongSlot>> slots;
        for (const auto& data : slotDataList) {
            auto slot = std::make_shared<SongSlot>(list_position, data);
            slots.push_back(slot);
        }
        return slots;  
    }
}

// Forward declaration
class SongSelect; 
class SongList;

class SongSlot : public std::enable_shared_from_this<SongSlot> {
public:
    static sf::Font Montserrat;

    std::string SongName;
    std::string Artist;
    std::string Mapper;
    std::string FolderLocation;
    int Difficulty;

    sf::Text SongNameLabel;
    sf::Text ArtistLabel;
    sf::Text MapperLabel;
    sf::Text DificultyLabel;
    Button SongButton;

    sf::Color originalColor = sf::Color::White;

    ValueTween PositionTweenX;
    ValueTween PositionTweenY;
    ValueTween SelectedOffsetTween;
    ValueTween WhiteIntensityTween;
    float whiteIntensity = 0.f;

    sf::Vector2f Position;

    SongSlot(sf::Vector2f startPos, const SongSlotData& data)
     : 
    SongNameLabel(Montserrat),
    ArtistLabel(Montserrat), 
    MapperLabel(Montserrat), 
    DificultyLabel(Montserrat), 
    SongButton("assets/sprites/song_select/song_select_button.png", startPos.x, startPos.y, 411, 130, 0.50f, 0.25f),
    PositionTweenX(startPos.x, startPos.x, 1.0f, Tween::easeOutQuad),
    PositionTweenY(startPos.y, startPos.y, 1.0f, Tween::easeOutQuad),
    SelectedOffsetTween(0.f, 0.f, 1.0f, Tween::easeOutQuad),
    WhiteIntensityTween(255.f, 0.f, 0.3f, Tween::easeOutQuad),
    whiteIntensity(0.f)
    {
        FolderLocation = data.FolderLocation;
        SongName = data.SongName;
        Artist = data.Artist;
        Mapper = data.Mapper;
        Difficulty = data.Difficulty;

        SongNameLabel.setString(SongName);
        SongNameLabel.setCharacterSize(20);

        ArtistLabel.setString(Artist);
        ArtistLabel.setCharacterSize(11);

        MapperLabel.setString(Mapper);
        MapperLabel.setCharacterSize(11);

        DificultyLabel.setString(std::to_string(Difficulty));
        DificultyLabel.setCharacterSize(30);

        originalColor = SongButton.sprite->getColor();

        WhiteIntensityTween.reset();

        SetButtonAndWidjetsRelativePosition(startPos);
    }

    void SetButtonAndWidjetsRelativePosition(sf::Vector2f newPos);
    void setPositionTweened(sf::Vector2f newPos);
    void update(float dt);
    void clicked(
        std::vector<std::shared_ptr<SongSlot>>& slots, 
        std::shared_ptr<SongSlot>& selectedSlot, 
        SongList& list, StateStack& mStack, 
        sf::RenderWindow& mWindow
    );
    void renderButton(sf::RenderWindow& window);
};

class SongList {
public:
    sf::Vector2f ListPosition;
    std::vector<std::shared_ptr<SongSlot>> ButtonVector;
    std::shared_ptr<SongSlot> SelectedSlot;
    sf::RenderWindow& window;
    Object select_slot_background1;
    Object select_slot_background2;

    ValueTween backgroundTransparencyTweenIn;
    ValueTween backgroundTransparencyTweenOut;
    bool isActiveBackground1 = true;

    int button_offset = 70;

    float BackgroundChangeCooldown = 0.2f;
    float BackgroundChangeTimer = 0.0f;
    bool backgroundChangePending = false;
    static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> BackgroundCache;

    SongSelect* parentSelect = nullptr;

    SongList(std::string path, sf::Vector2f list_position, sf::RenderWindow &mWindow, SongSelect* parent)
    : window(mWindow),
      parentSelect(parent),
      backgroundTransparencyTweenIn(0.0f, 1.0f, 0.5f),
      backgroundTransparencyTweenOut(1.0f, 0.0f, 0.5f),
      select_slot_background1("assets/sprites/main_menu/background.png",0,0),
      select_slot_background2("assets/sprites/main_menu/background.png",0,0)
    {
        ListPosition = list_position;
        select_slot_background1.blurredStrength = 2.0f;
        select_slot_background2.blurredStrength = 2.0f;
        try {
            // carrega os dados dos slots em outro thread
            std::vector<SongSlotData> slotDataList;
            std::future<std::vector<SongSlotData>> futureData = std::async(std::launch::async, load_song_slot_data, path);
            slotDataList = futureData.get();

            // cria os slots em outro thread
            std::future<std::vector<std::shared_ptr<SongSlot>>> futureSlots = std::async(std::launch::async, createSongSlots, slotDataList, list_position);
            ButtonVector = futureSlots.get();

            //seleciona o slot do meio como padrão
            if (!ButtonVector.empty()) {
                if (AudioManager::getInstance().isPlaying()) {
                    auto currentSlot = AudioManager::getInstance().getCurrentSlot();
                    auto it = std::find_if(ButtonVector.begin(), ButtonVector.end(),
                        [&](const std::shared_ptr<SongSlot>& slot) {
                            return currentSlot && slot->FolderLocation == currentSlot->FolderLocation;
                        });
                    if (it != ButtonVector.end()) {
                        SelectedSlot = *it;
                    } else {
                        SelectedSlot = ButtonVector[ButtonVector.size() / 2];
                    }
                } else {
                    SelectedSlot = ButtonVector[ButtonVector.size() / 2];
                }  
            }

            updateSlotPositions();
            
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }

        setBackgroundForSelectedSlot();
    }

    // Functions
    void listUpdate(float dt);
    void setBackgroundForSelectedSlot();    
    void updateSlotPositions();
    void updateSlotTweens(float dt);
    void scrollListUpByOne();
    void scrollListDownByOne();
    void selectSlotByIndex(int index);
    void ResizeSpriteToFitWindow(Object& obj, sf::RenderWindow& window);
    void RenderList(sf::RenderWindow& window);
};

class SongSelect : public State {
public:
    SongSelect(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;
    ~SongSelect() override;

    bool isActive = true;

    std::unique_ptr<SongList> List;
    Object Cursor;
    sf::Vector2i mouse_pos;
    float mouseScrollQueueCooldown = 0.0f;
    std::vector<int> pendingScrolls;
};
