#pragma once

#include "../../graphics/RendererBase.h"
#include "../../utils/Types.h"
#include "../../math/Vector2.h"
#include "../../math/Vector3.h"

namespace Engine
{
    /// Raylib implementation of the RendererBase class.
    /// Wraps Raylib's drawing API and manages hardware texture loading.
    class RaylibRenderer : public RendererBase
    {
    public:
        RaylibRenderer() = default;
        ~RaylibRenderer() override = default;

        bool OnInit() override;
        void OnShutdown() override;

        float GetDeltaTime() const override;

        void BeginFrame() override;
        void ClearScreen(const Color& color) override;
        void EndFrame() override;

        void BeginCamera(const Vector2f& targetPosition, float zoom) override;
        void BeginCamera3D(const Vector3f& position, const Vector3f& target, const Vector3f& up, float fov) override;
        void EndCamera() override;

        virtual void SetRenderTarget(std::optional<RenderTexture2D> target) override;
        virtual void DrawRenderTexture(RenderTexture2D target, const Vector2f& position, const Vector2f& scale, const Color& tint) override;

        virtual void Flush(RenderLayer layer) override;
        virtual void FlushDebug(RenderLayer layer) override;

    protected:
        Texture2D LoadTexture(const char* filepath) override;
        void UnloadTexture(Texture2D texture) override;

        Engine::Font* LoadFont(const char* filepath, int baseSize) override;
        void UnloadFont(Font* font) override;

        virtual RenderTexture2D CreateRenderTexture(const Vector2i& size) override;
        virtual void UnloadRenderTexture(RenderTexture2D target) override;

        virtual Texture2D CreateWhitePixel() override;

        virtual Model LoadModel(const char* filepath) override;
        virtual void UnloadModel(Model model) override;

    private:

        // --- Render Passes (Backend Specific Helpers) ---

        /// Renders all opaque 3D models queued for this frame.
        void RenderOpaqueModels3D();

        /// Renders 2D sprites oriented towards the 3D camera.
        /// @param queue The command queue to process.
        void RenderBillboards3D(const std::vector<SpriteRenderCommand>& queue);

        /// Renders flat 2D sprites (Orthographic).
        /// @param queue The command queue to process.
        /// @param isWorldSpace True if coordinates need Y-inversion for world space.
        void RenderSprites2D(const std::vector<SpriteRenderCommand>& queue, bool isWorldSpace);

        /// Renders primitive 3D debug shapes (Lines, Cubes, Paths).
        void RenderDebugShapes3D();

        /// Renders raw geometry wireframes for 3D models.
        void RenderDebugWireframes3D();

        /// Renders primitive 2D debug shapes.
        /// @param queue The command queue to process.
        /// @param isWorldSpace True if coordinates need Y-inversion for world space.
        void RenderDebugShapes2D(const std::vector<DebugRenderCommand>& queue, bool isWorldSpace);

        Vector3f camPosition;
        Vector3f camTarget;
        Vector3f camUp;
        float camFov = 45.0f;
    };
}