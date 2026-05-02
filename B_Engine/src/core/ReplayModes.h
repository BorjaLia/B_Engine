#pragma once

namespace Engine
{
    /// Defines the operational state of the engine's playback and deterministic simulation system.
    /// @ingroup Core
    enum class EngineMode
    {
        Normal,
        AutoRecord,
        AutoPlayback
    };
}