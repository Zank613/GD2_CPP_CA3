#ifndef HUNTER_H
#define HUNTER_H

#include <utility>
#include <string>

#include "Bug.h"

class Board;

/**
 * @brief A scent-tracking bug type.
 *
 * Hunter does not just follow a fixed direction like the basic bug types.
 * Instead, it queries the board's scent system and chooses movement based
 * on nearby scent values.
 */
class Hunter : public Bug {
    /**
     * @brief Non-owning pointer to the board.
     *
     * Hunter needs access to the board so it can read scent values.
     * The Board still owns itself and owns all bugs. Hunter only borrows
     * this pointer to inspect the environment.
     */
    Board* board;

    /**
     * @brief Chooses the best direction based on nearby scent.
     * @return Direction chosen from local scent values.
     */
    Direction chooseScentDirection() const;

public:
    /**
     * @brief Constructs a Hunter bug.
     * @param id Unique bug id.
     * @param position Starting position.
     * @param direction Starting direction.
     * @param health Starting health.
     * @param board Non-owning pointer to the board for scent lookup.
     */
    Hunter(int id,
           std::pair<int, int> position,
           Direction direction,
           int health,
           Board* board);

    /**
     * @brief Moves the Hunter according to scent-based behaviour.
     */
    void move() override;

    /**
     * @brief Gets the type name of this bug.
     * @return "Hunter"
     */
    std::string getType() const override;

    /**
     * @brief Updates the board pointer used for scent lookup.
     * @param board New board pointer.
     */
    void setBoard(Board* board);

    /**
     * @brief Gets the board pointer used by this Hunter.
     * @return Current board pointer.
     */
    Board* getBoard() const;
};

#endif