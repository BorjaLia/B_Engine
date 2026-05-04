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
        ss << "CameraComponent [Zoom: " << zoom;

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

    void CameraComponent::SetRenderTarget(const RenderTexture2D& target)
    {
        renderTarget = target;
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
        if (!owner || !owner->transform) return screenPos; // Fallback

        // 1. Get the camera's position in the world
        Vector2f cameraPos = owner->transform->GetPosition();

        // 2. Get the logic screen size
        Vector2i screenSize = Application::Get().GetRenderer()->GetLogicalResolution();
        Vector2f screenCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };

        // 3. Calculate position considering the zoom and center
        Vector2f worldPos;
        worldPos.x = cameraPos.x + ((screenPos.x - screenCenter.x) / zoom);

        // Warning with Y-Axis: If the world Y grows UPwards (math style)
        // but screen Y grows DOWNwards (monitor style), the formula is inverted.
        worldPos.y = cameraPos.y + ((screenPos.y + screenCenter.y) / zoom);

        return worldPos;
    }

    Vector2f CameraComponent::WorldToScreen(const Vector2f& worldPos) const
    {
        if (!owner || !owner->transform) return worldPos;

        Vector2f cameraPos = owner->transform->GetPosition();
        Vector2i screenSize = Application::Get().GetRenderer()->GetLogicalResolution();
        Vector2f screenCenter = { screenSize.x / 2.0f, screenSize.y / 2.0f };

        Vector2f screenPos;
        screenPos.x = screenCenter.x + ((worldPos.x - cameraPos.x) * zoom);
        screenPos.y = screenCenter.y + ((worldPos.y - cameraPos.y) * zoom);

        return screenPos;
    }
}