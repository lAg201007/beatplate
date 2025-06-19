#include "utilities.h"
#include <thread>
#include <chrono>

void taskDelay(int milliseconds, std::function<void()> callback) {
    std::thread([milliseconds, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        callback();
    }).detach();
}


void fitTextToWidth(sf::Text& text, float maxWidth, unsigned int minSize) {
    unsigned int size = text.getCharacterSize();

    while (text.getLocalBounds().size.x > maxWidth && size > minSize) {
        size--;
        text.setCharacterSize(size);
    }
}

