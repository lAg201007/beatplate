#include "utilities.h"
#include <thread>
#include <chrono>

void taskDelay(int milliseconds, std::function<void()> callback) {
    std::thread([milliseconds, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        callback();
    }).detach();
}


