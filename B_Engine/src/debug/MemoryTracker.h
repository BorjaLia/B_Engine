#pragma once

#include <string>

namespace Engine
{
    /// Global memory tracking utility.
    /// Overrides global new/delete operators to detect memory leaks.
    /// @ingroup Debug
    class MemoryTracker
    {
    public:
        MemoryTracker() = delete;

        /// Takes a snapshot of current allocations. Call this at the VERY START of main()
        /// to ignore CRT (C++ Runtime) allocations like std::cout buffers.
        static void RecordBaseline();

        /// Call this function anywhere in your code to dump the current heap memory state to the console.
        static void Print();
    };
}