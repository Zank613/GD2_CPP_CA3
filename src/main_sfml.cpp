#include "Seeder.h"
#include "SfmlGame.h"

#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    bool customSeed = false;
    unsigned int seed = 0;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--seed" && i + 1 < argc) {
            std::stringstream ss(argv[i + 1]);
            ss >> seed;
            customSeed = true;
            i++;
        }
    }

    if (customSeed) {
        Seeder::getInstance().setSeed(seed);
    } else {
        Seeder::getInstance().seedFromClock();
    }

    std::cout << "Seed: " << Seeder::getInstance().getCurrentSeed() << "\n";

    SfmlGame game;
    game.run();

    return 0;
}