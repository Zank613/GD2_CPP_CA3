#include "Board.h"
#include "ConsoleRenderer.h"
#include "Seeder.h"

#include <iostream>
#include <limits>
#include <sstream>
#include <string>

/**
 * @brief Stores simple command-line options for the program.
 */
struct ProgramOptions {
    bool useCustomSeed = false;
    unsigned int seed = 0;

    bool showVisual = true;
    bool useColour = true;
    bool debugMode = false;
};

/**
 * @brief Parses command-line arguments in a simple way.
 * @param argc Argument count from main.
 * @param argv Argument values from main.
 * @param options Output options structure.
 * @return true if parsing succeeded, false otherwise.
 */
bool parseArguments(int argc, char* argv[], ProgramOptions& options) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--seed") {
            if (i + 1 >= argc) {
                std::cout << "Missing value after --seed\n";
                return false;
            }

            std::stringstream ss(argv[i + 1]);
            unsigned int parsedSeed;

            if (!(ss >> parsedSeed)) {
                std::cout << "Invalid seed value\n";
                return false;
            }

            options.seed = parsedSeed;
            options.useCustomSeed = true;
            i++;
        } else if (arg == "--novisual") {
            options.showVisual = false;
        } else if (arg == "--nocolour") {
            options.useColour = false;
        } else if (arg == "--debug") {
            options.debugMode = true;
        } else {
            std::cout << "Unknown argument: " << arg << "\n";
            return false;
        }
    }

    return true;
}

/**
 * @brief Prints the program menu.
 */
void printMenu() {
    std::cout << "\n===== BUG BOARD MENU =====\n";
    std::cout << "1. Initialize Bug Board (load data from file)\n";
    std::cout << "2. Display all Bugs\n";
    std::cout << "3. Find a Bug (given an id)\n";
    std::cout << "4. Tap the Bug Board\n";
    std::cout << "5. Display Life History of all Bugs\n";
    std::cout << "6. Display all Cells listing their Bugs\n";
    std::cout << "7. Run simulation\n";
    std::cout << "8. Exit\n";
    std::cout << "Choose an option: ";
}

/**
 * @brief Reads an integer safely from standard input.
 * @param value Output integer value.
 * @return true if successful, false otherwise.
 */
bool readInt(int& value) {
    std::cin >> value;

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

int main(int argc, char* argv[]) {
    ProgramOptions options;

    if (!parseArguments(argc, argv, options)) {
        std::cout << "Usage: ./GD2_CPP_CA3 [--seed N] [--novisual] [--nocolour] [--debug]\n";
        return 1;
    }

    if (options.useCustomSeed) {
        Seeder::getInstance().setSeed(options.seed);
    } else {
        Seeder::getInstance().seedFromClock();
    }

    ConsoleRenderer renderer(options.useColour, options.showVisual, options.debugMode);

    Board board;
    board.setRenderer(&renderer);

    std::cout << "Seed: " << Seeder::getInstance().getCurrentSeed() << "\n";

    if (options.debugMode) {
        renderer.printDebug("Debug mode enabled");
        renderer.printDebug("Visual mode: " + std::string(options.showVisual ? "on" : "off"));
        renderer.printDebug("Colour mode: " + std::string(options.useColour ? "on" : "off"));
    }

    bool boardInitialized = false;
    bool outputWritten = false;

    while (true) {
        printMenu();

        int choice;
        if (!readInt(choice)) {
            std::cout << "Invalid input. Please enter a number from 1 to 8.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                board.initializeFromFile("data/bugs.txt");
                boardInitialized = true;
                outputWritten = false;
                renderer.printStatus("Bug board initialized from data/bugs.txt");

                if (options.debugMode) {
                    renderer.printDebug("Board initialized from file");
                }

                break;
            }

            case 2: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                board.displayAllBugs();
                break;
            }

            case 3: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                int bugId;
                std::cout << "Enter bug id: ";

                if (!readInt(bugId)) {
                    std::cout << "Invalid bug id.\n";
                    break;
                }

                board.findBug(bugId);
                break;
            }

            case 4: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                board.tapBoard();
                renderer.printStatus("Board tapped. Tap count: " + std::to_string(board.getTapCount()));
                break;
            }

            case 5: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                board.displayLifeHistory();
                break;
            }

            case 6: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                board.displayAllCells();
                break;
            }

            case 7: {
                if (!boardInitialized) {
                    std::cout << "Please initialize the board first.\n";
                    break;
                }

                board.runSimulation();
                outputWritten = true;
                break;
            }

            case 8: {
                if (boardInitialized && !outputWritten) {
                    board.writeLifeHistoryToFile();
                    renderer.printStatus("Life history written to output file.");
                }

                renderer.printStatus("Goodbye.");
                return 0;
            }

            default:
                std::cout << "Please choose a number from 1 to 8.\n";
                break;
        }
    }
}