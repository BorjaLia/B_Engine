#include "PlatformerGame.h"
#include "core/Application.h"
#include "debug/MemoryTracker.h"

int main()
{
    PlatformerGame game;

    Engine::Application app;

    if (!app.Initialize("Platformer!"))
    {
        return -1;
    }

    game.SetupInputs();
    game.RegisterScenes();

    app.Run();
    app.Shutdown();

    Engine::MemoryTracker::Print();

    return 0;
}