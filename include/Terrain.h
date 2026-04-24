#ifndef TERRAIN_H
#define TERRAIN_H

/**
 * @brief Terrain types used by the board.
 *
 * NORMAL has no effect.
 * MUD slows bugs by making them skip a later movement.
 * ROCK blocks landing or stepping onto that cell.
 * FOOD heals a bug and is consumed after use.
 */
enum class TerrainType {
    NORMAL,
    MUD,
    ROCK,
    FOOD
};

#endif