#pragma once

#include <memory>
#include "../scenes/Node.h"

namespace Engine
{
    /// Constructs a root node populated with UI components to display debugging information.
    std::unique_ptr<Node> CreateDebugNode();

    /// Updates the specific text components inside the debug node.
    /// @param debugNode Pointer to the node created by CreateDebugNode.
    /// @param currentFPS The framerate calculated in the current cycle.
    void UpdateDebugNode(Node* debugNode, int currentFPS);
}