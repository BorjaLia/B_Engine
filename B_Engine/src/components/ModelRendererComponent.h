#pragma once

#include "Component.h"
#include "../utils/Types.h"
#include "../math/Vector3.h"

namespace Engine
{
	class RendererBase;

	/// @brief Renders a 3D model at the entity's transform position.
	/// @ingroup Components
	class ModelRendererComponent : public Component
	{
	public:
		Model model;
		Color tint;
		Color debugColor = { 0, 255, 0, 255 };

		// Local offsets
		Vector3f modelOffset = { 0.0f, 0.0f, 0.0f };

		bool drawWireframe = true; // Flag to render only the wireframe using debug tools

		/// @param model The Opaque Model Handle obtained from the ResourceManager
		ModelRendererComponent(const Model& model, Color tint = { 255, 255, 255, 255 });
		~ModelRendererComponent() override = default;

		void Draw(RendererBase* renderer) override;
		void DebugDraw(RendererBase* renderer) override;
	};
}