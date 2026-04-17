#include "Crawler.h"
#include <random>

Crawler::Crawler(int id, std::pair<int,int> pos, Direction dir, int health)
    : Bug(id, pos, dir, health) {}

void Crawler::move() {
    // Random generator
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dirDist(1, 4);

    // If blocked, keep picking random directions
    while (isWayBlocked()) {
        direction = static_cast<Direction>(dirDist(rng));
    }

    int x = position.first;
    int y = position.second;

    // Move 1 unit
    switch (direction) {
        case Direction::NORTH: y--; break;
        case Direction::EAST:  x++; break;
        case Direction::SOUTH: y++; break;
        case Direction::WEST:  x--; break;
    }

    setPosition({x, y});
}

std::string Crawler::getType() const {
    return "Crawler";
}