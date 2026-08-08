#include "CameraLookScript.h"

#include "scenes/Node.h"
#include "core/Application.h"

#include "math/MathUtils.h"
#include "math/Vector2.h"
#include "math/Vector3.h"


    CameraLookScript::CameraLookScript(float lookSensitivity)
        : sensitivity(lookSensitivity), pitch(0.0f), yaw(0.0f)
    {
    }

    void CameraLookScript::OnStart()
    {
        if (!owner) return;

        // Initialize pitch and yaw based on current rotation so it doesn't snap to 0
        // (Assuming Transform provides GetRotation() returning Vector3f in degrees or radians)
        Engine::Vector3f initialRotation = owner->transform.GetEulerAngles();
        pitch = initialRotation.x;
        yaw = initialRotation.y;
    }

    void CameraLookScript::OnUpdate(float /*deltaTime*/)
    {
        if (!owner) return;

        auto& inputManager = Engine::Application::Get().GetInputManager();

        // Get mouse delta (assuming this exists and returns a Vector2f of pixel differences)
        Engine::Vector2f mouseDelta = inputManager.GetMouseDelta();

        // Apply sensitivity
        yaw -= mouseDelta.x * sensitivity;
        pitch -= mouseDelta.y * sensitivity;

        // Clamp pitch to avoid the camera flipping upside down (Gimbal Lock prevention)
        // Usually clamped between -89 and 89 degrees.
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // Create the new rotation vector
        Engine::Vector3f newRotation = { pitch * Engine::DEG2RAD, yaw * Engine::DEG2RAD, 0.0f };

        // Apply rotation back to the Transform
        owner->transform.SetEulerAngles(newRotation);
    }
