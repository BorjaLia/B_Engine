#pragma once

#include <cmath>
#include <type_traits>
#include <string>

namespace Engine
{
    /// @defgroup Utils Utilities & Math
    /// @brief Core mathematical structures, hashing, and system utilities.
    ///
    /// A templated 2D vector representing coordinates or directions.
    /// @ingroup Utils
    template <typename T>
    struct Vector2
    {
        T x;
        T y;

        constexpr Vector2(T x = 0, T y = 0) : x(x), y(y) {}

        /// Conversion constructor.
        /// Allows explicit or implicit casting (e.g., Vector2i to Vector2f).
        template <typename U>
        constexpr Vector2(const Vector2<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
        {
        }

        constexpr Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
        constexpr Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }

        constexpr Vector2 operator+(const Vector2& other) const { return { x + other.x, y + other.y }; }
        constexpr Vector2 operator-(const Vector2& other) const { return { x - other.x, y - other.y }; }

        /// Component-wise multiplication.
        constexpr Vector2 operator*(const Vector2& other) const { return { x * other.x, y * other.y }; }

        /// Scalar multiplication with deduced type.
        template <typename U>
        constexpr auto operator*(U scalar) const
        {
            using ResultType = decltype(std::declval<T>()* std::declval<U>());
            return Vector2<ResultType>(x * scalar, y * scalar);
        }

        /// Scalar division with deduced type.
        template <typename U>
        constexpr auto operator/(U scalar) const
        {
            using ResultType = decltype(std::declval<T>() / std::declval<U>());
            return Vector2<ResultType>(x / scalar, y / scalar);
        }

        constexpr Vector2 operator-() const { return { -x, -y }; }

        /// Calculates the length of the vector.
        float Magnitude() const
        {
            return std::sqrt(static_cast<float>(x * x + y * y));
        }

        /// Calculates the squared length of the vector.
        /// Faster than Magnitude() and useful for distance comparisons.
        constexpr T MagnitudeSquared() const
        {
            return x * x + y * y;
        }

        /// Returns a normalized copy of the vector.
        [[nodiscard]] Vector2<float> Normalized() const
        {
            float len = Magnitude();
            if (len > 0)
            {
                return Vector2<float>(static_cast<float>(x) / len, static_cast<float>(y) / len);
            }
            return Vector2<float>(0, 0);
        }

        /// Normalizes this vector in place.
        void Normalize()
        {
            float len = Magnitude();
            if (len > 0)
            {
                x = static_cast<T>(static_cast<float>(x) / len);
                y = static_cast<T>(static_cast<float>(y) / len);
            }
        }

        /// Calculates the dot product of two vectors.
        constexpr T Dot(const Vector2& other) const
        {
            return (x * other.x) + (y * other.y);
        }

        /// 2D Cross Product (Vector vs Vector).
        /// Returns a scalar representing the magnitude of the Z-axis.
        constexpr T Cross(const Vector2& other) const
        {
            return (x * other.y) - (y * other.x);
        }

        /// 2D Cross Product (Vector vs Scalar).
        /// Returns a perpendicular vector.
        constexpr Vector2 Cross(T scalar) const
        {
            return Vector2(-scalar * y, scalar * x);
        }

        /// Returns a formatted string representation of the vector.
        std::string ToString() const
        {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
        }
    };

    // Standard engine aliases
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;

    template <typename T>
    constexpr Vector2<T> Cross(T scalar, const Vector2<T>& vec)
    {
        return Vector2<T>(-scalar * vec.y, scalar * vec.x);
    }

    template <typename T>
    constexpr Vector2<T> Abs(const Vector2<T>& vec)
    {
        return Vector2<T>(std::abs(vec.x), std::abs(vec.y));
    }
}