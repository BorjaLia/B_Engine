#pragma once

#include <memory>
#include "../scenes/Node.h"

namespace Engine
{
    /// Constructs a root node populated with UI components to display debugging information.
    /// @ingroup Debug
    Node* CreateDebugNode();

    /// Updates the specific text components inside the debug node.
    /// @param debugNode Pointer to the node created by CreateDebugNode.
    /// @param currentFPS The framerate calculated in the current cycle.
    /// @ingroup Debug
    void UpdateDebugNode(Node* debugNode, int currentFPS);
}