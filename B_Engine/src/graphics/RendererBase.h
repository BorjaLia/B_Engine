#pragma once

#include <string>
#include <vector>

#include "../utils/Types.h"
#include "../utils/Math.h"
#include "../resources/Font.h"

namespace Engine
{
    class ResourceManager;

    /// Base class for the rendering backend.
    /// @ingroup Core
    class RendererBase
    {
    public:
        virtual ~RendererBase() = default;

        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual float GetDeltaTime() const = 0;

        virtual void BeginFrame() = 0;
        virtual void ClearScreen(const Color& color) = 0;
        virtual void EndFrame() = 0;

        virtual void BeginCamera(const Vector2f& targetPosition, float zoom = 1.0f) = 0;
        virtual void EndCamera() = 0;

        virtual void BeginRenderToTexture(RenderTexture2D target) = 0;
        virtual void EndRenderToTexture() = 0;
        virtual void DrawRenderTexture(RenderTexture2D target, const Vector2f& position, const Color& tint = { 255, 255, 255, 255 }) = 0;

        /// Queues a full sprite for rendering in the specified layer.
        virtual void SubmitSprite(RenderLayer layer, const Texture2D& texture, const Vector2f& position, float rotation, const Vector2f& scale, Pivot pivot, const Color& tint = { 255, 255, 255, 255 }, bool flipX = false, bool flipY = false)
        {
            SpriteRenderCommand cmd = { texture, Rect(), position, rotation, scale, pivot, tint, flipX, flipY, false };
            if (layer == RenderLayer::World) worldQueue.push_back(cmd);
            else if (layer == RenderLayer::UI) uiQueue.push_back(cmd);
        }

        /// Queues a partial sprite (spritesheet) or text for rendering.
        virtual void SubmitSprite(RenderLayer layer, const Texture2D& texture, const Rect& sourceRect, const Vector2f& position, float rotation, const Vector2f& scale, Pivot pivot, const Color& tint = { 255, 255, 255, 255 }, bool flipX = false, bool flipY = false)
        {
            SpriteRenderCommand cmd = { texture, sourceRect, position, rotation, scale, pivot, tint, flipX, flipY, true };
            if (layer == RenderLayer::World) worldQueue.push_back(cmd);
            else if (layer == RenderLayer::UI) uiQueue.push_back(cmd);
        }

        virtual void Flush(RenderLayer layer) = 0;

        /// Queues a debug wireframe shape for rendering.
        virtual void SubmitDebugShape(RenderLayer layer, const Shape& shape, const Vector2f& position, float rotation, const Color& color)
        {
            if (!debugRenderEnabled) return;

            if (layer == RenderLayer::World)
                debugWorldQueue.push_back({ shape, position, rotation, color });
            else if (layer == RenderLayer::UI)
                debugUIQueue.push_back({ shape, position, rotation, color });
        }

        virtual void FlushDebug(RenderLayer layer) = 0;

        virtual void SetDebugRenderEnabled(bool enabled) { debugRenderEnabled = enabled; }
        virtual bool IsDebugRenderEnabled() const { return debugRenderEnabled; }

    protected:
        friend class ResourceManager;

        bool debugRenderEnabled = false;

        std::vector<SpriteRenderCommand> worldQueue;
        std::vector<SpriteRenderCommand> uiQueue;

        std::vector<DebugRenderCommand> debugWorldQueue;
        std::vector<DebugRenderCommand> debugUIQueue;

        bool isRenderingToTexture = false;
        Vector2f currentTextureSize = { 0.0f, 0.0f };

        virtual void ClearQueues()
        {
            worldQueue.clear();
            uiQueue.clear();
            debugWorldQueue.clear();
            debugUIQueue.clear();
        }

        virtual Texture2D LoadTexture(const char* filepath) = 0;
        virtual void UnloadTexture(Texture2D texture) = 0;

        virtual Font* LoadFont(const char* filepath, int baseSize) = 0;
        virtual void UnloadFont(Font* font) = 0;

        virtual RenderTexture2D CreateRenderTexture(const Vector2i& size) = 0;
        virtual void UnloadRenderTexture(RenderTexture2D target) = 0;

        virtual Texture2D CreateWhitePixel() = 0;
    };
}