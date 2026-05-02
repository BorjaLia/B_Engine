#pragma once

#include <cmath>
#include <type_traits>
#include <iostream>

namespace Engine
{
#pragma region Vector2
    /// A templated 2D vector representing coordinates or directions.
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
    };

    // Standard engine aliases
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;

    inline std::ostream& operator<<(std::ostream& os, const Vector2f& v)
    {
        return os << "(" << v.x << ", " << v.y << ")";
    }

    inline std::ostream& operator<<(std::ostream& os, const Vector2i& v)
    {
        return os << "(" << v.x << ", " << v.y << ")";
    }

    template <typename T>
    constexpr Vector2<T> Cross(T scalar, const Vector2<T>& vec)
    {
        return Vector2<T>(-scalar * vec.y, scalar * vec.x);
    }
#pragma endregion

#pragma region Matrix3x3
    /// A 3x3 Matrix for 2D transformations (Position, Rotation, Scale).
    struct Matrix3x3
    {
        /// Stored as a 2D array [row][column]
        float m[3][3];

        /// Default constructor creates an Identity Matrix.
        Matrix3x3()
        {
            m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f;
            m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f;
            m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f;
        }

        /// Multiplies Matrix by Matrix to combine transformations.
        Matrix3x3 operator*(const Matrix3x3& other) const
        {
            Matrix3x3 result;
            // Direct calculation of the 3x3 matrix for performance
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    result.m[i][j] = m[i][0] * other.m[0][j] +
                        m[i][1] * other.m[1][j] +
                        m[i][2] * other.m[2][j];
                }
            }
            return result;
        }

        /// Multiplies Matrix by Vector2f to apply the transformation to a point.
        Vector2f operator*(const Vector2f& vec) const
        {
            return Vector2f(
                m[0][0] * vec.x + m[0][1] * vec.y + m[0][2],
                m[1][0] * vec.x + m[1][1] * vec.y + m[1][2]
            );
        }

        /// Creates a translation matrix.
        static Matrix3x3 Translation(const Vector2f& pos)
        {
            Matrix3x3 result;
            result.m[0][2] = pos.x;
            result.m[1][2] = pos.y;
            return result;
        }

        /// Creates a rotation matrix.
        /// @param angleRadians The rotation angle in radians.
        static Matrix3x3 Rotation(float angleRadians)
        {
            Matrix3x3 result;
            float c = std::cos(angleRadians);
            float s = std::sin(angleRadians);
            result.m[0][0] = c;   result.m[0][1] = -s;
            result.m[1][0] = s;   result.m[1][1] = c;
            return result;
        }

        /// Creates a scaling matrix.
        static Matrix3x3 Scale(const Vector2f& scale)
        {
            Matrix3x3 result;
            result.m[0][0] = scale.x;
            result.m[1][1] = scale.y;
            return result;
        }
    };
#pragma endregion
}