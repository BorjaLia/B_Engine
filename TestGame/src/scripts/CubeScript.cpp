#include "CubeScript.h"

#include "scenes/Node.h"
#include "core/Application.h"
#include "utils/StringHash.h"
#include "math/MathUtils.h"
#include "math/Vector3.h"
#include "debug/Debug.h"

CubeScript::CubeScript() {}

void CubeScript::OnFixedUpdate(float fixedDeltaTime)
{
	if (!owner) return;

	auto& mapper = Engine::Application::Get().GetInputManager();

	if (mapper.IsActionJustReleased(Engine::Hash::GetHash("Game_CubeRot")))
	{
		isRotating = !isRotating;
		ENGINE_LOG("rotating by {}",rot);
	}

	if (!isRotating) return;

	if (mapper.IsActionJustPressed(Engine::Hash::GetHash("Game_CubeRotDir"))) rot *= -1.0f;

	Engine::Vector3f rotation = owner->transform.GetEulerAngles();

	rotation.y += (rot * fixedDeltaTime) * Engine::DEG2RAD;

	owner->transform.SetEulerAngles(rotation);
}
