#pragma once

#include "../../core/interfaces/IRenderer.h"
#include "../../utils/Types.h"

namespace Engine
{
    /// Raylib implementation of the IRenderer interface.
    /// Wraps Raylib's drawing API and manages hardware texture loading.
    class RaylibRenderer : public IRenderer
    {
    public:
        RaylibRenderer() = default;
        ~RaylibRenderer() override = default;

        bool Initialize() override;
        void Shutdown() override;

        float GetDeltaTime() const override;

        void BeginFrame() override;
        void ClearScreen(const Color& color) override;
        void EndFrame() override;

        void BeginCamera(const Vector2f& targetPosition, float zoom) override;
        void EndCamera() override;

        virtual void BeginRenderToTexture(RenderTexture2D target) override;
        virtual void EndRenderToTexture() override;
        virtual void DrawRenderTexture(RenderTexture2D target, const Vector2f& position, const Color& tint) override;

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
    };
}