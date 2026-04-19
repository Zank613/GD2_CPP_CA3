#include "Hunter.h"

#include "Board.h"
#include "Seeder.h"
#include "Utils.h"

#include <random>
#include <vector>

Hunter::Hunter(int id, std::pair<int, int> position, Direction direction, int health, Board* board)
    : Bug(id, position, direction, health), board(board) {
}

Direction Hunter::chooseScentDirection() const {
    // If Hunter has no board fall back to current direction.
    if (board == nullptr) {
        return direction;
    }

    std::vector<Direction> validDirections;
    std::vector<Direction> bestDirections;

    double bestScent = -1;

    const Direction allDirections[] = {
        Direction::NORTH,
        Direction::EAST,
        Direction::SOUTH,
        Direction::WEST
    };

    for (Direction candidateDirection : allDirections) {
        std::pair<int, int> nextPos = utils::nextPosition(position, candidateDirection, 1);

        // Ignore directions that would go off the board.
        if (nextPos.first < 0 || nextPos.first >= utils::BOARD_SIZE ||
            nextPos.second < 0 || nextPos.second >= utils::BOARD_SIZE) {
            continue;
        }

        validDirections.push_back(candidateDirection);

        double scentValue = board->getScentAt(nextPos);

        if (scentValue > bestScent) {
            bestScent = scentValue;
            bestDirections.clear();
            bestDirections.push_back(candidateDirection);
        } else if (scentValue == bestScent) {
            bestDirections.push_back(candidateDirection);
        }
    }

    // No valid moves should not really happen on a 10x10 board
    // but return current direction as fallback.
    if (validDirections.empty()) {
        return direction;
    }

    std::mt19937& rng = Seeder::getInstance().getRNG();

    // If no scent is nearby Hunter falls back to random exploration.
    if (bestScent <= 0.0) {
        std::uniform_int_distribution<int> dist(0, static_cast<int>(validDirections.size()) - 1);
        return validDirections[dist(rng)];
    }

    // If multiple directions have the same best scent pick one randomly.
    std::uniform_int_distribution<int> dist(0, static_cast<int>(bestDirections.size()) - 1);
    return bestDirections[dist(rng)];
}

void Hunter::move() {
    Direction chosenDirection = chooseScentDirection();
    setDirection(chosenDirection);

    std::pair<int, int> newPosition = utils::nextPosition(position, direction, 1);
    newPosition = utils::clampPositionToBoard(newPosition);

    setPosition(newPosition);
}

std::string Hunter::getType() const {
    return "Hunter";
}

void Hunter::setBoard(Board* board) {
    this->board = board;
}

Board* Hunter::getBoard() const {
    return board;
}