#pragma once

#include "Event.h"
#include "../math/Vector2.h"

namespace Engine
{
    /// Triggered when the player changes its logical position.
    /// @ingroup Events
    class PlayerMovedEvent : public EventBase<PlayerMovedEvent>
    {
    public:
        PlayerMovedEvent(const Vector2f& position) : newPosition(position) {}

        Vector2f GetPosition() const { return newPosition; }

        const char* GetName() const override { return "PlayerMovedEvent"; }

    private:
        Vector2f newPosition;
    };
}