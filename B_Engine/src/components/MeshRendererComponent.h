#pragma once

#include "Component.h"
#include "../utils/Types.h"
#include "../math/Vector3.h"

namespace Engine
{
	class RendererBase;

	/// @brief Renders a 3D model at the entity's transform position.
	/// @ingroup Components
	class MeshRendererComponent : public Component
	{
	public:
		Model model;
		Color tint;
		Color debugColor = { 0, 255, 0, 255 };

		// Local offsets
		Vector3f modelOffset = { 0.0f, 0.0f, 0.0f };

		/// @param model The Opaque Model Handle obtained from the ResourceManager
		MeshRendererComponent(const Model& model, Color tint = { 255, 255, 255, 255 });
		~MeshRendererComponent() override = default;

		void Draw(RendererBase* renderer) override;
		void DebugDraw(RendererBase* renderer) override;
	};
}