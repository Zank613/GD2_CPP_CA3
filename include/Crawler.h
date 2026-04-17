#ifndef CRAWLER_H
#define CRAWLER_H

#include "Bug.h"

class Crawler : public Bug {
public:
    Crawler(int id, std::pair<int,int> position, Direction direction, int health);

    void move() override;
    std::string getType() const override;
};

#endif