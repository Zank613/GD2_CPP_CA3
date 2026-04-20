#include "Hunter.h"

#include "Board.h"
#include "Seeder.h"
#include "Utils.h"

#include <random>
#include <vector>

Hunter::Hunter(int id, std::pair<int, int> position, Direction direction, int health, Board* board)
    : Bug(id, position, direction, health), board(board), lastPosition({-1, -1}), hasLastPosition(false), scentTolerance(0.5) {}

Direction Hunter::chooseScentDirection() const {
    // If Hunter has no board fall back to current direction.
    if (board == nullptr) {
        return direction;
    }

    struct Option {
        Direction direction;
        std::pair<int, int> nextPosition;
        double scent;
    };

    std::vector<Option> forwardOptions;
    std::vector<Option> backtrackOptions;

    const Direction allDirections[4] = {
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

        double scentValue = board->getScentAt(nextPos);
        Option option{candidateDirection, nextPos, scentValue};

        if (hasLastPosition && nextPos == lastPosition) {
            backtrackOptions.push_back(option);
        } else {
            forwardOptions.push_back(option);
        }
    }

    // Prefer not to immediately reverse direction unless there is no other move.
    const std::vector<Option>& optionsToUse =
        !forwardOptions.empty() ? forwardOptions : backtrackOptions;

    if (optionsToUse.empty()) {
        return direction;
    }

    double bestScent = -1.0;
    for (const Option& option : optionsToUse) {
        if (option.scent > bestScent) {
            bestScent = option.scent;
        }
    }

    std::vector<Direction> candidateDirections;

    // If there is no scent nearby do a random valid move (still avoiding immediate reversal if possible).
    if (bestScent <= 0.0) {
        for (const Option& option : optionsToUse) {
            candidateDirections.push_back(option.direction);
        }
    } else {
        for (const Option& option : optionsToUse) {
            if (option.scent >= bestScent - scentTolerance) {
                candidateDirections.push_back(option.direction);
            }
        }
    }

    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dist(0, static_cast<int>(candidateDirections.size()) - 1);

    return candidateDirections[dist(rng)];
}

void Hunter::move() {
    std::pair<int, int> oldPosition = position;

    Direction chosenDirection = chooseScentDirection();
    setDirection(chosenDirection);

    std::pair<int, int> newPosition = utils::nextPosition(position, direction, 1);
    newPosition = utils::clampPositionToBoard(newPosition);

    setPosition(newPosition);

    lastPosition = oldPosition;
    hasLastPosition = true;
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