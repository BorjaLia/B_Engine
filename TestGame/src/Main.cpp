#include "PlatformerGame.h"
#include "core/Application.h"
#include "debug/MemoryTracker.h"

int game()
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

    return 0;
}

int main()
{
    int ret = game();

    Engine::MemoryTracker::Print();

    return ret;
}