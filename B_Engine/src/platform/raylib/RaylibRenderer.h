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
        Vector3f camPosition;
        Vector3f camTarget;
        Vector3f camUp;
        float camFov = 45.0f;
    };
}