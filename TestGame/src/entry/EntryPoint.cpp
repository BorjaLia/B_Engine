#include "EntryPoint.h"

Engine::IGame* CreateGame()
{
	return new PlatformerGame();
}
