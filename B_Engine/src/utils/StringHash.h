#pragma once

#include <cstdint>

namespace Engine
{
    namespace Hash
    {
        // Magic constants for the 32-bit FNV-1a algorithm
        constexpr uint32_t FNV_PRIME = 16777619u;
        constexpr uint32_t OFFSET_BASIS = 2166136261u;

        /// Recursive constexpr hash function.
        /// Because it is constexpr, string hashes are calculated at compile time,
        /// resulting in zero CPU overhead during runtime.
        constexpr uint32_t GetHash(const char* str, uint32_t hash = OFFSET_BASIS)
        {
            return *str == '\0' ? hash : GetHash(str + 1, (hash ^ static_cast<uint32_t>(*str)) * FNV_PRIME);
        }
    }
}