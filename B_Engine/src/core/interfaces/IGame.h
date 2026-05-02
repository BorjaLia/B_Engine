#pragma once

#include <string>
#include "../../utils/Math.h"

namespace Engine
{
    /// The base interface that every user game must inherit from.
    class IGame
    {
    public:
        virtual ~IGame() = default;

        /// The developer registers all persistent inputs here.
        virtual void SetupInputs() = 0;

        /// The developer registers all available scenes and sets the initial one here.
        virtual void RegisterScenes() = 0;
    };
}