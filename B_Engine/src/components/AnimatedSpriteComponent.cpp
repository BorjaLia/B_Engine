#include "AnimatedSpriteComponent.h"

#include <cmath>

#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "../graphics/RendererBase.h"
#include "../core/Application.h"
#include "../events/AnimationEvents.h"
#include "../debug/Debug.h"

namespace Engine
{
    AnimatedSpriteComponent::AnimatedSpriteComponent(const Texture2D& texture, Pivot pivot, Color tint, RenderLayer layer)
        : texture(texture), pivot(pivot), tint(tint), layer(layer)
    {
    }

    void AnimatedSpriteComponent::AddAnimation(const std::string& name, const Animation& animation)
    {
        animations[name] = animation;
    }

    void AnimatedSpriteComponent::AddAnimationGrid(const std::string& name, int startCol, int startRow, int frameCount, float frameWidth, float frameHeight, float duration, bool looping)
    {
        Animation anim;
        anim.frameDuration = duration;
        anim.isLooping = looping;

        // Calculate rectangles based on their position in the grid
        for (int i = 0; i < frameCount; ++i)
        {
            float x = (startCol + i) * frameWidth;
            float y = startRow * frameHeight;

            Rect rec;
            rec.pos = { x, y };
            rec.size = { frameWidth, frameHeight };
            anim.frames.push_back(rec);
        }

        animations[name] = anim;
    }

    void AnimatedSpriteComponent::Play(const std::string& name)
    {
        if (animations.find(name) == animations.end())
        {
            ENGINE_WARN("Animation '{}' not found!", name);
            return;
        }

        // Do not restart if it is already playing the exact same animation
        if (currentAnimation == name && isPlaying) return;

        currentAnimation = name;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
        isPlaying = true;
    }

    void AnimatedSpriteComponent::Stop()
    {
        isPlaying = false;
        currentFrameIndex = 0;
        frameTimer = 0.0f;
    }

    void AnimatedSpriteComponent::Pause()
    {
        isPlaying = false;
    }

    void AnimatedSpriteComponent::Resume()
    {
        if (!currentAnimation.empty())
        {
            isPlaying = true;
        }
    }

    void AnimatedSpriteComponent::Update(float deltaTime)
    {
        if (!isPlaying || currentAnimation.empty()) return;

        auto it = animations.find(currentAnimation);
        if (it == animations.end()) return;

        const Animation& anim = it->second;

        // Abort if the animation has no frames
        if (anim.frames.empty()) return;

        frameTimer += (deltaTime * speedMultiplier);

        // Advance frames based on elapsed time
        while (frameTimer >= anim.frameDuration)
        {
            frameTimer -= anim.frameDuration;
            currentFrameIndex++;

            if (currentFrameIndex >= anim.frames.size())
            {
                if (anim.isLooping)
                {
                    currentFrameIndex = 0;
                }
                else
                {
                    // Leave it on the last frame and stop playback
                    currentFrameIndex = static_cast<int>(anim.frames.size()) - 1;
                    isPlaying = false;

                    AnimationFinishedEvent event(currentAnimation, owner);
                    Application::Get().GetEventBus().Publish(event);

                    break;
                }
            }
        }
    }

    void AnimatedSpriteComponent::Draw(RendererBase* renderer)
    {
        if (owner == nullptr || renderer == nullptr) return;
        if (currentAnimation.empty() || animations.find(currentAnimation) == animations.end()) return;

        const Animation& anim = animations[currentAnimation];
        if (anim.frames.empty() || currentFrameIndex >= anim.frames.size()) return;

        Vector3f position = owner->transform.GetGlobalPosition();

        float rotation = owner->transform.GetRotation2D();
        Vector2f scale = { owner->transform.GetScale().x, owner->transform.GetScale().y };

        // Get the cropped rect for the current frame
        Rect sourceRect = anim.frames[currentFrameIndex];

        if (targetSize.x != 0.0f && targetSize.y != 0.0f)
        {
            scale.x *= (targetSize.x / texture.size.x);
            scale.y *= (targetSize.y / texture.size.y);
        }

        renderer->SubmitSprite(layer, texture, sourceRect, position, rotation, scale, pivot, tint, flipX, flipY);
    }

    Vector2f AnimatedSpriteComponent::GetCurrentFrameSize() const
    {
        // 1. If active, return current frame size
        if (!currentAnimation.empty() && animations.find(currentAnimation) != animations.end())
        {
            const auto& frames = animations.at(currentAnimation).frames;
            if (!frames.empty() && currentFrameIndex < frames.size())
            {
                return { frames[currentFrameIndex].size.x, frames[currentFrameIndex].size.y };
            }
        }
        // 2. If nothing is playing, return the size of the first frame of the first loaded animation
        else if (!animations.empty())
        {
            const auto& frames = animations.begin()->second.frames;
            if (!frames.empty())
            {
                return { frames[0].size.x, frames[0].size.y };
            }
        }

        // 3. Absolute fallback
        return { static_cast<float>(texture.size.x), static_cast<float>(texture.size.y) };
    }
}