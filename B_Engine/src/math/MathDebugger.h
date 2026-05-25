#pragma once

#include <iostream>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Engine
{
    /// @defgroup Debugging
    /// @brief Optional debugging utilities and formatters.
    ///
    /// Provides std::ostream overloads for engine math types. 
    /// Include this file ONLY where you need to log these types to the console to prevent compilation bloat.
    /// @ingroup Debugging

    /// Streams a Vector2 to the output stream.
    template <typename T>
    inline std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
    {
        return os << v.ToString();
    }

    /// Streams a Vector3 to the output stream.
    template <typename T>
    inline std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
    {
        return os << v.ToString();
    }

    /// Streams a Vector4 to the output stream.
    template <typename T>
    inline std::ostream& operator<<(std::ostream& os, const Vector4<T>& v)
    {
        return os << v.ToString();
    }
}