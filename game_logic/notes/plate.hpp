#pragma once

#include "../game_logic/clock.hpp"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../game_logic/notes/note.hpp"
#include <print>

class Plate : public Note {
public:
    Plate(int offset, const std::pair<std::string, std::string>& binds, int xPos, int yPos,int finalYPos,int PS, int ACD, float AR = 0.0f)
        : Note(offset, "plate", xPos, AR), binds(binds), xPos(xPos), yPos(yPos), PS(PS), ACD(ACD), finalYPos(finalYPos), pixelSize(150 * PS),
          object("assets/sprites/game/objects/plate.png", xPos, yPos, 200, 200, 1.0f, 1.0f),
          aproachCircle("assets/sprites/game/objects/plate_approach_circle.png", xPos, yPos, 200, 200, 1.0f, 1.0f),
          perfectHitWindow(getPerfectWindowMs(ACD)),
          earlyLateWindow(getEarlyLateWindowMs(ACD)),
          tooEarlyLateWindow(getTooEarlyLateWindowMs(ACD)),
          appearTime(getAppearTimeMs(AR, offset))
    {
        object.sprite->setScale({
            static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.x,
            static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.y
        });
    }

    bool DetectHoverX(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = object.sprite->getGlobalBounds();
        return bounds.contains(sf::Vector2f({static_cast<float>(mousePos.x), finalYPos}));
    }

    bool DetectClickWithBind(sf::RenderWindow& window) {
        bool bindPressed = false;

        // Check first bind
        if (binds.first.size() == 1 && std::isalpha(binds.first[0])) {
            sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(
                static_cast<int>(sf::Keyboard::Key::A) + (std::toupper(binds.first[0]) - 'A')
            );
            if (key != sf::Keyboard::Key::Unknown && sf::Keyboard::isKeyPressed(key)) {
                bindPressed = true;
            }
        }

        // Check second bind if first wasn't pressed
        if (!bindPressed && binds.second.size() == 1 && std::isalpha(binds.second[0])) {
            sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(
                static_cast<int>(sf::Keyboard::Key::A) + (std::toupper(binds.second[0]) - 'A')
            );
            if (key != sf::Keyboard::Key::Unknown && sf::Keyboard::isKeyPressed(key)) {
                bindPressed = true;
            }
        }

        bool hovered = DetectHoverX(window);

        if (hovered && bindPressed && !PressedLastFrame) {
            PressedLastFrame = true;
            return true;
        }
        
        PressedLastFrame = bindPressed;
        return false;
    }

    void update(sf::Time elapsed, sf::RenderWindow& window) override {
        const int milliTime = elapsed.asMilliseconds();
        const int offset = getOffset();

        if (milliTime < appearTime && getState() != NoteState::Waiting) {
            setState(NoteState::Waiting);
        }
        else if (milliTime >= appearTime && milliTime < offset - tooEarlyLateWindow && getState() != NoteState::Active) {
            setState(NoteState::Active);
        }
        else if (milliTime >= offset - tooEarlyLateWindow && milliTime <= offset + earlyLateWindow && getState() != NoteState::Judging) {
            setState(NoteState::Judging);
        }
        else if (milliTime > offset + earlyLateWindow && getState() != NoteState::Missed && getState() != NoteState::Hit) {
            setState(NoteState::Missed);
            setHitResult(HitResult::None);
        }
        
        if (getState() == NoteState::Judging) {
            if (DetectClickWithBind(window)) {
                int hitWindow = std::abs(milliTime - offset);
                if (hitWindow <= perfectHitWindow) {
                    setHitResult(HitResult::Perfect);
                    setState(NoteState::Hit);
                    hitTime = milliTime;
                }
                if (hitWindow > perfectHitWindow && hitWindow <= earlyLateWindow) {
                    if (milliTime < offset) {
                        setHitResult(HitResult::PerfectEarly);
                    } else {
                        setHitResult(HitResult::PerfectLate);
                    }
                    setState(NoteState::Hit);
                    hitTime = milliTime;
                }
                if (hitWindow > earlyLateWindow && hitWindow <= tooEarlyLateWindow) {
                    if (milliTime < offset) {
                        setHitResult(HitResult::TooEarly);
                    } else {
                        setHitResult(HitResult::TooLate);
                    }
                    setState(NoteState::Hit);
                    hitTime = milliTime;
                }
            }
        }

        if (getState() == NoteState::Active) {
            float progress = static_cast<float>(milliTime - appearTime) / 
                    static_cast<float>(offset - tooEarlyLateWindow - appearTime);
            float easedProgress = (1 - (1 - progress) * (1 - progress)); // easeOutQuad
            
            float newY = static_cast<float>(finalYPos) / easedProgress;

            object.sprite->setPosition({static_cast<float>(xPos), newY});
        }

        if (getState() == NoteState::Active || getState() == NoteState::Judging) {
            float targetScale = static_cast<float>(pixelSize) / aproachCircle.sprite->getLocalBounds().size.x;

            float approachProgress = static_cast<float>(milliTime - appearTime) / 
                    static_cast<float>(offset - tooEarlyLateWindow - appearTime);

            // Interpolação linear de 3.0f até targetScale
            float scale = (approachProgress < 1) ? 1.0f - ((1.0f - targetScale) * approachProgress) : targetScale;
            aproachCircle.sprite->setPosition(object.sprite->getPosition());
            aproachCircle.sprite->setScale({scale, scale});
        }
    }

    void render(sf::RenderWindow& window) override {
        if (getState() == NoteState::Active || getState() == NoteState::Judging ||
            getState() == NoteState::Hitting || getState() == NoteState::Missing) {
            
            window.draw(*object.sprite);
            window.draw(*aproachCircle.sprite);
        }   
    }

private:
    float xPos;
    float yPos;
    float finalYPos;
    int PS;
    int ACD;
    int perfectHitWindow;
    int earlyLateWindow;
    int tooEarlyLateWindow;
    int appearTime;
    int hitTime = -1;
    int pixelSize;
    bool PressedLastFrame = false;

    Button object;
    Object aproachCircle;
    const std::pair<std::string, std::string> binds;
};