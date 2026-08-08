#include "CameraComponent.h"

#include <sstream>

#include "../core/Application.h"
#include "../scenes/Node.h"
#include "TransformComponent.h"

namespace Engine
{
    CameraComponent::CameraComponent(float startZoom)
        : zoom(startZoom)
    {
    }

    CameraComponent::~CameraComponent()
    {
        Application::Get().UnregisterCamera(this);
    }

    void CameraComponent::Start()
    {
        Application::Get().RegisterCamera(this);
    }

    std::string CameraComponent::ToString() const
    {
        std::stringstream ss;
        ss << "CameraComponent [Proj: " << (projectionType == CameraProjection::Perspective ? "3D" : "2D");
        ss << " | Zoom: " << zoom;

        if (renderTarget.has_value())
        {
            ss << " | RenderTarget: YES (" << renderTarget.value().texture.size.x << "x" << renderTarget.value().texture.size.y << ")";
        }
        else
        {
            ss << " | RenderTarget: NO (Main Monitor)";
        }

        ss << "]";
        return ss.str();
    }

    float CameraComponent::GetZoom() const
    {
        return zoom;
    }

    void CameraComponent::SetZoom(float newZoom)
    {
        zoom = newZoom;
    }

    Vector3f CameraComponent::GetTarget() const
    {
        if (!owner)
        {
            return { 0.0f, 0.0f, -1.0f }; // Safe fallback if no owner exists
        }

        // Dynamically calculate the look-at point based on current Transform
        Vector3f currentPos = owner->transform.GetGlobalPosition();
        Vector3f forward = owner->transform.GetForward();

        return currentPos + forward;
    }

    Vector3f CameraComponent::GetUpVector() const
    {
        // Ideally, if your Transform provides GetUp(), use: return owner->transform.GetUp();
        // But allowing a manual override for things like camera shake/rolls is also useful.
        if (owner->GetParent() != nullptr) return owner->GetParent()->transform.GetUp();
        return upVector;
    }

    void CameraComponent::SetRenderTarget(const RenderTexture2D& newTarget)
    {
        renderTarget = newTarget;
    }

    void CameraComponent::ClearRenderTarget()
    {
        renderTarget = std::nullopt;
    }

    bool CameraComponent::HasRenderTarget() const
    {
        return renderTarget.has_value();
    }

    RenderTexture2D CameraComponent::GetRenderTarget() const
    {
        return renderTarget.value();
    }

    Vector2f CameraComponent::ScreenToWorld(const Vector2f& screenPos) const
    {
        if (!owner) return screenPos; // Fallback

        Vector2f cameraPos = owner->transform.GetPosition();
        Vector2i screenSize = Application::Get().GetRenderer()->GetLogicalResolution();
        Vector2f screenCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };

        Vector2f worldPos;
        worldPos.x = cameraPos.x + ((screenPos.x - screenCenter.x) / zoom);
        worldPos.y = cameraPos.y + ((screenPos.y + screenCenter.y) / zoom);

        return worldPos;
    }

    Vector2f CameraComponent::WorldToScreen(const Vector2f& worldPos) const
    {
        if (!owner) return worldPos;

        Vector2f cameraPos = owner->transform.GetPosition();
        Vector2i screenSize = Application::Get().GetRenderer()->GetLogicalResolution();
        Vector2f screenCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };

        Vector2f screenPos;
        screenPos.x = screenCenter.x + ((worldPos.x - cameraPos.x) * zoom);
        screenPos.y = screenCenter.y + ((worldPos.y - cameraPos.y) * zoom);

        return screenPos;
    }
}