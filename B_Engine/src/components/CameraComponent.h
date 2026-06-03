#pragma once

#include <optional>
#include <string>

#include "Component.h"
#include "../core/interfaces/IWindow.h"
#include "../utils/Types.h"

namespace Engine
{
    /// Defines the projection type of the camera.
    enum class CameraProjection
    {
        Orthographic, // Used for pure 2D games
        Perspective   // Used for 3D games
    };

    /// Defines a viewpoint through which the game world is rendered.
    /// @ingroup Components
    class CameraComponent : public Component
    {
    public:
        CameraComponent(float startZoom = 1.0f);
        ~CameraComponent() override;

        void Start() override;
        std::string ToString() const override;

        // --- 2D / General Settings ---
        float GetZoom() const;
        void SetZoom(float newZoom);

        void SetShowDebug(bool show) { showDebug = show; }
        bool GetShowDebug() const { return showDebug; }

        // --- 3D Settings ---
        void SetProjectionType(CameraProjection proj) { projectionType = proj; }
        CameraProjection GetProjectionType() const { return projectionType; }

        void SetFOV(float fieldOfView) { fov = fieldOfView; }
        float GetFOV() const { return fov; }

        void SetNearPlane(float nearP) { nearPlane = nearP; }
        float GetNearPlane() const { return nearPlane; }

        void SetFarPlane(float farP) { farPlane = farP; }
        float GetFarPlane() const { return farPlane; }

        void SetTarget(const Vector3f& newTarget) { target = newTarget; }
        Vector3f GetTarget() const { return target; }

        void SetUpVector(const Vector3f& newUp) { upVector = newUp; }
        Vector3f GetUpVector() const { return upVector; }

        // --- Render Target API ---
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

        CameraProjection projectionType = CameraProjection::Orthographic;

        float zoom;

        // 3D Specific Data
        float fov = 45.0f;           // Field of View in degrees
        float nearPlane = 0.1f;
        float farPlane = 1000.0f;

        Vector3f target = { 0.0f, 0.0f, 0.0f }; // Where the camera is looking
        Vector3f upVector = { 0.0f, 1.0f, 0.0f }; // The "up" direction for the camera

        std::optional<RenderTexture2D> renderTarget;
    };
}