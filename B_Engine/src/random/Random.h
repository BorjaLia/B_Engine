#pragma once

#include <random>
#include <string>
#include <sstream>
#include <cstdint>

#include "../math/Vector2.h"

namespace Engine
{
    /// A robust random number generator using the Mersenne Twister engine.
    /// Supports deterministic seeding, state serialization for replays, and spatial hashing.
    /// @ingroup Utils
    class Random
    {
    public:
#pragma region Constructors
        /// Initializes the generator using hardware entropy for a truly random seed.
        Random();

        /// Initializes the generator with a specific seed for deterministic results.
        /// @param seed The deterministic seed.
        Random(uint32_t seed);
#pragma endregion

#pragma region State Management
        /// Sets a new seed and resets the internal generator.
        void SetSeed(uint32_t seed);

        uint32_t GetSeed() const { return currentSeed; }

        /// Serializes the entire internal state of the Mersenne Twister.
        /// Useful for save states or deterministic replays.
        std::string GetState() const;

        /// Reconstructs the internal state from a serialized string.
        void SetState(const std::string& state);
#pragma endregion

#pragma region Sequential Uniform RNG
        /// Returns a random integer between min and max (inclusive).
        int GetInt(int min, int max);

        /// Returns a random float between min and max (inclusive).
        float GetFloat(float min, float max);

        /// Returns true if a random roll is less than or equal to the probability.
        /// @param probability A value between 0.0f and 1.0f.
        bool GetChance(float probability);

        /// Returns a normalized 2D vector pointing in a random direction.
        Vector2f GetDirection2D();
#pragma endregion

#pragma region Sequential Gaussian RNG
        /// Returns a random float using a normal (Gaussian) distribution.
        /// @param mean The central value.
        /// @param stdDev The standard deviation (spread).
        float GetGaussian(float mean, float stdDev);
#pragma endregion

#pragma region Spatial RNG (Stateless)
        /// Returns a deterministic pseudo-random float between 0.0f and 1.0f based on a 1D coordinate and a seed.
        static float GetHash1D(int x, uint32_t seed);

        /// Returns a deterministic pseudo-random float between 0.0f and 1.0f based on a 2D coordinate and a seed.
        static float GetHash2D(int x, int y, uint32_t seed);
#pragma endregion

    private:
        std::mt19937 generator;
        uint32_t currentSeed;
    };
}