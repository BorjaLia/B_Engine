#include "ModelRendererComponent.h"

#include "../scenes/Node.h"
#include "../scenes/Transform.h" 
#include "../graphics/RendererBase.h"
#include "../math/MathUtils.h"

namespace Engine
{

	ModelRendererComponent::ModelRendererComponent(const Model& model, Color tint)
		: model(model), tint(tint)
	{
	}

    void ModelRendererComponent::Draw(RendererBase* renderer)
    {
        if (owner == nullptr || renderer == nullptr) return;
        if (model.meshes.empty()) return; // Do not draw invalid/unloaded models

        Matrix4x4 finalMat = owner->transform.GetGlobalMatrix();

        // Apply the local model offset directly into the cached transformation matrix
        // This uses the directional vectors to keep the offset local relative to the object's rotation.
        Vector3f right = owner->transform.GetRight();
        Vector3f up = owner->transform.GetUp();
        Vector3f forward = owner->transform.GetForward();

        Vector3f rotatedOffset = {
            (right.x * modelOffset.x) + (up.x * modelOffset.y) + (forward.x * modelOffset.z),
            (right.y * modelOffset.x) + (up.y * modelOffset.y) + (forward.y * modelOffset.z),
            (right.z * modelOffset.x) + (up.z * modelOffset.y) + (forward.z * modelOffset.z)
        };

        // Add the rotated offset to the translation column of the matrix
        finalMat.m[0][3] += rotatedOffset.x;
        finalMat.m[1][3] += rotatedOffset.y;
        finalMat.m[2][3] += rotatedOffset.z;

        // If a stylized wireframe is needed later, it will be handled by a Material property.
        renderer->SubmitModel(&model, finalMat, tint);
    }

    void ModelRendererComponent::DebugDraw(RendererBase* renderer)
    {
        if (owner == nullptr || renderer == nullptr) return;
        if (model.meshes.empty()) return; // No model, no bounds

        Vector3f globalPos = owner->transform.GetGlobalPosition();
        Vector3f scale = owner->transform.GetScale();

        // Use the new GetGlobalEulerAngles method we added!
        Vector3f globalEuler = owner->transform.GetGlobalEulerAngles();

        Vector3f right = owner->transform.GetRight();
        Vector3f up = owner->transform.GetUp();
        Vector3f forward = owner->transform.GetForward();

        // --- Calculate the exact pivot position (with rotation offsets) ---
        Vector3f rotatedModelOffset = {
            (right.x * modelOffset.x) + (up.x * modelOffset.y) + (forward.x * modelOffset.z),
            (right.y * modelOffset.x) + (up.y * modelOffset.y) + (forward.y * modelOffset.z),
            (right.z * modelOffset.x) + (up.z * modelOffset.y) + (forward.z * modelOffset.z)
        };

        Vector3f pivotPos = {
            globalPos.x + rotatedModelOffset.x,
            globalPos.y + rotatedModelOffset.y,
            globalPos.z + rotatedModelOffset.z
        };

        // 1. Submit Full Mesh Wireframe (If enabled for debugging)
        if (drawWireframe)
        {
            Matrix4x4 finalMat = owner->transform.GetGlobalMatrix();
            // Override the translation with our offset-adjusted pivot
            finalMat.m[0][3] = pivotPos.x;
            finalMat.m[1][3] = pivotPos.y;
            finalMat.m[2][3] = pivotPos.z;

            // Use the debug color so it matches our bounding boxes visually
            renderer->SubmitDebugModelWireframe(&model, finalMat, debugColor);
        }

        // 2. Calculate and submit Bounding Box Cube
        Vector3f boundsSize = {
            (model.bounds.max.x - model.bounds.min.x) * scale.x,
            (model.bounds.max.y - model.bounds.min.y) * scale.y,
            (model.bounds.max.z - model.bounds.min.z) * scale.z
        };

        Vector3f localBoundsCenterOffset = {
            (model.bounds.max.x + model.bounds.min.x) * 0.5f * scale.x,
            (model.bounds.max.y + model.bounds.min.y) * 0.5f * scale.y,
            (model.bounds.max.z + model.bounds.min.z) * 0.5f * scale.z
        };

        Vector3f totalLocalOffset = {
            modelOffset.x + localBoundsCenterOffset.x,
            modelOffset.y + localBoundsCenterOffset.y,
            modelOffset.z + localBoundsCenterOffset.z
        };

        Vector3f rotatedBoundsOffset = {
            (right.x * totalLocalOffset.x) + (up.x * totalLocalOffset.y) + (forward.x * totalLocalOffset.z),
            (right.y * totalLocalOffset.x) + (up.y * totalLocalOffset.y) + (forward.y * totalLocalOffset.z),
            (right.z * totalLocalOffset.x) + (up.z * totalLocalOffset.y) + (forward.z * totalLocalOffset.z)
        };

        Vector3f boundsPos = {
            globalPos.x + rotatedBoundsOffset.x,
            globalPos.y + rotatedBoundsOffset.y,
            globalPos.z + rotatedBoundsOffset.z
        };

        Cube3DShape boundsCube{ boundsSize };
        renderer->SubmitDebugShape3D(boundsCube, boundsPos, globalEuler, debugColor);

        // Direction Lines
        {
            float pointerLength = scale.Magnitude();
            Vector3f endPos = {
                right.x * pointerLength,
                right.y * pointerLength,
                right.z * pointerLength
            };

            Line3DShape pointerLine{ {0.0f, 0.0f, 0.0f}, endPos };
            Color pointerColor = { 255, 0, 0, 255 };

            // Zero rotation because 'endPos' already has the global right transformation baked in!
            renderer->SubmitDebugShape3D(pointerLine, pivotPos, { 0.0f, 0.0f, 0.0f }, pointerColor);
        }

        {
            float pointerLength = scale.Magnitude();
            Vector3f endPos = {
                up.x * pointerLength,
                up.y * pointerLength,
                up.z * pointerLength
            };

            Line3DShape pointerLine{ {0.0f, 0.0f, 0.0f}, endPos };
            Color pointerColor = { 0, 255, 0, 255 };

            // Zero rotation because 'endPos' already has the global up transformation baked in!
            renderer->SubmitDebugShape3D(pointerLine, pivotPos, { 0.0f, 0.0f, 0.0f }, pointerColor);
        }

        {
            float pointerLength = scale.Magnitude();
            Vector3f endPos = {
                forward.x * pointerLength,
                forward.y * pointerLength,
                forward.z * pointerLength
            };

            Line3DShape pointerLine{ {0.0f, 0.0f, 0.0f}, endPos };
            Color pointerColor = { 0, 0, 255, 255 };

            // Zero rotation because 'endPos' already has the global forward transformation baked in!
            renderer->SubmitDebugShape3D(pointerLine, pivotPos, { 0.0f, 0.0f, 0.0f }, pointerColor);
        }
    }
}