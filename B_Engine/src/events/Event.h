#pragma once

#include <cstdint>

namespace Engine
{
    using EventType = uint32_t;

    /// @defgroup Events Event System
    /// @brief The EventBus, CRTP event generation, and system events.

    /// 1. AUTOMATIC ID GENERATOR
    /// @ingroup Events
    class EventTypeGenerator
    {
    public:
        template <typename T>
        static EventType GetID()
        {
            // This static variable is initialized ONLY ONCE per distinct class T.
            // It assigns a unique dynamic number at runtime securely.
            static const EventType id = ++counter;
            return id;
        }

    private:
        inline static EventType counter = 0;
    };

    /// 2. BASE EVENT CLASS
    /// @ingroup Events
    class Event
    {
    public:
        virtual ~Event() = default;

        virtual EventType GetType() const = 0;
        virtual const char* GetName() const = 0;

        bool handled = false;
    };

    /// 3. CRTP (Curiously Recurring Template Pattern)
    /// Any new event inherits from here, passing itself as the template argument.
    /// This automatically injects the GetType() function with a unique runtime ID.
    /// @ingroup Events
    template <typename T>
    class EventBase : public Event
    {
    public:
        /// Retrieves the static ID for this specific event type (useful for EventBus registration).
        static EventType GetStaticType()
        {
            return EventTypeGenerator::GetID<T>();
        }

        /// Fulfills the virtual contract of the parent Event class.
        EventType GetType() const override
        {
            return GetStaticType();
        }
    };
}