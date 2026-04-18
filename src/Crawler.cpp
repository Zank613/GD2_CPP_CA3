#include "Crawler.h"
#include "Seeder.h"
#include <random>
#include "Utils.h"

Crawler::Crawler(int id, std::pair<int, int> position, Direction direction, int health)
    : Bug(id, position, direction, health) {
}

void Crawler::move() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dirDist(1, 4);

    // If blocked, keep picking random directions
    while (isWayBlocked()) {
        setDirection(static_cast<Direction>(dirDist(rng)));
    }

    std::pair<int, int> newPosition = utils::nextPosition(position, direction, 1);
    setPosition(newPosition);
}

std::string Crawler::getType() const {
    return "Crawler";
}