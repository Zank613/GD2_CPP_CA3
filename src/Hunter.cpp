#include "Hunter.h"

#include "Board.h"
#include "Seeder.h"
#include "Utils.h"

#include <random>
#include <vector>

Hunter::Hunter(int id, std::pair<int, int> position, Direction direction, int health, Board* board)
    : Bug(id, position, direction, health), lastPosition({-1, -1}), hasLastPosition(false),
      scentTolerance(0.2), scentThreshold(0.4), maxTrailSize(24), maxDecisionPoints(8) {
    this->board = board;
}

std::vector<Hunter::Option> Hunter::collectOptions(bool avoidImmediateReverse) const {
    std::vector<Option> options;
    options.reserve(4);         // Only 4 options can be made so, allocate only ONCE.

    const Direction allDirections[] = {
        Direction::NORTH,
        Direction::EAST,
        Direction::SOUTH,
        Direction::WEST
    };

    for (Direction candidateDirection : allDirections) {
        std::pair<int, int> nextPos = utils::nextPosition(position, candidateDirection, 1);

        if (nextPos.first < 0 || nextPos.first >= utils::BOARD_SIZE ||
            nextPos.second < 0 || nextPos.second >= utils::BOARD_SIZE) {
            continue;
        }

        if (board != nullptr && !board->isCellTraversable(nextPos)) {
            continue;
        }

        if (avoidImmediateReverse && hasLastPosition && nextPos == lastPosition) {
            continue;
        }

        double scentValue = 0.0;
        if (board != nullptr) {
            scentValue = board->getScentAt(nextPos);
        }

        options.push_back({candidateDirection, nextPos, scentValue});
    }

    return options;
}

Direction Hunter::chooseRandomDirection(const std::vector<Option>& options) const {
    if (options.empty()) {
        return direction;
    }

    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dist(0, static_cast<int>(options.size()) - 1);

    return options[dist(rng)].direction;
}

Direction Hunter::directionToward(const std::pair<int, int>& from, const std::pair<int, int>& to) const {
    if (to.first == from.first && to.second == from.second - 1) {
        return Direction::NORTH;
    }
    if (to.first == from.first + 1 && to.second == from.second) {
        return Direction::EAST;
    }
    if (to.first == from.first && to.second == from.second + 1) {
        return Direction::SOUTH;
    }
    if (to.first == from.first - 1 && to.second == from.second) {
        return Direction::WEST;
    }

    return direction;
}

void Hunter::pushTrailPosition(const std::pair<int, int>& position) {
    if (!trailStack.empty() && trailStack.back() == position) {
        return;
    }

    trailStack.push_back(position);

    if (trailStack.size() > maxTrailSize) {
        trailStack.erase(trailStack.begin());
    }
}

void Hunter::pushDecisionPoint(const std::pair<int, int>& position, const std::vector<Direction>& alternatives) {
    if (alternatives.empty()) {
        return;
    }

    if (!decisionStack.empty() && decisionStack.back().position == position) {
        return;
    }

    decisionStack.push_back({position, alternatives});

    if (decisionStack.size() > maxDecisionPoints) {
        decisionStack.erase(decisionStack.begin());
    }
}

bool Hunter::chooseFromDecisionMemory(Direction& chosenDirection, bool& movingBackward) {
    std::mt19937& rng = Seeder::getInstance().getRNG();

    while (!decisionStack.empty()) {
        DecisionPoint& latest = decisionStack.back();

        // If the Hunter is standing on a remembered fork try one unused branch.
        if (position == latest.position) {
            if (!latest.alternatives.empty()) {
                std::uniform_int_distribution<int> dist(
                    0, static_cast<int>(latest.alternatives.size()) - 1);

                int chosenIndex = dist(rng);
                chosenDirection = latest.alternatives[chosenIndex];
                latest.alternatives.erase(latest.alternatives.begin() + chosenIndex);

                if (latest.alternatives.empty()) {
                    decisionStack.pop_back();
                }

                movingBackward = false;
                return true;
            }

            decisionStack.pop_back();
            continue;
        }

        // Otherwise keep stepping backward along the remembered trail.
        if (!trailStack.empty()) {
            std::pair<int, int> previousPosition = trailStack.back();
            trailStack.pop_back();

            if (previousPosition == position) {
                continue;
            }

            chosenDirection = directionToward(position, previousPosition);
            movingBackward = true;
            return true;
        }

        // No trail left to follow, so discard stale decision points.
        decisionStack.pop_back();
    }

    return false;
}

void Hunter::move() {
    std::pair<int, int> oldPosition = position;

    std::vector<Option> options = collectOptions(true);
    if (options.empty()) {
        options = collectOptions(false);
    }

    Direction chosenDirection = direction;
    bool movingBackward = false;

    if (!options.empty()) {
        double bestScent = -1.0;

        for (const Option& option : options) {
            if (option.scent > bestScent) {
                bestScent = option.scent;
            }
        }

        // If the scent trail is too weak, try memory backtracking first.
        if (bestScent < scentThreshold) {
            if (!chooseFromDecisionMemory(chosenDirection, movingBackward)) {
                chosenDirection = chooseRandomDirection(options);
            }
        } else {
            std::vector<Option> candidateOptions;

            for (const Option& option : options) {
                if (option.scent >= bestScent - scentTolerance) {
                    candidateOptions.push_back(option);
                }
            }

            std::mt19937& rng = Seeder::getInstance().getRNG();
            std::uniform_int_distribution<int> dist(
                0, static_cast<int>(candidateOptions.size()) - 1);

            int chosenIndex = dist(rng);
            chosenDirection = candidateOptions[chosenIndex].direction;

            std::vector<Direction> alternatives;
            for (int i = 0; i < static_cast<int>(candidateOptions.size()); i++) {
                if (i != chosenIndex) {
                    alternatives.push_back(candidateOptions[i].direction);
                }
            }

            pushDecisionPoint(position, alternatives);
        }
    }

    setDirection(chosenDirection);

    if (!movingBackward) {
        pushTrailPosition(oldPosition);
    }

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