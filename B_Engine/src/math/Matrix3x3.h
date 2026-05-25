#pragma once

#include "Vector2.h"

namespace Engine
{
    /// A 3x3 Matrix for 2D transformations (Position, Rotation, Scale).
    /// @ingroup Utils
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
}