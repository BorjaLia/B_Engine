#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../utils/Types.h"
#include "../utils/Math.h"
#include "../resources/Font.h"
#include "debug/Debug.h"

namespace Engine
{
    class ResourceManager;

    /// Base class for the rendering backend.
    /// Manages the global canvas, logical resolution scaling, and the rendering pipeline.
    /// @ingroup Core
    class RendererBase
    {
    public:
        virtual ~RendererBase() = default;

        bool Initialize();
        void Shutdown();

        virtual float GetDeltaTime() const = 0;

        virtual void BeginFrame() = 0;
        virtual void ClearScreen(const Color& color) = 0;
        virtual void EndFrame() = 0;

        virtual void SetRenderTarget(std::optional<RenderTexture2D> target) = 0;
        std::optional<RenderTexture2D> GetActiveRenderTarget() const { return activeRenderTarget; }

        void SetLogicalResolution(const Vector2i& size);
        Vector2i GetLogicalResolution() const { return logicalSize; }

        Vector2f ScreenToLogical(const Vector2f& screenPos) const;

        /// Locks or unlocks the aspect ratio, forcing letterboxing if locked.
        /// @param locked True to force letterboxing, false to stretch/adapt.
        /// @param targetLogicalResolution The native resolution to lock to (e.g., 1920x1080).
        void SetAspectRatioLocked(bool locked, float aspectRatio = 16.0f / 9.0f);

        /// Checks if the aspect ratio is currently locked.
        bool IsAspectRatioLocked() const { return aspectRatioLocked; }

        bool IsUsingGlobalCanvas() const { return useGlobalCanvas; }
        RenderTexture2D GetGlobalCanvas() const { return globalCanvas; }

        Vector2f GetLetterboxOffset() const { return lbOffset; }
        float GetLetterboxScale() const { return lbScale; }

        virtual void BeginCamera(const Vector2f& targetPosition, float zoom = 1.0f) = 0;
        virtual void EndCamera() = 0;

        virtual void DrawRenderTexture(RenderTexture2D target, const Vector2f& position, const Vector2f& scale = { 1.0f, 1.0f }, const Color& tint = { 255, 255, 255, 255 }) = 0;

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

        virtual bool OnInit() = 0;
        virtual void OnShutdown() = 0;

        virtual Texture2D LoadTexture(const char* filepath) = 0;
        virtual void UnloadTexture(Texture2D texture) = 0;

        virtual Font* LoadFont(const char* filepath, int baseSize) = 0;
        virtual void UnloadFont(Font* font) = 0;

        virtual RenderTexture2D CreateRenderTexture(const Vector2i& size) = 0;
        virtual void UnloadRenderTexture(RenderTexture2D target) = 0;

        virtual Texture2D CreateWhitePixel() = 0;

        virtual void ClearQueues()
        {
            worldQueue.clear();
            uiQueue.clear();
            debugWorldQueue.clear();
            debugUIQueue.clear();
        }

        void CalculateLetterbox();
        void OnWindowResize(class WindowResizeEvent& e);
        void OnReplayStateChanged(class ReplayStateEvent& e);

        bool debugRenderEnabled = false;

        std::vector<SpriteRenderCommand> worldQueue;
        std::vector<SpriteRenderCommand> uiQueue;

        std::vector<DebugRenderCommand> debugWorldQueue;
        std::vector<DebugRenderCommand> debugUIQueue;

        std::optional<RenderTexture2D> activeRenderTarget = std::nullopt;

        RenderTexture2D globalCanvas = { 0 };
        bool useGlobalCanvas = false;
        bool isReplaying = false;

        Vector2i logicalSize = { 1920, 1080 };
        Vector2i windowSize = { 1920, 1080 };

        bool aspectRatioLocked = false;
        float targetAspectRatio = 16.0f / 9.0f;

        float lbScale = 1.0f;
        Vector2f lbOffset = { 0.0f, 0.0f };

        uint32_t resizeEventId = 0;
        uint32_t replayEventId = 0;
    };
}