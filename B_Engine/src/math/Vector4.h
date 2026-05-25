#pragma once

#include <cmath>
#include <type_traits>
#include <string>

namespace Engine
{
    /// @defgroup Utils Utilities & Math
    /// @brief Core mathematical structures, hashing, and system utilities.
    ///
    /// A templated 4D vector representing coordinates, homogeneous points, or RGBA colors.
    /// @ingroup Utils
    template <typename T>
    struct Vector4
    {
        T x;
        T y;
        T z;
        T w;

        constexpr Vector4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

        /// Conversion constructor.
        template <typename U>
        constexpr Vector4(const Vector4<U>& other)
            : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)),
            z(static_cast<T>(other.z)), w(static_cast<T>(other.w))
        {
        }

        constexpr Vector4& operator+=(const Vector4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
        constexpr Vector4& operator-=(const Vector4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }

        constexpr Vector4 operator+(const Vector4& other) const { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
        constexpr Vector4 operator-(const Vector4& other) const { return { x - other.x, y - other.y, z - other.z, w - other.w }; }

        /// Component-wise multiplication.
        constexpr Vector4 operator*(const Vector4& other) const { return { x * other.x, y * other.y, z * other.z, w * other.w }; }

        /// Scalar multiplication with deduced type.
        template <typename U>
        constexpr auto operator*(U scalar) const
        {
            using ResultType = decltype(std::declval<T>()* std::declval<U>());
            return Vector4<ResultType>(x * scalar, y * scalar, z * scalar, w * scalar);
        }

        /// Scalar division with deduced type.
        template <typename U>
        constexpr auto operator/(U scalar) const
        {
            using ResultType = decltype(std::declval<T>() / std::declval<U>());
            return Vector4<ResultType>(x / scalar, y / scalar, z / scalar, w / scalar);
        }

        constexpr Vector4 operator-() const { return { -x, -y, -z, -w }; }

        /// Calculates the length of the vector.
        float Magnitude() const
        {
            return std::sqrt(static_cast<float>(x * x + y * y + z * z + w * w));
        }

        /// Calculates the squared length of the vector.
        constexpr T MagnitudeSquared() const
        {
            return x * x + y * y + z * z + w * w;
        }

        /// Returns a normalized copy of the vector.
        [[nodiscard]] Vector4<float> Normalized() const
        {
            float len = Magnitude();
            if (len > 0)
            {
                return Vector4<float>(
                    static_cast<float>(x) / len,
                    static_cast<float>(y) / len,
                    static_cast<float>(z) / len,
                    static_cast<float>(w) / len
                );
            }
            return Vector4<float>(0, 0, 0, 0);
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
                w = static_cast<T>(static_cast<float>(w) / len);
            }
        }

        /// Calculates the dot product of two vectors.
        constexpr T Dot(const Vector4& other) const
        {
            return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
        }

        /// Returns a formatted string representation of the vector.
        std::string ToString() const
        {
            return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " +
                std::to_string(z) + ", " + std::to_string(w) + ")";
        }
    };

    // Standard engine aliases
    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int>;
}