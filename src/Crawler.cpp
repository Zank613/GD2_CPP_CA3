#include "Crawler.h"
#include "Seeder.h"
#include <random>
#include "Utils.h"
#include "Board.h"

Crawler::Crawler(int id, std::pair<int, int> position, Direction direction, int health)
    : Bug(id, position, direction, health) {
}

void Crawler::move() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dirDist(1, 4);

    const int maxAttempts = 20;
    int attempts = 0;

    while (attempts < maxAttempts) {
        std::pair<int, int> nextPos = utils::nextPosition(position, direction, 1);

        bool canMoveForward = !isWayBlocked();

        if (canMoveForward && board != nullptr) {
            canMoveForward = board->isCellTraversable(nextPos);
        }

        if (canMoveForward) {
            setPosition(nextPos);
            return;
        }

        setDirection(static_cast<Direction>(dirDist(rng)));
        attempts++;
    }
}

std::string Crawler::getType() const {
    return "Crawler";
}