#include "plate.h"
#include "../../libs/json.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>

static sf::Texture earlyLateTexture;
static sf::Texture tooEarlyLateTexture;
static sf::Texture missTexture;
static bool texturesLoaded = false;

Plate::Plate(sf::RenderWindow& rWindow, int offset, int xPos, int AR, int ACD, int PS)
	: Note(offset, "plate", xPos, AR),
	  plateObject("assets/sprites/game/objects/plate.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  approachCircle("assets/sprites/game/objects/plate_approach_circle.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  hitEffect("assets/sprites/game/effects/perfect_hit_effect.png", xPos, 300, 200, 200, 0.25f, 0.25f),
	  HitScaleTween(*plateObject.sprite, 0.1f),
	  HitTransparencyTween(*plateObject.sprite, 0.1f),
	  MissScaleTween(*plateObject.sprite, 0.3f),
	  MissTransparencyTween(*plateObject.sprite, 0.3f),
	  HitEffectTransparencyTween(*hitEffect.sprite, 0.3f),
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

		if (!texturesLoaded) {
			if (!earlyLateTexture.loadFromFile("assets/sprites/game/effects/early_late_hit_effect.png"))
				std::cerr << "Falha ao carregar early_late_hit_effect.png\n";
			if (!tooEarlyLateTexture.loadFromFile("assets/sprites/game/effects/too_early_too_late_hit_effect.png"))
				std::cerr << "Falha ao carregar too_early_too_late_hit_effect.png\n";
			if (!missTexture.loadFromFile("assets/sprites/game/effects/miss_hit_effect.png"))
				std::cerr << "Falha ao carregar miss_hit_effect.png\n";
			texturesLoaded = true;
		}
	};

namespace {
	void StartHit(Tween& tweenT, Tween& tweenS, Tween& tweenE, Object& HitEffect, Object& plate, HitResult hitResult) {
		tweenT.initTransparency(1.0f, 0.0f);
		tweenS.initScale(0.25f, 0.30f);

		tweenT.play();
		tweenS.play();


		switch (hitResult) {
			case HitResult::Perfect:
				HitEffect.sprite->setPosition(plate.sprite->getPosition());
				tweenE.setDuration(0.1f);
				break;
			case HitResult::PerfectEarly:
			case HitResult::PerfectLate:
				HitEffect.sprite->setTexture(earlyLateTexture);
				HitEffect.sprite->setPosition(plate.sprite->getPosition());
				break;
			case HitResult::TooEarly:
			case HitResult::TooLate:
				HitEffect.sprite->setTexture(tooEarlyLateTexture);
				HitEffect.sprite->setPosition(plate.sprite->getPosition());
				tweenE.setDuration(0.6f);
				break;
		}

		tweenE.initTransparency(1.0f,0.0f);
		tweenE.play();
	}
	void StartMiss(Tween& tweenT, Tween& tweenS, Tween& tweenE, Object& HitEffect, Object& plate) {
		tweenT.initTransparency(1.0f,0.0f);
		tweenS.initScale(0.25f,0.20f);
		tweenT.play();
		tweenS.play();

		tweenE.setDuration(1.0f);
		tweenE.initTransparency(1.0f,0.0f);
		tweenE.play();

		HitEffect.sprite->setTexture(missTexture);
		HitEffect.sprite->setPosition(plate.sprite->getPosition());
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
			state = NoteState::Missing;
			StartMiss(MissTransparencyTween, MissScaleTween, HitEffectTransparencyTween,hitEffect, plateObject);
		}

		if (DetectClickWithBind(this->window)) {
			if (std::abs(hitWindow) <= perfectWindow) {
				hitResult = HitResult::Perfect;
			}
			else if (std::abs(hitWindow) <= earlyLateWindow) {
				hitResult = (hitWindow < 0) ? HitResult::PerfectEarly : HitResult::PerfectLate;
			} 
			else if (std::abs(hitWindow) <= tooEarlyLateWindow) {
				hitResult = (hitWindow < 0) ? HitResult::TooEarly : HitResult::TooLate;
			} 
			else {
				state = NoteState::Missing;
				StartMiss(MissTransparencyTween, MissScaleTween, HitEffectTransparencyTween, hitEffect, plateObject);
			}
			state = NoteState::Hitting;
			StartHit(HitTransparencyTween, HitScaleTween,HitEffectTransparencyTween, hitEffect, plateObject, hitResult);
		}
    }

	if (state == NoteState::Hitting || state == NoteState::Missing) {
		HitScaleTween.update(dt);
		HitTransparencyTween.update(dt);
		HitEffectTransparencyTween.update(dt);
		if (!HitTransparencyTween.isActive() && !HitScaleTween.isActive()) {
			if (state == NoteState::Hitting) { state = NoteState::Hit; }
			else { state = NoteState::Missed; }
		}
	}

	if (state == NoteState::Missed || state == NoteState::Hit) {
		HitEffectTransparencyTween.update(dt);
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
		window.draw(*hitEffect.sprite);
	}
	if (state == NoteState::Missed || state == NoteState::Hit) {
		if (HitEffectTransparencyTween.isActive()) {
			window.draw(*hitEffect.sprite);
		}
	}
}