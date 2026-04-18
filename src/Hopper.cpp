#include "Hopper.h"
#include "Seeder.h"
#include <random>
#include "Utils.h"

Hopper::Hopper(int id, std::pair<int, int> position, Direction direction, int health, int hopLength)
    : Bug(id, position, direction, health), hopLength(hopLength) {
}

void Hopper::move() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dirDist(1, 4);

    while (isWayBlocked()) {
        setDirection(static_cast<Direction>(dirDist(rng)));
    }

    std::pair<int, int> newPosition = utils::nextPosition(position, direction, hopLength);
    newPosition = utils::clampPositionToBoard(newPosition);

    setPosition(newPosition);
}

std::string Hopper::getType() const {
    return "Hopper";
}

int Hopper::getHopLength() const {
    return hopLength;
}