#pragma once

#include "scripts/Script.h"

#include "math/Vector2.h"
#include "math/Vector3.h"


/// Script to handle First-Person style camera rotation using mouse input.
/// @ingroup Scripts
class CameraLookScript : public Engine::Script
{
public:
	CameraLookScript(float lookSensitivity = 0.1f);
	~CameraLookScript() override = default;

	void OnStart() override;
	void OnUpdate(float deltaTime) override;

	void SetSensitivity(float newSensitivity) { sensitivity = newSensitivity; }
	float GetSensitivity() const { return sensitivity; }

private:
	float sensitivity;

	// Accumulators to prevent Gimbal Lock and precision loss
	float pitch; // X-axis rotation (Up/Down)
	float yaw;   // Y-axis rotation (Left/Right)
};
