#include "ButtonComponent.h"

#include <cmath>

#include "../scenes/Node.h"
#include "../core/Application.h"
#include "SpriteComponent.h" 
#include "../graphics/RendererBase.h" 
#include "../utils/StringHash.h"
#include "../debug/Debug.h"

namespace Engine
{
	ButtonComponent::ButtonComponent(const RectangleShape& initialShape, RenderLayer layer, const Vector2f& offset, bool autoFit)
		: shape(initialShape), layer(layer), offset(offset), autoFitToSprite(autoFit)
	{
		auto& bus = Application::Get().GetEventBus();

		// Subscribe to action changes from the InputMapper
		actionEventId = bus.Subscribe<ActionChangedEvent>(
			[this](ActionChangedEvent& e) { this->OnActionChanged(e); }
		);
	}

	ButtonComponent::~ButtonComponent()
	{
		Application::Get().GetEventBus().Unsubscribe(ActionChangedEvent::GetStaticType(), actionEventId);
	}

	void ButtonComponent::Start()
	{
		if (owner == nullptr) return;

		if (auto* sprite = owner->GetComponent<SpriteComponent>())
		{
			layer = sprite->GetLayer();

			if (!autoFitToSprite) return;

			// 1. Get the final transform scale
			Vector2f scale = owner->transform.GetScale();

			// 2. Calculate the real scaled size
			float finalWidth = sprite->GetTargetSize().x * std::abs(scale.x);
			float finalHeight = sprite->GetTargetSize().y * std::abs(scale.y);

			// 3. Bake the final size into the shape
			shape = RectangleShape{ {finalWidth, finalHeight} };

			// 4. THE PIVOT MAGIC
			// Calculate the offset so that the mathematical center matches the drawing origin
			Vector2f pivotMult = GetPivotMultiplier(sprite->pivot);

			offset.x = (0.5f - pivotMult.x) * finalWidth;
			offset.y = (0.5f - pivotMult.y) * finalHeight;
		}
	}

	void ButtonComponent::Update(float /*deltaTime*/)
	{
		if (owner == nullptr) return;

		auto& input = Application::Get().GetInputManager();

		// 1. GET THE MOUSE FROM THE MAPPER (Absolute Axes)
		Vector2f mousePos = {
			input.GetAxis(Hash::GetHash("Pointer_X")),
			input.GetAxis(Hash::GetHash("Pointer_Y"))
		};

		if (layer == RenderLayer::UI)
		{
			mousePos.y = -mousePos.y; // Convert Y-Up back to Y-Down for UI layer calculations
		}

		Vector2f globalPos = owner->transform.GetGlobalPosition();
		Vector2f centerPos = { globalPos.x + offset.x, globalPos.y + offset.y };

		float halfW = shape.size.x / 2.0f;
		float halfH = shape.size.y / 2.0f;

		// Update Hover State
		isHovered = (mousePos.x >= centerPos.x - halfW && mousePos.x <= centerPos.x + halfW &&
			mousePos.y >= centerPos.y - halfH && mousePos.y <= centerPos.y + halfH);
	}

	void ButtonComponent::OnActionChanged(ActionChangedEvent& e)
	{
		// We only care about left click (UI_Interact)
		if (e.GetHash() == Hash::GetHash("UI_Interact"))
		{
			// Value > 0.0f means it was pressed
			if (e.GetValue() > 0.0f)
			{
				if (isHovered)
				{
					isPressed = true;
					e.handled = true; // Consume the event to prevent background clicks

					if (onPressCallback) onPressCallback();

					ButtonPressedEvent pressedEvent(owner->name);
					Application::Get().GetEventBus().Publish(pressedEvent);
				}
			}
			else // Value == 0.0f means it was released
			{
				if (isPressed)
				{
					isPressed = false;
					e.handled = true; // Consume the event to prevent background clicks

					if (onReleaseCallback) onReleaseCallback();

					ButtonReleasedEvent releasedEvent(owner->name);
					Application::Get().GetEventBus().Publish(releasedEvent);

					if (isHovered)
					{
						if (onClickCallback) onClickCallback();

						ButtonClickedEvent clickedEvent(owner->name);
						Application::Get().GetEventBus().Publish(clickedEvent);
					}
				}
			}
		}
	}

	void ButtonComponent::DebugDraw(RendererBase* renderer)
	{
		if (owner == nullptr || renderer == nullptr) return;

		Vector2f globalPos = owner->transform.GetGlobalPosition();
		Vector2f centerPos = { globalPos.x + offset.x, globalPos.y + (offset.y * (layer == Engine::RenderLayer::World ? 1.0f : -1.0f)) };
		float rot = owner->transform.GetRotation2D();

		// Dynamic colors: Yellow (Normal), Green (Hover), Red (Click)
		Color debugColor = { 255, 255, 0, 255 };
		if (isPressed)
		{
			debugColor = { 255, 0, 0, 255 };
		}
		else if (isHovered)
		{
			debugColor = { 0, 255, 0, 255 };
		}

		renderer->SubmitDebugShape(layer, shape, centerPos, rot, debugColor);
	}
}