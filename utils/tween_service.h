#ifndef TWEEN_H
#define TWEEN_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <algorithm>
#include <cmath>

class Tween {
public:
    using EasingFunc = std::function<float(float)>;

    Tween(sf::Sprite& sprite, float duration, EasingFunc easing = linear)
    : sprite(sprite), duration(duration), easing(easing), elapsed(0.f), active(false), type(Type::Position) {}


    // Tipos de tween
    enum class Type { Position, Scale, Rotation, Transparency };

    // Inicializadores
    void initPosition(const sf::Vector2f& start, const sf::Vector2f& end) {
        type = Type::Position;
        startPos = start;
        endPos = end;
    }

    void initScale(float start, float end) {
        type = Type::Scale;
        startScale = start;
        endScale = end;
    }

    void initRotation(float start, float end) {
        type = Type::Rotation;
        startRot = start;
        endRot = end;
    }

    void initTransparency(float start, float end) {
        type = Type::Transparency;
        startTransparency = start;
        endTransparency = end;
    }

    void update(float dt) {
        if (!active) return;

        elapsed += dt;
        float t = std::min(elapsed / duration, 1.f);
        float easedT = easing(t);

        switch (type) {
            case Type::Position: {
                sf::Vector2f pos = lerp(startPos, endPos, easedT);
                sprite.setPosition(pos);
                break;
            }
            case Type::Scale: {
                float s = lerp(startScale, endScale, easedT);
                sprite.setScale(sf::Vector2f(s, s));
                break;
            }
            case Type::Rotation: {
                float angle = lerp(startRot, endRot, easedT);
                sprite.setRotation(sf::degrees(angle));
                break;
            }
            case Type::Transparency: {
                float tweenTransparency = lerp(startTransparency, endTransparency, easedT);
                int transparency = static_cast<int>(std::round(tweenTransparency * 255.0f));
                sf::Color color = sprite.getColor(); color.a = transparency;
                sprite.setColor(color);
            }
        }

        if (elapsed >= duration) {
            active = false; // Finalizou o tween
        }
    }

    void reset() {
        elapsed = 0.f;
        active = true;
    }

    void play() {
        active = true;
    }

    void pause() {
        active = false;
    }

    bool isActive() const {
        return active;
    }

    void setDuration(float newDuration) {
        duration = newDuration;
    }

    // Easings padrões
    static float linear(float t) { return t; }
    static float easeInQuad(float t) { return t * t; }
    static float easeOutQuad(float t) { return t * (2 - t); }
    static float easeInOutQuad(float t) {
        if (t < 0.5f)
            return 2 * t * t;
        else
            return -1 + (4 - 2 * t) * t;
    }

private:
    sf::Sprite& sprite;
    float duration;
    float elapsed;
    bool active;
    EasingFunc easing;

    Type type;

    // Variáveis para os valores inicial e final de cada tipo
    sf::Vector2f startPos, endPos;
    float startScale, endScale;
    float startRot, endRot;
    float startTransparency, endTransparency;

    // Função de interpolação genérica
    template<typename T>
    static T lerp(const T& a, const T& b, float t) {
        return a + (b - a) * t;
    }
};

class ValueTween {
public:
    using EasingFunc = std::function<float(float)>;

    ValueTween(float start, float end, float duration, EasingFunc easing = Tween::linear)
        : startValue(start), endValue(end), duration(duration),
          easing(easing), elapsed(0.f), active(false), currentValue(start) {}

    void update(float dt) {
        if (!active) return;

        elapsed += dt;
        float t = std::min(elapsed / duration, 1.f);
        float easedT = easing(t);

        currentValue = lerp(startValue, endValue, easedT);

        if (elapsed >= duration) {
            active = false;
        }
    }

    void reset() {
        elapsed = 0.f;
        active = true;
        currentValue = startValue;
    }

    void play() { active = true; }
    void pause() { active = false; }

    bool isActive() const { return active; }

    float getValue() const { return currentValue; }

private:
    float startValue;
    float endValue;
    float currentValue;
    float duration;
    float elapsed;
    bool active;
    EasingFunc easing;

    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
};

#endif

