#pragma once

#include <string>

#include "Component.h"
#include "../utils/Types.h"

namespace Engine
{
    /// Utility component that logs its owner's position whenever it changes.
    /// Useful for tracking entity movement issues during development.
    class DebugTrackingComponent : public Component
    {
    public:
        DebugTrackingComponent();
        ~DebugTrackingComponent() override = default;

        void Update(float deltaTime) override;

        std::string ToString() const override;

    private:
        Vector2f lastPosition;
        bool wasMoving;
    };
}