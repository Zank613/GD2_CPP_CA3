#ifndef BOARD_H
#define BOARD_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Bug.h"
#include "ConsoleRenderer.h"
#include "Utils.h"

/**
 * @brief Owns and manages the whole bug board simulation.
 *
 * Board is responsible for:
 * - owning all dynamically allocated bugs
 * - loading them from file
 * - keeping track of which cells contain which bugs
 * - running taps, fights, and simulation
 * - handling display/output features from the menu
 *
 * The public interface is kept fairly high-level on purpose so the Board
 * does not leak its internal containers outside as brief rules.
 */
class Board {

    std::vector<Bug*> bugs;
    std::map<std::pair<int, int>, std::vector<Bug*>> cellOccupants;
    int tapCount;
    ConsoleRenderer* renderer;
    double scentGrid[utils::BOARD_SIZE][utils::BOARD_SIZE];

    /**
     * @brief Finds a bug internally by id.
     * @param id Bug id to search for.
     * @return Pointer to matching bug or nullptr if not found.
     */
    Bug* findBugByIdInternal(int id) const;

    /**
     * @brief Prints one bug in display format.
     * @param bug Bug to display.
     */
    void displayBugDetails(const Bug* bug) const;

    /**
     * @brief Rebuilds the cell occupancy map from alive bugs.
     */
    void updateCellOccupants();

    /**
     * @brief Counts how many bugs are still alive.
     * @return Number of alive bugs.
     */
    int countAliveBugs() const;

    /**
     * @brief Picks one alive bug index at random.
     * @return Index of a random alive bug, or -1 if none are alive.
     */
    int getRandomAliveBugIndex() const;

    /**
     * @brief Resolves fights in cells that contain multiple alive bugs.
     *
     * Pairing strategy:
     * bugs in the same cell are sorted by id and paired in order.
     * If there is an odd number, the last one is left unscathed.
     */
    void resolveFights();

    /**
    * @brief Sets all scent values on the board to 0.
    */
    void initializeScentGrid();

    /**
     * @brief Adds scent to a board cell.
     * @param position Cell position where scent is deposited.
     * @param amount Amount of scent to add.
     */
    void depositScent(const std::pair<int, int>& position, double amount);

    /**
     * @brief Reduces scent values across the whole board after each tap.
     */
    void decayScent();

public:
    /**
     * @brief Constructs a board using the current Seeder state.
     */
    Board();

    /**
     * @brief Constructs a board and sets a deterministic seed.
     * @param seed Seed used for the shared RNG.
     */
    explicit Board(unsigned int seed);

    /**
     * @brief Destructor that frees all dynamically allocated bugs.
     */
    ~Board();

    /**
     * @brief Copy construction is disabled.
     *
     * Board owns dynamically allocated Bug objects through raw pointers.
     * A shallow copy would copy the pointers only, causing multiple Board
     * objects to believe they own the same memory.
     */
    Board(const Board&) = delete;

    /**
     * @brief Copy assignment is disabled.
     *
     * This prevents shallow copying the owned Bug pointers, which would
     * otherwise lead to double deletion and invalid memory access.
     */
    Board& operator=(const Board&) = delete;

    /**
     * @brief Loads bugs from a text file and initializes the board.
     * @param filename Input file path.
     */
    void initializeFromFile(const std::string& filename);

    /**
     * @brief Adds a dynamically allocated bug to the board.
     * @param bug Pointer to a heap-allocated bug.
     */
    void addBug(Bug* bug);

    /**
     * @brief Displays all bugs in human-readable form.
     */
    void displayAllBugs() const;

    /**
     * @brief Finds a bug by id and displays its details.
     * @param id Bug id to search for.
     */
    void findBug(int id) const;

    /**
     * @brief Performs one board tap.
     *
     * One alive bug is frozen for this tap.
     * All other alive bugs move.
     * After movement, fights are resolved in shared cells.
     */
    void tapBoard();

    /**
     * @brief Displays the full life history of every bug.
     */
    void displayLifeHistory() const;

    /**
     * @brief Displays every board cell and the bugs in it.
     */
    void displayAllCells() const;

    /**
     * @brief Runs the simulation automatically until the game is over.
     */
    void runSimulation();

    /**
     * @brief Writes life histories to a timestamped output file.
     */
    void writeLifeHistoryToFile() const;

    /**
     * @brief Checks whether the simulation has finished.
     * @return true if 0 or 1 bugs remain alive.
     */
    bool isSimulationOver() const;

    /**
     * @brief Gets current tap count.
     * @return Number of taps performed so far.
     */
    int getTapCount() const;

    /**
    * @brief Sets the renderer used for console output.
    * @param renderer Pointer to a renderer owned elsewhere.
    */
    void setRenderer(ConsoleRenderer* renderer);

    /**
     * @brief Gets the scent value at a given cell.
     * @param position Cell position to inspect.
     * @return Scent value at that cell, or 0.0 if out of bounds.
     */
    double getScentAt(const std::pair<int, int>& position) const;
};

#endif