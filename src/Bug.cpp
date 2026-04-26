#include "Bug.h"
#include "Utils.h"

Bug::Bug(int id, std::pair<int, int> position, Direction direction, int health)
    : id(id), position(position), direction(direction), health(health), alive(true), eatenById(-1),
        board(nullptr), stuckTurns(0)
{
    path.push_back(position);
}

Bug::~Bug() = default;

bool Bug::isWayBlocked() const {
    int x = position.first;
    int y = position.second;

    switch (direction) {
        case Direction::NORTH:
            return y == 0;
        case Direction::EAST:
            return x == utils::BOARD_MAX_INDEX;
        case Direction::SOUTH:
            return y == utils::BOARD_MAX_INDEX;
        case Direction::WEST:
            return x == 0;
        default:
            return true;
    }
}

void Bug::setPosition(const std::pair<int, int>& newPosition) {
    position = newPosition;
    path.push_back(newPosition);
}

void Bug::setDirection(Direction newDirection) {
    direction = newDirection;
}

void Bug::takeDamage(int damage) {
    health -= damage;

    if (health <= 0) {
        health = 0;
        alive = false;
    }
}

void Bug::setEatenById(int eaterID) {
    eatenById = eaterID;
}

void Bug::setBoard(Board* board) {
    this->board = board;
}

void Bug::setStuckTurns(int turns) {
    if (turns < 0) {
        stuckTurns = 0;
    } else {
        stuckTurns = turns;
    }
}

void Bug::decrementStuckTurns() {
    if (stuckTurns > 0) {
        stuckTurns--;
    }
}

void Bug::heal(int amount) {
    if (amount <= 0) {
        return;
    }

    health += amount;

    if (health > 20) {
        health = 20;
    }
}