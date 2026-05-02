#pragma once

#include <string>

namespace Engine
{
    /// Global memory tracking utility.
    /// Overrides global new/delete operators to detect memory leaks.
    class MemoryTracker
    {
    public:
        MemoryTracker() = delete;

        /// Call this function anywhere in your code to dump the current heap memory state to the console.
        static void Print();
    };
}