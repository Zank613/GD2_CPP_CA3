#ifndef BUG_H
#define BUG_H

#include <list>
#include <string>
#include <utility>

enum class Direction {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4
};

class Bug {
protected:
    int id;
    std::pair<int, int> position;
    Direction direction;
    int health;
    bool alive;
    std::list<std::pair<int, int>> path;

public:
    Bug(int id, std::pair<int, int> position, Direction direction, int health);

    virtual ~Bug();
    virtual void move() = 0;
    virtual std::string getType() const = 0;

    bool isWayBlocked() const;
    bool isAlive() const;

    int getId() const;
    int getHealth() const;

    std::pair<int, int> getPosition() const;
    const std::list<std::pair<int, int>>& getPath() const;

    Direction getDirection() const;

    void setPosition(const std::pair<int, int>& newPosition);
    void setDirection(Direction newDirection);
    void takeDamage(int damage);
};

#endif