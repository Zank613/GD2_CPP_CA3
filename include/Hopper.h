#ifndef HOPPER_H
#define HOPPER_H

#include "Bug.h"

/**
 * @brief Bug type that moves by a fixed hop length.
 *
 * If blocked at the edge, it keeps choosing a new random direction
 * until it can move forward. If the full hop would go off the board,
 * it stops on the edge square it hits.
 */
class Hopper : public Bug {
    int hopLength;

public:
    /**
     * @brief Constructs a hopper bug.
     * @param id Unique bug id.
     * @param position Starting position.
     * @param direction Starting facing direction.
     * @param health Starting health.
     * @param hopLength Fixed hop distance for this hopper.
     */
    Hopper(int id, std::pair<int, int> position, Direction direction, int health, int hopLength);

    /**
     * @brief Moves the hopper according to the brief rules.
     */
    void move() override;

    /**
     * @brief Gets bug type name.
     * @return "Hopper"
     */
    std::string getType() const override;

    /**
     * @brief Gets the hopper's hop length.
     * @return Hop distance.
     */
    int getHopLength() const;
};

#endif