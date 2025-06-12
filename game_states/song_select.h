#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../libs/json.hpp"
#include "../shaders/shader_manager.h"
#include <iostream>
#include <fstream>
#include <filesystem>

class SongSlot {
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

    sf::Vector2f Position;

    SongSlot(std::string SongFolder, sf::Vector2f startPos)
     : 
    SongNameLabel(Montserrat),
    ArtistLabel(Montserrat), 
    MapperLabel(Montserrat), 
    DificultyLabel(Montserrat), 
    SongButton("assets/sprites/song_select/song_select_button.png",Position.x,Position.y,512,512,0.50f,0.25f)
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

        SetButtonAndWidjetsRelativePosition(startPos);
    }

    void SetButtonAndWidjetsRelativePosition(sf::Vector2f newPos) {
        Position = newPos;

        SongButton.sprite->setPosition(Position + sf::Vector2f({80,0}));
        ArtistLabel.setPosition(Position + sf::Vector2f({-60,5}));
        DificultyLabel.setPosition(Position + sf::Vector2f({-90,-20}));
        SongNameLabel.setPosition(Position + sf::Vector2f({-60,-20}));
        MapperLabel.setPosition(Position + sf::Vector2f({0,5}));
    }

    void renderButton(sf::RenderWindow& window) {
        window.draw(*SongButton.sprite);
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
    Object select_slot_background;

    int button_offset = 70;

    SongList(std::string path, sf::Vector2f list_position, sf::RenderWindow &window)
    : select_slot_background("assets/sprites/main_menu/background.png",0,0)
        {
        ListPosition = list_position;
        try {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_directory()) {
                    auto slot = std::make_shared<SongSlot>(entry.path().string(), sf::Vector2f{200, 200});
                    ButtonVector.push_back(slot);
                }
            }
            SelectedSlot = ButtonVector.back(); 
            updateSlotPositions();
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Erro: " << e.what() << std::endl;
        }

        if (!select_slot_background.spriteTexture->loadFromFile(SelectedSlot->FolderLocation + "/background.png")) {
            std::cerr << "N�o foi poss�vel carregar a imagem " << std::endl;
        }

        select_slot_background.sprite->setTexture(*select_slot_background.spriteTexture);
        select_slot_background.blurredStrength = 2.0f;
        
        sf::Vector2u windowSize = window.getSize();                
        sf::Vector2u textureSize = select_slot_background.sprite->getTexture().getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        select_slot_background.sprite->setScale({scaleX, scaleY});
    }

    void updateSlotPositions() {
        auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
        if (it != ButtonVector.end()) {
            int index = std::distance(ButtonVector.begin(), it);

            ButtonVector[index]->SetButtonAndWidjetsRelativePosition(ListPosition);

            for (int i = index - 1, offset = -1; i >= 0; --i, --offset) {
                ButtonVector[i]->SetButtonAndWidjetsRelativePosition(ListPosition + sf::Vector2f(0.f, offset * button_offset));
            }

            for (int i = index + 1, offset = 1; i < ButtonVector.size(); ++i, ++offset) {
                ButtonVector[i]->SetButtonAndWidjetsRelativePosition(ListPosition + sf::Vector2f(0.f, offset * button_offset));
            }
        }
    }

    void RenderList(sf::RenderWindow& window) {
        ShaderUtils::drawVerticalBlurSprite(window,select_slot_background,select_slot_background.blurredStrength);
        for (auto slot : ButtonVector) {
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
};
