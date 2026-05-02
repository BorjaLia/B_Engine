#pragma once

#include <functional>
#include <map>
#include <cstdint>

#include "Event.h"

namespace Engine
{
    using EventCallback = std::function<void(Event&)>;
    using EventListenerID = uint32_t;

    /// Central hub for dispatching and subscribing to system-wide events.
    class EventBus
    {
    public:
        EventBus(const EventBus&) = delete;
        EventBus& operator=(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;
        EventBus& operator=(EventBus&&) = delete;

        // ------------------------------------------------------------------
        // METHOD 1: Original low-level subscription
        // ------------------------------------------------------------------
        EventListenerID Subscribe(EventType type, EventCallback callback)
        {
            EventListenerID id = nextId++;
            subscribers[type][id] = callback;
            return id;
        }

        // ------------------------------------------------------------------
        // METHOD 2: The Magic Wrapper (High-level)
        // ------------------------------------------------------------------
        /// Automatically deduces the desired event type based on the provided lambda parameter.
        template <typename T>
        EventListenerID Subscribe(std::function<void(T&)> callback)
        {
            // We wrap your strongly-typed callback into a generic engine callback
            auto wrapper = [callback](Event& e)
                {
                    // The engine safely casts it for you before execution
                    callback(static_cast<T&>(e));
                };

            // Subscribe using the auto-generated static ID of this event
            return Subscribe(T::GetStaticType(), wrapper);
        }

        /// Removes a listener from a specific event type.
        void Unsubscribe(EventType type, EventListenerID id)
        {
            auto it = subscribers.find(type);
            if (it != subscribers.end())
            {
                it->second.erase(id);
            }
        }

        /// Broadcasts an event to all interested subscribers.
        void Publish(Event& event)
        {
            EventType type = event.GetType();
            auto it = subscribers.find(type);

            if (it != subscribers.end())
            {
                for (auto& pair : it->second)
                {
                    // Stop propagating if an earlier subscriber marked it as handled
                    if (event.handled) break;
                    pair.second(event);
                }
            }
        }

    private:
        // Restricted creation to the Application core.
        friend class Application;

        EventBus() = default;
        ~EventBus() = default;

        std::map<EventType, std::map<EventListenerID, EventCallback>> subscribers;
        EventListenerID nextId = 0;
    };
}