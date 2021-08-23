#include "INTERNALBASE.hpp"
#include "HookManager.hpp"
#include "Menu.hpp"
#include "SourceInterface.hpp"

DWORD WINAPI MAIN_ATTACH(LPVOID handle)
{
	Con::Attach();
	Con::Out("Welcome!");
	
	Con::Out("\n////////////////////Grabbing Interfaces\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
	SDK::__INIT__();

	Con::Out("\n////////////////////Initalizing Netvars\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
	SDK::NetvarSys::Get().Initialize();

	Con::Out("\n////////////////////Grabbing Internal Game Functions\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
	SDK::Hooks::HOOK_ALL();


	Con::Out("\n\n\n\n\n\n\n\n\n\n\n\n\n Completed Initalization Successfully! \n\n\n\n\n\n\n\n\n\n\n\n\n");

	bool lastGameValue = !SDK::g_EngineClient->IsInGame();
	while (true)
	{
		bool currentGameValue = SDK::g_EngineClient->IsInGame();
		if (lastGameValue != currentGameValue)
		{
			SDK::Hooks::CRMove::getGameMode(true);
		}
		lastGameValue = currentGameValue;
		Sleep(5000);
	}
	return 0;
}

DWORD WINAPI MAIN_DETACH()
{
	Con::Out("\n////////////////////Releasing Internal Game Functions\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
	SDK::Hooks::UNHOOK_ALL();
	return 0;
}