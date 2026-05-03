#include "TransformComponent.h"

#include <sstream>

#include "../scenes/Node.h"
#include "../core/interfaces/IRenderer.h"

namespace Engine
{
    TransformComponent::TransformComponent()
        : localPosition(0.0f, 0.0f), localRotation(0.0f), localScale(1.0f, 1.0f), isDirty(true)
    {
    }

    void TransformComponent::SetChildrenDirty()
    {
        if (owner == nullptr) return;

        for (auto& child : owner->GetChildren())
        {
            TransformComponent* childTransform = child->transform;

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

    void TransformComponent::DebugDraw(IRenderer* renderer)
    {
        if (renderer == nullptr) return;

        CircleShape debugCircle{ 3.0f };
        Color debugColor{ 255, 0, 0, 255 };

        renderer->SubmitDebugShape(RenderLayer::World, debugCircle, GetGlobalPosition(), 0.0f, debugColor);
    }

    std::string TransformComponent::ToString() const
    {
        std::stringstream ss;
        ss << "TransformComponent [Pos: " << GetPosition();
        ss << " | Rotation: " << GetRotation();
        ss << " | Scale: " << GetScale();
        ss << "]";
        return ss.str();
    }

    void TransformComponent::SetPosition(const Vector2f& pos)
    {
        // Optimization: Do nothing if the position hasn't actually changed
        if (localPosition.x == pos.x && localPosition.y == pos.y) return;

        localPosition = pos;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::SetRotation(float rot)
    {
        if (localRotation == rot) return;

        localRotation = rot;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::SetScale(const Vector2f& scale)
    {
        if (localScale.x == scale.x && localScale.y == scale.y) return;

        localScale = scale;
        if (!isDirty)
        {
            isDirty = true;
            SetChildrenDirty();
        }
    }

    void TransformComponent::UpdateTransform()
    {
        // 1. If clean, early exit. Zero cost!
        if (!isDirty) return;

        // 2. Calculate our local matrix
        Matrix3x3 t = Matrix3x3::Translation(localPosition);
        Matrix3x3 r = Matrix3x3::Rotation(localRotation * DEG2RAD);
        Matrix3x3 s = Matrix3x3::Scale(localScale);

        localMatrix = t * r * s;
        isDirty = false;

        // 3. Calculate our global matrix based on the parent
        if (owner != nullptr && owner->GetParent() != nullptr)
        {
            TransformComponent* parentTransform = owner->GetParent()->transform;

            // Force the parent to recalculate if it's dirty before we use its matrix
            if (parentTransform->isDirty)
            {
                parentTransform->UpdateTransform();
            }

            // Always inherit parent space!
            globalMatrix = parentTransform->GetGlobalMatrix() * localMatrix;
        }
        else
        {
            // We are the root node
            globalMatrix = localMatrix;
        }
    }

    const Matrix3x3& TransformComponent::GetGlobalMatrix()
    {
        return globalMatrix;
    }

    Vector2f TransformComponent::GetGlobalPosition()
    {
        // Global position is always in the 3rd column of the final matrix (indices [0][2] and [1][2])
        return Vector2f(globalMatrix.m[0][2], globalMatrix.m[1][2]);
    }
}