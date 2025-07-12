#include <SFML/Window/Mouse.hpp>
#include "song_select.h"
#include "game.h"
#include "menu.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"

sf::Font SongSlot::Montserrat;
std::unordered_map<std::string, std::shared_ptr<sf::Texture>> SongList::BackgroundCache;

SongSelect::SongSelect(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
      Cursor("assets/sprites/cursor.png", 400, 300, 256, 256, 0.05f, 0.05f)
{
    List = std::make_unique<SongList>("assets/songs", sf::Vector2f(974,432), mWindow, this);

    if (!SongSlot::Montserrat.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
        std::cerr << "não foi possível carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
    }

    for (auto& slot : List->ButtonVector) {
        slot->SetButtonAndWidjetsRelativePosition(slot->Position);
    }
}

void SongSlot::clicked(std::vector<std::shared_ptr<SongSlot>>& slots, std::shared_ptr<SongSlot>& selectedSlot, SongList& list, StateStack& mStack, sf::RenderWindow& mWindow) {
    auto it = std::find(slots.begin(), slots.end(), shared_from_this());
    if (it != slots.end()) {
        if (selectedSlot != *it) {
            selectedSlot = *it;
            list.updateSlotPositions();
        } else {
            std::string FolderLoc = selectedSlot->FolderLocation;
            Object Background = (list.isActiveBackground1 ? list.select_slot_background1 : list.select_slot_background2);
            mStack.popState();
            mStack.pushState(std::make_unique<Game>(mStack, mWindow, FolderLoc, Background)); 
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
                List->scrollListUpByOne();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                List->scrollListDownByOne();
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

    List->listUpdate(dt.asSeconds());

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
            List->scrollListUpByOne();
        } else {
            List->scrollListDownByOne();
        }

        float baseCooldown = 0.07f;
        float minCooldown = 0.01f;
        int maxScrollsForMinCooldown = 5;
        size_t queueSize = pendingScrolls.size();
        float t = (queueSize > 0) ? std::min(queueSize, (size_t)maxScrollsForMinCooldown) / (float)maxScrollsForMinCooldown : 0.f;
        mouseScrollQueueCooldown = baseCooldown * (1.0f - t) + minCooldown * t;
    }

    for (auto& slot : List->ButtonVector) {
        if (slot->SongButton.DetectButtonClick(mWindow)) {
            slot->clicked(List->ButtonVector, List->SelectedSlot, *List, mStack, mWindow);
        }
    }
}

void SongSelect::render() {   
    mWindow.clear(sf::Color::Transparent);
    List->RenderList(mWindow);
    mWindow.draw(*Cursor.sprite);
}

SongSelect::~SongSelect() {
    isActive = false;
    // Para todos os tweens dos slots
    for (auto& slot : List->ButtonVector) {
        std::cout << "Pausing tweens for slot: " << slot->SongName << std::endl;
        slot->PositionTweenX.pause();
        slot->PositionTweenY.pause();
        slot->SelectedOffsetTween.pause();
        slot->WhiteIntensityTween.pause();
        // Adicione outros tweens aqui se criar mais no futuro
    }
    // Para os tweens do SongList
    std::cout << "Pausing tweens for SongList background\n";
    List->backgroundTransparencyTweenIn.pause();
    List->backgroundTransparencyTweenOut.pause();
}

void SongList::setBackgroundForSelectedSlot() {
    if (!parentSelect->isActive) return;
    std::string bgPath = SelectedSlot->FolderLocation + "/background.png";
    auto it = BackgroundCache.find(bgPath);
    if (it != BackgroundCache.end()) {
        if (isActiveBackground1) {
            select_slot_background2.spriteTexture = it->second;
        } else {
            select_slot_background1.spriteTexture = it->second;
        }
    } else {
        auto tex = std::make_shared<sf::Texture>();
        if (!tex->loadFromFile(bgPath)) {
            std::cerr << "Não foi possível carregar a imagem " << bgPath << std::endl;
        }
        BackgroundCache[bgPath] = tex;
        if (isActiveBackground1)
            select_slot_background2.spriteTexture = tex;
        else
            select_slot_background1.spriteTexture = tex;
    }
    if (isActiveBackground1) {
        select_slot_background2.sprite->setTexture(*select_slot_background2.spriteTexture);
        ResizeSpriteToFitWindow(select_slot_background2, window);
        backgroundTransparencyTweenIn.play();
        backgroundTransparencyTweenOut.reset(); backgroundTransparencyTweenOut.pause();
        isActiveBackground1 = false;
    }
    else {
        select_slot_background1.sprite->setTexture(*select_slot_background1.spriteTexture);
        ResizeSpriteToFitWindow(select_slot_background1, window);
        backgroundTransparencyTweenOut.play();
        backgroundTransparencyTweenIn.reset(); backgroundTransparencyTweenIn.pause();
        isActiveBackground1 = true;
    }
}

void SongList::listUpdate(float dt) {
    if (!parentSelect || !parentSelect->isActive) return;
    backgroundTransparencyTweenIn.update(dt);
    backgroundTransparencyTweenOut.update(dt);
    updateSlotTweens(dt);

    if (backgroundTransparencyTweenOut.isActive()) {
        select_slot_background2.sprite->setColor(sf::Color(255, 255, 255, static_cast<int>(std::round(backgroundTransparencyTweenOut.getValue() * 255))));
    }
    else if (backgroundTransparencyTweenIn.isActive()) {
        select_slot_background2.sprite->setColor(sf::Color(255, 255, 255, static_cast<int>(std::round(backgroundTransparencyTweenIn.getValue() * 255))));
    }

    if (backgroundChangePending) {
        BackgroundChangeTimer += dt;
        if (BackgroundChangeTimer >= BackgroundChangeCooldown) {
            setBackgroundForSelectedSlot();
            if (!parentSelect || !parentSelect->isActive) return;
            if (AudioManager::getInstance().isPlaying()) {
                if (AudioManager::getInstance().getCurrentMusicPath() != SelectedSlot->FolderLocation + "/song.mp3") {
                    AudioManager::getInstance().playMusic(SelectedSlot->FolderLocation + "/song.mp3", true, SelectedSlot); 
                }
            }
            else {
                AudioManager::getInstance().playMusic(SelectedSlot->FolderLocation + "/song.mp3", true, SelectedSlot); 
            }
            backgroundChangePending = false;
        }
    }
}
