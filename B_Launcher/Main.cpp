///
/// B_Engine Launcher
/// Only entrypoint for the engine.
/// Starts the engine and loads the game
///

#include "core/Application.h"
#include "debug/MemoryTracker.h"
#include "utils/FileSystem.h" 

//#include "../TestGame/src/entry/EntryPoint.h"

// Game must implement "CreateGame"
extern "C" Engine::IGame* CreateGame();

int main(int argc, char** argv)
{
    Engine::MemoryTracker::RecordBaseline();

    if (argc > 1)
    {
        Engine::FileSystem::SetRootPath(argv[1]);
        std::cout << "Engine Launcher: Working directory set to '" << argv[1] << "'\n";
    }

    Engine::IGame* gameInstance = CreateGame();
    if (!gameInstance)
    {
        ENGINE_ERROR("FATAL CRASH - COULDNT GET GAME INSTANCE");
        return -1;
    }

    Engine::Application app;

    if (!app.Initialize("B_Engine Launcher", gameInstance))
    {
        delete gameInstance;
        return -1;
    }

    app.Run();

    app.Shutdown();

    Engine::MemoryTracker::Print();

    return 0;
}