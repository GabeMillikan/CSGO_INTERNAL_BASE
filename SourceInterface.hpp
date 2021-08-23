#pragma once

#include <Windows.h>

#include "IVEngineClient.hpp"
#include "IClientMode.hpp"
#include "IBaseClientDLL.hpp"
#include "IClientEntityList.hpp"
#include "ICVar.hpp"
#include "ISurface.hpp"

#include "CUserCMD.hpp"

namespace SDK
{
	extern IVEngineClient*			g_EngineClient;
	extern IClientMode*				g_ClientMode;
	extern IEngineTrace*			g_EngineTrace;
	extern IBaseClientDLL*			g_CHLClient;
	extern IClientEntityList*		g_EntityList;
	extern CGlobalVarsBase*			g_GlobalVars;
	extern IWeaponSystem*			g_WeaponSystem;
	extern IMaterialSystem*			g_MatSystem;
	extern ICvar*					g_CVar;
	extern IVModelInfoClient*		g_MdlInfo;
	extern ISurface*				g_VGuiSurface;
	extern IPanel*					g_VGuiPanel;
	extern IVModelRender*			g_MdlRender;
	extern IVModelInfo*				g_pModelInfo;
	extern IVRenderView*			g_RenderView;
}

// Gotta define this somewhere....
inline SDK::Vector SDK::C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		studiohdr_t* studio_model = SDK::g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			mstudiobbox_t* hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				Vector min;
				Vector max;
				
				min[0] = hitbox->bbmin.Dot(boneMatrix[hitbox->bone][0]) + boneMatrix[hitbox->bone][0][3];
				min[1] = hitbox->bbmin.Dot(boneMatrix[hitbox->bone][1]) + boneMatrix[hitbox->bone][1][3];
				min[2] = hitbox->bbmin.Dot(boneMatrix[hitbox->bone][2]) + boneMatrix[hitbox->bone][2][3];

				max[0] = hitbox->bbmax.Dot(boneMatrix[hitbox->bone][0]) + boneMatrix[hitbox->bone][0][3];
				max[1] = hitbox->bbmax.Dot(boneMatrix[hitbox->bone][1]) + boneMatrix[hitbox->bone][1][3];
				max[2] = hitbox->bbmax.Dot(boneMatrix[hitbox->bone][2]) + boneMatrix[hitbox->bone][2][3];

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

namespace SDK
{
	typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
	CreateInterfaceFn get_module_factory(HMODULE module);
}

namespace SDK
{
	template<typename T>
	T* get_interface(CreateInterfaceFn f, const char* szInterfaceVersion);
	//main function that grabs all interfaces
	bool __INIT__();
}