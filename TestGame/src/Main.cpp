#include "PlatformerGame.h"
#include "core/Application.h"
#include "debug/MemoryTracker.h"

#include <cstdlib> // Para atexit

void PrintLeaks()
{
    Engine::MemoryTracker::Print();
}

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
    Engine::MemoryTracker::RecordBaseline();

    std::atexit(PrintLeaks);

    int ret = game();

    return ret;
}