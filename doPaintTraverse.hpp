#pragma once
#include "SourceInterface.hpp"

namespace SDK
{
	namespace Hooks
	{

		void doPaintTraverse(void(__fastcall *ofunc)(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce),
			void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce);


		namespace PaintTrav
		{
			bool screen_transform(const Vector& in, Vector& out);
			bool WorldToScreen(const Vector& in, Vector& out);
			bool DrawHealth(C_BasePlayer* entity, int hp, unsigned long FONT);
		}
	}
}