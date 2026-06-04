#include "MemoryTracker.h"

#include <iostream>
#include <new>     
#include <cstdlib>

// Anonymous namespace prevents these variables from being visible to other files,
// while allowing the global new/delete operators below to access them cleanly.
#ifdef _DEBUG
namespace
{
    size_t printAmount = 0;

    size_t allocCount = 0;
    size_t deleteCount = 0;
    size_t sizeAllocCount = 0;
    size_t sizeDeleteCount = 0;

    size_t baseAllocCount = 0;
    size_t baseSizeAllocCount = 0;
}
#endif // _DEBUG

namespace Engine
{
    void MemoryTracker::RecordBaseline()
    {
#ifdef _DEBUG
        baseAllocCount = allocCount;
        baseSizeAllocCount = sizeAllocCount;
#endif
    }

    void MemoryTracker::Print()
    {
#ifdef _DEBUG
        size_t gameAllocCount = allocCount - baseAllocCount;
        size_t gameSizeAllocCount = sizeAllocCount - baseSizeAllocCount;

        size_t totalCount = gameAllocCount - deleteCount;
        size_t sizeDifference = gameSizeAllocCount - sizeDeleteCount;

        std::cout << "\n=== [ Memory track calls: " << printAmount << " ] ===\n";
        std::cout << "Difference (Leaks): " << totalCount << '\n';
        std::cout << "MemAllocCounter: " << allocCount << '\n';
        std::cout << "SizeAllocCounter: " << sizeAllocCount << " bytes\n";
        std::cout << "MemDeleteCounter: " << deleteCount << '\n';
        std::cout << "SizeDeleteCounter: " << sizeDeleteCount << " bytes\n";
        std::cout << "SizeDifferenceCounter: " << sizeDifference << " bytes\n";
        std::cout << "===========================\n\n";

        printAmount++;
#endif // _DEBUG
    }
}

// =============================================================================
// GLOBAL OVERLOADS
// =============================================================================

#ifdef _DEBUG

#pragma warning(push)
#pragma warning(disable: 28251) // Disables MSVC warning regarding inconsistent annotations for 'new'

[[nodiscard]] void* operator new(size_t size)
{
    sizeAllocCount += size;
    allocCount++;

    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc{};
    return ptr;
}

void operator delete(void* memory, size_t size) noexcept
{
    sizeDeleteCount += size;
    deleteCount++;
    free(memory);
}

[[nodiscard]] void* operator new[](size_t size)
{
    sizeAllocCount += size;
    allocCount++;

    void* ptr = malloc(size);
    if (!ptr) throw std::bad_alloc{};
    return ptr;
}

void operator delete[](void* memory, size_t size) noexcept
{
    sizeDeleteCount += size;
    deleteCount++;
    free(memory);
}

void operator delete(void* memory) noexcept
{
    deleteCount++;
    free(memory);
}

void operator delete[](void* memory) noexcept
{
    deleteCount++;
    free(memory);
}

#pragma warning(pop)

#endif // _DEBUG