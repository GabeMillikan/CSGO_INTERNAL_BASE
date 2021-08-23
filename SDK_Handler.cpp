#include "SourceInterface.hpp"
#include "Console.hpp"

//this likes to error if its in another file
SDK::CCSWeaponInfo* SDK::C_BaseCombatWeapon::GetCSWeaponData()
{
	return SDK::g_WeaponSystem->GetWpnData(this->m_Item().m_iItemDefinitionIndex());
}
namespace SDK
{
	IVEngineClient*		g_EngineClient		= nullptr;
	IClientMode*		g_ClientMode		= nullptr;
	IBaseClientDLL*     g_CHLClient			= nullptr;
	IClientEntityList*  g_EntityList		= nullptr;
	CGlobalVarsBase*    g_GlobalVars		= nullptr;
	IWeaponSystem*		g_WeaponSystem		= nullptr;
	IMaterialSystem*	g_MatSystem			= nullptr;
	ICvar*				g_CVar				= nullptr;
	IEngineTrace*		g_EngineTrace		= nullptr;
	IVModelInfoClient*	g_MdlInfo			= nullptr;
	ISurface*			g_VGuiSurface		= nullptr;
	IPanel*				g_VGuiPanel			= nullptr;
	IVModelRender*		g_MdlRender			= nullptr;
	IVModelInfo*		g_pModelInfo		= nullptr;
	IVRenderView*		g_RenderView		= nullptr;
}

namespace SDK
{
	CreateInterfaceFn get_module_factory(HMODULE module)
	{
		return reinterpret_cast<CreateInterfaceFn>(GetProcAddress(module, "CreateInterface"));
	}

	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion)
	{
		auto result = reinterpret_cast<T*>(f(szInterfaceVersion, nullptr));

		/*
		NOTICE:
			it is possible that !result, and all hooks that use this interface should be disabled
			the rest of the program should continue to function
		*/

		return result;
	}
}


bool SDK::__INIT__()
{
	/*
		engine.dll
	*/
	Con::Load("Grabbing engine.dll");
	auto enginedll = GetModuleHandleW(L"engine.dll");
	auto engineFactory = get_module_factory(enginedll);

	Con::Out("->VEngineClient014");
	g_EngineClient = get_interface<IVEngineClient>(engineFactory, "VEngineClient014");

	Con::Out("->EngineTraceClient004");
	g_EngineTrace = get_interface<IEngineTrace>(engineFactory, "EngineTraceClient004");

	Con::Out("->VModelInfoClient004");
	g_MdlInfo = get_interface<IVModelInfoClient>(engineFactory, "VModelInfoClient004");

	Con::Out("->VEngineModel016");
	g_MdlRender = get_interface<IVModelRender>(engineFactory, "VEngineModel016");

	Con::Out("->VModelInfoClient004");
	g_pModelInfo = get_interface<IVModelInfo>(engineFactory, "VModelInfoClient004");

	Con::Out("->VEngineRenderView014");
	g_RenderView = get_interface<IVRenderView>(engineFactory, "VEngineRenderView014");

	/*
		materialsystem.dll
	*/
	Con::Load("Grabbing materialsystem.dll");
	auto matSysFactory = get_module_factory(GetModuleHandleW(L"materialsystem.dll"));
	Con::Out("->VMaterialSystem080");
	g_MatSystem = get_interface<IMaterialSystem>(matSysFactory, "VMaterialSystem080");

	/*
		vstdlib.dll
	*/
	Con::Load("Grabbing vstdlib.dll");
	auto valveStdFactory = get_module_factory(GetModuleHandleW(L"vstdlib.dll"));
	Con::Out("->VEngineCvar007");
	g_CVar = get_interface<ICvar>(valveStdFactory, "VEngineCvar007");

	/*
		vguimatsurface.dll
	*/
	Con::Load("Grabbing vguimatsurface.dll");
	auto vguiFactory = get_module_factory(GetModuleHandleW(L"vguimatsurface.dll"));

	Con::Out("->VGUI_Surface031");
	g_VGuiSurface = get_interface<ISurface>(vguiFactory, "VGUI_Surface031");


	/*
	vgui2.dll
	*/
	Con::Load("Grabbing vgui2.dll");
	auto vgui2Factory = get_module_factory(GetModuleHandleW(L"vgui2.dll"));

	Con::Out("->VGUI_Panel009");
	g_VGuiPanel = get_interface<IPanel>(vgui2Factory, "VGUI_Panel009");


	/*
		client.dll
	*/
	Con::Load("Grabbing client.dll");
	auto clientdll = GetModuleHandleW(L"client.dll");
	Con::OutNumber((int)clientdll);
	auto clientFactory = get_module_factory(clientdll);
	Con::OutNumber((int)clientFactory);

	Con::Out("->VClient018");
	g_CHLClient = get_interface<IBaseClientDLL>(clientFactory, "VClient018");

	Con::Out("->VClientEntityList");
	g_EntityList = get_interface<IClientEntityList>(clientFactory, "VClientEntityList003");

	Con::Out("->g_WeaponSystem");
	g_WeaponSystem = *(IWeaponSystem * *)(Util::PatternScan(clientdll, "8B 35 ? ? ? ? FF 10 0F B7 C0") + 2);

	Con::Out("->g_ClientMode");
	do {
		g_ClientMode = **(IClientMode***)((*(uintptr_t**)g_CHLClient)[10] + 0x5);
	} while (!g_ClientMode);
	Con::Out("->g_GlobalVars");
	g_GlobalVars = **(CGlobalVarsBase***)((*(uintptr_t**)g_CHLClient)[11] + 10);



	return true;
}