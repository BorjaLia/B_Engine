#include "RaylibRenderer.h"

#include <raylib.h> // The only place where Raylib rendering API exists
#include <iostream>

namespace Engine
{
    // Internal helper to translate our Color to Raylib's Color
    static ::Color ToRaylibColor(const Color& color)
    {
        return ::Color{ color.r, color.g, color.b, color.a };
    }

    bool RaylibRenderer::OnInit()
    {
        // Window initialization is handled by RaylibWindow.
        // We just return true to validate the renderer is ready.
        return true;
    }

    void RaylibRenderer::OnShutdown()
    {
        // Nothing to handle here, window closing happens in window
        return;
    }

    float RaylibRenderer::GetDeltaTime() const
    {
        return ::GetFrameTime();
    }

    void RaylibRenderer::BeginFrame()
    {
        ::BeginDrawing();
    }

    void RaylibRenderer::ClearScreen(const Color& color)
    {
        ::ClearBackground(ToRaylibColor(color));
    }

    void RaylibRenderer::EndFrame()
    {
        ::EndDrawing();
        ClearQueues();
    }

    void RaylibRenderer::BeginCamera(const Vector2f& targetPosition, float zoom)
    {
        float screenW = activeRenderTarget.has_value()
            ? static_cast<float>(activeRenderTarget->texture.size.x)
            : static_cast<float>(::GetScreenWidth());

        float screenH = activeRenderTarget.has_value()
            ? static_cast<float>(activeRenderTarget->texture.size.y)
            : static_cast<float>(::GetScreenHeight());
        // Configure Raylib's internal camera
        ::Camera2D cam = { 0 };

        // Offset: Place the focal point at the exact center of the current screen/texture
        cam.offset = { screenW / 2.0f, screenH / 2.0f };

        // Target: The world coordinates we are looking at (invert Y to match Raylib)
        cam.target = { targetPosition.x, -targetPosition.y };
        cam.rotation = 0.0f;
        cam.zoom = zoom;

        // Tell the GPU to apply this mathematical transformation
        ::BeginMode2D(cam);
    }

    void RaylibRenderer::BeginCamera3D(const Vector3f& position, const Vector3f& target, const Vector3f& up, float fov)
    {
        isCamera3DActive = true;

        // Guardamos los valores de forma nativa para el momento del Flush
        camPosition = position;
        camTarget = target;
        camUp = up;
        camFov = fov;

        // Construimos la cámara de Raylib en la pila
        ::Camera3D internalCam3D = { 0 };
        internalCam3D.position = { position.x, position.y, position.z };
        internalCam3D.target = { target.x, target.y, target.z };
        internalCam3D.up = { up.x, up.y, up.z };
        internalCam3D.fovy = fov;
        internalCam3D.projection = CAMERA_PERSPECTIVE;

        ::BeginMode3D(internalCam3D);
    }

    void RaylibRenderer::EndCamera()
    {
        if (isCamera3DActive)
        {
            ::EndMode3D();
        }
        else
        {
            ::EndMode2D();
        }
    }

    void RaylibRenderer::SetRenderTarget(std::optional<Engine::RenderTexture2D> target)
    {
        if (activeRenderTarget.has_value() && target.has_value())
        {
            if (activeRenderTarget->texture.id == target->texture.id) return;
        }
        else if (!activeRenderTarget.has_value() && !target.has_value())
        {
            return;
        }

        if (activeRenderTarget.has_value())
        {
            ::EndTextureMode();
        }

        if (target.has_value())
        {
            ::RenderTexture2D raylibRT = { 0 };
            raylibRT.id = target->id;
            raylibRT.texture.id = target->texture.id;
            raylibRT.texture.width = target->texture.size.x;
            raylibRT.texture.height = target->texture.size.y;

            ::BeginTextureMode(raylibRT);
            ::ClearBackground({ 0,0,0,0 });
        }

        activeRenderTarget = target;
    }

    void RaylibRenderer::DrawRenderTexture(Engine::RenderTexture2D target, const Vector2f& position, const Vector2f& scale, const Color& tint)
    {
        // We use a negative height to prevent the texture from rendering upside down
        ::Rectangle sourceRec = {
            0.0f,
            0.0f,
            static_cast<float>(target.texture.size.x),
            -static_cast<float>(target.texture.size.y)
        };

        ::Rectangle destRec = {
            position.x,
            position.y,
            static_cast<float>(target.texture.size.x) * scale.x,
            static_cast<float>(target.texture.size.y) * scale.y
        };

        ::Vector2 origin = { 0.0f, 0.0f };
        ::Color raylibTint = { tint.r, tint.g, tint.b, tint.a };

        // Draw using ONLY the internal texture ID
        ::Texture2D raylibTex = { 0 };
        raylibTex.id = target.texture.id;
        raylibTex.width = target.texture.size.x;
        raylibTex.height = target.texture.size.y;

        ::DrawTexturePro(raylibTex, sourceRec, destRec, origin, 0.0f, raylibTint);
    }

    void RaylibRenderer::Flush(RenderLayer layer)
    {
        std::vector<SpriteRenderCommand>& queue = (layer == RenderLayer::World) ? worldQueue : uiQueue;

        bool use3DBillboards = (layer == RenderLayer::World && isCamera3DActive);

        ::Camera3D internalCam3D = { 0 };
        if (use3DBillboards)
        {
            internalCam3D.position = { camPosition.x, camPosition.y, camPosition.z };
            internalCam3D.target = { camTarget.x, camTarget.y, camTarget.z };
            internalCam3D.up = { camUp.x, camUp.y, camUp.z };
            internalCam3D.fovy = camFov;
            internalCam3D.projection = CAMERA_PERSPECTIVE;
        }

        for (const auto& cmd : queue)
        {
            // Reconstruct Raylib texture from our generic ID
            ::Texture2D raylibTex = {
                cmd.texture.id,
                cmd.texture.size.x,
                cmd.texture.size.y,
                1,
                7  // PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
            };

            ::Rectangle sourceRec;

            if (cmd.useSourceRect)
            {
                // Snippet usage (Ideal for Text and Spritesheet animations)
                // Raylib flips the image if width/height is negative
                sourceRec = {
                    cmd.sourceRect.pos.x,
                    cmd.sourceRect.pos.y,
                    cmd.flipX ? -cmd.sourceRect.size.x : cmd.sourceRect.size.x,
                    cmd.flipY ? -cmd.sourceRect.size.y : cmd.sourceRect.size.y
                };
            }
            else
            {
                // Use the entire original texture
                sourceRec = {
                    0.0f,
                    0.0f,
                    cmd.flipX ? -static_cast<float>(cmd.texture.size.x) : static_cast<float>(cmd.texture.size.x),
                    cmd.flipY ? -static_cast<float>(cmd.texture.size.y) : static_cast<float>(cmd.texture.size.y)
                };
            }

            float baseWidth = cmd.useSourceRect ? cmd.sourceRect.size.x : static_cast<float>(cmd.texture.size.x);
            float baseHeight = cmd.useSourceRect ? cmd.sourceRect.size.y : static_cast<float>(cmd.texture.size.y);

            float destWidth = baseWidth * cmd.scale.x;
            float destHeight = baseHeight * cmd.scale.y;

            ::Color raylibTint = { cmd.tint.r, cmd.tint.g, cmd.tint.b, cmd.tint.a };

            if (use3DBillboards)
            {
                // --- 3D BILLBOARDING PATH ---
                ::Vector3 pos3D = { cmd.position.x, cmd.position.y, cmd.position.z };
                ::Vector2 size2D = { destWidth, destHeight };

                Engine::Vector2f pivotMult = Engine::GetPivotMultiplier(cmd.pivot);
                ::Vector2 origin = { destWidth * pivotMult.x, destHeight * pivotMult.y };
                ::Vector3 upDir = { 0.0f, 1.0f, 0.0f }; // Standard Y-up

                ::DrawBillboardPro(internalCam3D, raylibTex, sourceRec, pos3D, upDir, size2D, origin, -cmd.rotation, raylibTint);
            }
            else
            {
                // --- 2D ORTHOGRAPHIC PATH ---
                float finalY = (layer == RenderLayer::World) ? -cmd.position.y : cmd.position.y;
                ::Rectangle destRec = { cmd.position.x, finalY, destWidth, destHeight };
                Engine::Vector2f pivotMult = Engine::GetPivotMultiplier(cmd.pivot);
                ::Vector2 origin = { destWidth * pivotMult.x, destHeight * pivotMult.y };

                ::DrawTexturePro(raylibTex, sourceRec, destRec, origin, -cmd.rotation, raylibTint);
            }
        }
    }

    void RaylibRenderer::FlushDebug(RenderLayer layer)
    {
        std::vector<DebugRenderCommand>& queue = (layer == RenderLayer::World) ? debugWorldQueue : debugUIQueue;

        for (const auto& cmd : queue)
        {
            ::Color raylibColor = ToRaylibColor(cmd.color);

            // std::visit is the modern C++ approach for std::variant
            // It safely determines the active shape at runtime
            std::visit([&](auto&& shape)
                {
                    using T = std::decay_t<decltype(shape)>;

                    if constexpr (std::is_same_v<T, RectangleShape>)
                    {
                        float hw = shape.size.x / 2.0f;
                        float hh = shape.size.y / 2.0f;

                        // Convert to radians (Raylib uses inverted Y, so we invert the angle)
                        float rad = cmd.rotation * (3.14159265f / 180.0f);
                        float c = std::cos(rad);
                        float s = std::sin(rad);

                        ::Vector2 corners[4] = {
                            { -hw, -hh }, {  hw, -hh },
                            {  hw,  hh }, { -hw,  hh }
                        };

                        // Rotate and translate to world space
                        for (int i = 0; i < 4; ++i)
                        {
                            float rx = corners[i].x * c - corners[i].y * s;
                            float ry = corners[i].x * s + corners[i].y * c;

                            corners[i].x = cmd.position.x + rx;
                            corners[i].y = -(cmd.position.y + ry); // Y inverted for Raylib
                        }

                        for (int i = 0; i < 4; ++i)
                        {
                            ::DrawLineV(corners[i], corners[(i + 1) % 4], raylibColor);
                        }
                    }
                    else if constexpr (std::is_same_v<T, CircleShape>)
                    {
                        // Circles are drawn from the center by default
                        ::DrawCircleLines(
                            static_cast<int>(cmd.position.x),
                            static_cast<int>(-cmd.position.y),
                            shape.radius,
                            raylibColor
                        );
                    }
                    else if constexpr (std::is_same_v<T, LineShape>)
                    {
                        ::Vector2 start = { cmd.position.x + shape.start.x, -(cmd.position.y + shape.start.y) };
                        ::Vector2 end = { cmd.position.x + shape.end.x, -(cmd.position.y + shape.end.y) };
                        ::DrawLineV(start, end, raylibColor);
                    }
                    else if constexpr (std::is_same_v<T, PolygonShape>)
                    {
                        size_t vertexCount = shape.localVertices.size();

                        if (vertexCount > 1)
                        {
                            for (size_t i = 0; i < vertexCount; ++i)
                            {
                                size_t next = (i + 1) % vertexCount;

                                ::Vector2 start = {
                                    cmd.position.x + shape.localVertices[i].x,
                                    -(cmd.position.y + shape.localVertices[i].y)
                                };
                                ::Vector2 end = {
                                    cmd.position.x + shape.localVertices[next].x,
                                    -(cmd.position.y + shape.localVertices[next].y)
                                };

                                ::DrawLineV(start, end, raylibColor);
                            }
                        }
                    }
                }, cmd.shape);
        }
    }

    Texture2D RaylibRenderer::LoadTexture(const char* filepath)
    {
        ::Texture2D rlTex = ::LoadTexture(filepath);

        Texture2D engTex;
        engTex.id = rlTex.id;
        engTex.size = Vector2i(rlTex.width, rlTex.height);
        engTex.mipmaps = rlTex.mipmaps;
        engTex.format = rlTex.format;

        return engTex;
    }

    void RaylibRenderer::UnloadTexture(Texture2D texture)
    {
        ::Texture2D rlTex = { texture.id, texture.size.x, texture.size.y, texture.mipmaps, texture.format };
        ::UnloadTexture(rlTex);
    }

    Engine::Font* RaylibRenderer::LoadFont(const char* filepath, int baseSize)
    {
        // 1. Ask Raylib to read the file and generate the Atlas.
        // 'nullptr' and '0' tell it to load the 95 standard ASCII characters.
        ::Font rlFont = ::LoadFontEx(filepath, baseSize, nullptr, 0);

        if (rlFont.texture.id == 0)
        {
            std::cerr << "RaylibRenderer ERROR: Could not load font: " << filepath << "\n";
            return nullptr;
        }

        // 2. Create our pure Engine font
        Engine::Font* engFont = new Font();
        engFont->SetBaseSize(baseSize);
        engFont->SetLineHeight(static_cast<float>(rlFont.baseSize));

        // 3. Extract the Texture Atlas and give it to our Font
        engFont->atlas.id = rlFont.texture.id;
        engFont->atlas.size = Vector2i(rlFont.texture.width, rlFont.texture.height);
        engFont->atlas.mipmaps = rlFont.texture.mipmaps;
        engFont->atlas.format = rlFont.texture.format;

        // 4. Extract mathematical data for each glyph
        for (int i = 0; i < rlFont.glyphCount; i++)
        {
            int codepoint = rlFont.glyphs[i].value;
            ::Rectangle rec = rlFont.recs[i];
            ::GlyphInfo info = rlFont.glyphs[i];

            Glyph g;
            g.sourceRect = { {rec.x, rec.y}, {rec.width, rec.height} };
            g.offset = Vector2(info.offsetX, info.offsetY);

            // If Raylib fails to provide advanceX, fallback to rect width
            g.advanceX = (info.advanceX == 0) ? rec.width : static_cast<float>(info.advanceX);

            engFont->AddGlyph(static_cast<char>(codepoint), g);
        }

        // 5. Delete the texture ID in Raylib's struct to prevent UnloadFont from killing it in VRAM
        rlFont.texture.id = 0;
        ::UnloadFont(rlFont);

        return engFont;
    }

    void RaylibRenderer::UnloadFont(Engine::Font* font)
    {
        if (font != nullptr)
        {
            // Temporarily reconstruct the Raylib struct just to kill the texture in VRAM
            ::Texture2D rlTex = { font->atlas.id, font->atlas.size.x, font->atlas.size.y, font->atlas.mipmaps, font->atlas.format };
            ::UnloadTexture(rlTex);
        }
    }

    Engine::RenderTexture2D RaylibRenderer::CreateRenderTexture(const Vector2i& size)
    {
        ::RenderTexture2D raylibRT = ::LoadRenderTexture(size.x, size.y);

        Engine::RenderTexture2D engineRT;
        engineRT.id = raylibRT.id;
        engineRT.texture.id = raylibRT.texture.id;
        engineRT.texture.size.x = raylibRT.texture.width;
        engineRT.texture.size.y = raylibRT.texture.height;

        return engineRT;
    }

    void RaylibRenderer::UnloadRenderTexture(Engine::RenderTexture2D target)
    {
        ::RenderTexture2D raylibRT = { 0 };
        raylibRT.id = target.id;
        raylibRT.texture.id = target.texture.id;

        ::UnloadRenderTexture(raylibRT);
    }

    Texture2D RaylibRenderer::CreateWhitePixel()
    {
        ::Image img = ::GenImageColor(1, 1, ::WHITE);
        ::Texture2D tex = ::LoadTextureFromImage(img);
        ::UnloadImage(img);

        return Engine::Texture2D{ tex.id, {tex.width, tex.height} };
    }
}