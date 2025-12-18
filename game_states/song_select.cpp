#include <SFML/Window/Mouse.hpp>
#include "song_select.h"
#include "game.h"
#include "menu.h"
#include "../utils/tween_service.h"
#include "../shaders/shader_manager.h"
#include "../utils/utilities.h"
#include "../utils/tween_storage.h"
#include "../state_stack.h"
#include "../utils/audio_manager.h"
#include "../utils/texture_caching.h"
#include "../utils/scale_manager.h"
#include "print"

sf::Font SongSlot::Montserrat;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// RENDERING AND LOGIC ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor for SongSlot
SongSelect::SongSelect(StateStack& stack, sf::RenderWindow& window)
    : State(stack, window),
      Cursor("assets/sprites/cursor.png", 400, 300, 0, 0), 
      click_buffer("assets/sounds/song_select/click.mp3"),
      back_buffer("assets/sounds/song_select/back.wav"),
      play_buffer("assets/sounds/song_select/play.wav"),
      play_sound(play_buffer),
      back_sound(back_buffer),
      click_sound(click_buffer)
{
    List = std::make_unique<SongList>("assets/songs", sf::Vector2f(974,432), mWindow, this);

    if (!SongSlot::Montserrat.openFromFile("assets/fonts/Montserrat-SemiBold.ttf")) {
        std::cerr << "não foi possível carregar a fonte Montserrat-SemiBold.ttf" << std::endl;
    }

    for (auto& slot : List->ButtonVector) {
        slot->SetButtonAndWidjetsRelativePosition(slot->Position);
    }
}

// Event Handler for SongSelect
void SongSelect::handleEvent(const sf::Event& event) {
    if (event.is<sf::Event::KeyPressed>()) {
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                pendingPop = true; // Marque para pop depois
                AudioManager::getInstance().playTemporarySound(back_buffer);
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Up) {
                List->scrollListUpByOne();
                click_sound.play();
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Down) {
                List->scrollListDownByOne();
                click_sound.play();
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

// Update Event for SongSelect
void SongSelect::update(sf::Time dt) {
    mouse_pos = sf::Mouse::getPosition(mWindow);
    sf::Vector2f unscaledMouse = ScaleManager::UnscalePosition(mouse_pos.x, mouse_pos.y);
    Cursor.sprite->setPosition(unscaledMouse.x, unscaledMouse.y);

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
            click_sound.play();
        } else {
            List->scrollListDownByOne();
            click_sound.play();
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
            slot->clicked(List->ButtonVector, List->SelectedSlot, *List, mStack, mWindow, play_buffer, click_sound);
        }
    }

    if (pendingPop) {
        bool anyTweenActive = false;
        if (List->backgroundTransparencyTweenIn.isActive() || List->backgroundTransparencyTweenOut.isActive())
            anyTweenActive = true;
        for (auto& slot : List->ButtonVector) {
            if (slot->PositionTweenX.isActive() || slot->PositionTweenY.isActive() ||
                slot->SelectedOffsetTween.isActive() || slot->WhiteIntensityTween.isActive()) {
                anyTweenActive = true;
                break;
            }
        }
        if (!anyTweenActive) {
            mStack.popState();
            pendingPop = false;
        }
    }
}

// Render Event for SongSelect
void SongSelect::render() {   
    mWindow.clear(sf::Color::Transparent);
    List->RenderList(mWindow);
    mWindow.draw(*Cursor.sprite);
}

// Destructor for SongSelect
SongSelect::~SongSelect() {
    isActive = false;
    for (auto& slot : List->ButtonVector) {
        slot->PositionTweenX.pause();
        slot->PositionTweenY.pause();
        slot->SelectedOffsetTween.pause();
        slot->WhiteIntensityTween.pause();
    }
    List->backgroundTransparencyTweenIn.pause();
    List->backgroundTransparencyTweenOut.pause();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// SONG SLOT FUNCTIONS ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SongSlot Functions:
void SongSlot::clicked(std::vector<std::shared_ptr<SongSlot>>& slots, std::shared_ptr<SongSlot>& selectedSlot, SongList& list, StateStack& mStack, sf::RenderWindow& mWindow, sf::SoundBuffer& play_sound_buffer, sf::Sound& click_sound) {
    auto it = std::find(slots.begin(), slots.end(), shared_from_this());
    if (it != slots.end()) {
        if (selectedSlot != *it) {
            selectedSlot = *it;
            list.updateSlotPositions();
            click_sound.play();
        } else {
            std::string FolderLoc = selectedSlot->FolderLocation;
            Object Background = (list.isActiveBackground1 ? list.select_slot_background1 : list.select_slot_background2);
            AudioManager::getInstance().playTemporarySound(play_sound_buffer);
            mStack.pushState(std::make_unique<Game>(mStack, mWindow, FolderLoc, Background)); 
        }
    }
}

void SongSlot::SetButtonAndWidjetsRelativePosition(sf::Vector2f newPos) {
    Position = newPos;
    float offset = SelectedOffsetTween.getValue();
    sf::Vector2f offsetVec = {offset, 0.f};

    SongButton.sprite->setPosition(Position + offsetVec + sf::Vector2f({0,0}));
    ArtistLabel.setPosition(Position + offsetVec + sf::Vector2f({-60,5}));
    DificultyLabel.setPosition(Position + offsetVec + sf::Vector2f({-90,-20}));
    SongNameLabel.setPosition(Position + offsetVec + sf::Vector2f({-60,-20}));
    MapperLabel.setPosition(Position + offsetVec + sf::Vector2f({0,5}));

    if (SongNameLabel.getCharacterSize() != fitTextToWidth(SongNameLabel, 330)) {
        SongNameLabel.setCharacterSize(fitTextToWidth(SongNameLabel, 330));
    } 
    if (ArtistLabel.getCharacterSize() != fitTextToWidth(ArtistLabel, 300)) {
        ArtistLabel.setCharacterSize(fitTextToWidth(ArtistLabel, 300));
    }
    if (MapperLabel.getCharacterSize() != fitTextToWidth(MapperLabel, 300)) {
        MapperLabel.setCharacterSize(fitTextToWidth(MapperLabel, 300));
    }

    //std::println("Position: {}, {},  ArtistLabel Position, {}, {}", Position.x, Position.y, ArtistLabel.getPosition().x, ArtistLabel.getPosition().y);
}

void SongSlot::setPositionTweened(sf::Vector2f newPos) {
    PositionTweenX = ValueTween(Position.x, newPos.x, 0.5f, Tween::easeOutQuad);
    PositionTweenY = ValueTween(Position.y, newPos.y, 0.5f, Tween::easeOutQuad);
    PositionTweenX.play();
    PositionTweenY.play();
}

void SongSlot::update(float dt) {
    PositionTweenX.update(dt);
    PositionTweenY.update(dt);
    SelectedOffsetTween.update(dt);
    WhiteIntensityTween.update(dt);
    whiteIntensity = WhiteIntensityTween.getValue();
    whiteIntensityCache = whiteIntensity;
    if (PositionTweenX.isActive() || PositionTweenY.isActive() || SelectedOffsetTween.isActive()) {
        SetButtonAndWidjetsRelativePosition({PositionTweenX.getValue(), PositionTweenY.getValue()});
    }
}

void SongSlot::renderButton(sf::RenderWindow& window) {
    if (WhiteIntensityTween.isActive() || whiteIntensity > 0.f && whiteIntensityCache != whiteIntensity) {
        whiteMaskShader = ShaderUtils::createWhiteMaskShader(static_cast<int>(std::round(whiteIntensity)), window);
        window.draw(*SongButton.sprite, &whiteMaskShader);
    }
    else if (whiteIntensity > 0.f && whiteIntensityCache == whiteIntensity) {
        window.draw(*SongButton.sprite, &whiteMaskShader);
    }
    else {
        window.draw(*SongButton.sprite);
    }

    window.draw(SongNameLabel);
    window.draw(ArtistLabel);
    window.draw(MapperLabel);
    window.draw(DificultyLabel);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// SONG LIST FUNCTIONS /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SongList::updateSlotTweens(float dt) {
    for (auto& slot : ButtonVector) {
        slot->update(dt);
    }
}

void SongList::selectSlotByIndex(int index) {
    if (index >= 0 && index < ButtonVector.size()) {
        SelectedSlot = ButtonVector[index];
        updateSlotPositions();
    }
}

void SongList::scrollListUpByOne() {
    auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
    if (it != ButtonVector.end()) {
        int index = std::distance(ButtonVector.begin(), it);
        if (index > 0) {
            SelectedSlot = ButtonVector[index - 1];
        }
        updateSlotPositions();        
    }
}

void SongList::scrollListDownByOne() {
    auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
    if (it != ButtonVector.end()) {
        int index = std::distance(ButtonVector.begin(), it);
        if (index + 1 != ButtonVector.size()) {
            SelectedSlot = ButtonVector[index + 1];
        }   
        updateSlotPositions();     
    }
}

void SongList::RenderList(sf::RenderWindow& window) {
    window.draw(*select_slot_background1.sprite);
    window.draw(*select_slot_background2.sprite);

    for (auto slot : ButtonVector) {
        if (slot->Position.x > ScaleManager::GetBaseWidth() + 100
            || slot->Position.y > ScaleManager::GetBaseHeight() + 100
            || slot->Position.x < -100 
            || slot->Position.y < -100) 
        {
            continue;
        }
        slot->renderButton(window);
    }
}

void SongList::setBackgroundForSelectedSlot() {
    if (!parentSelect->isActive) return;
    std::string bgPath = SelectedSlot->FolderLocation + "/background.png";
    auto& texture = LoadTexture(bgPath);

    select_slot_background1.spriteTexture = &texture;
    select_slot_background2.spriteTexture = &texture;

    if (isActiveBackground1) {
        select_slot_background2.sprite->setTexture(*select_slot_background2.spriteTexture, true);
        ResizeSpriteToFitWindow(*select_slot_background2.sprite, window);
        backgroundTransparencyTweenIn.play();
        backgroundTransparencyTweenOut.reset(); backgroundTransparencyTweenOut.pause();
        select_slot_background2 = ShaderUtils::applyBlurToObject(window, select_slot_background2, 5.0f);
        isActiveBackground1 = false;
    }
    else {
        select_slot_background1.sprite->setTexture(*select_slot_background1.spriteTexture, true);
        ResizeSpriteToFitWindow(*select_slot_background1.sprite, window);
        backgroundTransparencyTweenOut.play();
        backgroundTransparencyTweenIn.reset(); backgroundTransparencyTweenIn.pause();
        select_slot_background1 = ShaderUtils::applyBlurToObject(window, select_slot_background1, 5.0f);
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

void SongList::updateSlotPositions() {
    auto it = std::find(ButtonVector.begin(), ButtonVector.end(), SelectedSlot);
    if (it != ButtonVector.end()) {
        int index = std::distance(ButtonVector.begin(), it);
        float selectedOffsetX = -40.f; // Offset do slot selecionado

        // move selected slot
        ButtonVector[index]->setPositionTweened(ListPosition);
        ButtonVector[index]->SelectedOffsetTween = ValueTween(ButtonVector[index]->SelectedOffsetTween.getValue(), selectedOffsetX, 0.5f, Tween::easeOutQuad);
        ButtonVector[index]->SelectedOffsetTween.play();

        ButtonVector[index]->WhiteIntensityTween = ValueTween(255.f, 50.f, 0.6f, Tween::easeOutQuad);
        ButtonVector[index]->WhiteIntensityTween.play();

        // Move slots acima do selecionado
        for (int i = index - 1, offset = -1; i >= 0; --i, --offset) {
            float distance = std::abs(i - index);
            // Nova fórmula para offsetX com transição mais suave
            float offsetX = selectedOffsetX * std::exp(-distance * 0.8f);
            
            sf::Vector2f newPos = ListPosition + sf::Vector2f(0.f, offset * button_offset);
            //std::println("newPos: {}, {}", newPos.x, newPos.y);
            
            ButtonVector[i]->setPositionTweened(newPos);
            ButtonVector[i]->SelectedOffsetTween = ValueTween(ButtonVector[i]->SelectedOffsetTween.getValue(), offsetX, 0.5f, Tween::easeOutQuad);
            ButtonVector[i]->SelectedOffsetTween.play();

            if (ButtonVector[i]->whiteIntensity > 0.f) {
                ButtonVector[i]->WhiteIntensityTween = ValueTween(ButtonVector[i]->WhiteIntensityTween.getValue(), 0.f, 0.3f, Tween::easeOutQuad);
                ButtonVector[i]->WhiteIntensityTween.play();
            }
        }

        // Move slots abaixo do selecionado
        for (int i = index + 1, offset = 1; i < ButtonVector.size(); ++i, ++offset) {
            float distance = std::abs(i - index);
            // Nova fórmula para offsetX com transição mais suave
            float offsetX = selectedOffsetX * std::exp(-distance * 0.8f);

            sf::Vector2f newPos = ListPosition + sf::Vector2f(0.f, offset * button_offset);
            ButtonVector[i]->setPositionTweened(newPos);
            ButtonVector[i]->SelectedOffsetTween = ValueTween(ButtonVector[i]->SelectedOffsetTween.getValue(), offsetX, 0.5f, Tween::easeOutQuad);
            ButtonVector[i]->SelectedOffsetTween.play();

            if (ButtonVector[i]->whiteIntensity > 0.f) {
                ButtonVector[i]->WhiteIntensityTween = ValueTween(ButtonVector[i]->WhiteIntensityTween.getValue(), 0.f, 0.3f, Tween::easeOutQuad);
                ButtonVector[i]->WhiteIntensityTween.play();
            }
        }

        BackgroundChangeTimer = 0.0f;
        backgroundChangePending = true;
    }
}
