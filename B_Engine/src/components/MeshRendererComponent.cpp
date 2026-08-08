#include "MeshRendererComponent.h"

#include "../scenes/Node.h"
#include "../scenes/Transform.h" 
#include "../graphics/RendererBase.h"
#include "../math/MathUtils.h"

namespace Engine
{
	MeshRendererComponent::MeshRendererComponent(const Model& model, Color tint)
		: model(model), tint(tint)
	{
	}

	void MeshRendererComponent::Draw(RendererBase* renderer)
	{
		if (owner == nullptr || renderer == nullptr) return;
		if (model.meshes.empty()) return; // Do not draw invalid/unloaded models

		Vector3f position = owner->transform.GetGlobalPosition();
		Vector3f scale = owner->transform.GetScale();

		position.x += modelOffset.x;
		position.y += modelOffset.y;
		position.z += modelOffset.z;

		Vector3f euler = owner->transform.GetEulerAngles();
		Vector3f rotationAxis = { 0.0f, 1.0f, 0.0f };
		float rotationAngle = euler.y * RAD2DEG;

		renderer->SubmitModel(model, position, rotationAxis, rotationAngle, scale, tint);
	}

	void MeshRendererComponent::DebugDraw(RendererBase* renderer)
	{
		if (owner == nullptr || renderer == nullptr) return;
		if (model.meshes.empty()) return; // No model, no bounds

		Vector3f globalPos = owner->transform.GetGlobalPosition();
		Vector3f scale = owner->transform.GetScale();

		Vector3f boundsSize = {
			(model.bounds.max.x - model.bounds.min.x) * scale.x,
			(model.bounds.max.y - model.bounds.min.y) * scale.y,
			(model.bounds.max.z - model.bounds.min.z) * scale.z
		};

		Vector3f boundsCenterOffset = {
			(model.bounds.max.x + model.bounds.min.x) * 0.5f * scale.x,
			(model.bounds.max.y + model.bounds.min.y) * 0.5f * scale.y,
			(model.bounds.max.z + model.bounds.min.z) * 0.5f * scale.z
		};

		Vector3f finalPos = {
			globalPos.x + modelOffset.x + boundsCenterOffset.x,
			globalPos.y + modelOffset.y + boundsCenterOffset.y,
			globalPos.z + modelOffset.z + boundsCenterOffset.z
		};

		Cube3DShape boundsCube{ boundsSize };
		renderer->SubmitDebugShape3D(boundsCube, finalPos, debugColor);

		float pointerLength = 2.0f;
		Vector3f forward = owner->transform.GetForward();

		Vector3f endPos = {
			forward.x * pointerLength,
			forward.y * pointerLength,
			forward.z * pointerLength
		};

		Vector3f pivotPos = {
			globalPos.x + modelOffset.x,
			globalPos.y + modelOffset.y,
			globalPos.z + modelOffset.z
		};

		Line3DShape pointerLine{ {0.0f, 0.0f, 0.0f}, endPos };
		Color pointerColor = { 0, 255, 255, 255 }; // Cyan

		renderer->SubmitDebugShape3D(pointerLine, pivotPos, pointerColor);
	}
}