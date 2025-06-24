#include "plate.h"
#include "../../libs/json.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

Plate::Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS)
	: Note(offset, "plate", xPos, AR),
	  plateObject("assets/sprites/game/objects/plate.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  window(rWindow),
	  AR(AR),
	  ACD(ACD),
	  PS(PS)
	{
		std::ifstream dataFile("config.json");
		if (!dataFile.is_open()) {
			std::cerr << "Não foi possível abrir config.json" << std::endl;
			return;
		}
		nlohmann::json configData;
		dataFile >> configData;
		binds = configData["settings"]["binds"]["game_click"].get<std::vector<std::string>>();
	};

void Plate::start()
{
	if (state == NoteState::Waiting)
	{
		state = NoteState::Active;
		startTimeMs = offset - approachMs;
		plateObject.sprite->setPosition({static_cast<float>(xPos), static_cast<float>(StartYPos)});
	}
}

void Plate::update(float elapsed)
{
	int AR = this->AR;
	if (AR < 5) {
        approachMs = 1800 - 120 * AR;
    } else if (AR == 5) {
        approachMs = 1200;
    } else {
        approachMs = 1200 - 150 * (AR - 5);
	}
	
	if (state == NoteState::Waiting && elapsed >= offset - approachMs) {
        start();
    }

    if (state == NoteState::Active) {
        float progress = (elapsed- startTimeMs) / approachMs;
        progress = std::clamp(progress, 0.0f, 1.0f);
        float y = StartYPos + (TargetYPos - StartYPos) * progress;
        plateObject.sprite->setPosition({static_cast<float>(xPos), y});
		if (progress >= 1.0f) {
			state = NoteState::Hittable;
		}
    }
	if (state == NoteState::Hittable) {
		if (DetectClickWithBind(this->window)) {
			state = NoteState::Hitting;
		}
	}
}

void Plate::render(sf::RenderWindow& window)
{
	if (state == NoteState::Active || state == NoteState::Hittable || state == NoteState::Hitting)
	{
		window.draw(*plateObject.sprite);
	}
}