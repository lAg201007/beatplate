#include "plate.h"
#include <algorithm>
#include <iostream>

Plate::Plate(int offset, int xPos, const float AR)
	: Note(offset, "plate", xPos),
	  plateObject("assets/sprites/game/objects/plate.png", xPos, 300, 200, 200, 0.25f, 0.25f)
	  
	{
		
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
	if (AR < 5.0f)
		approachMs = 1800.0f - 120.0f * AR;
	else
		approachMs = 1200.0f - 150.0f * (AR - 5.0f);

	if (state == NoteState::Waiting && elapsed >= offset - approachMs) {
        start();
    }

    if (state == NoteState::Active) {
        float progress = (elapsed- startTimeMs) / approachMs;
        progress = std::clamp(progress, 0.0f, 1.0f);
        float y = StartYPos + (TargetYPos - StartYPos) * progress;
        plateObject.sprite->setPosition({static_cast<float>(xPos), y});
    }
}

void Plate::render(sf::RenderWindow &window)
{
	if (state == NoteState::Active)
	{
		window.draw(*plateObject.sprite);
	}
}