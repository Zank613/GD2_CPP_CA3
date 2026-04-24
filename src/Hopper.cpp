#include "Hopper.h"
#include "Seeder.h"
#include <random>
#include "Utils.h"
#include "Board.h"

Hopper::Hopper(int id, std::pair<int, int> position, Direction direction, int health, int hopLength)
    : Bug(id, position, direction, health), hopLength(hopLength) {
}

void Hopper::move() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dirDist(1, 4);

    const int maxAttempts = 20;
    int attempts = 0;

    while (attempts < maxAttempts) {
        if (isWayBlocked()) {
            setDirection(static_cast<Direction>(dirDist(rng)));
            attempts++;
            continue;
        }

        std::pair<int, int> landingPosition = utils::nextPosition(position, direction, hopLength);
        landingPosition = utils::clampPositionToBoard(landingPosition);

        bool canLand = true;

        if (board != nullptr) {
            canLand = board->isCellTraversable(landingPosition);
        }

        if (canLand) {
            setPosition(landingPosition);
            return;
        }

        setDirection(static_cast<Direction>(dirDist(rng)));
        attempts++;
    }
}

std::string Hopper::getType() const {
    return "Hopper";
}

int Hopper::getHopLength() const {
    return hopLength;
}