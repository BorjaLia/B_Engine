#include "TransformComponent.h"

#include <sstream>

#include "../math/MathUtils.h"
#include "../math/MathDebugger.h"
#include "../scenes/Node.h"
#include "../graphics/RendererBase.h"

namespace Engine
{
    TransformComponent::TransformComponent()
        : localPosition(0.0f, 0.0f, 0.0f),
        localRotation(0.0f, 0.0f, 0.0f),
        localScale(1.0f, 1.0f, 1.0f),
        isDirty(true)
    {
    }

    void TransformComponent::SetChildrenDirty()
    {
        if (owner == nullptr) return;

        for (auto& child : owner->GetChildren())
        {
            TransformComponent* childTransform = &child->transform;

            if (!childTransform->isDirty)
            {
                childTransform->isDirty = true;
                childTransform->SetChildrenDirty();
            }
        }
    }

    void TransformComponent::Start()
    {
        UpdateTransform();
    }

    void TransformComponent::DebugDraw(RendererBase* renderer)
    {
        if (renderer == nullptr) return;

        // Hook for future 3D/2D gizmos
    }

    std::string TransformComponent::ToString() const
    {
        std::stringstream ss;
        ss << "TransformComponent [Pos: " << localPosition;
        ss << " | Rot(Euler): " << localRotation;
        ss << " | Scale: " << localScale;
        ss << "]";
        return ss.str();
    }

#pragma region API Implementation
    void TransformComponent::SetPosition(const Vector3f& pos)
    {
        if (localPosition.x == pos.x && localPosition.y == pos.y && localPosition.z == pos.z) return;

        localPosition = pos;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::SetPosition(const Vector2f& pos)
    {
        // Update X and Y, leave Z intact for hybrid functionality
        SetPosition(Vector3f(pos.x, pos.y, localPosition.z));
    }

    void TransformComponent::SetEulerAngles(const Vector3f& rotRadians)
    {
        if (localRotation.x == rotRadians.x && localRotation.y == rotRadians.y && localRotation.z == rotRadians.z) return;

        localRotation = rotRadians;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::SetRotation2D(float rotDegrees)
    {
        // In 2D, rotation is exclusively around the Z axis.
        SetEulerAngles(Vector3f(localRotation.x, localRotation.y, rotDegrees * DEG2RAD));
    }

    float TransformComponent::GetRotation2D() const
    {
        return localRotation.z * RAD2DEG;
    }

    void TransformComponent::SetScale(const Vector3f& scale)
    {
        if (localScale.x == scale.x && localScale.y == scale.y && localScale.z == scale.z) return;

        localScale = scale;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::SetScale(const Vector2f& scale)
    {
        // Update X and Y, leave Z scale intact
        SetScale(Vector3f(scale.x, scale.y, localScale.z));
    }

    Vector3f TransformComponent::GetGlobalPosition() const
    {
        // Global position is always in the 4th column of a 4x4 matrix (indices [0][3], [1][3], [2][3])
        return Vector3f(globalMatrix.m[0][3], globalMatrix.m[1][3], globalMatrix.m[2][3]);
    }
#pragma endregion

    void TransformComponent::UpdateTransform()
    {
        // 1. If clean, early exit. Zero cost!
        if (!isDirty) return;

        // 2. Calculate our local 3D matrix
        Matrix4x4 t = Matrix4x4::Translation(localPosition);
        Matrix4x4 r = Matrix4x4::Rotation(localRotation);
        Matrix4x4 s = Matrix4x4::Scale(localScale);

        // TRS order for proper affine transformations
        localMatrix = t * r * s;
        isDirty = false;

        // 3. Calculate our global matrix based on the parent
        if (owner != nullptr && owner->GetParent() != nullptr)
        {
            TransformComponent* parentTransform = &owner->GetParent()->transform;

            // Force the parent to recalculate if it's dirty before we use its matrix
            if (parentTransform->isDirty)
            {
                parentTransform->UpdateTransform();
            }

            // Inherit parent space
            globalMatrix = parentTransform->GetGlobalMatrix() * localMatrix;
        }
        else
        {
            // We are the root node
            globalMatrix = localMatrix;
        }
    }

    const Matrix4x4& TransformComponent::GetGlobalMatrix() const
    {
        return globalMatrix;
    }
}