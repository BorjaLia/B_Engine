#pragma once

#include <cmath>
#include <type_traits>
#include <string>

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
        /// Allows explicit or implicit casting (e.g., Vector3i to Vector3f).
        template <typename U>
        constexpr Vector3(const Vector3<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z))
        {
        }

        constexpr Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
        constexpr Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }

        constexpr Vector3 operator+(const Vector3& other) const { return { x + other.x, y + other.y, z + other.z }; }
        constexpr Vector3 operator-(const Vector3& other) const { return { x - other.x, y - other.y, z - other.z }; }

        /// Component-wise multiplication.
        constexpr Vector3 operator*(const Vector3& other) const { return { x * other.x, y * other.y, z * other.z }; }

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

        /// Calculates the length of the vector.
        float Magnitude() const
        {
            return std::sqrt(static_cast<float>(x * x + y * y + z * z));
        }

        /// Calculates the squared length of the vector.
        /// Faster than Magnitude() and useful for distance comparisons.
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
        /// Returns a vector perpendicular to both input vectors.
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