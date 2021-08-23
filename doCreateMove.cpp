#include "doCreateMove.hpp"
#include "Console.hpp"
#include "Menu.hpp"


double clamp(double sm, double lg, double val)
{
	return max(sm, min(lg, val));
}
double force(double a, double b, double val)
{//returns what is closer
	return abs(a - val) <= abs(b - val) ? a : b;
}

int CachedGameMode = -1;
int SDK::Hooks::CRMove::getGameMode(bool update)
{
	if (!update && CachedGameMode>=0)
		return CachedGameMode;

	int game_type = g_CVar->FindVar("game_type")->GetInt();
	int game_mode = g_CVar->FindVar("game_mode")->GetInt();

	switch (game_type)
	{
	case 0:
		switch (game_mode)
		{
		case 0:
			CachedGameMode = GAMEMODE_CASUAL;
			return GAMEMODE_CASUAL;
		case 1:
			CachedGameMode = GAMEMODE_COMPETITIVE;
			return GAMEMODE_COMPETITIVE;
		case 2:
			CachedGameMode = GAMEMODE_WINGMAN;
			return GAMEMODE_WINGMAN;
		default:
			CachedGameMode = GAMEMODE_UNKNOWN;
			return GAMEMODE_UNKNOWN;
		}
		return GAMEMODE_UNKNOWN;
	case 1:
		switch (game_mode)
		{
		case 0:
			CachedGameMode = GAMEMODE_ARMSRACE;
			return GAMEMODE_ARMSRACE;
		case 1:
			CachedGameMode = GAMEMODE_DEMOLITION;
			return GAMEMODE_DEMOLITION;
		case 2:
			CachedGameMode = GAMEMODE_DEATHMATCH;
			return GAMEMODE_DEATHMATCH;
		default:
			CachedGameMode = GAMEMODE_UNKNOWN;
			return GAMEMODE_UNKNOWN;
		}
		return GAMEMODE_UNKNOWN;
	case 4:
		switch (game_mode)
		{
		case 0:
			CachedGameMode = GAMEMODE_GAURDIAN;
			return GAMEMODE_GAURDIAN;
		case 1:
			CachedGameMode = GAMEMODE_COOPSTRIKE;
			return GAMEMODE_COOPSTRIKE;
		default:
			CachedGameMode = GAMEMODE_UNKNOWN;
			return GAMEMODE_UNKNOWN;
		}
		return GAMEMODE_UNKNOWN;
	case 6:
		switch (game_mode)
		{
		case 0:
			CachedGameMode = GAMEMODE_DANGERZONE;
			return GAMEMODE_DANGERZONE;
		default:
			CachedGameMode = GAMEMODE_UNKNOWN;
			return GAMEMODE_UNKNOWN;
		}
		CachedGameMode = GAMEMODE_UNKNOWN;
		return GAMEMODE_UNKNOWN;
	default:
		CachedGameMode = GAMEMODE_UNKNOWN;
		return GAMEMODE_UNKNOWN;
	}
	CachedGameMode = GAMEMODE_UNKNOWN;
	return GAMEMODE_UNKNOWN;
	//will never reach most of above "return GAMEMODE_UNKNOWN;" 's but its good practice to include them
}

void SDK::Hooks::CRMove::SetServerClient(QAngle ServerAngles, CUserCmd* pCmd)
{
	//add a clientangles parameter for removing recoil later
	QAngle ClientAngles = pCmd->viewangles;

	float clientHorz = ClientAngles.yaw;
	float serverHorz = ServerAngles.yaw;
	//the requested movement, by defalut will apply to server angles
	float velocity = sqrt(pow(pCmd->forwardmove, 2) + pow(pCmd->sidemove, 2));
	//but we want to move relative to the client angles

	//get the angle that i *want* to be moving towards
	float inputDirection = (float)atan2f(-pCmd->sidemove, pCmd->forwardmove) * 180 / (float)3.1415926535;
	float targetDirection = clientHorz + inputDirection;

	//subtract server angle and get rectangular velocities
	//then set movement relative to client
	pCmd->forwardmove = (float)(velocity * cos((targetDirection - serverHorz)*3.1415926535 / 180));
	pCmd->sidemove = (float)(velocity * -sin((targetDirection - serverHorz)*3.1415926535 / 180));

	pCmd->viewangles = ServerAngles;

}

bool SDK::Hooks::CRMove::BHOP(int flags, CUserCmd* pCmd)
{
	//returns wether of not im gonna jump
	if (!(flags & FL_ONGROUND))
	{ //if in the air, then disable jump
		pCmd->buttons &= ~IN_JUMP;
		return false;
	}
	return true;
}

bool SDK::Hooks::CRMove::AutoStrafe(Vector Velocity, CUserCmd* pCmd)
{
	if (Velocity.Length() < 25)
		return false;

	int mx = pCmd->mousedx;
	if (mx > 0)
	{
		pCmd->sidemove = 450;
		pCmd->buttons |= IN_MOVERIGHT;
		pCmd->buttons &= ~IN_MOVELEFT;
		return true;
	}
	else if (mx < 0)
	{
		pCmd->sidemove = -450;
		pCmd->buttons |= IN_MOVELEFT;
		pCmd->buttons &= ~IN_MOVERIGHT;
		return true;
	}
	return false;
}

bool SDK::Hooks::CRMove::GetBestAimbotEntity(QAngle CurrentViewAngle, C_BasePlayer* returnEntity, C_BasePlayer* local, Vector* shootOffset)
{
	float bestScore = 0;

	Vector ShootingFrom = local->GetEyePos();
	for (int i = 0; i <= g_GlobalVars->maxClients; ++i)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity)
			continue;
		if (!entity->IsPlayer())
			continue;
		if (entity->m_iTeamNum() == local->m_iTeamNum() && !Menu::CConfig.m_bShootTeammates)
			continue;
		if (!entity->IsAlive() || entity->IsDormant() || entity->m_bGunGameImmunity())
			continue;

		Vector entityloc = entity->GetHitboxPos(HITBOX_HEAD);

		Ray_t ray;
		trace_t trace;
		CTraceFilter filter;

		ray.Init(ShootingFrom, entityloc);
		filter.pSkip = local;

		g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
		QAngle anglesTo(ShootingFrom, entityloc);
		//254 is the absolute maximum turn angle
		//sqrt((89-(-89))^2 + 180^2) = 253.14818
		float myscore = 254-CurrentViewAngle.distanceTo(anglesTo);

		float units_left = (entityloc - ShootingFrom).Length() * (1 - trace.fraction);
		if (!(units_left < 10 || trace.hit_entity == entity))
		{
			//try pelvis
			entityloc = entity->GetHitboxPos(HITBOX_PELVIS);
			
			ray.Init(ShootingFrom, entityloc);
			filter.pSkip = local;

			g_EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &trace);
			anglesTo = QAngle(ShootingFrom, entityloc);
			//254 is the absolute maximum turn angle
			//sqrt((89-(-89))^2 + 180^2) = 253.14818
			myscore = 254 - CurrentViewAngle.distanceTo(anglesTo);

			units_left = (entityloc - ShootingFrom).Length() * (1 - trace.fraction);
			if (!(units_left < 10 || trace.hit_entity == entity))
			{
				//if i cant hit the enemy, then dont even try
				continue;
			}
		}

		if (myscore > bestScore)
		{
			*shootOffset = entityloc - ShootingFrom;
			returnEntity = entity;
			if (myscore == 0.f) myscore = 0.001f; //fix boolean return
			bestScore = myscore;
		}
	}
	return bestScore>0.f;
}

bool SDK::Hooks::CRMove::AimStep(QAngle* from, QAngle * to, float step)
{
	float yawNegative = fmod((from->yaw - (-180)) + (180 - to->yaw), 360);
	float yawPositive = fmod((180 - from->yaw) + (to->yaw - (-180)), 360);
	float yawChange = yawNegative < yawPositive ? -yawNegative : yawPositive;
	float pitchChange = to->pitch - from->pitch;
	QAngle adjustment(pitchChange, yawChange, 0);

	if (sqrt(pow(adjustment.yaw, 2) + pow(adjustment.pitch, 2)) < step)
	{//adjustment is less then step
	 //should make operator for this
		from->yaw += adjustment.yaw;
		from->pitch += adjustment.pitch;
		return true;
	}
	else
	{
		adjustment.Unit();
		adjustment *= step;
		//should make operator for this
		from->yaw += adjustment.yaw;
		from->pitch += adjustment.pitch;
		return false;
	}

}


bool SDK::Hooks::doCreateMove(bool(_stdcall *ofunc)(float sampleTime, CUserCmd* m_bAimbot),
	float sampletime,
	SDK::CUserCmd* pCmd)
{
	/*
		Determine if i should continue
	*/
	if (!ofunc(sampletime, pCmd))
		return false;
	//this is pretty rare, so if it happens, then dont mess with it
	if (!pCmd->tick_count) //no clue as to why tickcount is sometimes 0, this check seems to do nothing
		return true;

	// quick duck
	pCmd->buttons |= IN_BULLRUSH;

	C_BasePlayer* localPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!localPlayer) return true;
	if (localPlayer->m_lifeState() != LIFE_ALIVE) return true;
	if (!g_EngineClient->IsInGame()) return true;

	/*
		Setup
	*/
	int CurrentGameMode = CRMove::getGameMode();
	int ticknum = pCmd->tick_count;
	QAngle ClientAngles = *localPlayer->GetVAngles();
	auto ActiveWeapon = localPlayer->m_hActiveWeapon();
	if (!g_EntityList->GetClientEntityFromHandle(ActiveWeapon)) return true;

	float flServerTime = localPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	float timeToAttack = localPlayer->m_flNextAttack() - flServerTime;
	float timeToPrimary = ActiveWeapon->m_flNextPrimaryAttack() - flServerTime;
	float timeToSecondary = ActiveWeapon->m_flNextSecondaryAttack() - flServerTime;
	bool isReloading = ActiveWeapon->IsReloading();
	int bulletsLeft = ActiveWeapon->m_iClip1();

	bool inPrimary = pCmd->buttons & IN_ATTACK;
	bool inSecondary = pCmd->buttons & IN_ATTACK2;
	int FFlags = localPlayer->m_fFlags();
	Vector localVelocity = localPlayer->m_vecVelocity();
	bool jumping = pCmd->buttons & IN_JUMP;
	bool bhopped = jumping;
	bool autostrafed = jumping;
	bool WillShoot = false;

	bool CanPrimary = (timeToAttack <= 0 && timeToPrimary <= 0 && (bulletsLeft > 0 || !ActiveWeapon->IsGun()));
	bool CanSecondary = (timeToAttack <= 0 && timeToSecondary <= 0 && !ActiveWeapon->IsGun());

	if (CanPrimary && inPrimary || CanSecondary && inSecondary)
		WillShoot = true;

	if (ActiveWeapon->m_fThrowTime() > 0) //grenade is being released from hand
		WillShoot = true;
	else if (ActiveWeapon->IsGrenade()) //holding grenade, but not released
		WillShoot = false;

	//dont press mouse if you cant shoot "full auto" :lol:
	if (!WillShoot && !ActiveWeapon->IsGrenade() && !ActiveWeapon->IsC4())
	{
		pCmd->buttons &= ~IN_ATTACK;
		if (!ActiveWeapon->IsGun())
			pCmd->buttons &= ~IN_ATTACK2;
	}

	/*
	Options setup
	*/
	bool BhopToggledOn = jumping;//replace with some keypress
	BhopToggledOn = BhopToggledOn && Menu::CConfig.m_bBHOP;

	bool AutoStrafeToggledOn = Menu::CConfig.m_bAutoStrafe;

	bool AimbotToggledOn = Menu::CConfig.m_bAimbot && ActiveWeapon->IsGun();

	int AimstepDistance = (Menu::CConfig.m_bAdjAimstepNonComp && CurrentGameMode != GAMEMODE_COMPETITIVE) ? Menu::CConfig.m_iAimstepNonComp : Menu::CConfig.m_iAimstep;

	bool AntiAimToggledOn = Menu::CConfig.m_bAntiAim;
	AntiAimToggledOn = AntiAimToggledOn && (Menu::CConfig.m_bAntiAimInNonComp || CurrentGameMode == GAMEMODE_COMPETITIVE);


	/*
		Preform Adjustments
	*/
	//BHOP
	if (BhopToggledOn)
		bhopped = CRMove::BHOP(FFlags, pCmd);
	//Autostrafe
	if (!bhopped && BhopToggledOn && AutoStrafeToggledOn)
		autostrafed = CRMove::AutoStrafe(localVelocity, pCmd);


	//Aimbot
	bool AimbotOverrideAntiAim = false;
	if (AimbotToggledOn && (CanPrimary && inPrimary))
	{
		AimbotOverrideAntiAim = false;
		if ((CurrentGameMode != GAMEMODE_COMPETITIVE) ? Menu::CConfig.m_bAimbotMouseLockNonComp : Menu::CConfig.m_bAimbotMouseLock)
		{
			pCmd->buttons &= ~IN_ATTACK;
			WillShoot = false;
		}
		Vector shootOffset;
		C_BasePlayer* plrToHit = localPlayer; //placeholder
		if (CRMove::GetBestAimbotEntity(ClientAngles, plrToHit, localPlayer, &shootOffset))
		{
			
			/*
			todo: make autostop that isnt shit

			Vector velocity = localPlayer->m_vecVelocity();
			Vector BestVelocity = -velocity;

			//how do i have to move to get that velocity?
			float bestyaw = atan2f(BestVelocity.y, BestVelocity.x)*180 / 3.14159265;
			float curyaw = ClientAngles.yaw;
			float deltayaw = bestyaw - curyaw;

			//rotate BestVelocity by deltayaw amount and apply as movement
			float newx = cos(deltayaw*3.14159265 / 180) * 450;
			float newy = sin(deltayaw*3.14159265 / 180) * 450;
			pCmd->sidemove = force(-450, 450, -newy);
			pCmd->forwardmove = force(-450, 450, newx);

			if (velocity.Length2D() <= localPlayer->m_flMaxspeed()/5)
			*/
			{
				AimbotOverrideAntiAim = true;

				QAngle angles(shootOffset);

				QAngle punchAngles = localPlayer->m_aimPunchAngle();
				angles -= punchAngles * 2;

				QAngle newAngle = lastServerAngle;
				//AimStep will adjust first parameter by a factor of AimstepDistance
				bool Aimstep_Completed = CRMove::AimStep(&newAngle, &angles, AimstepDistance);
				newAngle.Normalize();
				//motion control
				CRMove::SetServerClient(newAngle, pCmd);

				if (Aimstep_Completed)
				{
					pCmd->buttons |= IN_ATTACK;
					pCmd->buttons &= ~IN_ATTACK2;
					WillShoot = true;
				}
			}
		}
	}

	//AntiAim
	QAngle ServerAngles(89, ClientAngles.yaw + 180, 0);
	ServerAngles.Normalize();
	if (!WillShoot && AntiAimToggledOn && !AimbotOverrideAntiAim)
		CRMove::SetServerClient(ServerAngles, pCmd);


	//CRMove::AimStep(&lastServerAngle, &pCmd->viewangles, AimstepDistance);
	lastServerAngle = pCmd->viewangles;
	lastServerAngle.Normalize();
	CRMove::SetServerClient(lastServerAngle, pCmd);

	lastServerAngle = pCmd->viewangles;

	bool silentAim = (CurrentGameMode != GAMEMODE_COMPETITIVE) ? Menu::CConfig.m_bSilentAimNonComp : Menu::CConfig.m_bSilentAim;
	return !silentAim;
}
