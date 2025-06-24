#include "plate.h"
#include "../../libs/json.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

Plate::Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS)
	: Note(offset, "plate", xPos, AR),
	  plateObject("assets/sprites/game/objects/plate.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  approachCircle("assets/sprites/game/objects/plate_approach_circle.png", xPos, 300, 200, 200, 0.25f, 0.25f),
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
		hitWindow = elapsed - offset;
		float perfectWindow = 80 - 6 * ACD;
		float earlyLateWindow = 140 - 8 * ACD;
		float tooEarlyLateWindow = 200 - 10 * ACD;

		if (hitWindow > tooEarlyLateWindow) {
			state = NoteState::Missed;
			std::cout << "Plate missed!" << " hitwindow: " << hitWindow << ", elapsed: " << elapsed << std::endl;
		}

		if (DetectClickWithBind(this->window)) {
			if (std::abs(hitWindow) <= perfectWindow) {
				hitResult = HitResult::Perfect;
				std::cout << "Perfect" << std::endl;
			} else if (std::abs(hitWindow) <= earlyLateWindow) {
				hitResult = (hitWindow < 0) ? HitResult::PerfectEarly : HitResult::PerfectLate;
				std::cout << "Early/Late" << std::endl;
			} else if (std::abs(hitWindow) <= tooEarlyLateWindow) {
				hitResult = (hitWindow < 0) ? HitResult::TooEarly : HitResult::TooLate;
				std::cout << "TooEarly/TooLate" << std::endl;
			} else {
				std::cout << "Plate missed!" << " hitwindow: " << hitWindow << ", elapsed: " << elapsed << std::endl;
				state = NoteState::Missed;
			}
			state = NoteState::Hitting;
		}
	}
}

void Plate::render(sf::RenderWindow& window)
{
	if (state == NoteState::Active || state == NoteState::Hittable)
	{
		window.draw(*plateObject.sprite);
	}
}