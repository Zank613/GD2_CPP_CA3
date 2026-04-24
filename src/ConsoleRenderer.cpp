#include "ConsoleRenderer.h"
#include "Hopper.h"
#include "Utils.h"
#include "Terrain.h"

#include <iostream>
#include <sstream>
#include <iomanip>

ConsoleRenderer::ConsoleRenderer(bool useColour, bool showVisual, bool debugMode)
    : useColour(useColour), showVisual(showVisual), debugMode(debugMode) {
}

bool ConsoleRenderer::isColourEnabled() const {
    return useColour;
}

bool ConsoleRenderer::isVisualEnabled() const {
    return showVisual;
}

bool ConsoleRenderer::isDebugEnabled() const {
    return debugMode;
}

void ConsoleRenderer::setColourEnabled(bool enabled) {
    useColour = enabled;
}

void ConsoleRenderer::setVisualEnabled(bool enabled) {
    showVisual = enabled;
}

void ConsoleRenderer::setDebugEnabled(bool enabled) {
    debugMode = enabled;
}

std::string ConsoleRenderer::colourToAnsi(Colour colour) const {
    if (!useColour) {
        return "";
    }

    switch (colour) {
        case Colour::RESET:
            return ANSI_RESET;
        case Colour::RED:
            return ANSI_RED;
        case Colour::GREEN:
            return ANSI_GREEN;
        case Colour::YELLOW:
            return ANSI_YELLOW;
        case Colour::BLUE:
            return ANSI_BLUE;
        case Colour::MAGENTA:
            return ANSI_MAGENTA;
        case Colour::CYAN:
            return ANSI_CYAN;
        case Colour::GREY:
            return ANSI_GREY;
        case Colour::WHITE:
            return ANSI_WHITE;

        case Colour::BRIGHT_RED:
            return ANSI_BRIGHT_RED;
        case Colour::BRIGHT_GREEN:
            return ANSI_BRIGHT_GREEN;
        case Colour::BRIGHT_YELLOW:
            return ANSI_BRIGHT_YELLOW;
        case Colour::BRIGHT_BLUE:
            return ANSI_BRIGHT_BLUE;
        case Colour::BRIGHT_MAGENTA:
            return ANSI_BRIGHT_MAGENTA;
        case Colour::BRIGHT_CYAN:
            return ANSI_BRIGHT_CYAN;
        case Colour::BRIGHT_WHITE:
            return ANSI_BRIGHT_WHITE;

        case Colour::ORANGE:
            return ANSI_ORANGE;
        case Colour::BROWN:
            return ANSI_BROWN;
        case Colour::DARK_GREEN:
            return ANSI_DARK_GREEN;

        case Colour::BOLD:
            return ANSI_BOLD;
        default:
            return "";
    }
}

std::string ConsoleRenderer::applyColour(const std::string& text, Colour colour) const {
    if (!useColour) {
        return text;
    }

    return colourToAnsi(colour) + text + colourToAnsi(Colour::RESET);
}

std::string ConsoleRenderer::bugSymbol(const Bug* bug) const {
    if (bug == nullptr) {
        return ".";
    }

    return utils::bugTypeToSymbol(bug->getType());
}

std::string ConsoleRenderer::terrainSymbol(TerrainType terrain) const {
    switch (terrain) {
        case TerrainType::MUD:
            return "M";
        case TerrainType::ROCK:
            return "R";
        case TerrainType::FOOD:
            return "F";
        case TerrainType::NORMAL:
        default:
            return ".";
    }
}

void ConsoleRenderer::printBugDetails(const Bug* bug) const {
    if (bug == nullptr) {
        return;
    }

    std::pair<int, int> pos = bug->getPosition();

    std::string typeText = bug->getType();
    if (typeText == "Crawler") {
        typeText = applyColour(typeText, Colour::CYAN);
    } else if (typeText == "Hopper") {
        typeText = applyColour(typeText, Colour::YELLOW);
    } else if (typeText == "Hunter") {
        typeText = applyColour(typeText, Colour::BLUE);
    }

    std::cout << bug->getId() << " "
              << typeText << " "
              << "(" << pos.first << "," << pos.second << ") "
              << bug->getHealth() << " "
              << utils::directionToString(bug->getDirection()) << " ";

    const Hopper* hopper = dynamic_cast<const Hopper*>(bug);
    if (hopper != nullptr) {
        std::cout << hopper->getHopLength() << " ";
    }

    std::string statusText = utils::buildBugEndStateText(bug);

    if (bug->isAlive()) {
        std::cout << applyColour(statusText, Colour::GREEN);
    } else {
        std::cout << applyColour(statusText, Colour::RED);
    }

    std::cout << "\n";
}

void ConsoleRenderer::printLifeHistory(const Bug* bug) const {
    if (bug == nullptr) {
        return;
    }

    std::string typeText = bug->getType();
    if (typeText == "Crawler") {
        typeText = applyColour(typeText, Colour::CYAN);
    } else if (typeText == "Hopper") {
        typeText = applyColour(typeText, Colour::YELLOW);
    } else if (typeText == "Hunter") {
        typeText = applyColour(typeText, Colour::BLUE);
    }

    std::cout << bug->getId() << " "
          << typeText << " Path: "
          << utils::formatPath(bug->getPath()) << " ";

    std::string statusText = utils::buildBugEndStateText(bug);

    if (bug->isAlive()) {
        std::cout << applyColour(statusText, Colour::GREEN);
    } else {
        std::cout << applyColour(statusText, Colour::RED);
    }

    std::cout << "\n";
}

void ConsoleRenderer::renderBoard( const std::map<std::pair<int, int>, std::vector<Bug*>>& cellOccupants,
    const TerrainType terrainGrid[utils::BOARD_SIZE][utils::BOARD_SIZE]) const {
    if (!showVisual) {
        return;
    }

    std::string board[utils::BOARD_SIZE][utils::BOARD_SIZE];

    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            std::string symbol = terrainSymbol(terrainGrid[y][x]);

            switch (terrainGrid[y][x]) {
                case TerrainType::MUD:
                    board[y][x] = applyColour(symbol, Colour::BROWN);
                    break;
                case TerrainType::ROCK:
                    board[y][x] = applyColour(symbol, Colour::GREY);
                    break;
                case TerrainType::FOOD:
                    board[y][x] = applyColour(symbol, Colour::BRIGHT_GREEN);
                    break;
                case TerrainType::NORMAL:
                default:
                    board[y][x] = symbol;
                    break;
            }
        }
    }

    for (const auto& entry : cellOccupants) {
        int x = entry.first.first;
        int y = entry.first.second;
        const std::vector<Bug*>& bugsInCell = entry.second;

        if (bugsInCell.empty()) {
            continue;
        }

        if (bugsInCell.size() > 1) {
            board[y][x] = applyColour("*", Colour::MAGENTA);
            continue;
        }

        const Bug* bug = bugsInCell[0];
        std::string symbol = bugSymbol(bug);
        std::string type = bug->getType();

        if (type == "Crawler") {
            board[y][x] = applyColour(symbol, Colour::CYAN);
        } else if (type == "Hopper") {
            board[y][x] = applyColour(symbol, Colour::YELLOW);
        } else if (type == "Hunter") {
            board[y][x] = applyColour(symbol, Colour::BLUE);
        } else {
            board[y][x] = symbol;
        }
    }

    std::cout << "\n";
    std::cout << applyColour("   0 1 2 3 4 5 6 7 8 9", Colour::BOLD) << "\n";

    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        std::cout << applyColour(std::to_string(y) + "  ", Colour::BOLD);

        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            std::cout << board[y][x];
            if (x < utils::BOARD_SIZE - 1) {
                std::cout << " ";
            }
        }

        std::cout << "\n";
    }

    std::cout << "\n";
}

void ConsoleRenderer::printDebug(const std::string& message) const {
    if (!debugMode) {
        return;
    }

    std::cout << applyColour("[DEBUG] ", Colour::MAGENTA) << message << "\n";
}

void ConsoleRenderer::printStatus(const std::string& message) const {
    std::cout << applyColour(message, Colour::BOLD) << "\n";
}

void ConsoleRenderer::printHeatmap(const std::string& title, const int heatmap[utils::BOARD_SIZE][utils::BOARD_SIZE]) const {
    if (!showVisual) {
        return;
    }

    std::cout << "\n";
    std::cout << applyColour(title, Colour::BOLD) << "\n";
    std::cout << applyColour("    0  1  2  3  4  5  6  7  8  9", Colour::BOLD) << "\n";

    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        std::cout << applyColour(std::to_string(y) + " ", Colour::BOLD);

        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            int value = heatmap[y][x];

            std::string cellText;
            if (value == 0) {
                cellText = " .";
            } else if (value < 10) {
                cellText = " " + std::to_string(value);
            } else {
                cellText = std::to_string(value);
            }

            if (value == 0) {
                std::cout << " " << cellText;
            } else if (value < 5) {
                std::cout << " " << applyColour(cellText, Colour::YELLOW);
            } else if (value < 10) {
                std::cout << " " << applyColour(cellText, Colour::ORANGE);
            } else {
                std::cout << " " << applyColour(cellText, Colour::RED);
            }
        }

        std::cout << "\n";
    }

    std::cout << "\n";
}