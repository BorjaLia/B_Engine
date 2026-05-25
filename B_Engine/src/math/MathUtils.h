#pragma once

#include <cmath>
#include <numbers>

namespace Engine
{
    /// @defgroup Utils Utilities & Math
    /// @brief Core mathematical constants and utility functions.
    /// @ingroup Utils

    constexpr float PI = std::numbers::pi_v<float>;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

}