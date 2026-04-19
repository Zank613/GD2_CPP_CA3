#ifndef CONSOLE_RENDERER_H
#define CONSOLE_RENDERER_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Bug.h"

/**
 * @brief Raw ANSI escape sequences used by the console renderer.
 *
 * These are kept as macros because they are simple string constants.
 * The renderer can decide whether to apply them or ignore them when
 * colour output is disabled.
 */
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_BOLD    "\033[1m"

/**
 * @brief Logical colour names used by the renderer.
 *
 * The enum gives readable names in code, while the renderer maps them
 * to ANSI escape sequences internally.
 */
enum class Colour {
    RESET,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    BOLD
};

/**
 * @brief Handles console formatting, colour output, and board visualisation.
 *
 * This class is only responsible for presentation.
 * It does not contain simulation logic such as movement, fights,
 * file loading or board state updates.
 */
class ConsoleRenderer {
    bool useColour;
    bool showVisual;
    bool debugMode;

public:
    /**
     * @brief Constructs a console renderer with the chosen output settings.
     * @param useColour Enables ANSI colour formatting when true.
     * @param showVisual Enables board visualisation when true.
     * @param debugMode Enables debug-style output when true.
     */
    ConsoleRenderer(bool useColour = true, bool showVisual = true, bool debugMode = false);

    /**
     * @brief Checks whether colour output is enabled.
     * @return true if colour output is enabled.
     */
    bool isColourEnabled() const;

    /**
     * @brief Checks whether board visualisation is enabled.
     * @return true if board visualisation is enabled.
     */
    bool isVisualEnabled() const;

    /**
     * @brief Checks whether debug output is enabled.
     * @return true if debug output is enabled.
     */
    bool isDebugEnabled() const;

    /**
     * @brief Enables or disables colour output.
     * @param enabled New colour mode.
     */
    void setColourEnabled(bool enabled);

    /**
     * @brief Enables or disables board visualisation.
     * @param enabled New visual mode.
     */
    void setVisualEnabled(bool enabled);

    /**
     * @brief Enables or disables debug output.
     * @param enabled New debug mode.
     */
    void setDebugEnabled(bool enabled);

    /**
     * @brief Maps a logical colour to its ANSI escape sequence.
     * @param colour Logical colour value.
     * @return ANSI escape sequence, or an empty string if colour is disabled.
     */
    std::string colourToAnsi(Colour colour) const;

    /**
     * @brief Wraps text in a colour if colour output is enabled.
     * @param text Text to format.
     * @param colour Colour to apply.
     * @return Formatted string with ANSI codes or plain text.
     */
    std::string applyColour(const std::string& text, Colour colour) const;

    /**
     * @brief Returns the symbol used to represent a bug type on the board.
     * @param bug Bug to inspect.
     * @return Single-character symbol as a string.
     */
    std::string bugSymbol(const Bug* bug) const;

    /**
     * @brief Prints one bug in a human-readable format.
     * @param bug Bug to display.
     */
    void printBugDetails(const Bug* bug) const;

    /**
     * @brief Prints one bug's life history line.
     * @param bug Bug whose path and final state will be displayed.
     */
    void printLifeHistory(const Bug* bug) const;

    /**
     * @brief Renders a 10x10 board view using the current alive bug positions.
     * @param cellOccupants Mapping from board cells to bugs currently in those cells.
     */
    void renderBoard(const std::map<std::pair<int, int>, std::vector<Bug*>>& cellOccupants) const;

    /**
     * @brief Prints a formatted debug message if debug mode is enabled.
     * @param message Debug message text.
     */
    void printDebug(const std::string& message) const;

    /**
     * @brief Prints a general status or heading line.
     * @param message Text to print.
     */
    void printStatus(const std::string& message) const;
};

#endif