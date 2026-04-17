#include "Seeder.h"

#include <chrono>

Seeder::Seeder() : currentSeed(0) {
    seedFromClock();
}

Seeder& Seeder::getInstance() {
    static Seeder instance;
    return instance;
}

void Seeder::setSeed(unsigned int seed) {
    currentSeed = seed;
    rng.seed(currentSeed);
}

void Seeder::seedFromClock() {
    currentSeed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    );
    rng.seed(currentSeed);
}

std::mt19937& Seeder::getRNG() {
    return rng;
}

unsigned int Seeder::getCurrentSeed() const {
    return currentSeed;
}