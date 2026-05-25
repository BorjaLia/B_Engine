#include "Random.h"

#include <sstream>
#include <cmath>

#include "../math/MathUtils.h"
#include "../debug/Debug.h"

namespace Engine
{
    Random::Random()
    {
        // Use random_device to ask the OS for a truly random number
        std::random_device rd;
        currentSeed = rd();
        generator.seed(currentSeed);

        ENGINE_WARN("Random: Started seedless. (Auto-Generated: {})", currentSeed);
    }

    Random::Random(uint32_t seed)
    {
        SetSeed(seed);
        ENGINE_INFO("Random: Started with seed: {}", seed);
    }

    void Random::SetSeed(uint32_t seed)
    {
        currentSeed = seed;
        generator.seed(currentSeed);
        ENGINE_INFO("Random: Seed updated to: {}", currentSeed);
    }

    std::string Random::GetState() const
    {
        std::stringstream ss;
        ss << generator; // C++ magic: Serializes the entire internal engine state
        return ss.str();
    }

    void Random::SetState(const std::string& state)
    {
        std::stringstream ss(state);
        ss >> generator; // Rebuilds the engine state from text
        ENGINE_INFO("Random: State has been injected successfully.");
    }

    int Random::GetInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator);
    }

    float Random::GetFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator);
    }

    bool Random::GetChance(float probability)
    {
        return GetFloat(0.0f, 1.0f) <= probability;
    }

    Vector2f Random::GetDirection2D()
    {
        float angle = GetFloat(0.0f, 2.0f * PI);
        return { std::cos(angle), std::sin(angle) };
    }

    float Random::GetGaussian(float mean, float stdDev)
    {
        std::normal_distribution<float> dist(mean, stdDev);
        return dist(generator);
    }

    // Anonymous namespace to hide these constants from the rest of the engine
    namespace
    {
        // Magic constants for bit mixing (Avalanche effect)
        constexpr uint32_t BIT_NOISE1 = 0xB5297A4D;
        constexpr uint32_t BIT_NOISE2 = 0x68E31DA4;
        constexpr uint32_t BIT_NOISE3 = 0x1B56C4E9;
    }

    float Random::GetHash1D(int x, uint32_t seed)
    {
        uint32_t mangled = static_cast<uint32_t>(x);
        mangled *= BIT_NOISE1;
        mangled += seed;
        mangled ^= (mangled >> 8);
        mangled += BIT_NOISE2;
        mangled ^= (mangled << 8);
        mangled *= BIT_NOISE3;
        mangled ^= (mangled >> 8);

        // Convert the giant integer to a float between 0.0 and 1.0
        return static_cast<float>(mangled) / static_cast<float>(0xFFFFFFFF);
    }

    float Random::GetHash2D(int x, int y, uint32_t seed)
    {
        // Transform X and Y into a single 1D index using a large prime number
        constexpr int PRIME_NUMBER = 198491317;
        int index = x + (y * PRIME_NUMBER);

        return GetHash1D(index, seed);
    }
}