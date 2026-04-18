#include "Hopper.h"
#include "Seeder.h"
#include <random>

Hopper::Hopper(int id, std::pair<int, int> position, Direction direction, int health, int hopLength)
    : Bug(id, position, direction, health), hopLength(hopLength) {
}

void Hopper::move() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dirDist(1, 4);

    while (isWayBlocked()) {
        setDirection(static_cast<Direction>(dirDist(rng)));
    }

    int x = position.first;
    int y = position.second;

    switch (direction) {
        case Direction::NORTH:
            y -= hopLength;
            break;
        case Direction::EAST:
            x += hopLength;
            break;
        case Direction::SOUTH:
            y += hopLength;
            break;
        case Direction::WEST:
            x -= hopLength;
            break;
    }

    // If the full hop would go out of bounds, assign to the edges.
    if (x < 0) {
        x = 0;
    }
    if (x > 9) {
        x = 9;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > 9) {
        y = 9;
    }

    setPosition({x, y});
}

std::string Hopper::getType() const {
    return "Hopper";
}

int Hopper::getHopLength() const {
    return hopLength;
}