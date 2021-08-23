#include "HookManager.hpp"

void SDK::Hooks::HOOK_ALL()
{
	Con::Load("Hooking");
	clientmode_hook.setup(g_ClientMode);
	Con::Out(">>Create Move");
	clientmode_hook.hook_index(index::CreateMove, CreateMove);

	vguipanel_hook.setup(g_VGuiPanel);
	Con::Out(">>Paint Traverse");
	vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);

	modelrender_hook.setup(g_MdlRender);
	Con::Out(">>Draw Model Execute");
	modelrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);

}

void SDK::Hooks::UNHOOK_ALL()
{
	clientmode_hook.unhook_all();
	Con::Out("All functions unhooked");
}

bool __stdcall SDK::Hooks::CreateMove(float sampleTime, CUserCmd* pCmd)
{
	static auto oCreateMove = clientmode_hook.get_original<decltype(&CreateMove)>(index::CreateMove);
	return doCreateMove(oCreateMove, sampleTime, pCmd);
}

void __fastcall SDK::Hooks::hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);
	doPaintTraverse(oPaintTraverse, _this, edx, panel, forceRepaint, allowForce);
}

void __fastcall SDK::Hooks::hkDrawModelExecute(void* _this, int edx, IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld)
{
	static auto oDrawModelExecute = modelrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

	if (g_MdlRender->IsForcedMaterialOverride() &&
		!strstr(pInfo.pModel->szName, "arms") &&
		!strstr(pInfo.pModel->szName, "weapons/v_"))
			return oDrawModelExecute(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

	doDrawModelExecute(oDrawModelExecute, _this, edx, ctx, state, pInfo, pCustomBoneToWorld);
	oDrawModelExecute(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

	g_MdlRender->ForcedMaterialOverride(nullptr);


	PMEMORY_BASIC_INFORMATION x;
	x->AllocationBase;
}