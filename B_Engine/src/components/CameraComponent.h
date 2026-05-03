#pragma once

#include <optional>
#include <string>

#include "Component.h"
#include "../core/interfaces/IWindow.h"
#include "../utils/Types.h"

namespace Engine
{
    /// Defines a viewpoint through which the game world is rendered.
    /// @ingroup Components
    class CameraComponent : public Component
    {
    public:
        CameraComponent(IWindow* window = nullptr, float startZoom = 1.0f);
        ~CameraComponent();

        void Start() override;
        std::string ToString() const override;

        float GetZoom() const;
        void SetZoom(float newZoom);

        void SetShowDebug(bool show) { showDebug = show; }
        bool GetShowDebug() const { return showDebug; }

        void SetRenderTarget(const RenderTexture2D& target);
        void ClearRenderTarget();
        bool HasRenderTarget() const;
        RenderTexture2D GetRenderTarget() const;

        /// Converts a screen space coordinate (pixels) into world space.
        Vector2f ScreenToWorld(const Vector2f& screenPos) const;

        /// Converts a world space coordinate into screen space (pixels).
        Vector2f WorldToScreen(const Vector2f& worldPos) const;

    private:
        bool showDebug = true;

        IWindow* window;
        float zoom;
        std::optional<RenderTexture2D> renderTarget;
    };
}