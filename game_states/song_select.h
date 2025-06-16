#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../libs/json.hpp"
#include "../shaders/shader_manager.h"
#include "../utils/tween_storage.h"
#include "../utils/utilities.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <cstdint>

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

    SongSlot(std::string SongFolder, sf::Vector2f startPos)
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
        FolderLocation = SongFolder;
        std::ifstream dataFile(FolderLocation + "/data.json");
        nlohmann::json data;
        dataFile >> data;

        SongName = data["SongName"];
        Artist = data["Artist"];
        Mapper = data["Mapper"];
        Difficulty = data["Difficulty"];

        SongNameLabel.setString(SongName);
        SongNameLabel.setCharacterSize(20);

        ArtistLabel.setString(Artist);
        ArtistLabel.setCharacterSize(11);

        MapperLabel.setString(Mapper);
        MapperLabel.setCharacterSize(11);

        DificultyLabel.setString(std::to_string(Difficulty));
        DificultyLabel.setCharacterSize(30);

        originalColor = SongButton.sprite->getColor();

        SetButtonAndWidjetsRelativePosition(startPos);

        WhiteIntensityTween.reset();
    }

    void SetButtonAndWidjetsRelativePosition(sf::Vector2f newPos) {
        Position = newPos;
        float offset = SelectedOffsetTween.getValue();
        sf::Vector2f offsetVec = {offset, 0.f};

        SongButton.sprite->setPosition(Position + offsetVec + sf::Vector2f({80,0}));
        ArtistLabel.setPosition(Position + offsetVec + sf::Vector2f({-60,5}));
        DificultyLabel.setPosition(Position + offsetVec + sf::Vector2f({-90,-20}));
        SongNameLabel.setPosition(Position + offsetVec + sf::Vector2f({-60,-20}));
        MapperLabel.setPosition(Position + offsetVec + sf::Vector2f({0,5}));
    }

    void setPositionTweened(sf::Vector2f newPos) {
        PositionTweenX = ValueTween(Position.x, newPos.x, 0.5f, Tween::easeOutQuad);
        PositionTweenY = ValueTween(Position.y, newPos.y, 0.5f, Tween::easeOutQuad);
        PositionTweenX.play();
        PositionTweenY.play();
    }

    void update(float dt) {
        PositionTweenX.update(dt);
        PositionTweenY.update(dt);
        SelectedOffsetTween.update(dt);
        WhiteIntensityTween.update(dt);
        whiteIntensity = WhiteIntensityTween.getValue();
        if (PositionTweenX.isActive() || PositionTweenY.isActive() || SelectedOffsetTween.isActive()) {
            SetButtonAndWidjetsRelativePosition({PositionTweenX.getValue(), PositionTweenY.getValue()});
        }
    }

    void clicked(std::vector<std::shared_ptr<SongSlot>>& slots, std::shared_ptr<SongSlot>& selectedSlot, SongList& list);

    void renderButton(sf::RenderWindow& window) {
        if (WhiteIntensityTween.isActive() || whiteIntensity > 0.f) {
            ShaderUtils::drawSpriteWithWhiteMaskShader(window, *SongButton.sprite, static_cast<uint8_t>(whiteIntensity));
        }   
        else {
            window.draw(*SongButton.sprite);
        }
        window.draw(SongNameLabel);
        window.draw(ArtistLabel);
        window.draw(MapperLabel);
        window.draw(DificultyLabel);
    }
};

class SongList {
public:
    sf::Vector2f ListPosition;
    std::vector<std::shared_ptr<SongSlot>> ButtonVector;
    std::shared_ptr<SongSlot> SelectedSlot;
    sf::RenderWindow& window;
    Object select_slot_background;

    int button_offset = 70;

    // Cache de backgrounds
    static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> BackgroundCache;

    SongList(std::string path, sf::Vector2f list_position, sf::RenderWindow &mWindow)
    : select_slot_background("assets/sprites/main_menu/background.png",0,0),
      window(mWindow)
    {
        ListPosition = list_position;
        select_slot_background.blurredStrength = 2.0f; // <-- Adicione esta linha

        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    auto slot = std::make_shared<SongSlot>(entry.path().string(), sf::Vector2f{200, 200});
                    ButtonVector.push_back(slot);
                }
            }
            if (!ButtonVector.empty()) {
                SelectedSlot = ButtonVector[ButtonVector.size() / 2];
            }
            updateSlotPositions();

            auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
            if (it != ButtonVector.end()) {
                int index = std::distance(ButtonVector.begin(), it);

                ButtonVector[index]->SetButtonAndWidjetsRelativePosition(ListPosition + sf::Vector2f(-40.f, 0.f));

                for (int i = index - 1, offset = -1; i >= 0; --i, --offset) {
                    ButtonVector[i]->SetButtonAndWidjetsRelativePosition(ListPosition + sf::Vector2f(0.f, offset * button_offset));
                }

                for (int i = index + 1, offset = 1; i < ButtonVector.size(); ++i, ++offset) {
                    ButtonVector[i]->SetButtonAndWidjetsRelativePosition(ListPosition + sf::Vector2f(0.f, offset * button_offset));
                }
            }
            
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }

        setBackgroundForSelectedSlot();
    }

    void setBackgroundForSelectedSlot() {
        std::string bgPath = SelectedSlot->FolderLocation + "/background.png";
        auto it = BackgroundCache.find(bgPath);
        if (it != BackgroundCache.end()) {
            select_slot_background.spriteTexture = it->second;
        } else {
            auto tex = std::make_shared<sf::Texture>();
            if (!tex->loadFromFile(bgPath)) {
                std::cerr << "Não foi possível carregar a imagem " << bgPath << std::endl;
            }
            BackgroundCache[bgPath] = tex;
            select_slot_background.spriteTexture = tex;
        }
        select_slot_background.sprite->setTexture(*select_slot_background.spriteTexture);
        ResizeSpriteToFitWindow(select_slot_background, window);
    }

    void updateSlotPositions() {
        auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
        if (it != ButtonVector.end()) {
            int index = std::distance(ButtonVector.begin(), it);

            // Selected slot: move left
            ButtonVector[index]->setPositionTweened(ListPosition);
            ButtonVector[index]->SelectedOffsetTween = ValueTween(ButtonVector[index]->SelectedOffsetTween.getValue(), -40.f, 0.5f, Tween::easeOutQuad);
            ButtonVector[index]->SelectedOffsetTween.play();

            // Ativa o tween de whiteIntensity
            ButtonVector[index]->WhiteIntensityTween = ValueTween(255.f, 50.f, 0.6f, Tween::easeOutQuad);
            ButtonVector[index]->WhiteIntensityTween.play();

            // Other slots: move back to normal
            for (int i = index - 1, offset = -1; i >= 0; --i, --offset) {
                ButtonVector[i]->setPositionTweened(ListPosition + sf::Vector2f(0.f, offset * button_offset));
                ButtonVector[i]->SelectedOffsetTween = ValueTween(ButtonVector[i]->SelectedOffsetTween.getValue(), 0.f, 0.5f, Tween::easeOutQuad);
                ButtonVector[i]->SelectedOffsetTween.play();

                 if (ButtonVector[i]->whiteIntensity > 0.f) { // <-- Verifica se o valor é maior que 0 antes de aplicar o tween
                    ButtonVector[i]->WhiteIntensityTween = ValueTween(ButtonVector[i]->WhiteIntensityTween.getValue(), 0.f, 0.3f, Tween::easeOutQuad);
                    ButtonVector[i]->WhiteIntensityTween.play();
                }
            }
            for (int i = index + 1, offset = 1; i < ButtonVector.size(); ++i, ++offset) {
                ButtonVector[i]->setPositionTweened(ListPosition + sf::Vector2f(0.f, offset * button_offset));
                ButtonVector[i]->SelectedOffsetTween = ValueTween(ButtonVector[i]->SelectedOffsetTween.getValue(), 0.f, 0.5f, Tween::easeOutQuad);
                ButtonVector[i]->SelectedOffsetTween.play();

                if (ButtonVector[i]->whiteIntensity > 0.f) { // <-- Verifica se o valor é maior que 0 antes de aplicar o tween
                    ButtonVector[i]->WhiteIntensityTween = ValueTween(ButtonVector[i]->WhiteIntensityTween.getValue(), 0.f, 0.3f, Tween::easeOutQuad);
                    ButtonVector[i]->WhiteIntensityTween.play();
                }
            }

            setBackgroundForSelectedSlot();
        }
    }

    void updateSlotTweens(float dt) {
        for (auto& slot : ButtonVector) {
            slot->update(dt);
        }
    }

    void scrollListUpByOne() {
        auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
        if (it != ButtonVector.end()) {
            int index = std::distance(ButtonVector.begin(), it);
            if (index > 0) {
                SelectedSlot = ButtonVector[index - 1];
            }
            updateSlotPositions();        
        }
    }

    void scrollListDownByOne() {
         auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
        if (it != ButtonVector.end()) {
            int index = std::distance(ButtonVector.begin(), it);
            if (index + 1 != ButtonVector.size()) {
                SelectedSlot = ButtonVector[index + 1];
            }   
            updateSlotPositions();     
        }
    }

    void selectSlotByIndex(int index) {
        if (index >= 0 && index < ButtonVector.size()) {
            SelectedSlot = ButtonVector[index];
            updateSlotPositions();
        }
    }

    void ResizeSpriteToFitWindow(Object obj, sf::RenderWindow& window) {
        sf::Vector2u windowSize = window.getSize();                
        sf::Vector2u textureSize = obj.sprite->getTexture().getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        obj.sprite->setScale({scaleX, scaleY});
    }

    void RenderList(sf::RenderWindow& window) {
        ShaderUtils::drawVerticalBlurSprite(window,*select_slot_background.sprite,select_slot_background.blurredStrength);
        for (auto slot : ButtonVector) {
            if (slot->Position.x > window.getSize().x + 100
                || slot->Position.y > window.getSize().y + 100
                || slot->Position.x < -100 
                || slot->Position.y < -100) 
            {
                continue; // Skip rendering if the slot is out of bounds
            }
            slot->renderButton(window);
        }
    }
};

class SongSelect : public State {
public:
    SongSelect(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
    SongList List;
    Object Cursor;
    sf::Vector2i mouse_pos;
    float mouseScrollQueueCooldown = 0.0f;
    std::vector<int> pendingScrolls;
};
