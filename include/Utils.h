#ifndef UTILS_H
#define UTILS_H

#include <list>
#include <string>
#include <utility>

#include "Bug.h"

namespace utils {

/**
 * @brief Fixed board size used throughout the simulation.
 *
 * The brief defines the board as 10x10, so valid coordinates are 0 to 9.
 */
constexpr int BOARD_SIZE = 10;

/**
 * @brief Largest valid board index.
 */
constexpr int BOARD_MAX_INDEX = BOARD_SIZE - 1;

/**
 * @brief Converts a direction enum into readable text.
 * @param direction Direction value to convert.
 * @return Human-readable direction string.
 */
std::string directionToString(Direction direction);

/**
 * @brief Calculates a new position by moving a number of steps
 * in the given direction.
 * @param position Starting position.
 * @param direction Direction to move in.
 * @param steps Number of squares to move.
 * @return New position before any boundary clamping.
 */
std::pair<int, int> nextPosition(std::pair<int, int> position, Direction direction, int steps);

/**
 * @brief Clamps a position so it remains inside the 10x10 board.
 * @param position Position to clamp.
 * @return Position adjusted to valid board bounds.
 */
std::pair<int, int> clampPositionToBoard(std::pair<int, int> position);

/**
 * @brief Converts a path list into a comma-separated string.
 * @param path Path history to format.
 * @return Path text such as "(0,0),(0,1),(1,1)".
 */
std::string formatPath(const std::list<std::pair<int, int>>& path);

/**
 * @brief Builds the final status text for a bug.
 *
 * Example results:
 * - "Alive!"
 * - "Eaten by 203"
 * - "Dead"
 *
 * @param bug Bug to inspect.
 * @return Final status text.
 */
std::string buildBugEndStateText(const Bug* bug);

/**
 * @brief Maps a bug type name to its board symbol.
 *
 * Example:
 * - "Crawler" -> "C"
 * - "Hopper" -> "H"
 * - "Hunter" / "HunterBug" -> "U"
 *
 * @param bugType Bug type name.
 * @return Single-character symbol as a string.
 */
std::string bugTypeToSymbol(const std::string& bugType);

}

#endif