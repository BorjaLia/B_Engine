#pragma once

#include "scripts/Script.h"

/// @ingroup Scripts
class CubeScript : public Engine::Script
{
public:
	CubeScript();
	~CubeScript() override = default;

	void OnUpdate(float deltaTime) override;

private:

	bool isRotating = false;
	float rot = 100.0f;
};
