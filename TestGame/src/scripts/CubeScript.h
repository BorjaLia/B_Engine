#pragma once

#include "scripts/Script.h"

/// @ingroup Scripts
class CubeScript : public Engine::Script
{
public:
	CubeScript();
	~CubeScript() override = default;

	void OnFixedUpdate(float fixedDeltaTime) override;

private:

	bool isRotating = false;
	float rot = 100.0f;
};
