#include "ConsoleRenderer.h"
#include <iostream>

// IMPORTANT: Define STB_IMAGE_IMPLEMENTATION in exactly ONE .cpp file globally.
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine
{
    ConsoleRenderer::~ConsoleRenderer() { Shutdown(); }

    bool ConsoleRenderer::Initialize()
    {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (hConsole == INVALID_HANDLE_VALUE) return false;

        // 1. Configure logical size (e.g., 160 columns, 50 rows)
        bufferSize = { 160, 50 };

        // 2. FORCE FONT SIZE (This prevents zoom/scroll issues in Windows 10/11 consoles)
        CONSOLE_FONT_INFOEX cfi;
        cfi.cbSize = sizeof(cfi);
        cfi.nFont = 0;
        cfi.dwFontSize.X = 12; // Width in pixels
        cfi.dwFontSize.Y = 12; // Height in pixels
        cfi.FontFamily = FF_DONTCARE;
        cfi.FontWeight = FW_NORMAL;
        wcscpy_s(cfi.FaceName, L"Consolas");
        SetCurrentConsoleFontEx(hConsole, FALSE, &cfi);

        // 3. ADJUST WINDOW (Win32 requires this specific order)
        COORD coord = { (SHORT)bufferSize.x, (SHORT)bufferSize.y };
        SMALL_RECT rect = { 0, 0, (SHORT)(bufferSize.x - 1), (SHORT)(bufferSize.y - 1) };

        SetConsoleScreenBufferSize(hConsole, coord);
        SetConsoleWindowInfo(hConsole, TRUE, &rect);

        // 4. Lock manual resizing so the buffer layout doesn't break
        HWND hwnd = GetConsoleWindow();
        SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

        frameBuffer.resize(bufferSize.x * bufferSize.y);
        consoleWriteArea = rect;

        return true;
    }

    void ConsoleRenderer::Shutdown()
    {
        for (auto& pair : textureData)
        {
            stbi_image_free(pair.second);
        }
        textureData.clear();
    }

    Texture2D ConsoleRenderer::LoadTexture(const char* filepath)
    {
        int w, h, ch;
        unsigned char* pixels = stbi_load(filepath, &w, &h, &ch, 4); // Force RGBA

        if (!pixels)
        {
            return Texture2D{ 0, {0,0} };
        }

        unsigned int id = nextTextureId++;
        textureData[id] = pixels;

        return Texture2D{ id, {w, h} };
    }

    void ConsoleRenderer::UnloadTexture(Texture2D texture)
    {
        if (textureData.count(texture.id))
        {
            stbi_image_free(textureData[texture.id]);
            textureData.erase(texture.id);
        }
    }

    void ConsoleRenderer::Flush(RenderLayer layer)
    {
        auto& queue = (layer == RenderLayer::World) ? worldQueue : uiQueue;

        float ratioX = static_cast<float>(bufferSize.x) / static_cast<float>(virtualSize.x);
        float ratioY = static_cast<float>(bufferSize.y) / static_cast<float>(virtualSize.y);

        for (const auto& cmd : queue)
        {
            unsigned char* pixels = textureData.count(cmd.texture.id) ? textureData[cmd.texture.id] : nullptr;

            // 1. Real size of the image fragment to draw
            float baseWidth = cmd.useSourceRect ? cmd.sourceRect.size.x : static_cast<float>(cmd.texture.size.x);
            float baseHeight = cmd.useSourceRect ? cmd.sourceRect.size.y : static_cast<float>(cmd.texture.size.y);

            float gamePixelWidth = baseWidth * cmd.scale.x;
            float gamePixelHeight = baseHeight * cmd.scale.y;

            Vector2f pivotMult = GetPivotMultiplier(cmd.pivot);
            float originX = gamePixelWidth * pivotMult.x;
            float originY = gamePixelHeight * pivotMult.y;

            float finalScreenX = 0.0f;
            float finalScreenY = 0.0f;
            float currentZoom = 1.0f;

            // 2. COORDINATE MAGIC: World vs UI
            if (layer == RenderLayer::World)
            {
                currentZoom = cameraZoom;

                // Center of the virtual screen
                float screenOffsetX = virtualSize.x / 2.0f;
                float screenOffsetY = virtualSize.y / 2.0f;

                // Transformation: Cartesian (Y-Up) to Screen (Y-Down)
                float screenX = (cmd.position.x - cameraTarget.x) * currentZoom + screenOffsetX;
                float screenY = (cameraTarget.y - cmd.position.y) * currentZoom + screenOffsetY;

                finalScreenX = screenX - (originX * currentZoom);
                finalScreenY = screenY - (originY * currentZoom);
            }
            else // RenderLayer::UI (UI is usually designed with Y-Down naturally)
            {
                currentZoom = 1.0f;
                finalScreenX = cmd.position.x - originX;
                finalScreenY = cmd.position.y - originY;
            }

            // 3. Final conversion to console "blocks"
            int startX = static_cast<int>(finalScreenX * ratioX);
            int startY = static_cast<int>(finalScreenY * ratioY);
            int drawW = static_cast<int>(gamePixelWidth * currentZoom * ratioX);
            int drawH = static_cast<int>(gamePixelHeight * currentZoom * ratioY);

            if (drawW < 1) drawW = 1;
            if (drawH < 1) drawH = 1;

            // 4. Rasterization to RAM
            for (int y = 0; y < drawH; ++y)
            {
                for (int x = 0; x < drawW; ++x)
                {
                    WORD colorAttr = clearColorAttribute;

                    if (pixels)
                    {
                        float u = (float)x / (float)drawW;
                        float v = (float)y / (float)drawH;

                        if (cmd.flipX) u = 1.0f - u;
                        if (cmd.flipY) v = 1.0f - v;

                        int texX, texY;

                        // Support for Spritesheets (Sub-rectangles)
                        if (cmd.useSourceRect)
                        {
                            texX = static_cast<int>(cmd.sourceRect.pos.x + (u * cmd.sourceRect.size.x));
                            texY = static_cast<int>(cmd.sourceRect.pos.y + (v * cmd.sourceRect.size.y));
                        }
                        else
                        {
                            texX = static_cast<int>(u * cmd.texture.size.x);
                            texY = static_cast<int>(v * cmd.texture.size.y);
                        }

                        // Bounds check to prevent crashes due to float rounding
                        if (texX < 0) texX = 0; else if (texX >= cmd.texture.size.x) texX = cmd.texture.size.x - 1;
                        if (texY < 0) texY = 0; else if (texY >= cmd.texture.size.y) texY = cmd.texture.size.y - 1;

                        int idx = (texY * cmd.texture.size.x + texX) * 4;

                        if (pixels[idx + 3] < 128) continue; // Discard transparency

                        colorAttr = ConvertToConsoleColor({ pixels[idx], pixels[idx + 1], pixels[idx + 2], 255 });
                    }
                    else
                    {
                        colorAttr = ConvertToConsoleColor(cmd.tint);
                    }

                    DrawBlock(startX + x, startY + y, colorAttr);
                }
            }
        }
    }

    void ConsoleRenderer::EndFrame()
    {
        COORD bSize = { (SHORT)bufferSize.x, (SHORT)bufferSize.y };
        COORD bCoord = { 0, 0 };
        WriteConsoleOutputA(hConsole, frameBuffer.data(), bSize, bCoord, &consoleWriteArea);
        ClearQueues();
    }

    void ConsoleRenderer::ClearScreen(const Color& color)
    {
        clearColorAttribute = ConvertToConsoleColor(color);
        for (auto& charInfo : frameBuffer)
        {
            charInfo.Char.AsciiChar = ' ';
            charInfo.Attributes = clearColorAttribute;
        }
    }

    void ConsoleRenderer::DrawBlock(int x, int y, WORD colorAttr)
    {
        if (x >= 0 && x < bufferSize.x && y >= 0 && y < bufferSize.y)
        {
            frameBuffer[y * bufferSize.x + x].Char.AsciiChar = static_cast<char>(219); // Solid block
            frameBuffer[y * bufferSize.x + x].Attributes = colorAttr;
        }
    }

    WORD ConsoleRenderer::ConvertToConsoleColor(const Color& color)
    {
        WORD attr = 0;
        if (color.r > 100) attr |= FOREGROUND_RED;
        if (color.g > 100) attr |= FOREGROUND_GREEN;
        if (color.b > 100) attr |= FOREGROUND_BLUE;
        if (color.r > 180 || color.g > 180 || color.b > 180) attr |= FOREGROUND_INTENSITY;
        return attr | (attr << 4); // Paint both foreground and background the same color
    }

    void ConsoleRenderer::BeginCamera(const Vector2f& targetPosition, float zoom)
    {
        cameraTarget = targetPosition;
        cameraZoom = zoom;
    }

    void ConsoleRenderer::EndCamera()
    {
        cameraTarget = { 0.0f, 0.0f };
        cameraZoom = 1.0f;
    }

    void ConsoleRenderer::BeginFrame() {}
    void ConsoleRenderer::FlushDebug(RenderLayer) {}
    Texture2D ConsoleRenderer::CreateWhitePixel() { return Texture2D{ 0, {1,1} }; }
}