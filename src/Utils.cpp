#include "Utils.h"

#include <sstream>

namespace utils {

std::string directionToString(Direction direction) {
    switch (direction) {
        case Direction::NORTH:
            return "North";
        case Direction::EAST:
            return "East";
        case Direction::SOUTH:
            return "South";
        case Direction::WEST:
            return "West";
        default:
            return "Unknown";
    }
}

std::pair<int, int> nextPosition(std::pair<int, int> position, Direction direction, int steps) {
    int x = position.first;
    int y = position.second;

    switch (direction) {
        case Direction::NORTH:
            y -= steps;
            break;
        case Direction::EAST:
            x += steps;
            break;
        case Direction::SOUTH:
            y += steps;
            break;
        case Direction::WEST:
            x -= steps;
            break;
        default:
            break;
    }

    return {x, y};
}

std::pair<int, int> clampPositionToBoard(std::pair<int, int> position) {
    int x = position.first;
    int y = position.second;

    if (x < 0) {
        x = 0;
    }
    if (x > BOARD_MAX_INDEX) {
        x = BOARD_MAX_INDEX;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > BOARD_MAX_INDEX) {
        y = BOARD_MAX_INDEX;
    }

    return {x, y};
}

std::string formatPath(const std::list<std::pair<int, int>>& path) {
    std::ostringstream output;
    bool first = true;

    for (const std::pair<int, int>& pos : path) {
        if (!first) {
            output << ",";
        }

        output << "(" << pos.first << "," << pos.second << ")";
        first = false;
    }

    return output.str();
}

std::string buildBugEndStateText(const Bug* bug) {
    if (bug == nullptr) {
        return "Unknown";
    }

    if (bug->isAlive()) {
        return "Alive";
    }

    if (bug->getEatenById() != -1) {
        return "Eaten by " + std::to_string(bug->getEatenById());
    }

    return "Dead";
}

std::string bugTypeToSymbol(const std::string& bugType) {
    if (bugType == "Crawler") {
        return "C";
    }

    if (bugType == "Hopper") {
        return "H";
    }

    if (bugType == "Hunter") {
        return "U";
    }

    return "?";
}

}