#include "Board.h"

#include "Crawler.h"
#include "Hopper.h"
#include "Seeder.h"
#include "Utils.h"
#include "Terrain.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <thread>

#include "Hunter.h"

Board::Board() : tapCount(0), renderer(nullptr), simulationDelayMs(1000) {
    initializeScentGrid();
    initializeTerrainGrid();
}

Board::Board(unsigned int seed) : tapCount(0), renderer(nullptr), simulationDelayMs(1000) {
    Seeder::getInstance().setSeed(seed);
    initializeScentGrid();
    initializeTerrainGrid();
}

Board::~Board() {
    for (Bug* bug : bugs) {
        delete bug;
    }
    bugs.clear();
}

void Board::addBug(Bug* bug) {
    if (bug != nullptr) {
        bugs.push_back(bug);
    }
}

Bug* Board::findBugByIdInternal(int id) const {
    for (Bug* bug : bugs) {
        if (bug != nullptr && bug->getId() == id) {
            return bug;
        }
    }

    return nullptr;
}

void Board::displayBugDetails(const Bug* bug) const {
    if (bug == nullptr) {
        return;
    }

    std::pair<int, int> pos = bug->getPosition();

    std::cout << bug->getId() << " "
              << bug->getType() << " "
              << "(" << pos.first << "," << pos.second << ") "
              << bug->getHealth() << " "
              << utils::directionToString(bug->getDirection()) << " ";

    const Hopper* hopper = dynamic_cast<const Hopper*>(bug);
    if (hopper != nullptr) {
        std::cout << hopper->getHopLength() << " ";
    }

    std::cout << (bug->isAlive() ? "Alive" : "Dead") << "\n";
}

void Board::updateCellOccupants() {
    cellOccupants.clear();

    for (Bug* bug : bugs) {
        if (bug != nullptr && bug->isAlive()) {
            cellOccupants[bug->getPosition()].push_back(bug);
        }
    }
}

int Board::countAliveBugs() const {
    int count = 0;

    for (Bug* bug : bugs) {
        if (bug != nullptr && bug->isAlive()) {
            count++;
        }
    }

    return count;
}

int Board::getRandomAliveBugIndex() const {
    std::vector<int> aliveIndices;

    for (int i = 0; i < static_cast<int>(bugs.size()); i++) {
        if (bugs[i] != nullptr && bugs[i]->isAlive()) {
            aliveIndices.push_back(i);
        }
    }

    if (aliveIndices.empty()) {
        return -1;
    }

    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> dist(0, static_cast<int>(aliveIndices.size()) - 1);

    return aliveIndices[dist(rng)];
}

void Board::resolveFights() {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> damageDist(0, 5);

    for (auto& entry : cellOccupants) {
        std::vector<Bug*> fighters = entry.second;

        // Keep only alive bugs in this cell.
        fighters.erase(
            std::remove_if(
                fighters.begin(),
                fighters.end(),
                [](Bug* bug) {
                    return bug == nullptr || !bug->isAlive();
                }),
            fighters.end()
        );

        // If fewer than 2 alive bugs are here, no fight happens.
        if (fighters.size() < 2) {
            continue;
        }

        // Make battles smell strongly of scent.
        depositScent(entry.first, 5.0);

        // Deterministic pairing strategy: sort by id, then pair in order.
        std::sort(
            fighters.begin(),
            fighters.end(),
            [](Bug* left, Bug* right) {
                return left->getId() < right->getId();
            }
        );

        for (std::size_t i = 0; i + 1 < fighters.size(); i += 2) {
            Bug* first = fighters[i];
            Bug* second = fighters[i + 1];

            // Fight lasts up to 3 rounds.
            for (int round = 0; round < 3; round++) {
                if (!first->isAlive() || !second->isAlive()) {
                    break;
                }

                int damageToFirst = damageDist(rng);
                int damageToSecond = damageDist(rng);

                first->takeDamage(damageToFirst);
                second->takeDamage(damageToSecond);

                if (!first->isAlive() && second->isAlive()) {
                    first->setEatenById(second->getId());
                    break;
                }

                if (!second->isAlive() && first->isAlive()) {
                    second->setEatenById(first->getId());
                    break;
                }
            }

            // If both are still alive after 3 rounds, force a winner.
            if (first->isAlive() && second->isAlive()) {
                if (first->getHealth() > second->getHealth()) {
                    second->setEatenById(first->getId());
                    second->takeDamage(second->getHealth());
                } else if (second->getHealth() > first->getHealth()) {
                    first->setEatenById(second->getId());
                    first->takeDamage(first->getHealth());
                } else {
                    // Deterministic tie-break: lower id wins.
                    if (first->getId() < second->getId()) {
                        second->setEatenById(first->getId());
                        second->takeDamage(second->getHealth());
                    } else {
                        first->setEatenById(second->getId());
                        first->takeDamage(first->getHealth());
                    }
                }
            }
        }
    }

    updateCellOccupants();
}

void Board::initializeFromFile(const std::string& filename) {
    std::ifstream input(filename);

    if (!input) {
        std::cerr << "Error: could not open file '" << filename << "'.\n";
        return;
    }

    for (Bug* bug : bugs) {
        delete bug;
    }
    bugs.clear();
    cellOccupants.clear();
    tapCount = 0;

    initializeScentGrid();
    initializeTerrainGrid();

    std::string line;

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::vector<std::string> parts;
        std::string token;

        while (std::getline(ss, token, ';')) {
            parts.push_back(token);
        }

        if (parts.empty()) {
            continue;
        }

        char bugType = parts[0][0];
        int id = std::stoi(parts[1]);
        int x = std::stoi(parts[2]);
        int y = std::stoi(parts[3]);
        Direction direction = static_cast<Direction>(std::stoi(parts[4]));
        int health = std::stoi(parts[5]);

        Bug* bug = nullptr;

        if (bugType == 'C') {
            bug = new Crawler(id, {x, y}, direction, health);
        } else if (bugType == 'H') {
            int hopLength = std::stoi(parts[6]);
            bug = new Hopper(id, {x, y}, direction, health, hopLength);
        } else if (bugType == 'U') {
            bug = new Hunter(id, {x, y}, direction, health, this);
        }

        if (bug != nullptr) {
            bug->setBoard(this);
            addBug(bug);
        }
    }
    generateTerrain();
    updateCellOccupants();

    if (renderer != nullptr && renderer->isDebugEnabled()) {
        renderer->printDebug("Board initialized from file: " + filename);
    }
}

void Board::displayAllBugs() const {
    for (Bug* bug : bugs) {
        if (renderer != nullptr) {
            renderer->printBugDetails(bug);
        } else {
            displayBugDetails(bug);
        }
    }
}

void Board::findBug(int id) const {
    Bug* bug = findBugByIdInternal(id);

    if (bug == nullptr) {
        std::cout << "Bug " << id << " not found\n";
        return;
    }

    displayBugDetails(bug);
}

void Board::tapBoard() {
    if (countAliveBugs() <= 1) {
        return;
    }

    tapCount++;

    int frozenIndex = getRandomAliveBugIndex();

    for (int i = 0; i < static_cast<int>(bugs.size()); i++) {
        if (bugs[i] == nullptr || !bugs[i]->isAlive()) {
            continue;
        }

        if (i == frozenIndex) {
            continue;
        }

        if (renderer != nullptr && renderer->isDebugEnabled() && frozenIndex != -1) {
            renderer->printDebug("Frozen bug id: " + std::to_string(bugs[frozenIndex]->getId()));
        }

        bugs[i]->move();
        applyTerrainEffect(bugs[i]);
        // Hunter type of bug, doesn't leave any scent.
        if (bugs[i]->getType() != "Hunter") {
            depositScent(bugs[i]->getPosition(), 1.0);
        }
    }

    updateCellOccupants();
    resolveFights();
    decayScent();

    if (renderer != nullptr) {
        renderer->printStatus("Board after tap " + std::to_string(tapCount));
        renderer->renderBoard(cellOccupants, terrainGrid);
    }
}

void Board::displayLifeHistory() const {
    for (Bug* bug : bugs) {
        if (bug == nullptr) {
            continue;
        }

        if (renderer != nullptr) {
            renderer->printLifeHistory(bug);
        } else {
            std::cout << bug->getId() << " "
                      << bug->getType() << " Path: "
                      << utils::formatPath(bug->getPath()) << " "
                      << utils::buildBugEndStateText(bug)
                      << "\n";
        }
    }
}

void Board::displayAllCells() const {
    for (int x = 0; x < utils::BOARD_SIZE; x++) {
        for (int y = 0; y < utils::BOARD_SIZE; y++) {
            std::pair<int, int> cell = {x, y};

            std::cout << "(" << x << "," << y << "): ";

            auto it = cellOccupants.find(cell);

            if (it == cellOccupants.end() || it->second.empty()) {
                std::cout << "empty";
            } else {
                const std::vector<Bug*>& bugsInCell = it->second;

                for (std::size_t i = 0; i < bugsInCell.size(); i++) {
                    std::cout << bugsInCell[i]->getType() << " " << bugsInCell[i]->getId();

                    if (i + 1 < bugsInCell.size()) {
                        std::cout << ", ";
                    }
                }
            }

            std::cout << "\n";
        }
    }
}

void Board::runSimulation() {
    while (!isSimulationOver()) {
        tapBoard();

        if (renderer != nullptr) {
            renderer->printStatus("Tap: " + std::to_string(tapCount));
        } else {
            std::cout << "Tap: " << tapCount << "\n";
        }

        displayAllBugs();
        std::cout << "\n";

        if (simulationDelayMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(simulationDelayMs));
        }
    }

    if (renderer != nullptr) {
        renderer->printStatus("Simulation finished.");
    } else {
        std::cout << "Simulation finished.\n";
    }

    writeLifeHistoryToFile();
}

void Board::writeLifeHistoryToFile() const {
    std::time_t now = std::time(nullptr);
    std::tm localTime = *std::localtime(&now);

    std::ostringstream filename;
    filename << "bugs_life_history_"
             << std::put_time(&localTime, "%Y%m%d_%H%M%S")
             << ".out";

    std::ofstream output(filename.str());

    if (!output.is_open()) {
        std::cerr << "Error: could not write life history file.\n";
        return;
    }

    output << "Seed: " << Seeder::getInstance().getCurrentSeed() << "\n";
    output << "Tap Count: " << tapCount << "\n";
    output << "Generated: " << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "\n\n";

    for (Bug* bug : bugs) {
        if (bug == nullptr) {
            continue;
        }

        output << bug->getId() << " "
       << bug->getType() << " Path: "
       << utils::formatPath(bug->getPath()) << " "
       << utils::buildBugEndStateText(bug)
       << "\n";
    }
}

bool Board::isSimulationOver() const {
    return countAliveBugs() <= 1;
}

int Board::getTapCount() const {
    return tapCount;
}

void Board::setRenderer(ConsoleRenderer* renderer) {
    this->renderer = renderer;
}

void Board::initializeScentGrid() {
    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            scentGrid[y][x] = 0.0;
        }
    }
}

void Board::depositScent(const std::pair<int, int>& position, double amount) {
    int x = position.first;
    int y = position.second;

    if (x < 0 || x >= utils::BOARD_SIZE || y < 0 || y >= utils::BOARD_SIZE) {
        return;
    }

    scentGrid[y][x] += amount;
}

void Board::decayScent() {
    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            scentGrid[y][x] *= 0.5;

            if (scentGrid[y][x] < 0.1) {
                scentGrid[y][x] = 0.0;
            }
        }
    }
}

double Board::getScentAt(const std::pair<int, int>& position) const {
    int x = position.first;
    int y = position.second;

    if (x < 0 || x >= utils::BOARD_SIZE || y < 0 || y >= utils::BOARD_SIZE) {
        return 0.0;
    }

    return scentGrid[y][x];
}

void Board::setSimulationDelay(int delayMs) {
    simulationDelayMs = delayMs;
}

void Board::initializeTerrainGrid() {
    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            terrainGrid[y][x] = TerrainType::NORMAL;
        }
    }
}

bool Board::isWithinBounds(const std::pair<int, int>& position) const {
    return position.first >= 0 && position.first < utils::BOARD_SIZE &&
           position.second >= 0 && position.second < utils::BOARD_SIZE;
}

TerrainType Board::getTerrainAt(const std::pair<int, int>& position) const {
    if (!isWithinBounds(position)) {
        return TerrainType::ROCK;
    }

    return terrainGrid[position.second][position.first];
}

void Board::setTerrainAt(const std::pair<int, int>& position, TerrainType terrain) {
    if (!isWithinBounds(position)) {
        return;
    }

    terrainGrid[position.second][position.first] = terrain;
}

bool Board::isCellTraversable(const std::pair<int, int>& position) const {
    return isWithinBounds(position) && getTerrainAt(position) != TerrainType::ROCK;
}

bool Board::isBugAtPosition(const std::pair<int, int>& position) const {
    for (Bug* bug : bugs) {
        if (bug != nullptr && bug->isAlive() && bug->getPosition() == position) {
            return true;
        }
    }

    return false;
}

int Board::countNeighbourTerrain(const std::pair<int, int>& position, TerrainType terrain) const {
    const Direction directions[4] = {
        Direction::NORTH,
        Direction::EAST,
        Direction::SOUTH,
        Direction::WEST
    };

    int count = 0;

    for (Direction direction : directions) {
        std::pair<int, int> neighbour = utils::nextPosition(position, direction, 1);

        if (isWithinBounds(neighbour) && getTerrainAt(neighbour) == terrain) {
            count++;
        }
    }

    return count;
}

bool Board::canPlaceTerrainAt(const std::pair<int, int>& position, TerrainType terrain) const {
    if (!isWithinBounds(position)) {
        return false;
    }

    if (isBugAtPosition(position)) {
        return false;
    }

    if (getTerrainAt(position) != TerrainType::NORMAL) {
        return false;
    }

    // Keep rocks useful but avoid making big walls that trap the board too much.
    if (terrain == TerrainType::ROCK && countNeighbourTerrain(position, TerrainType::ROCK) >= 1) {
        return false;
    }

    // Keep food spread out so it is not all in one cluster.
    if (terrain == TerrainType::FOOD && countNeighbourTerrain(position, TerrainType::FOOD) >= 1) {
        return false;
    }

    return true;
}

bool Board::placeRandomTerrain(TerrainType terrain) {
    std::mt19937& rng = Seeder::getInstance().getRNG();
    std::uniform_int_distribution<int> coordDist(0, utils::BOARD_MAX_INDEX);

    const int maxAttempts = 100;

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        std::pair<int, int> position = {coordDist(rng), coordDist(rng)};

        if (canPlaceTerrainAt(position, terrain)) {
            setTerrainAt(position, terrain);
            return true;
        }
    }

    return false;
}

void Board::placeTerrainTiles(TerrainType terrain, int amount) {
    int placed = 0;
    int attempts = 0;
    const int maxAttempts = amount * 50;

    while (placed < amount && attempts < maxAttempts) {
        if (placeRandomTerrain(terrain)) {
            placed++;
        }

        attempts++;
    }
}

void Board::generateTerrain() {
    initializeTerrainGrid();

    placeTerrainTiles(TerrainType::MUD, 8);
    placeTerrainTiles(TerrainType::ROCK, 3);
    placeTerrainTiles(TerrainType::FOOD, 4);
}

void Board::applyTerrainEffect(Bug* bug) {
    if (bug == nullptr || !bug->isAlive()) {
        return;
    }

    TerrainType terrain = getTerrainAt(bug->getPosition());

    switch (terrain) {
        case TerrainType::MUD:
            bug->setStuckTurns(1);
            break;

        case TerrainType::FOOD:
            bug->heal(2);
            setTerrainAt(bug->getPosition(), TerrainType::NORMAL);
            break;

        case TerrainType::ROCK:
        case TerrainType::NORMAL:
        default:
            break;
    }
}