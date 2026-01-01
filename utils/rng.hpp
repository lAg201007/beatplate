#pragma once
#include <random>
#include <chrono>

namespace Random {

    inline std::mt19937& engine() {
        static std::mt19937 rng(
            static_cast<uint32_t>(
                std::chrono::high_resolution_clock::now()
                    .time_since_epoch().count()
            )
        );
        return rng;
    }

    inline int rangeInt(int min, int max) {
        if (min > max)
            std::swap(min, max);
    
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }

    inline float rangeFloat(float a, float b) {
        if (a > b)
            std::swap(a, b);

        std::uniform_real_distribution<float> dist(a, b);
        return dist(engine());
    }

    inline bool chance(float probability) {
        std::bernoulli_distribution dist(probability);
        return dist(engine());
    }

}

