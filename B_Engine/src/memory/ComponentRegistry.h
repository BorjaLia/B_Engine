#pragma once

#include "../memory/BlockAllocator.h"

namespace Engine
{
    /// @defgroup Scenes Scene Management
    /// A global registry that automatically generates and provides 
    /// contiguous memory pools for any component type used in the engine.
    /// @ingroup Scenes
    class ComponentRegistry
    {
    public:
        /// Generates and retrieves a contiguous memory pool for ANY component type instantly.
        template <typename T>
        static BlockAllocator<T>& GetPool()
        {
            // Static local ensures exactly ONE pool is created per component type across the engine.
            static BlockAllocator<T> pool(5000);
            return pool;
        }
    };
}