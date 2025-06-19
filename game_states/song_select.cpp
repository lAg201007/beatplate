#include <SFML/Window/Mouse.hpp>
#include "song_select.h"
#include "game.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"

sf::Font SongSlot::Montserrat;
std::unordered_map<std::string, std::shared_ptr<sf::Texture>> SongList::BackgroundCache;

SongSelect::SongSelect(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
    Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f), 
    List("assets/songs", {974,432},mWindow)
{
    if (!SongSlot::Montserrat.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
        std::cerr << "não foi possível carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
    }
}

void SongSlot::clicked(std::vector<std::shared_ptr<SongSlot>>& slots, std::shared_ptr<SongSlot>& selectedSlot, SongList& list, StateStack& mStack, sf::RenderWindow& mWindow) {
    auto it = std::find(slots.begin(), slots.end(), shared_from_this());
    if (it != slots.end()) {
        if (selectedSlot != *it) {
            selectedSlot = *it;
            list.updateSlotPositions();
        } else {
            mStack.pushState(std::make_unique<Game>(mStack,mWindow, selectedSlot->FolderLocation));
        }
    }
}

void SongSelect::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                mStack.popState();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                List.scrollListUpByOne();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                List.scrollListDownByOne();
            }
        }
    }

    if (const auto mouseWheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>()) {
        int delta = static_cast<int>(mouseWheelScrolled->delta);
        for (int i = 0; i < std::abs(delta); ++i) {
            pendingScrolls.push_back((delta > 0) ? 1 : -1);
        }
    }

    if (event.is<sf::Event::Closed>()) {
        mWindow.close();
    }
}

void SongSelect::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    Cursor.sprite->setPosition({static_cast<float>(mouse_pos.x),static_cast<float>(mouse_pos.y)});

    List.listUpdate(dt.asSeconds());

    if (mouseScrollQueueCooldown > 0.f) {
        mouseScrollQueueCooldown -= dt.asSeconds();
    }
    if (mouseScrollQueueCooldown < 0.f) {
        mouseScrollQueueCooldown = 0.f;
    }

    if (mouseScrollQueueCooldown <= 0.f && !pendingScrolls.empty()) {
        int scrollDir = pendingScrolls.front();
        pendingScrolls.erase(pendingScrolls.begin());

        if (scrollDir > 0) {
            List.scrollListUpByOne();
        } else {
            List.scrollListDownByOne();
        }

        float baseCooldown = 0.07f;
        float minCooldown = 0.01f;
        int maxScrollsForMinCooldown = 5;
        size_t queueSize = pendingScrolls.size();
        float t = (queueSize > 0) ? std::min(queueSize, (size_t)maxScrollsForMinCooldown) / (float)maxScrollsForMinCooldown : 0.f;
        mouseScrollQueueCooldown = baseCooldown * (1.0f - t) + minCooldown * t;
    }

    for (auto& slot : List.ButtonVector) {
        if (slot->SongButton.DetectButtonClick(mWindow)) {
            slot->clicked(List.ButtonVector, List.SelectedSlot, List, mStack, mWindow);
        }
    }
}

void SongSelect::render() {   
    mWindow.clear(sf::Color::Transparent);
    List.RenderList(mWindow);
    mWindow.draw(*Cursor.sprite);
}
