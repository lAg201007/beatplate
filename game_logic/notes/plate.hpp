#pragma once

#include "../game_logic/clock.hpp"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../game_logic/notes/note.hpp"
#include <print>

// h e k são o x e y do apice da parábola
// a diferenca entre ixpos e xpos é de 100px - 350px
// a altura do y é de 100px à 500px
inline sf::Vector2f getXYTrajectory(float x0, float y0, float h, float k, float t) {
    float xf = 2 * h - x0;
    float yf = y0;
    float a = (y0 - k) / ((x0 - h) * (x0 - h));
    float x = x0 + (xf - x0) * t;
    float y = a * (x - h) * (x - h) + k;

    return sf::Vector2f({x, y});
}

class Plate : public Note {
public:
    Plate(int offset, const std::pair<std::string, std::string>& binds, int xPos, int yPos,int finalYPos, int finalXPos, int plateNumber, int PS, int ACD, float AR = 0.0f, float Vel = 1.0f)
        : Note(offset, "plate", xPos, AR), binds(binds), xPos(xPos), yPos(yPos), PS(PS), ACD(ACD), finalYPos(finalYPos), finalXPos(finalXPos), pixelSize(150 * PS),
          object(std::string("assets/sprites/game/objects/plates/plate_") + std::to_string(plateNumber) + ".png", xPos, yPos, 200, 200, 1.0f, 1.0f),
          perfectHitWindow(getPerfectWindowMs(ACD)),
          earlyLateWindow(getEarlyLateWindowMs(ACD)),
          tooEarlyLateWindow(getTooEarlyLateWindowMs(ACD)),
          appearTime(getAppearTimeMs(AR, offset)),
          plateNumber(plateNumber),
          initialXPos(xPos),
          initialYPos(yPos),
          Velocity(Vel)
    {
        object.sprite->setScale({
            static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.x,
            static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.y
        });
    }

    bool DetectHover(sf::RenderWindow& window) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::FloatRect bounds = object.sprite->getGlobalBounds();
        return bounds.contains(sf::Vector2f({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)}));
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

        bool hovered = DetectHover(window);

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

            sf::Vector2f newPos = getXYTrajectory(static_cast<float>(xPos), static_cast<float>(yPos), finalXPos, finalYPos, (progress / 2.0f) * Velocity);
            object.sprite->setPosition(newPos);

            float spinFactor =
                (std::abs(initialYPos - finalYPos) / (float)window.getSize().y) *
                (std::abs(initialXPos - finalXPos) / (float)window.getSize().x) *
                8.0f; // amplifica o spin

            sf::Angle newAngle = sf::degrees(progress * 360.0f * Velocity * spinFactor);

            object.sprite->setRotation(newAngle);
        }

        if (getState() == NoteState::Missing) {
            float progress = static_cast<float>(milliTime - appearTime) / 
                    static_cast<float>(offset - tooEarlyLateWindow - appearTime);

            sf::Vector2f newPos = getXYTrajectory(static_cast<float>(xPos), static_cast<float>(yPos), finalXPos, finalYPos, progress / 4.0f);
            object.sprite->setPosition(newPos);
        }

        if (getState() == NoteState::Active || getState() == NoteState::Judging) {
            //float targetScale = static_cast<float>(pixelSize) / aproachCircle.sprite->getLocalBounds().size.x;
            
        }
    }

    void render(sf::RenderWindow& window) override {
        if (getState() == NoteState::Active || getState() == NoteState::Judging ||
            getState() == NoteState::Hitting || getState() == NoteState::Missing) {
            
            window.draw(*object.sprite);
            //window.draw(*aproachCircle.sprite);
        }   
    }

private:
    float xPos;
    float yPos;
    float initialXPos;
    float initialYPos;
    float finalYPos;
    float finalXPos;
    float Velocity;
    int plateNumber;
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
    const std::pair<std::string, std::string> binds;
};