#ifndef SEEDER_H
#define SEEDER_H

#include <random>

/**
 * @brief Central random number source for the whole program.
 *
 * I learned the Singleton idea in OOP classes. For this project I looked up
 * the common C++ version that uses a function local static instance.
 *
 * I used it here for a practical reason:
 * I only want one RNG shared by the whole program so that a seed can make
 * the simulation deterministic. That way, the same seed gives the same run.
 *
 * It also avoids accidentally creating different random generators in
 * different classes.
 */
class Seeder {
    std::mt19937 rng;
    unsigned int currentSeed;

    /**
     * @brief Private constructor so the class controls its single instance.
     */
    Seeder();

public:
    Seeder(const Seeder&) = delete;
    Seeder& operator=(const Seeder&) = delete;

    /**
     * @brief Gets the single shared Seeder instance.
     *
     * This is the C++ Singleton version I researched and applied here.
     * A function local static object is created once and then reused for the
     * whole program. That gives the project one shared RNG without manually passing it
     * around everywhere.
     *
     * @return Reference to the single Seeder object.
     */
    static Seeder& getInstance();

    /**
     * @brief Seeds the RNG with a user provided value.
     * @param seed Deterministic seed value.
     */
    void setSeed(unsigned int seed);

    /**
     * @brief Seeds the RNG using the system clock.
     *
     * Useful when I want a normal non-deterministic run.
     */
    void seedFromClock();

    /**
     * @brief Gets the shared RNG object.
     * @return Reference to the program's shared mt19937.
     */
    std::mt19937& getRNG();

    /**
     * @brief Gets the current seed value.
     * @return Current seed stored by Seeder.
     */
    unsigned int getCurrentSeed() const;
};

#endif