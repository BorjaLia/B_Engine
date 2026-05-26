#include "Transform.h"

#include <sstream>

#include "../math/MathUtils.h"
#include "../math/MathDebugger.h"
#include "../scenes/Node.h"

namespace Engine
{
    Transform::Transform()
        : localPosition(0.0f, 0.0f, 0.0f), 
          localRotation(0.0f, 0.0f, 0.0f), 
          localScale(1.0f, 1.0f, 1.0f), 
          isDirty(true),
          owner(nullptr)
    {
    }

    void Transform::SetChildrenDirty()
    {
        if (owner == nullptr) return;

        for (auto& child : owner->GetChildren())
        {
            Transform* childTransform = &child->transform;

            if (!childTransform->isDirty)
            {
                childTransform->isDirty = true;
                childTransform->SetChildrenDirty();
            }
        }
    }

    std::string Transform::ToString() const
    {
        std::stringstream ss;
        ss << "Transform [Pos: " << localPosition;
        ss << " | Rot(Euler): " << localRotation;
        ss << " | Scale: " << localScale;
        ss << "]";
        return ss.str();
    }

#pragma region API Implementation
    void Transform::SetPosition(const Vector3f& pos)
    {
        if (localPosition.x == pos.x && localPosition.y == pos.y && localPosition.z == pos.z) return;

        localPosition = pos;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void Transform::SetPosition(const Vector2f& pos)
    {
        SetPosition(Vector3f(pos.x, pos.y, localPosition.z));
    }

    void Transform::SetEulerAngles(const Vector3f& rotRadians)
    {
        if (localRotation.x == rotRadians.x && localRotation.y == rotRadians.y && localRotation.z == rotRadians.z) return;

        localRotation = rotRadians;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void Transform::SetRotation2D(float rotDegrees)
    {
        SetEulerAngles(Vector3f(localRotation.x, localRotation.y, rotDegrees * DEG2RAD));
    }

    float Transform::GetRotation2D() const
    {
        return localRotation.z * RAD2DEG;
    }

    void Transform::SetScale(const Vector3f& scale)
    {
        if (localScale.x == scale.x && localScale.y == scale.y && localScale.z == scale.z) return;

        localScale = scale;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void Transform::SetScale(const Vector2f& scale)
    {
        SetScale(Vector3f(scale.x, scale.y, localScale.z));
    }

    Vector3f Transform::GetGlobalPosition() const
    {
        return Vector3f(globalMatrix.m[0][3], globalMatrix.m[1][3], globalMatrix.m[2][3]);
    }
#pragma endregion

    void Transform::UpdateTransform()
    {
        if (!isDirty) return;

        Matrix4x4 t = Matrix4x4::Translation(localPosition);
        Matrix4x4 r = Matrix4x4::Rotation(localRotation);
        Matrix4x4 s = Matrix4x4::Scale(localScale);

        localMatrix = t * r * s;
        isDirty = false;

        if (owner != nullptr && owner->GetParent() != nullptr)
        {
            Transform* parentTransform = &owner->GetParent()->transform;

            if (parentTransform->isDirty)
            {
                parentTransform->UpdateTransform();
            }

            globalMatrix = parentTransform->GetGlobalMatrix() * localMatrix;
        }
        else
        {
            globalMatrix = localMatrix;
        }
    }

    const Matrix4x4& Transform::GetGlobalMatrix() const
    {
        return globalMatrix;
    }
}