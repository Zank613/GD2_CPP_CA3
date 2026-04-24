#ifndef HUNTER_H
#define HUNTER_H

#include <utility>
#include <string>
#include <vector>

#include "Bug.h"

class Board;

/**
 * @brief A scent-tracking bug type.
 *
 * Hunter reads nearby scent values from the board and uses them to decide
 * where to move.
 */
class Hunter : public Bug {
    /**
     * @brief One possible move option evaluated by the Hunter.
     */
    struct Option {
        Direction direction;
        std::pair<int, int> nextPosition;
        double scent;
    };

    /**
     * @brief Stores a remembered fork in the trail.
     *
     * The Hunter remembers the square where it had multiple good choices
     * and stores the directions it did not choose at that point.
     */
    struct DecisionPoint {
        std::pair<int, int> position;
        std::vector<Direction> alternatives;
    };

    /**
     * @brief Previous position, used to reduce immediate backtracking.
     */
    std::pair<int, int> lastPosition;

    /**
     * @brief Whether lastPosition is valid yet.
     */
    bool hasLastPosition;

    /**
     * @brief Tolerance used when comparing similar scent values.
     */
    double scentTolerance;

    /**
     * @brief If best scent falls below this, the trail is considered weak or lost.
     */
    double scentThreshold;

    /**
     * @brief Recent path positions used for backtracking.
     */
    std::vector<std::pair<int, int>> trailStack;

    /**
     * @brief Remembered decision points with unchosen branches.
     */
    std::vector<DecisionPoint> decisionStack;

    /**
     * @brief Maximum number of stored trail positions.
     */
    std::size_t maxTrailSize;

    /**
     * @brief Maximum number of remembered decision points.
     */
    std::size_t maxDecisionPoints;

    /**
     * @brief Collects valid one-step movement options.
     * @param avoidImmediateReverse If true, tries not to step straight back.
     * @return Valid move options from the current position.
     */
    std::vector<Option> collectOptions(bool avoidImmediateReverse) const;

    /**
     * @brief Chooses a random direction from valid options.
     * @param options Valid movement options.
     * @return Randomly selected direction.
     */
    Direction chooseRandomDirection(const std::vector<Option>& options) const;

    /**
     * @brief Tries to choose a move from remembered decision points.
     * @param chosenDirection Output direction if one is found.
     * @param movingBackward Set true if this move is a backtracking step.
     * @return true if a remembered move was chosen, false otherwise.
     */
    bool chooseFromDecisionMemory(Direction& chosenDirection, bool& movingBackward);

    /**
     * @brief Gets the direction needed to move from one adjacent square to another.
     * @param from Current position.
     * @param to Adjacent target position.
     * @return Direction from -> to.
     */
    Direction directionToward(const std::pair<int, int>& from, const std::pair<int, int>& to) const;

    /**
     * @brief Records a position in the backtracking trail memory.
     * @param position Position to remember.
     */
    void pushTrailPosition(const std::pair<int, int>& position);

    /**
     * @brief Records a decision point with alternative directions.
     * @param position Position where the choice happened.
     * @param alternatives Other valid directions not chosen.
     */
    void pushDecisionPoint(const std::pair<int, int>& position, const std::vector<Direction>& alternatives);

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