#pragma once

#include <functional>
#include <string>

#include "Component.h"
#include "../events/Event.h"
#include "../events/InputEvents.h"
#include "../utils/Types.h"

namespace Engine
{
    /// Triggered when a button component is successfully clicked.
    class ButtonPressedEvent : public EventBase<ButtonPressedEvent>
    {
    public:
        ButtonPressedEvent(const std::string& name) : buttonName(name) {}

        std::string GetButtonName() const { return buttonName; }
        const char* GetName() const override { return "ButtonPressedEvent"; }

    private:
        std::string buttonName;
    };

    /// UI Element that responds to mouse interactions and hover states.
    class ButtonComponent : public Component
    {
    public:
        /// @param initialShape The hit-box of the button.
        /// @param layer The render layer (usually UI).
        /// @param offset The local offset applied to the hitbox.
        /// @param autoFit If true, it automatically resizes to match the attached SpriteComponent.
        ButtonComponent(const RectangleShape& initialShape = RectangleShape{ {0.0f, 0.0f} },
            RenderLayer layer = RenderLayer::UI,
            const Vector2f& offset = { 0.0f, 0.0f },
            bool autoFit = true
        );

        ~ButtonComponent() override;

        void Start() override;
        void Update(float deltaTime) override;
        void DebugDraw(class IRenderer* renderer) override;

        void SetOnClick(std::function<void()> callback) { onClickCallback = callback; }

        bool IsHovered() const { return isHovered; }
        bool IsPressed() const { return isPressed; }

        void SetOffset(Vector2f newoffset) { offset = newoffset; }

        void SetLayer(RenderLayer newLayer) { layer = newLayer; }
        RenderLayer GetLayer() const { return layer; }

    private:
        RectangleShape shape;
        RenderLayer layer;

        bool autoFitToSprite;
        Vector2f offset;

        bool isHovered = false;
        bool isPressed = false;

        std::function<void()> onClickCallback;

        uint32_t actionEventId = 0;

        void OnActionChanged(ActionChangedEvent& e);
    };
}