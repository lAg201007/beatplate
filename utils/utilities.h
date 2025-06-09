#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

void taskDelay(int milliseconds, std::function<void()> callback) {
    std::thread([milliseconds, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        callback();
    }).detach();
}

#endif