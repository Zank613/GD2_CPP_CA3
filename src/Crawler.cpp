#include "Crawler.h"
#include "Seeder.h"
#include <random>

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

    int x = position.first;
    int y = position.second;

    // Move 1 unit
    switch (direction) {
        case Direction::NORTH:
            y -= 1;
            break;
        case Direction::EAST:
            x += 1;
            break;
        case Direction::SOUTH:
            y += 1;
            break;
        case Direction::WEST:
            x -= 1;
            break;
    }

    setPosition({x, y});
}

std::string Crawler::getType() const {
    return "Crawler";
}