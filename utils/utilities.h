#pragma once

#ifndef UTILITIES
#define UTILITIES

#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

void taskDelay(int milliseconds, std::function<void()> callback);

#endif