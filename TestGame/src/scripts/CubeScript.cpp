#include "CubeScript.h"

#include "scenes/Node.h"
#include "core/Application.h"

#include "math/MathUtils.h"
#include "math/Vector3.h"

CubeScript::CubeScript() {}

void CubeScript::OnUpdate(float deltaTime)
{
	if (!owner) return;

	auto& mapper = Engine::Application::Get().GetInputManager();

	if (mapper.IsActionJustPressed(Engine::Hash::GetHash("Game_CubeRot"))) isRotating = !isRotating;
	if (mapper.IsActionJustPressed(Engine::Hash::GetHash("Game_CubeRotDir"))) rot *= -1.0f;

	Engine::Vector3f rotation = owner->transform.GetEulerAngles();

	rotation.y += (rot * deltaTime) * Engine::DEG2RAD;
	

	owner->transform.SetEulerAngles(rotation);
}
