#include "SpriteComponent.h"

#include <sstream>

#include "../core/Application.h"
#include "../scenes/Node.h"
#include "TransformComponent.h"
#include "../graphics/RendererBase.h"

namespace Engine
{
	SpriteComponent::SpriteComponent(const Texture2D& texture, Pivot pivot, Color tint, RenderLayer layer)
		: texture(texture), pivot(pivot), tint(tint), layer(layer)
	{
		targetSize = texture.size;
	}

	SpriteComponent::SpriteComponent(Pivot pivot, Color tint, RenderLayer layer)
		: pivot(pivot), tint(tint), layer(layer)
	{
		ENGINE_WARN("Loading sprite component with no texture! - May cause issues with GetTargetSize()");
		texture = Engine::Application::Get().GetResourceManager()->GetTexture("");
	}

	void SpriteComponent::Update(float /*deltaTime*/)
	{
	}

	void SpriteComponent::Draw(RendererBase* renderer)
	{
		if (owner == nullptr || renderer == nullptr) return;

		Vector3f position = owner->transform.GetGlobalPosition();

		float rotation = owner->transform.GetRotation2D();
		Vector2f scale = { owner->transform.GetScale().x, owner->transform.GetScale().y }; // We only care about X,Y scaling for billboards

		if (targetSize.x != 0.0f && targetSize.y != 0.0f)
		{
			scale.x *= (targetSize.x / texture.size.x);
			scale.y *= (targetSize.y / texture.size.y);
		}

		// Submits with a Vector3f. If the renderer is in 2D mode, it ignores Z. If 3D, it billboards it
		renderer->SubmitSprite(layer, texture, position, rotation, scale, pivot, tint, flipX, flipY);
	}

	std::string SpriteComponent::ToString() const
	{
		std::stringstream ss;
		ss << "SpriteComponent [Color: " << tint << "]";
		return ss.str();
	}
}