#pragma once
#include "SourceInterface.hpp"

namespace SDK
{
	namespace Hooks
	{
		static QAngle lastServerAngle(0,0,0);

		bool doCreateMove(	bool(_stdcall *ofunc)(float sampleTime, CUserCmd* pCmd),
							float sampletime,
							SDK::CUserCmd* pCmd);


		namespace CRMove
		{
			int getGameMode(bool update = false);
			void SetServerClient(QAngle ServerAngles, CUserCmd* pCmd);
			bool BHOP(int flags, CUserCmd* pCmd);
			bool AutoStrafe(Vector Velocity, CUserCmd* pCmd);
			bool AimStep(QAngle* from, QAngle * to, float step);
			bool GetBestAimbotEntity(QAngle CurrentViewAngle, C_BasePlayer* returnEntity, C_BasePlayer* local, Vector* shootOffset);
		}
	}
}