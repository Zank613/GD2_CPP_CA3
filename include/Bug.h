#ifndef BUG_H
#define BUG_H

#include <list>
#include <string>
#include <utility>

/**
 * @brief Cardinal directions used by all bug types.
 */
enum class Direction {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4
};

/**
 * @brief Abstract base class shared by every bug on the board.
 *
 * This class stores the common state every bug needs:
 * id, position, direction, health, alive/dead status, and its path history.
 *
 * The class is abstract because the brief requires each bug type to implement
 * its own move() behaviour.
 */
class Bug {
protected:
    int id;
    std::pair<int, int> position;
    Direction direction;
    int health;
    bool alive;
    std::list<std::pair<int, int>> path;

public:
    /**
     * @brief Constructs a bug with its starting state.
     * @param id Unique bug id.
     * @param position Starting position on the 10x10 board.
     * @param direction Starting facing direction.
     * @param health Starting health value.
     */
    Bug(int id, std::pair<int, int> position, Direction direction, int health);

    /**
     * @brief Virtual destructor because bugs are deleted through Bug*.
     */
    virtual ~Bug();

    /**
     * @brief Moves the bug according to its own behaviour rules.
     *
     * This is pure virtual so Bug stays abstract.
     */
    virtual void move() = 0;

    /**
     * @brief Returns the type name of the derived bug.
     * @return Bug type as a string, e.g. "Crawler" or "Hopper".
     */
    virtual std::string getType() const = 0;

    /**
     * @brief Checks whether the bug is blocked by the board edge
     * in the direction it is currently facing.
     *
     * The board is fixed at 10x10, so valid coordinates are 0 to 9.
     *
     * @return true if the bug cannot move forward in its current direction.
     */
    bool isWayBlocked() const;

    /**
     * @brief Returns whether the bug is still alive.
     * @return true if alive, false if dead.
     */
    bool isAlive() const;

    /**
     * @brief Gets bug id.
     * @return Unique id.
     */
    int getId() const;

    /**
     * @brief Gets current health.
     * @return Current health value.
     */
    int getHealth() const;

    /**
     * @brief Gets current position.
     * @return Current (x, y) position.
     */
    std::pair<int, int> getPosition() const;

    /**
     * @brief Gets the full recorded path of the bug.
     * @return Read-only reference to the path history.
     */
    const std::list<std::pair<int, int>>& getPath() const;

    /**
     * @brief Gets current direction.
     * @return Current facing direction.
     */
    Direction getDirection() const;

    /**
     * @brief Updates the bug's position and records it in the path.
     * @param newPosition New board position.
     */
    void setPosition(const std::pair<int, int>& newPosition);

    /**
     * @brief Updates the current direction.
     * @param newDirection New facing direction.
     */
    void setDirection(Direction newDirection);

    /**
     * @brief Applies damage to the bug.
     * @param damage Damage amount to subtract from health.
     */
    void takeDamage(int damage);
};

#endif