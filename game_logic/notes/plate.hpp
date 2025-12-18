#pragma once

#include "../game_logic/clock.hpp"
#include "../utils/SFML_CLASSES.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../game_logic/notes/note.hpp"
#include <print>
#include <algorithm>
#include <iostream> // Necessário para std::cerr

// consts

// ms
const int hittingTime = 500;
const int fullOpacityTrajectoryDotOffset = 0;
const int trajectoryDotTimingOffset = 0;
const float missFadeDuration = 500;
const float aproachCircleScaleDiminish = 0.15f;
const float plateObjectAfterHitMaxScale = 1.005f;

extern sf::Sound hit1;
extern sf::Sound hit2;
extern sf::Sound hit3;

extern sf::SoundBuffer bufhit1;
extern sf::SoundBuffer bufhit2;
extern sf::SoundBuffer bufhit3;

struct HitResultObject {
    bool initialized = false;
    Object object;
};

struct TrajectorDot {
    Object dotObject;
    sf::Color fullOpacityColor;
    float positionT; // Valor entre 0.0 e 1.0 representando a posição ao longo da trajetória
    float opacity; 
    int vanishTime;
    bool visible = true;
};


inline sf::Sound initSound(sf::Sound& Sound, sf::SoundBuffer& Buffer, std::string path) {
    if (Buffer.loadFromFile(path)) {
        Sound.setBuffer(Buffer);
    }
    else {
        std::println("FAILED TO LOAD SOUND BUFFER FROM FILE: {}", path);
    }
    return Sound;
}

inline void loadHitSounds() {
    hit1 = initSound(hit1, bufhit1, "assets/sounds/game/plate/platehit1.wav");
    hit2 = initSound(hit2, bufhit2, "assets/sounds/game/plate/platehit2.wav");
    hit3 = initSound(hit3, bufhit3, "assets/sounds/game/plate/platehit3.wav");
}

inline sf::Vector2f getXYTrajectory(float x0, float y0, float h, float k, float t) {
    float xf = 2 * h - x0;
    float yf = y0;
    float a = (y0 - k) / ((x0 - h) * (x0 - h));
    float x = x0 + (xf - x0) * t;
    float y = a * (x - h) * (x - h) + k;

    return sf::Vector2f({x, y});
}

inline float getProgress(int milliTime, int appearTime, int offset) {
    return static_cast<float>(milliTime - appearTime) / 
           static_cast<float>(offset - appearTime);
}

inline void changeHitResultTexture(HitResultObject& object, std::string appendString, sf::Vector2f PlatePosition) {
    sf::Texture* newTexture = &LoadTexture(std::string("assets/sprites/game/effects/") + appendString);
    object.object.sprite = nullptr;
    object.object.sprite = std::make_shared<ScaledSprite>(*newTexture);
    object.object.sprite->setOrigin({object.object.sprite->getGlobalBounds().size.x / 2, object.object.sprite->getGlobalBounds().size.y / 2});
    object.object.sprite->setPosition(PlatePosition);
    object.initialized = true;
}

inline void addHitResultTexture(HitResultObject& hitResultObject, HitResult result, sf::Vector2f PlatePosition) {
    switch (result) {
        case HitResult::Perfect:
            changeHitResultTexture(hitResultObject, "PERFECT!.png", PlatePosition);
            break;
        case HitResult::PerfectEarly:
        case HitResult::PerfectLate:
            changeHitResultTexture(hitResultObject, "GREAT!.png", PlatePosition);
            break;
        case HitResult::TooLate:
        case HitResult::TooEarly:
            changeHitResultTexture(hitResultObject, "OK!.png", PlatePosition);
            break;
        case HitResult::Missed:
            changeHitResultTexture(hitResultObject, "MISS!.png", PlatePosition);
            break;
    }
}

// TODO: VER SE ELE TÁ ATUALIZANDO QUANDO ESTÁ EM NOTESTATE::HITTING
inline sf::Angle getRotationAngle(float progress, int initialYPos, int finalYPos, int initialXPos, int finalXPos, float windowSizeX, float windowSizeY, int division) {
    float spinFactor =
        (std::abs(initialYPos - finalYPos) / windowSizeY) *
        (std::abs(initialXPos - finalXPos) / windowSizeX) *
        8.0f; // amplifica o spin
    
    sf::Angle result = sf::degrees((progress * 360.0f * spinFactor) / division);
    return result;
}

inline void updatePlate(int milliTime, int offset, int xPos, int yPos, int finalXPos, int finalYPos,
                  sf::RenderWindow& window, Object& object, Object& aproachCircle,
                  sf::Vector2f& aproachCircleScale, int appearTime,
                  int initialXPos, int initialYPos) {

    float progress = getProgress(milliTime, appearTime, offset);
    // half progress because of the parabolic movement
    float p = std::clamp(progress / 2.0f, 0.0f, 1.0f);

    sf::Vector2f newPos = getXYTrajectory(
        static_cast<float>(xPos),
        static_cast<float>(yPos),
        finalXPos,
        finalYPos,
        p
    );

    float scaleFactor = std::max(-4.0f * p + 3.0f, 1.0f);

    sf::Vector2f newScale;
    newScale.x = aproachCircleScale.x * scaleFactor;
    newScale.y = aproachCircleScale.y * scaleFactor;

    sf::Angle newAngle = getRotationAngle(progress, initialYPos, finalYPos, initialXPos, finalXPos, ScaleManager::GetBaseWidth(), ScaleManager::GetBaseHeight(), 1);
    sf::Angle newAproachCircleAngle = getRotationAngle(progress, initialYPos, finalYPos, initialXPos, finalXPos, ScaleManager::GetBaseWidth(), ScaleManager::GetBaseHeight(), 4);   

    aproachCircle.sprite->setScale(newScale);
    aproachCircle.sprite->setRotation(newAproachCircleAngle);
    object.sprite->setPosition(newPos);
    object.sprite->setRotation(newAngle);
}

inline void updateAlpha(NoteState state, sf::Color& originalColor, Object& obj, int milliTime, int appearTime, int hitTime, float duration = missFadeDuration) {
    int fadeStart = appearTime;
    if (state == NoteState::Missing && hitTime != -1) {
        fadeStart = hitTime;
    }
    // CORREÇÃO 3: 'Note_state' corrigido para 'NoteState' (Case sensitive e nome correto do enum)
    if (state == NoteState::Hitting) {
        fadeStart = hitTime;
    }
    float fadeElapsed = static_cast<float>(milliTime - fadeStart);
    float fadeProgress = std::clamp(fadeElapsed / duration, 0.0f, 1.0f);
    sf::Color newColor = originalColor;
    newColor.a = static_cast<uint8_t>(originalColor.a * (1.0f - fadeProgress));
    obj.sprite->setColor(newColor);
}

inline void playHitSound(const int hitnum) {
    if (hitnum > 3 || hitnum < 1) { return; } 
    switch (hitnum) {
        case 1:
            hit1.play();
            break;
        case 2:
            hit2.play();
            break;
        case 3:
            hit3.play();
            break;
    }
}

inline std::string _debug_read_state(NoteState state) {
    switch (state) {
        case NoteState::Waiting:
            return "WAITING";
        case NoteState::Active:
            return "ACTIVE";
        case NoteState::Judging:
            return "JUDGING";
        case NoteState::Hitting:
            return "HITTING";
        case NoteState::Hit:
            return "HIT";
        case NoteState::Missing:
            return "MISSING";
        case NoteState::Missed:
            return "MISSED";
    }
    return "NONE";
}

inline std::string _debug_read_hitresult(HitResult result) {
    switch (result) {
        case HitResult::Perfect:
            return "PERFECT";
        case HitResult::PerfectEarly:
        case HitResult::PerfectLate:
            return "GREAT";
        case HitResult::TooEarly:
        case HitResult::TooLate:
            return "OK";
        case HitResult::Missed:
            return "MISS";
        case HitResult::None:
            return "NONE";
    }
    return "NONE";
}

// Nova função de debug preenchida com a lógica de tempo e hit result
inline std::string _debug_read_runtime_hitresult(
    int milliTime, 
    int offset, 
    int perfectHitWindow, 
    int earlyLateWindow, 
    int tooEarlyLateWindow) 
{
    if (milliTime >= offset - tooEarlyLateWindow && milliTime <= offset + tooEarlyLateWindow) {
        int hitWindow = std::abs(milliTime - offset); 
        
        if (hitWindow <= perfectHitWindow) {
            return _debug_read_hitresult(HitResult::Perfect);
        }
        if (hitWindow > perfectHitWindow && hitWindow <= earlyLateWindow) {
            return _debug_read_hitresult(HitResult::PerfectEarly); 
        }
        if (hitWindow > earlyLateWindow && hitWindow <= tooEarlyLateWindow) {
            return _debug_read_hitresult(HitResult::TooEarly);
        }
    } 
    if (milliTime > offset + tooEarlyLateWindow) {
        return _debug_read_hitresult(HitResult::Missed); 
    }

    return "NONE";
}

inline HitResult _debug_return_runtime_hitresult(
    int milliTime, 
    int offset, 
    int perfectHitWindow, 
    int earlyLateWindow, 
    int tooEarlyLateWindow) 
{
    if (milliTime >= offset - tooEarlyLateWindow && milliTime <= offset + tooEarlyLateWindow) {
        int hitWindow = std::abs(milliTime - offset); 
        
        if (hitWindow <= perfectHitWindow) {
            return HitResult::Perfect;
        }
        if (hitWindow > perfectHitWindow && hitWindow <= earlyLateWindow) {
            return HitResult::PerfectEarly;
        }
        if (hitWindow > earlyLateWindow && hitWindow <= tooEarlyLateWindow) {
            return HitResult::TooEarly;
        }
    } 
    if (milliTime > offset + tooEarlyLateWindow) {
        return HitResult::Missed;
    }

    return HitResult::None;
}

// returns center position of an ScaledObject when in a scaled resolution
inline sf::Vector2f GetScaledCenterForObject(Object& obj) {
    sf::FloatRect bounds = obj.sprite->getGlobalBounds();
    
    sf::Vector2f center(
        bounds.position.x + bounds.size.x / 2.0f,
        bounds.position.y + bounds.size.y / 2.0f
    );
    
    sf::Vector2f baseCenter = ScaleManager::UnscalePosition(center.x, center.y);

    return baseCenter;
}

class Plate : public Note {
public:
    Plate(int offset, float trajectory_dot_max_transparency, int xPos, int yPos,int finalYPos, int finalXPos, int plateNumber, int hitNum, int PS, int ACD, float AR = 0.0f, std::shared_ptr<bool> debug_mode = std::make_shared<bool>(false))
        : Note(offset, "plate", xPos, AR), xPos(xPos), yPos(yPos), PS(PS), ACD(ACD), finalYPos(finalYPos), finalXPos(finalXPos), pixelSize(150 * PS),
          object(std::string("assets/sprites/game/objects/plates/plate_") + std::to_string(plateNumber) + ".png", xPos, yPos, 195, 195, 1.0f, 1.0f),
          aproachCircle("assets/sprites/game/objects/plates/plate_aproach_circle.png", finalXPos, finalYPos, 150, 150, 1.0f, 1.0f),
          perfectHitWindow(getPerfectWindowMs(ACD)),
          earlyLateWindow(getEarlyLateWindowMs(ACD)),
          tooEarlyLateWindow(getTooEarlyLateWindowMs(ACD)),
          appearTime(getAppearTimeMs(AR, offset)),
          plateNumber(plateNumber),
          initialXPos(xPos),
          initialYPos(yPos),
          aproachCircleColor(aproachCircle.sprite->getColor()),
          objColor(object.sprite->getColor()),
          hitNum(hitNum),
          trajectory_dot_max_transparency(trajectory_dot_max_transparency),
          debug_mode(debug_mode)
    {
        ObjectScale.x = static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.x;
        ObjectScale.y = static_cast<float>(pixelSize) / object.sprite->getLocalBounds().size.y;

        object.sprite->setScale(ObjectScale);

        radius = static_cast<float>(pixelSize) / 2.7f;

        aproachCircleScale.x = (static_cast<float>(pixelSize) / aproachCircle.sprite->getLocalBounds().size.x);
        aproachCircleScale.y = (static_cast<float>(pixelSize) / aproachCircle.sprite->getLocalBounds().size.y);

        aproachCircleScale.x = std::max(0.0f, aproachCircleScale.x - aproachCircleScaleDiminish); // fix this later (making it fit perfectly)
        aproachCircleScale.y = std::max(0.0f, aproachCircleScale.y - aproachCircleScaleDiminish);

        aproachCircle.sprite->setScale(aproachCircleScale);

        sf::Vector2f dotScale = object.sprite->getScale();
        
        float realDiameter = object.sprite->getGlobalBounds().size.x; 
        float totalDistance = std::hypot(finalXPos - initialXPos, finalYPos - initialYPos);
        float availableSpace = std::max(0.0f, totalDistance - realDiameter);
        int dot_quantity = (static_cast<int>(std::floor(availableSpace / realDiameter))) * 3;

        // Calculamos o passo (step). Ex: Se tem 3 pontos, dividimos por 4 espaços.
        // Isso distribui eles igualmente: 25%, 50%, 75%.
        float step = 1.0f / static_cast<float>(dot_quantity + 1);

        for (int i = dot_quantity; i >= 1; i--) {
            float t = step * static_cast<float>(i);

            int totalDuration = ((offset - appearTime) * 2) - 150; // -30 pequeno ajuste
            int timeToReachDot = static_cast<int>(static_cast<float>(totalDuration) * t);

            int vanishTime = appearTime + timeToReachDot;

            // 1. Posição Atual
            sf::Vector2f newPos = getXYTrajectory(
                static_cast<float>(initialXPos),
                static_cast<float>(initialYPos),
                static_cast<float>(finalXPos),
                static_cast<float>(finalYPos),
                t
            );

            // 2. Calcula a posição do próximo passo para determinar a direção
            sf::Vector2f nextPos = getXYTrajectory(
                static_cast<float>(initialXPos),
                static_cast<float>(initialYPos),
                static_cast<float>(finalXPos),
                static_cast<float>(finalYPos),
                t + 0.01 // Olha um 'step' para frente
            );

            // 3. Calcula o ângulo (atan2 retorna radianos, convertemos para graus)
            float angle = std::atan2 (nextPos.y - newPos.y, nextPos.x - newPos.x) * 180.0f / 3.14159265f;

            Object newDot("assets/sprites/game/objects/plates/trajectory_dot.png", 
                        static_cast<int>(newPos.x), 
                        static_cast<int>(newPos.y), 
                        82, 117, dotScale.x, dotScale.y);

            // 4. Aplica a rotação
            sf::Angle newAngle = sf::degrees(angle - 90); // -90 pra deixar ele reto

            newDot.sprite->setRotation(newAngle);

            trajectoryDotArray.push_back({ newDot, newDot.sprite->getColor(), t, 0, vanishTime}); // start transparent
        }
    } // Fim do construtor
    
    bool DetectHover(sf::RenderWindow& window) {
        // Pega posição do mouse em coordenadas reais da tela
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseFloat(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        
        sf::Vector2f spriteCenter = object.sprite->getPosition();
        sf::FloatRect bounds = object.sprite->getGlobalBounds();

        // Colisão circular em coordenadas reais
        float dx = mouseFloat.x - spriteCenter.x;
        float dy = mouseFloat.y - spriteCenter.y;

        return (dx * dx + dy * dy) <= (radius * radius);
    }

    void click(sf::Time elapsed, sf::RenderWindow& window) {
        const int milliTime = elapsed.asMilliseconds();
        const int offset = getOffset();

        if (not DetectHover(window)) {
            return;
        }

        sf::Vector2f baseCenter = GetScaledCenterForObject(object);
        
        if (getState() == NoteState::Judging || milliTime >= offset - tooEarlyLateWindow && milliTime <= offset + tooEarlyLateWindow && 
            getState() != NoteState::Judging && getState() != NoteState::Hitting && 
            getState() != NoteState::Hit) {

            setState(NoteState::Judging);
            int hitWindow = std::abs(milliTime - offset);
            if (hitWindow <= perfectHitWindow) {
                setHitResult(HitResult::Perfect);
                addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
                setState(NoteState::Hitting);
                hitTime = milliTime;
                playHitSound(hitNum); 
                return;
            }
            if (hitWindow > perfectHitWindow && hitWindow <= earlyLateWindow) {
                if (milliTime < offset) {
                    setHitResult(HitResult::PerfectEarly);
                    addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
                } else {
                    setHitResult(HitResult::PerfectLate);
                    addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
                }
                setState(NoteState::Hitting);
                hitTime = milliTime;
                playHitSound(hitNum); 
                return;
            }
            if (hitWindow > earlyLateWindow && hitWindow <= tooEarlyLateWindow) {
                if (milliTime < offset) {
                    setHitResult(HitResult::TooEarly);
                    addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
                } else {
                    setHitResult(HitResult::TooLate);
                    addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
                }
                setState(NoteState::Hitting);
                hitTime = milliTime;
                playHitSound(hitNum); 
                return;
            }
        }
    }
    
    void update(sf::Time elapsed, sf::RenderWindow& window) override {
        const int milliTime = elapsed.asMilliseconds();
        const int offset = getOffset();

        _debug_milliTime = milliTime;

        if (milliTime < appearTime && getState() != NoteState::Waiting) {
            setState(NoteState::Waiting);
        }
        else if (milliTime >= appearTime && milliTime < offset - tooEarlyLateWindow && getState() != NoteState::Active) {
            setState(NoteState::Active);
        }
        else if (milliTime >= offset - tooEarlyLateWindow && milliTime <= offset + tooEarlyLateWindow && 
            getState() != NoteState::Judging && getState() != NoteState::Hitting && 
            getState() != NoteState::Hit) 
        { 
            setState(NoteState::Judging);
        }    
    
        if (milliTime > offset + tooEarlyLateWindow &&  
            getState() != NoteState::Missing &&
            getState() != NoteState::Missed &&
            getState() != NoteState::Hit &&
            getState() != NoteState::Hitting) {

            sf::Vector2f baseCenter = GetScaledCenterForObject(object);
            
            setState(NoteState::Missing);
            setHitResult(HitResult::Missed);
            addHitResultTexture(hitResultObject,getHitResult(),baseCenter);
            hitTime = milliTime;
        }

        // Transições finais (SEM ELSE - todos devem ser IFs independentes)
        if (getState() == NoteState::Hitting && milliTime >= hitTime + hittingTime) {
            setState(NoteState::Hit);
        }
        if (getState() == NoteState::Missing && milliTime >= hitTime + hittingTime) {
            setState(NoteState::Missed);
        }

        // Atualização visual
        if (getState() == NoteState::Active || getState() == NoteState::Judging) {
            updatePlate(milliTime, offset, xPos, yPos, finalXPos, finalYPos,
                        window, object, aproachCircle, aproachCircleScale,
                        appearTime, initialXPos, initialYPos);
        }

        if (getState() == NoteState::Missing) {
            updatePlate(milliTime, offset, xPos, yPos, finalXPos, finalYPos,
                        window, object, aproachCircle, aproachCircleScale,
                        appearTime, initialXPos, initialYPos);

            updateAlpha(getState(), aproachCircleColor, aproachCircle, milliTime, appearTime, hitTime);
            updateAlpha(getState(), objColor, object, milliTime, appearTime, hitTime);
        }

        if (getState() == NoteState::Hitting) {
            float casted_hitTime = static_cast<float>(hitTime);
            float casted_hittingTime = static_cast<float>(hittingTime);
            float casted_milliTime = static_cast<float>(milliTime);

            float elapsed = casted_milliTime - casted_hitTime;
            float progress = std::clamp(elapsed / (casted_hittingTime / 2), 0.f, 1.f);

            sf::Vector2f NewScale;

            NewScale.x = ObjectScale.x + (plateObjectAfterHitMaxScale * progress);
            NewScale.y = ObjectScale.y + (plateObjectAfterHitMaxScale * progress);

            updateAlpha(getState(), objColor, object, milliTime, appearTime, hitTime, casted_hittingTime / 2);
            object.sprite->setScale(NewScale);
        }

        for (auto& dot : trajectoryDotArray) {
            if (getState() == NoteState::Hitting) {
                int fadeStart = hitTime;
                float fadeElapsed = static_cast<float>(milliTime - fadeStart);
                float fadeProgress = std::clamp(fadeElapsed / missFadeDuration, 0.0f, 1.0f);
                sf::Color newColor = dot.fullOpacityColor;
                newColor.a = static_cast<uint8_t>(dot.fullOpacityColor.a * (1.0f - fadeProgress));
                dot.dotObject.sprite->setColor(newColor);
            }
            else {
                if (milliTime >= dot.vanishTime) {
                    dot.visible = false;
                    continue;
                }

                int fullOpacityStart = dot.vanishTime - fullOpacityTrajectoryDotOffset + trajectoryDotTimingOffset;
                int fadeInStart = appearTime;
                
                float alpha = 0.0f;
                
                if (milliTime < fadeInStart) {
                    alpha = 0.0f;
                }
                else if (milliTime >= fadeInStart && milliTime < fullOpacityStart) {
                    float fadeInProgress = float(milliTime - fadeInStart) / float(fullOpacityStart - fadeInStart);
                    fadeInProgress = std::clamp(fadeInProgress, 0.0f, 1.0f);
                    alpha = fadeInProgress * trajectory_dot_max_transparency; 
                }
                else if (milliTime >= fullOpacityStart && milliTime < dot.vanishTime) {
                    alpha = trajectory_dot_max_transparency; 
                }

                sf::Color newColor = dot.fullOpacityColor;
                newColor.a = static_cast<uint8_t>(255 * alpha); 
                dot.dotObject.sprite->setColor(newColor);
            }
        }
    }

    void render(sf::RenderWindow& window) override {
        if (getState() == NoteState::Active || getState() == NoteState::Judging ||
            getState() == NoteState::Hitting || getState() == NoteState::Missing) {
            for (auto& dot : trajectoryDotArray) {
                if (dot.visible) {
                    window.draw(*dot.dotObject.sprite);
                }
            }    
            
            if (hitResultObject.initialized) {
                window.draw(*hitResultObject.object.sprite);
            }
            
            window.draw(*object.sprite);
            window.draw(*aproachCircle.sprite);

            if (*this->debug_mode) { 
                
                sf::FloatRect bounds = object.sprite->getGlobalBounds();
                sf::Vector2f center(
                    bounds.position.x + bounds.size.x / 2.0f,
                    bounds.position.y + bounds.size.y / 2.0f
                );
        
                sf::CircleShape debugCircle(radius);
                debugCircle.setOrigin({radius, radius});
                debugCircle.setPosition(center);
                debugCircle.setFillColor(sf::Color::Transparent);
                debugCircle.setOutlineColor(sf::Color::Red);
                if (DetectHover(window)) {
                    debugCircle.setOutlineColor(sf::Color::Green);
                }
                debugCircle.setOutlineThickness(2.0f);
                window.draw(debugCircle);
                
                // DEBUG: Linha entre mouse e objeto
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                sf::Vector2f mouseFloat(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                sf::Vector2f spritePos = object.sprite->getPosition();
                
                sf::Vertex line[] = {
                    sf::Vertex({mouseFloat, sf::Color::Yellow}),
                    sf::Vertex({spritePos, sf::Color::Yellow})
                };
                window.draw(line, 2, sf::PrimitiveType::Lines);
                
                // DEBUG: Texto com informações
                static sf::Font debugFont;
                static bool fontLoaded = false;
                if (!fontLoaded) {
                    if (debugFont.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
                        fontLoaded = true;
                    }
                }
                
                if (fontLoaded) {
                    sf::Vector2f midPoint = (mouseFloat + spritePos) / 2.0f;
                    float dx = mouseFloat.x - spritePos.x;
                    float dy = mouseFloat.y - spritePos.y;
                    float distance = std::sqrt(dx * dx + dy * dy);
                    
                    sf::Text debugText(debugFont);
                    debugText.setCharacterSize(16);
                    debugText.setFillColor(sf::Color::White);
                    debugText.setOutlineColor(sf::Color::Black);
                    debugText.setOutlineThickness(2.0f);
                    debugText.setPosition({midPoint.x, midPoint.y - 50});
                    
                    std::string info = "Mouse: (" + std::to_string((int)mouseFloat.x) + ", " + std::to_string((int)mouseFloat.y) + ")\n";
                    info += "Object: (" + std::to_string((int)spritePos.x) + ", " + std::to_string((int)spritePos.y) + ")\n";
                    info += "Diff: (" + std::to_string((int)dx) + ", " + std::to_string((int)dy) + ")\n";
                    info += "Dist: " + std::to_string((int)distance) + " | Radius: " + std::to_string((int)radius);
                    
                    
                    sf::Text inPlateDebugText(debugFont);
                    inPlateDebugText.setCharacterSize(16);
                    inPlateDebugText.setFillColor(sf::Color::White);
                    inPlateDebugText.setOutlineColor(sf::Color::Black);
                    inPlateDebugText.setOutlineThickness(2.0f);
                    sf::Vector2f currentPos = object.sprite->getPosition();
                    inPlateDebugText.setPosition({currentPos.x, currentPos.y - 50.0f});
                    
                    std::string inPlateInfo = "State: (" + _debug_read_state(getState()) + ")\n" // Fechar e reabrir as strings
                                + "HitResult: (" + _debug_read_hitresult(getHitResult()) + ")\n"
                                + "Runtime HitResult: (" 
                                + _debug_read_runtime_hitresult(
                                    _debug_milliTime,
                                    getOffset(),
                                    perfectHitWindow,
                                    earlyLateWindow,
                                    tooEarlyLateWindow
                                ) + ")";

                    inPlateDebugText.setString(inPlateInfo);
                    debugText.setString(info);
                    window.draw(debugText);
                    window.draw(inPlateDebugText);
                    
                }
            }
        }   
    }

private:
    float xPos;
    float yPos;
    float initialXPos;
    float initialYPos;
    float finalYPos;
    float finalXPos;
    float trajectory_dot_max_transparency;
    float radius;
    int plateNumber;
    int PS;
    int ACD;
    int perfectHitWindow;
    int earlyLateWindow;
    int tooEarlyLateWindow;
    int appearTime;
    int hitTime = -1;
    int pixelSize;
    int hitNum;
    std::shared_ptr<bool> debug_mode;
    int _debug_milliTime;
    std::string _debug_runtime_hitresult;

    Button object;
    Button aproachCircle;
    HitResultObject hitResultObject;
    sf::Vector2f aproachCircleScale;
    sf::Vector2f ObjectScale;
    sf::Color aproachCircleColor;
    sf::Color objColor;
    std::vector<TrajectorDot> trajectoryDotArray;
};
