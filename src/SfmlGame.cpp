#include "SfmlGame.h"

#include "Seeder.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <optional>

SfmlGame::SfmlGame()
    : window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Playable Bug Sim"),
      fontLoaded(false),
      playerBug(nullptr),
      state(GameState::CHOOSE_TYPE),
      outputWritten(false),
      combatEventIndex(0),
      combatMessageIndex(0),
      pendingFinishAfterCombat(false),
      combatDurationSeconds(1.35f) {
    window.setFramerateLimit(60);
    loadAssets();
}

void SfmlGame::loadAssets() {
    fontLoaded = font.openFromFile("assets/font.ttf");

    textures.load("Crawler", "assets/crawler.png");
    textures.load("Hopper", "assets/hopper.png");
    textures.load("Hunter", "assets/hunter.png");
}

void SfmlGame::run() {
    while (window.isOpen()) {
        handleEvents();

        if (state == GameState::COMBAT) {
            updateCombatScreen();
        }

        render();
    }
}

void SfmlGame::handleEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (state == GameState::CHOOSE_TYPE) {
                handleChooseInput(keyPressed->code);
            } else if (state == GameState::PLAYING) {
                handlePlayingInput(keyPressed->code);
            } else if (state == GameState::COMBAT || state == GameState::FINISHED) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }
    }
}

void SfmlGame::handleChooseInput(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Key::Num1 || key == sf::Keyboard::Key::C) {
        startGame('C');
    } else if (key == sf::Keyboard::Key::Num2 || key == sf::Keyboard::Key::H) {
        startGame('H');
    } else if (key == sf::Keyboard::Key::Num3 || key == sf::Keyboard::Key::U) {
        startGame('U');
    } else if (key == sf::Keyboard::Key::Escape) {
        window.close();
    }
}

void SfmlGame::handlePlayingInput(sf::Keyboard::Key key) {
    Direction direction = Direction::NORTH;
    bool hasDirection = true;

    if (key == sf::Keyboard::Key::W || key == sf::Keyboard::Key::Up) {
        direction = Direction::NORTH;
    } else if (key == sf::Keyboard::Key::D || key == sf::Keyboard::Key::Right) {
        direction = Direction::EAST;
    } else if (key == sf::Keyboard::Key::S || key == sf::Keyboard::Key::Down) {
        direction = Direction::SOUTH;
    } else if (key == sf::Keyboard::Key::A || key == sf::Keyboard::Key::Left) {
        direction = Direction::WEST;
    } else if (key == sf::Keyboard::Key::Escape) {
        window.close();
        return;
    } else {
        hasDirection = false;
    }

    if (!hasDirection) {
        return;
    }

    bool turnAdvanced = board.playableTurn(playerBug, direction);

    if (!turnAdvanced) {
        return;
    }

    bool pendingFinish = playerBug == nullptr ||
                         !playerBug->isAlive() ||
                         board.isSimulationOver();

    handleFightEventsAfterTurn(pendingFinish);
}

void SfmlGame::startGame(char bugType) {
    board.initializeFromFile("data/bugs.txt");

    std::pair<int, int> spawnPosition = findPlayerSpawnPosition();
    playerBug = board.createPlayerBug(bugType, PLAYER_ID, spawnPosition);

    killFeed.clear();
    combatEvents.clear();
    combatMessages.clear();

    state = GameState::PLAYING;
    outputWritten = false;
}

std::pair<int, int> SfmlGame::findPlayerSpawnPosition() const {
    std::pair<int, int> preferred = {4, 4};

    if (board.isCellTraversable(preferred) && !isAliveBugAt(preferred)) {
        return preferred;
    }

    for (int radius = 1; radius < utils::BOARD_SIZE; radius++) {
        for (int y = 0; y < utils::BOARD_SIZE; y++) {
            for (int x = 0; x < utils::BOARD_SIZE; x++) {
                std::pair<int, int> position = {x, y};
                int distance = std::abs(x - preferred.first) + std::abs(y - preferred.second);

                if (distance == radius &&
                    board.isCellTraversable(position) &&
                    !isAliveBugAt(position)) {
                    return position;
                }
            }
        }
    }

    return {0, 0};
}

bool SfmlGame::isAliveBugAt(const std::pair<int, int>& position) const {
    const std::vector<Bug*>& bugs = board.getBugs();

    for (const Bug* bug : bugs) {
        if (bug != nullptr && bug->isAlive() && bug->getPosition() == position) {
            return true;
        }
    }

    return false;
}

bool SfmlGame::isSpectatorView() const {
    return state == GameState::FINISHED ||
           playerBug == nullptr ||
           !playerBug->isAlive();
}

bool SfmlGame::isVisibleToNormalPlayer(const std::pair<int, int>& position) const {
    if (isSpectatorView()) {
        return true;
    }

    if (playerBug == nullptr || !playerBug->isAlive()) {
        return false;
    }

    std::pair<int, int> playerPos = playerBug->getPosition();

    if (position == playerPos) {
        return true;
    }

    int dx = std::abs(position.first - playerPos.first);
    int dy = std::abs(position.second - playerPos.second);

    return dx + dy == 1;
}

bool SfmlGame::isVisibleToHunterPlayer(const std::pair<int, int>& position) const {
    if (isSpectatorView()) {
        return true;
    }

    if (playerBug == nullptr || !playerBug->isAlive()) {
        return false;
    }

    std::pair<int, int> playerPos = playerBug->getPosition();

    if (position == playerPos) {
        return true;
    }

    int dx = std::abs(position.first - playerPos.first);
    int dy = std::abs(position.second - playerPos.second);

    return dx + dy == 1;
}

bool SfmlGame::isCellVisibleToPlayer(const std::pair<int, int>& position) const {
    if (isSpectatorView()) {
        return true;
    }

    if (playerBug == nullptr || !playerBug->isAlive()) {
        return false;
    }

    if (playerBug->getType() == "Hunter") {
        return isVisibleToHunterPlayer(position);
    }

    return isVisibleToNormalPlayer(position);
}

bool SfmlGame::isInHunterScentRange(const std::pair<int, int>& position) const {
    if (playerBug == nullptr || !playerBug->isAlive()) {
        return false;
    }

    std::pair<int, int> playerPos = playerBug->getPosition();

    int dx = std::abs(position.first - playerPos.first);
    int dy = std::abs(position.second - playerPos.second);

    return dx + dy <= 2;
}

void SfmlGame::handleFightEventsAfterTurn(bool pendingFinish) {
    const std::vector<Board::FightEvent>& events = board.getRecentFightEvents();
    std::vector<Board::FightEvent> playerEvents;

    for (const Board::FightEvent& event : events) {
        if (fightInvolvesPlayer(event)) {
            playerEvents.push_back(event);
        }
    }

    addKillFeedFromEvents(events);

    if (!playerEvents.empty()) {
        beginCombatScreen(playerEvents, pendingFinish);
        return;
    }

    if (pendingFinish) {
        finishGame();
    }
}

bool SfmlGame::fightInvolvesPlayer(const Board::FightEvent& event) const {
    return event.firstId == PLAYER_ID || event.secondId == PLAYER_ID;
}

void SfmlGame::addKillFeedFromEvents(const std::vector<Board::FightEvent>& events) {
    for (const Board::FightEvent& event : events) {
        if (fightInvolvesPlayer(event)) {
            continue;
        }

        if (event.winnerId == -1 || event.loserId == -1) {
            addKillFeedLine(
                bugNameFromFightEvent(event, event.firstId) +
                " fought " +
                bugNameFromFightEvent(event, event.secondId)
            );
            continue;
        }

        addKillFeedLine(
            bugNameFromFightEvent(event, event.winnerId) +
            " ate " +
            bugNameFromFightEvent(event, event.loserId)
        );
    }
}

void SfmlGame::addKillFeedLine(const std::string& line) {
    killFeed.insert(killFeed.begin(), line);

    const std::size_t maxLines = 5;

    if (killFeed.size() > maxLines) {
        killFeed.resize(maxLines);
    }
}

std::string SfmlGame::bugNameFromFightEvent(const Board::FightEvent& event, int bugId) const {
    if (bugId == PLAYER_ID) {
        return "Player";
    }

    if (bugId == event.firstId) {
        return event.firstType + " #" + std::to_string(event.firstId);
    }

    if (bugId == event.secondId) {
        return event.secondType + " #" + std::to_string(event.secondId);
    }

    return "Bug #" + std::to_string(bugId);
}

std::string SfmlGame::formatCombatAction(
    const Board::FightEvent& event,
    const Board::CombatAction& action) const {
    std::string attacker = bugNameFromFightEvent(event, action.attackerId);
    std::string target = bugNameFromFightEvent(event, action.targetId);

    return attacker + " attacks " + target + " for " +
           std::to_string(action.damage) + " damage!";
}

void SfmlGame::buildCombatMessagesForCurrentEvent() {
    combatMessages.clear();
    combatMessageIndex = 0;

    if (combatEvents.empty() || combatEventIndex >= combatEvents.size()) {
        return;
    }

    const Board::FightEvent& event = combatEvents[combatEventIndex];

    combatMessages.push_back("BUG ENCOUNTERED! COMMENCING BATTLE!");

    const std::vector<Board::CombatAction>& actions = event.actions;

    for (std::size_t i = 0; i < actions.size(); i++) {
        if (i + 1 < actions.size()) {
            const Board::CombatAction& first = actions[i];
            const Board::CombatAction& second = actions[i + 1];

            if (second.attackerId == PLAYER_ID && first.attackerId != PLAYER_ID) {
                combatMessages.push_back(formatCombatAction(event, second));
                combatMessages.push_back(formatCombatAction(event, first));
                i++;
                continue;
            }
        }

        combatMessages.push_back(formatCombatAction(event, actions[i]));
    }

    if (event.loserId == PLAYER_ID || playerBug == nullptr || !playerBug->isAlive()) {
        combatMessages.push_back("YOU DIED");
    } else if (event.winnerId == PLAYER_ID) {
        combatMessages.push_back("PLAYER WINS!");
    } else if (event.winnerId != -1) {
        combatMessages.push_back(bugNameFromFightEvent(event, event.winnerId) + " wins!");
    } else {
        combatMessages.push_back("Both bugs collapse!");
    }
}

void SfmlGame::render() {
    window.clear(sf::Color(18, 18, 22));

    if (state == GameState::CHOOSE_TYPE) {
        renderChooseScreen();
    } else if (state == GameState::COMBAT) {
        renderPlayingScreen();
        renderCombatScreen();
    } else {
        renderPlayingScreen();
    }

    window.display();
}

void SfmlGame::renderChooseScreen() {
    drawText("Choose Your Bug", 310.0f, 150.0f, 36);
    drawText("1 / C  - Crawler", 330.0f, 240.0f, 24);
    drawText("2 / H  - Hopper", 330.0f, 290.0f, 24);
    drawText("3 / U  - Hunter", 330.0f, 340.0f, 24);
    drawText("ESC - Quit", 330.0f, 430.0f, 18);
}

void SfmlGame::renderPlayingScreen() {
    bool playerIsHunter = playerBug != nullptr && playerBug->getType() == "Hunter";
    bool spectator = isSpectatorView();

    for (int y = 0; y < utils::BOARD_SIZE; y++) {
        for (int x = 0; x < utils::BOARD_SIZE; x++) {
            std::pair<int, int> position = {x, y};
            bool visible = isCellVisibleToPlayer(position);

            drawCell(position, visible);
        }
    }

    if (playerIsHunter && !spectator && playerBug != nullptr && playerBug->isAlive()) {
        for (int y = 0; y < utils::BOARD_SIZE; y++) {
            for (int x = 0; x < utils::BOARD_SIZE; x++) {
                std::pair<int, int> position = {x, y};

                if (isInHunterScentRange(position)) {
                    drawScentOverlay(position);
                }
            }
        }
    }

    const std::vector<Bug*>& bugs = board.getBugs();

    for (const Bug* bug : bugs) {
        drawBug(bug);
    }

    drawText("Playable Bug Sim", 710.0f, 40.0f, 20);
    drawText("Move: WASD / Arrows", 710.0f, 90.0f, 16);
    drawText("Tap: " + std::to_string(board.getTapCount()), 710.0f, 130.0f, 16);
    drawText("Alive: " + std::to_string(board.getAliveBugCount()), 710.0f, 160.0f, 16);

    if (playerBug != nullptr) {
        drawText("Player: " + playerBug->getType(), 710.0f, 210.0f, 16);
        drawText("Health: " + std::to_string(playerBug->getHealth()), 710.0f, 240.0f, 16);
    }

    if (!spectator && playerIsHunter) {
        drawText("Hunter Mode: blind", 710.0f, 290.0f, 16);
        drawText("Yellow = scent", 710.0f, 320.0f, 16);
    } else if (!spectator) {
        drawText("Fog hides bugs only", 710.0f, 290.0f, 16);
        drawText("Terrain is always visible", 710.0f, 320.0f, 16);
    }

    renderKillFeed();

    if (spectator) {
        drawText("Spectator View", 710.0f, 330.0f, 20);

        if (playerBug != nullptr && playerBug->isAlive()) {
            drawText("You survived.", 710.0f, 370.0f, 16);
        } else {
            drawText("You were eaten.", 710.0f, 370.0f, 16);
        }

        drawText("Whole board revealed.", 710.0f, 400.0f, 16);
        drawText("ESC - Quit", 710.0f, 440.0f, 16);
    }
}

void SfmlGame::drawCell(const std::pair<int, int>& position, bool visible) {
    sf::RectangleShape cell({TILE_SIZE - 2.0f, TILE_SIZE - 2.0f});
    cell.setPosition(boardToPixel(position));
    cell.setOutlineThickness(1.0f);

    if (!visible) {
        cell.setFillColor(sf::Color(5, 5, 8));
        cell.setOutlineColor(sf::Color(18, 18, 24));
        window.draw(cell);
        return;
    }

    cell.setOutlineColor(sf::Color(65, 65, 75));

    TerrainType terrain = board.getTerrainAt(position);

    switch (terrain) {
        case TerrainType::MUD:
            cell.setFillColor(sf::Color(95, 62, 36));
            break;

        case TerrainType::ROCK:
            cell.setFillColor(sf::Color(90, 90, 95));
            break;

        case TerrainType::FOOD:
            cell.setFillColor(sf::Color(45, 120, 45));
            break;

        case TerrainType::NORMAL:
        default:
            cell.setFillColor(sf::Color(32, 38, 32));
            break;
    }

    window.draw(cell);
}

void SfmlGame::drawScentOverlay(const std::pair<int, int>& position) {
    double scent = board.getScentAt(position);

    if (scent <= 0.0) {
        return;
    }

    int alpha = static_cast<int>(90.0 + scent * 45.0);

    if (alpha > 220) {
        alpha = 220;
    }

    sf::RectangleShape overlay({TILE_SIZE - 8.0f, TILE_SIZE - 8.0f});
    overlay.setPosition(boardToPixel(position) + sf::Vector2f(4.0f, 4.0f));
    overlay.setFillColor(sf::Color(255, 218, 0, static_cast<std::uint8_t>(alpha)));
    overlay.setOutlineThickness(2.0f);
    overlay.setOutlineColor(sf::Color(255, 245, 120));

    window.draw(overlay);

    if (fontLoaded) {
        sf::Text scentText(font, "S", 18);
        scentText.setFillColor(sf::Color(40, 30, 0));
        scentText.setStyle(sf::Text::Bold);
        scentText.setPosition(boardToPixel(position) + sf::Vector2f(24.0f, 20.0f));

        window.draw(scentText);
    }
}

void SfmlGame::drawBug(const Bug* bug) {
    if (bug == nullptr || !bug->isAlive()) {
        return;
    }

    bool spectator = isSpectatorView();
    bool playerIsHunter = playerBug != nullptr && playerBug->getType() == "Hunter";

    if (bug == playerBug) {
        drawBugGraphic(bug, boardToPixel(bug->getPosition()));
        return;
    }

    if (spectator) {
        drawBugGraphic(bug, boardToPixel(bug->getPosition()));
        return;
    }

    if (playerIsHunter) {
        return;
    }

    if (!isVisibleToNormalPlayer(bug->getPosition())) {
        return;
    }

    drawBugGraphic(bug, boardToPixel(bug->getPosition()));
}

void SfmlGame::drawBugGraphic(const Bug* bug, const sf::Vector2f& pixelPosition) {
    if (bug == nullptr) {
        return;
    }

    drawBugTypeGraphic(
        bug->getType(),
        {pixelPosition.x + 6.0f, pixelPosition.y + 6.0f},
        static_cast<float>(TILE_SIZE - 12)
    );
}

void SfmlGame::drawBugTypeGraphic(
    const std::string& bugType,
    const sf::Vector2f& pixelPosition,
    float size) {
    const sf::Texture* texture = textures.get(bugType);

    if (texture != nullptr) {
        sf::Sprite sprite(*texture);

        sf::Vector2u textureSize = texture->getSize();
        float scaleX = size / static_cast<float>(textureSize.x);
        float scaleY = size / static_cast<float>(textureSize.y);

        sprite.setScale({scaleX, scaleY});
        sprite.setPosition(pixelPosition);

        window.draw(sprite);
        return;
    }

    sf::CircleShape fallback(size / 2.0f);
    fallback.setPosition(pixelPosition);
    fallback.setFillColor(bugTypeColour(bugType));
    fallback.setOutlineThickness(3.0f);
    fallback.setOutlineColor(sf::Color::White);

    window.draw(fallback);
}

sf::Color SfmlGame::bugTypeColour(const std::string& bugType) const {
    if (bugType == "Crawler") {
        return sf::Color(0, 210, 220);
    }

    if (bugType == "Hopper") {
        return sf::Color(230, 210, 60);
    }

    if (bugType == "Hunter") {
        return sf::Color(80, 120, 255);
    }

    return sf::Color(220, 220, 220);
}

void SfmlGame::drawText(const std::string& text, float x, float y, unsigned int size) {
    if (!fontLoaded) {
        return;
    }

    sf::Text drawable(font, text, size);
    drawable.setFillColor(sf::Color(230, 230, 235));
    drawable.setPosition({x, y});

    window.draw(drawable);
}

void SfmlGame::renderKillFeed() {
    if (killFeed.empty()) {
        return;
    }

    drawText("Kill Feed", 710.0f, 500.0f, 18);

    float y = 532.0f;

    for (const std::string& line : killFeed) {
        drawText(line, 710.0f, y, 13);
        y += 24.0f;
    }
}

sf::Vector2f SfmlGame::boardToPixel(const std::pair<int, int>& position) const {
    return {
        static_cast<float>(BOARD_OFFSET_X + position.first * TILE_SIZE),
        static_cast<float>(BOARD_OFFSET_Y + position.second * TILE_SIZE)
    };
}

void SfmlGame::finishGame() {
    state = GameState::FINISHED;

    if (!outputWritten) {
        board.writeLifeHistoryToFile();
        outputWritten = true;
    }
}

void SfmlGame::beginCombatScreen(
    const std::vector<Board::FightEvent>& playerEvents,
    bool pendingFinish) {
    combatEvents = playerEvents;
    combatEventIndex = 0;
    pendingFinishAfterCombat = pendingFinish;

    buildCombatMessagesForCurrentEvent();

    combatClock.restart();
    state = GameState::COMBAT;
}

void SfmlGame::updateCombatScreen() {
    if (combatClock.getElapsedTime().asSeconds() < combatDurationSeconds) {
        return;
    }

    combatMessageIndex++;

    if (combatMessageIndex < combatMessages.size()) {
        combatClock.restart();
        return;
    }

    combatEventIndex++;

    if (combatEventIndex < combatEvents.size()) {
        buildCombatMessagesForCurrentEvent();
        combatClock.restart();
        return;
    }

    combatEvents.clear();
    combatMessages.clear();
    combatEventIndex = 0;
    combatMessageIndex = 0;

    if (pendingFinishAfterCombat || playerBug == nullptr || !playerBug->isAlive()) {
        finishGame();
    } else {
        state = GameState::PLAYING;
    }
}

void SfmlGame::renderCombatScreen() {
    if (combatEvents.empty() ||
        combatEventIndex >= combatEvents.size() ||
        combatMessages.empty() ||
        combatMessageIndex >= combatMessages.size()) {
        return;
    }

    const Board::FightEvent& event = combatEvents[combatEventIndex];
    const std::string& message = combatMessages[combatMessageIndex];

    sf::RectangleShape dim({static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)});
    dim.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(dim);

    if (message == "YOU DIED") {
        drawText("YOU DIED", 345.0f, 250.0f, 58);
        drawText("The arena fades into spectator view...", 285.0f, 335.0f, 22);
        return;
    }

    bool playerIsFirst = event.firstId == PLAYER_ID;

    std::string playerType = playerIsFirst ? event.firstType : event.secondType;
    std::string enemyType = playerIsFirst ? event.secondType : event.firstType;
    int enemyId = playerIsFirst ? event.secondId : event.firstId;

    sf::RectangleShape panel({720.0f, 400.0f});
    panel.setPosition({120.0f, 150.0f});
    panel.setFillColor(sf::Color(24, 24, 30));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(180, 40, 40));
    window.draw(panel);

    drawText("BUG ENCOUNTERED!", 305.0f, 175.0f, 30);

    drawBugTypeGraphic(playerType, {230.0f, 260.0f}, 110.0f);
    drawBugTypeGraphic(enemyType, {620.0f, 260.0f}, 110.0f);

    drawText("PLAYER", 245.0f, 390.0f, 20);
    drawText(enemyType + " #" + std::to_string(enemyId), 590.0f, 390.0f, 20);

    drawText("VS", 462.0f, 305.0f, 40);

    sf::RectangleShape messageBox({620.0f, 80.0f});
    messageBox.setPosition({170.0f, 445.0f});
    messageBox.setFillColor(sf::Color(12, 12, 16));
    messageBox.setOutlineThickness(2.0f);
    messageBox.setOutlineColor(sf::Color(210, 210, 220));
    window.draw(messageBox);

    drawText(message, 195.0f, 470.0f, 20);
}