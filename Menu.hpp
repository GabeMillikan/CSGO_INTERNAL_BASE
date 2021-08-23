#pragma once
#include "Console.hpp"


namespace Menu
{
	struct Configuration
	{
		//use bhop?
		bool m_bBHOP = true;
		//autostrafe during bhop?
		bool m_bAutoStrafe = true;
		//use antiaim?
		bool m_bAntiAim = true;
		//use antiaim in non-competetive game?
		bool m_bAntiAimInNonComp = false;
		//use a seperate aimstep when in a non-competetive game?
		bool m_bAdjAimstepNonComp = true;
		//use aimbot?
		bool m_bAimbot = true;
		//only let aimbot shoot? pressing mouse1 activates aimbot, will not shoot
		bool m_bAimbotMouseLock = true;
		bool m_bAimbotMouseLockNonComp = false;
		//use wallhacks
		bool m_bWallHacks = true;

		//maximum number of degree rotation in a single tick
		int m_iAimstep = 999;
		//aimstep to use if in non-comp game and if m_bAdjAimstepNonComp
		int m_iAimstepNonComp = 3;

		//silent?
		bool m_bSilentAim = true;
		bool m_bSilentAimNonComp = false;

		//target teammates? good for battle royal -- DOES NOT WORK (yet)
		bool m_bShootTeammates = false;

	};

	extern Configuration CConfig;
}