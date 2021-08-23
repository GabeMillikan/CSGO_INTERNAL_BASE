#pragma once

#include "Console.hpp"
#include "SourceInterface.hpp"
#include "vfunc_hook.hpp"

#include "doCreateMove.hpp"
#include "doPaintTraverse.hpp"
#include "doDrawModelExecute.hpp"

namespace SDK
{
	namespace Hooks
	{
		void HOOK_ALL();
		void UNHOOK_ALL();
	}
}

namespace SDK
{
	namespace Hooks
	{
		static vfunc_hook clientmode_hook;
		bool __stdcall  CreateMove(float sampleTime, CUserCmd* pCmd);

		static vfunc_hook vguipanel_hook;
		void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);

		static vfunc_hook modelrender_hook;
		void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld);
	}
}