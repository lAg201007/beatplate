#pragma once
#include "game_state.h"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include <iostream>
#include <fstream>
#include "libs/json.hpp"

class SongSlot {
public:
    std::string SongName;
    std::string Artist;
    std::string Mapper;
    int Difficulty;

    sf::Font Montserrat;
    sf::Text SongNameLabel;
    sf::Text ArtistLabel;
    sf::Text MapperLabel;
    sf::Text DificultyLabel;
    Object SongButton;

    sf::Vector2f Position;

    SongSlot(std::string SongFolder, sf::Vector2f startPos)
     : 
    SongNameLabel(Montserrat),
    ArtistLabel(Montserrat), 
    MapperLabel(Montserrat), 
    DificultyLabel(Montserrat), 
    SongButton("assets/sprites/song_select/song_select_button.png",Position.x,Position.y,512,512,0.5f,0.5f)

    {
        if (!Montserrat.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
            std::cerr << "não foi possivel carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
        }

        std::ifstream dataFile(SongFolder + "/data.json");
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
        MapperLabel.setCharacterSize(8);

        DificultyLabel.setString(std::to_string(Difficulty));
        DificultyLabel.setCharacterSize(30);
    }

    void SetButtonAndWidjetsRelativePosition(sf::Vector2f newPos) {

    }
};

class SongSelect : public State {
public:
    SongSelect(StateStack& stack, sf::RenderWindow& window);

    void handleEvent(const sf::Event& event) override;
    void update(sf::Time dt) override;
    void render() override;

private:
 
};
