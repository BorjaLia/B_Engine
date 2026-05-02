#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include "Component.h"
#include "../utils/Types.h"
#include "../utils/Math.h"

namespace Engine
{
    /// Defines the rules and frames for a specific animation.
    struct Animation
    {
        std::vector<Rect> frames;
        float frameDuration = 0.1f;
        bool isLooping = true;
    };

    /// Handles spritesheet animations and playback controls.
    class AnimatedSpriteComponent : public Component
    {
    public:
        Texture2D texture;
        Color tint;
        Pivot pivot;
        RenderLayer layer;

        float speedMultiplier = 1.0f;

        AnimatedSpriteComponent(const Texture2D& texture, Pivot pivot = Pivot::TopLeft, Color tint = { 255, 255, 255, 255 }, RenderLayer layer = RenderLayer::World);
        ~AnimatedSpriteComponent() override = default;

        void Update(float deltaTime) override;
        void Draw(class IRenderer* renderer) override;

#pragma region Animation Management
        /// Adds an animation by manually providing the source rectangles.
        void AddAnimation(const std::string& name, const Animation& animation);

        /// Automatically generates frames based on a regular spritesheet grid.
        void AddAnimationGrid(const std::string& name, int startCol, int startRow, int frameCount, float frameWidth, float frameHeight, float duration = 0.1f, bool looping = true);
#pragma endregion

#pragma region Playback Controls
        void Play(const std::string& name);
        void Stop();
        void Pause();
        void Resume();

        bool IsPlaying() const { return isPlaying; }
        std::string GetCurrentAnimation() const { return currentAnimation; }

        Vector2f GetCurrentFrameSize() const;
#pragma endregion

#pragma region Visual Properties
        void SetFlipX(bool flip) { flipX = flip; }
        void SetFlipY(bool flip) { flipY = flip; }
        bool GetFlipX() const { return flipX; }
        bool GetFlipY() const { return flipY; }

        void SetTargetSize(const Vector2f& size) { targetSize = size; }
        Vector2f GetTargetSize() const { return targetSize; }
#pragma endregion

    private:
        std::unordered_map<std::string, Animation> animations;

        std::string currentAnimation;
        int currentFrameIndex = 0;
        float frameTimer = 0.0f;
        bool isPlaying = false;

        bool flipX = false;
        bool flipY = false;

        Vector2f targetSize = { 0.0f, 0.0f };
    };
}