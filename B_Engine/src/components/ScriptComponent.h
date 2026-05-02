#pragma once

#include <memory>

#include "Component.h"
#include "../scripts/Script.h"

namespace Engine
{
    /// Bridges custom user logic (Scripts) into the engine's Component/Node system.
    /// @ingroup Components
    class ScriptComponent : public Component
    {
    public:
        /// The constructor requires a script dynamically allocated with 'new'.
        /// @param userScript The instantiated user script.
        ScriptComponent(Script* userScript);
        ~ScriptComponent() override;

        void Initialize() override;
        void Start() override;
        void Update(float deltaTime) override;
        void FixedUpdate(float fixedDeltaTime) override;

        /// Optional utility: Allows other systems to retrieve the original user script.
        template <typename T>
        T* GetScript() const
        {
            return dynamic_cast<T*>(script.get());
        }

    private:
        // The engine takes absolute ownership of the user script's memory.
        std::unique_ptr<Script> script;
    };
}