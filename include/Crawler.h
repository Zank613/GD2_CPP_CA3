#ifndef CRAWLER_H
#define CRAWLER_H

#include "Bug.h"

/**
 * @brief Bug type that moves one square at a time.
 *
 * If blocked at the edge, it keeps choosing a new random direction
 * until it can move forward.
 */
class Crawler : public Bug {
public:
    /**
     * @brief Constructs a crawler bug.
     * @param id Unique bug id.
     * @param position Starting position.
     * @param direction Starting facing direction.
     * @param health Starting health.
     */
    Crawler(int id, std::pair<int,int> position, Direction direction, int health);

    /**
     * @brief Moves the crawler one square according to the brief rules.
     */
    void move() override;

    /**
     * @brief Gets bug type name.
     * @return "Crawler"
     */
    std::string getType() const override;
};

#endif