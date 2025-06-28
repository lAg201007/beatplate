#include "plate.h"
#include "../../libs/json.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

Plate::Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS)
	: Note(offset, "plate", xPos, AR),
	  plateObject("assets/sprites/game/objects/plate.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  approachCircle("assets/sprites/game/objects/plate_approach_circle.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  HitScaleTween(*plateObject.sprite, 0.1f),
	  HitTransparencyTween(*plateObject.sprite, 0.1f),
	  MissScaleTween(*plateObject.sprite, 0.3f),
	  MissTransparencyTween(*plateObject.sprite, 0.3f),
	  ApproachCircleScaleTween(*approachCircle.sprite, approachMs / 1000.0f),
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
		ApproachCircleScaleTween.initScale(0.5f,0.05f);

		if (AR < 5) {
			approachMs = 1800 - 120 * AR;
		} else if (AR == 5) {
			approachMs = 1200;
		} else {
			approachMs = 1200 - 150 * (AR - 5);
		}
	};

namespace {
	void StartHit(Tween& tweenT, Tween& tweenS) {
		tweenT.initTransparency(1.0f, 0.0f);
		tweenS.initScale(0.25f, 0.30f);
		tweenT.play();
		tweenS.play();
	}
	void StartMiss(Tween& tweenT, Tween& tweenS) {
		tweenT.initTransparency(1.0f,0.0f);
		tweenS.initScale(0.25f,0.20f);
		tweenT.play();
		tweenS.play();
	}
}

void Plate::start()
{
	if (state == NoteState::Waiting)
	{
		state = NoteState::Active;
		startTimeMs = offset - approachMs;
		plateObject.sprite->setPosition({static_cast<float>(xPos), static_cast<float>(StartYPos)});
		ApproachCircleScaleTween.play();
	}
}

void Plate::update(float elapsed, float dt)
{
	
	if (state == NoteState::Waiting && elapsed >= offset - approachMs) {
        start();
    }

    if (state == NoteState::Active) {
        float progress = (elapsed- startTimeMs) / approachMs;
        progress = std::clamp(progress, 0.0f, 1.0f);
        float y = StartYPos + (TargetYPos - StartYPos) * progress;
        plateObject.sprite->setPosition({static_cast<float>(xPos), y});

		approachCircle.sprite->setPosition(plateObject.sprite->getPosition());
		ApproachCircleScaleTween.update(dt);

		hitWindow = elapsed - offset;
		float perfectWindow = 80 - 6 * ACD;
		float earlyLateWindow = 140 - 8 * ACD;
		float tooEarlyLateWindow = 200 - 10 * ACD;

		if (hitWindow > tooEarlyLateWindow) {
			std::cout << "Plate missed!" << " hitwindow: " << hitWindow << ", elapsed: " << elapsed << std::endl;
			state = NoteState::Missing;
			StartMiss(MissTransparencyTween, MissScaleTween);
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
				state = NoteState::Missing;
				StartMiss(MissTransparencyTween, MissScaleTween);
			}
			state = NoteState::Hitting;
			StartHit(HitTransparencyTween, HitScaleTween);
		}
    }

	if (state == NoteState::Hitting) {
		HitScaleTween.update(dt);
		HitTransparencyTween.update(dt);
		if (!HitTransparencyTween.isActive() && !HitScaleTween.isActive()) {
			state = NoteState::Hit;
		}
	}

	if (state == NoteState::Missing) {
		MissScaleTween.update(dt);
		MissTransparencyTween.update(dt);
		if (!MissScaleTween.isActive() && !MissTransparencyTween.isActive()) {
			state = NoteState::Missed;
		}
	}
}

void Plate::render(sf::RenderWindow& window)
{
	if (state == NoteState::Active)
	{
		window.draw(*plateObject.sprite);
		window.draw(*approachCircle.sprite);
	}
	if (state == NoteState::Hitting || state == NoteState::Missing)
	{
		window.draw(*plateObject.sprite);
	}
}