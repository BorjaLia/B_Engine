#pragma once

#include <windows.h>
#include <unordered_map>
#include <vector>

#include "../../graphics/RendererBase.h"

namespace Engine
{
    /// Windows Console implementation of the RendererBase class.
    /// Rasterizes sprites into colored ASCII blocks using the Win32 Console API.
    class ConsoleRenderer : public RendererBase
    {
    public:
        ConsoleRenderer() = default;
        ~ConsoleRenderer() override;

        bool OnInit() override;
        void OnShutdown() override;

        float GetDeltaTime() const override { return 0.0016f; }

        void BeginFrame() override;
        void ClearScreen(const Color& color) override;
        void EndFrame() override;

        void BeginCamera(const Vector2f& targetPosition, float zoom) override;
        void EndCamera() override;

        virtual void SetRenderTarget(std::optional<RenderTexture2D>) override {};
        void DrawRenderTexture(RenderTexture2D , const Vector2f&, const Vector2f&, const Color& ) override {}

        void Flush(RenderLayer layer) override;
        void FlushDebug(RenderLayer layer) override;

        void SetVirtualSize(const Vector2i& size) { virtualSize = size; }

    protected:
        Texture2D LoadTexture(const char* filepath) override;
        void UnloadTexture(Texture2D texture) override;

        Font* LoadFont(const char* /*filepath*/, int /*baseSize*/) override { return nullptr; }
        void UnloadFont(Font* /*font*/) override {}

        RenderTexture2D CreateRenderTexture(const Vector2i& ) override { return {}; }
        void UnloadRenderTexture(RenderTexture2D ) override {}

        Texture2D CreateWhitePixel() override;

    private:
        HANDLE hConsole;
        SMALL_RECT consoleWriteArea;
        std::vector<CHAR_INFO> frameBuffer;
        Vector2i bufferSize;
        WORD clearColorAttribute = 0;

        Vector2i virtualSize = { 1600, 900 };

        // Texture storage: ID -> Pixels (RGBA)
        std::unordered_map<unsigned int, unsigned char*> textureData;
        unsigned int nextTextureId = 1;

        Vector2f cameraTarget = { 0.0f, 0.0f };
        float cameraZoom = 1.0f;

        WORD ConvertToConsoleColor(const Color& color);
        void DrawBlock(int x, int y, WORD colorAttr);
    };
}