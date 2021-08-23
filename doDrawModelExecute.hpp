#pragma once
#include "SourceInterface.hpp"

namespace SDK
{
	namespace Hooks
	{
		void doDrawModelExecute(

			void (__fastcall *ofunc)(void* _this, int edx, IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld),

			void* _this,
			int edx,
			IMatRenderContext * ctx,
			const DrawModelState_t &state,
			const ModelRenderInfo_t &pInfo,
			matrix3x4_t *pCustomBoneToWorld
		);

		namespace Chams
		{
			void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool transparent, bool glass, const Color& rgba);
		}
	}
}