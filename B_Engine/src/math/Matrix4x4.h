#pragma once

#include "Vector3.h"

namespace Engine
{
    /// A 4x4 Matrix for 3D transformations (Position, Rotation, Scale).
    /// @ingroup Utils
    struct Matrix4x4
    {
        /// Stored as a 2D array [row][column]
        float m[4][4];

        /// Default constructor creates an Identity Matrix.
        Matrix4x4()
        {
            m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
            m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
            m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
            m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        }

        /// Multiplies Matrix by Matrix to combine transformations.
        Matrix4x4 operator*(const Matrix4x4& other) const
        {
            Matrix4x4 result;
            // Direct calculation for performance
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    result.m[i][j] = m[i][0] * other.m[0][j] +
                        m[i][1] * other.m[1][j] +
                        m[i][2] * other.m[2][j] +
                        m[i][3] * other.m[3][j];
                }
            }
            return result;
        }

        /// Multiplies Matrix by Vector3f to apply the transformation to a point.
        /// Assumes the w-component of the vector is 1.0f (point translation).
        Vector3f operator*(const Vector3f& vec) const
        {
            return Vector3f(
                m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3],
                m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3],
                m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3]
            );
        }

        /// Creates a translation matrix.
        static Matrix4x4 Translation(const Vector3f& pos)
        {
            Matrix4x4 result;
            result.m[0][3] = pos.x;
            result.m[1][3] = pos.y;
            result.m[2][3] = pos.z;
            return result;
        }

        /// Creates a scaling matrix.
        static Matrix4x4 Scale(const Vector3f& scale)
        {
            Matrix4x4 result;
            result.m[0][0] = scale.x;
            result.m[1][1] = scale.y;
            result.m[2][2] = scale.z;
            return result;
        }

        /// Directly calculates the combined Translation * Rotation * Scale matrix.
        /// Bypasses the need for 4 heavy 4x4 matrix multiplications.
        static Matrix4x4 TRS(const Vector3f& translation, const Vector3f& rotation, const Vector3f& scale)
        {
            Matrix4x4 result;

            float cx = std::cos(rotation.x);
            float sx = std::sin(rotation.x);
            float cy = std::cos(rotation.y);
            float sy = std::sin(rotation.y);
            float cz = std::cos(rotation.z);
            float sz = std::sin(rotation.z);

            // Mathematical expansion of Scale * (Rz * Ry * Rx)
            result.m[0][0] = scale.x * (cz * cy);
            result.m[1][0] = scale.x * (sz * cy);
            result.m[2][0] = scale.x * (-sy);

            result.m[0][1] = scale.y * (cz * sy * sx - sz * cx);
            result.m[1][1] = scale.y * (sz * sy * sx + cz * cx);
            result.m[2][1] = scale.y * (cy * sx);

            result.m[0][2] = scale.z * (cz * sy * cx + sz * sx);
            result.m[1][2] = scale.z * (sz * sy * cx - cz * sx);
            result.m[2][2] = scale.z * (cy * cx);

            // Apply translation directly
            result.m[0][3] = translation.x;
            result.m[1][3] = translation.y;
            result.m[2][3] = translation.z;

            // Bottom row is kept as 0, 0, 0, 1 from the default constructor
            return result;
        }

        static Matrix4x4 Rotation(const Vector3f& rotationRadians)
        {
            return TRS(Vector3f(0.0f, 0.0f, 0.0f), rotationRadians, Vector3f(1.0f, 1.0f, 1.0f));
        }
    };
}