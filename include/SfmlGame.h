#ifndef SFML_GAME_H
#define SFML_GAME_H

#include <SFML/Graphics.hpp>

#include <string>
#include <utility>
#include <vector>

#include "Board.h"
#include "Bug.h"
#include "TextureManager.h"

/**
 * @brief Playable SFML version of the bug simulation.
 *
 * The player chooses a bug type and controls one bug. Each valid player move
 * advances the simulation by one turn.
 */
class SfmlGame {
    static const int TILE_SIZE = 64;
    static const int BOARD_PIXEL_SIZE = utils::BOARD_SIZE * TILE_SIZE;
    static const int WINDOW_WIDTH = 960;
    static const int WINDOW_HEIGHT = 720;
    static const int BOARD_OFFSET_X = 32;
    static const int BOARD_OFFSET_Y = 40;
    static const int PLAYER_ID = 999;

    enum class GameState {
        CHOOSE_TYPE,
        PLAYING,
        COMBAT,
        FINISHED
    };

    std::vector<Board::FightEvent> combatEvents;
    std::size_t combatEventIndex;

    std::vector<std::string> combatMessages;
    std::size_t combatMessageIndex;

    std::vector<std::string> killFeed;

    bool pendingFinishAfterCombat;
    sf::Clock combatClock;
    float combatDurationSeconds;

    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    Board board;
    TextureManager textures;
    Bug* playerBug;
    GameState state;
    bool outputWritten;

    /**
     * @brief Loads fonts and textures.
     */
    void loadAssets();

    /**
     * @brief Starts a new playable game.
     * @param bugType Player bug type.
     */
    void startGame(char bugType);

    /**
     * @brief Handles SFML events.
     */
    void handleEvents();

    /**
     * @brief Handles input on the bug selection screen.
     * @param key Pressed key.
     */
    void handleChooseInput(sf::Keyboard::Key key);

    /**
     * @brief Handles input while playing.
     * @param key Pressed key.
     */
    void handlePlayingInput(sf::Keyboard::Key key);

    /**
     * @brief Finds a safe spawn position for the player.
     * @return Spawn position.
     */
    std::pair<int, int> findPlayerSpawnPosition() const;

    /**
     * @brief Checks whether an alive bug occupies a position.
     * @param position Position to inspect.
     * @return true if occupied by an alive bug.
     */
    bool isAliveBugAt(const std::pair<int, int>& position) const;

    /**
     * @brief Checks whether the player is dead or the simulation has finished.
     * @return true if full spectator view should be shown.
     */
    bool isSpectatorView() const;

    /**
     * @brief Checks if a cell is visible to a normal non-Hunter player.
     * @param position Cell position.
     * @return true if visible.
     */
    bool isVisibleToNormalPlayer(const std::pair<int, int>& position) const;

    /**
     * @brief Checks if a cell is visible to the Hunter player.
     *
     * Hunter is mostly blind, but can still sense nearby terrain so movement
     * does not feel completely random.
     *
     * @param position Cell position.
     * @return true if the cell is visible to Hunter.
     */
    bool isVisibleToHunterPlayer(const std::pair<int, int>& position) const;

    /**
     * @brief Checks whether a board cell is visible to the current player.
     * @param position Cell position.
     * @return true if the cell should be drawn normally.
     */
    bool isCellVisibleToPlayer(const std::pair<int, int>& position) const;

    /**
     * @brief Checks if a cell can show scent to a Hunter player.
     * @param position Cell position.
     * @return true if within Hunter scent range.
     */
    bool isInHunterScentRange(const std::pair<int, int>& position) const;

    /**
     * @brief Handles fight events after a playable turn.
     * @param pendingFinish Whether the game should finish after combat handling.
     */
    void handleFightEventsAfterTurn(bool pendingFinish);

    /**
     * @brief Checks whether a fight involves the player.
     * @param event Fight event to inspect.
     * @return true if the player is one of the fighters.
     */
    bool fightInvolvesPlayer(const Board::FightEvent& event) const;

    /**
     * @brief Adds non-player fights to the kill feed.
     * @param events Fight events from the latest turn.
     */
    void addKillFeedFromEvents(const std::vector<Board::FightEvent>& events);

    /**
     * @brief Adds one line to the kill feed.
     * @param line Text to add.
     */
    void addKillFeedLine(const std::string& line);

    /**
     * @brief Gets a display name for a bug id inside a fight event.
     * @param event Fight event.
     * @param bugId Bug id.
     * @return Display name.
     */
    std::string bugNameFromFightEvent(const Board::FightEvent& event, int bugId) const;

    /**
     * @brief Builds combat messages for the current player fight.
     */
    void buildCombatMessagesForCurrentEvent();

    /**
     * @brief Makes one combat action readable.
     * @param event Fight event containing the action.
     * @param action Damage action to format.
     * @return Message describing the action.
     */
    std::string formatCombatAction(const Board::FightEvent& event,
                                   const Board::CombatAction& action) const;

    /**
     * @brief Draws the current screen.
     */
    void render();

    /**
     * @brief Draws the type selection screen.
     */
    void renderChooseScreen();

    /**
     * @brief Draws the playable board screen.
     */
    void renderPlayingScreen();

    /**
     * @brief Draws one board cell.
     * @param position Board position.
     * @param visible Whether the cell is visible.
     */
    void drawCell(const std::pair<int, int>& position, bool visible);

    /**
     * @brief Draws a scent overlay on a cell.
     * @param position Board position.
     */
    void drawScentOverlay(const std::pair<int, int>& position);

    /**
     * @brief Draws a bug if visible.
     * @param bug Bug to draw.
     */
    void drawBug(const Bug* bug);

    /**
     * @brief Draws a bug sprite or fallback shape.
     * @param bug Bug to draw.
     * @param pixelPosition Pixel position.
     */
    void drawBugGraphic(const Bug* bug, const sf::Vector2f& pixelPosition);

    /**
     * @brief Draws a bug graphic using only a bug type name.
     * @param bugType Bug type name.
     * @param pixelPosition Pixel position.
     * @param size Desired sprite size.
     */
    void drawBugTypeGraphic(const std::string& bugType,
                            const sf::Vector2f& pixelPosition,
                            float size);

    /**
     * @brief Gets the fallback colour for a bug type.
     * @param bugType Bug type name.
     * @return SFML colour for fallback rendering.
     */
    sf::Color bugTypeColour(const std::string& bugType) const;

    /**
     * @brief Draws text if the font loaded.
     * @param text Text to draw.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param size Character size.
     */
    void drawText(const std::string& text, float x, float y, unsigned int size);

    /**
     * @brief Draws the kill feed on the side panel.
     */
    void renderKillFeed();

    /**
     * @brief Converts board position to pixel position.
     * @param position Board position.
     * @return Pixel position.
     */
    sf::Vector2f boardToPixel(const std::pair<int, int>& position) const;

    /**
     * @brief Finishes the playable simulation and writes output once.
     */
    void finishGame();

    /**
     * @brief Starts the player combat replay screen.
     * @param playerEvents Fight events involving the player.
     * @param pendingFinish Whether the game should finish after combat display.
     */
    void beginCombatScreen(const std::vector<Board::FightEvent>& playerEvents,
                           bool pendingFinish);

    /**
     * @brief Advances combat replay timing.
     */
    void updateCombatScreen();

    /**
     * @brief Draws the combat replay screen.
     */
    void renderCombatScreen();

public:
    /**
     * @brief Constructs the SFML game.
     */
    SfmlGame();

    /**
     * @brief Runs the SFML game loop.
     */
    void run();
};

#endif