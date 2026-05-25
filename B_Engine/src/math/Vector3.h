#pragma once

#include <cmath>
#include <type_traits>
#include <string>
#include "Vector2.h"

namespace Engine
{
    /// @defgroup Utils Utilities & Math
    /// @brief Core mathematical structures, hashing, and system utilities.
    ///
    /// A templated 3D vector representing coordinates or directions.
    /// @ingroup Utils
    template <typename T>
    struct Vector3
    {
        T x;
        T y;
        T z;

        constexpr Vector3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

        /// Conversion constructor.
        template <typename U>
        constexpr Vector3(const Vector3<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z))
        {
        }

        /// Implicit conversion to Vector2
        /// Allows passing or assigning a Vector3 directly where a Vector2 is expected.
        constexpr operator Vector2<T>() const
        {
            return Vector2<T>(x, y);
        }

#pragma region Vector3 vs Vector3 Operations
        constexpr Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        constexpr Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

        constexpr Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
        constexpr Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }

        /// Component-wise multiplication.
        constexpr Vector3 operator*(const Vector3& other) const { return { x * other.x, y * other.y, z * other.z }; }
#pragma endregion

#pragma region Vector3 vs Vector2 Operations (Hybrid 2D/3D Support)
        // Mathematically treats the Vector2 as if its Z component was 0.

        constexpr Vector3& operator+=(const Vector2<T>& other) { x += other.x; y += other.y; return *this; }
        constexpr Vector3& operator-=(const Vector2<T>& other) { x -= other.x; y -= other.y; return *this; }

        constexpr Vector3 operator+(const Vector2<T>& other) const { return { x + other.x, y + other.y, z }; }
        constexpr Vector3 operator-(const Vector2<T>& other) const { return { x - other.x, y - other.y, z }; }

        /* * Note on Multiplication/Division:
         * We do NOT overload * or / for Vector2 here.
         * Why? If you scale a Vector3 by a Vector2, should Z become 0 (Z * 0)?
         * Or should it remain intact (Z * 1)? Because it's ambiguous and dangerous,
         * we force the developer to explicitly use a Vector3 for scaling/division to avoid silent bugs.
         */
#pragma endregion

#pragma region Scalar Operations
         /// Scalar multiplication with deduced type.
        template <typename U>
        constexpr auto operator*(U scalar) const
        {
            using ResultType = decltype(std::declval<T>()* std::declval<U>());
            return Vector3<ResultType>(x * scalar, y * scalar, z * scalar);
        }

        /// Scalar division with deduced type.
        template <typename U>
        constexpr auto operator/(U scalar) const
        {
            using ResultType = decltype(std::declval<T>() / std::declval<U>());
            return Vector3<ResultType>(x / scalar, y / scalar, z / scalar);
        }

        constexpr Vector3 operator-() const { return { -x, -y, -z }; }
#pragma endregion

#pragma region Math Functions
        /// Calculates the length of the vector.
        float Magnitude() const
        {
            return std::sqrt(static_cast<float>(x * x + y * y + z * z));
        }

        /// Calculates the squared length of the vector.
        constexpr T MagnitudeSquared() const
        {
            return x * x + y * y + z * z;
        }

        /// Returns a normalized copy of the vector.
        [[nodiscard]] Vector3<float> Normalized() const
        {
            float len = Magnitude();
            if (len > 0)
            {
                return Vector3<float>(static_cast<float>(x) / len, static_cast<float>(y) / len, static_cast<float>(z) / len);
            }
            return Vector3<float>(0, 0, 0);
        }

        /// Normalizes this vector in place.
        void Normalize()
        {
            float len = Magnitude();
            if (len > 0)
            {
                x = static_cast<T>(static_cast<float>(x) / len);
                y = static_cast<T>(static_cast<float>(y) / len);
                z = static_cast<T>(static_cast<float>(z) / len);
            }
        }

        /// Calculates the dot product of two vectors.
        constexpr T Dot(const Vector3& other) const
        {
            return (x * other.x) + (y * other.y) + (z * other.z);
        }

        /// 3D Cross Product.
        constexpr Vector3 Cross(const Vector3& other) const
        {
            return Vector3(
                (y * other.z) - (z * other.y),
                (z * other.x) - (x * other.z),
                (x * other.y) - (y * other.x)
            );
        }

        /// Returns a formatted string representation of the vector.
        std::string ToString() const
        {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
        }
#pragma endregion
    };

    // Standard engine aliases
    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;

    template <typename T>
    constexpr Vector3<T> Abs(const Vector3<T>& vec)
    {
        return Vector3<T>(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
    }
}