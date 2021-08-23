#include "doPaintTraverse.hpp"
#include "Console.hpp"
#include "Menu.hpp"

int screenWidth, screenHeight = 0;

bool SDK::Hooks::PaintTrav::screen_transform(const Vector& in, Vector& out)
{
	// not static because if they change resolution
	auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

	out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
	out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
	out.z = 0.0f;

	float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

	if (w < 0.00001f) {
		out.x *= 10000000;
		out.y *= 10000000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

bool SDK::Hooks::PaintTrav::WorldToScreen(const Vector& in, Vector& out)
{
	if (!screenWidth || !screenHeight) return false;
	if (screen_transform(in, out)) {
		out.x = (screenWidth / 2.0f) + (out.x * screenWidth) / 2.0f;
		out.y = (screenHeight / 2.0f) - (out.y * screenHeight) / 2.0f;

		return true;
	}
	return false;
}

bool SDK::Hooks::PaintTrav::DrawHealth(C_BasePlayer* entity, int hp, unsigned long FONT)
{
	Vector Headpos = entity->GetHitboxPos(HITBOX_HEAD) + Vector(0,0,5);
	Vector Origin = Headpos - entity->GetHitboxPos(HITBOX_LEFT_FOOT);
	Origin.Init(Headpos.x, Headpos.y, Origin.z-2);

	Vector ScreenHead;
	Vector ScreenOrigin;
	if (!WorldToScreen(Headpos, ScreenHead) || !WorldToScreen(Origin, ScreenOrigin))
		return false;


	std::wstring strbuffer = std::to_wstring(hp);
	int tw, th;
	g_VGuiSurface->GetTextSize(FONT, strbuffer.c_str(), tw, th);
	g_VGuiSurface->DrawSetTextColor(255, 255, 255, 255);
	g_VGuiSurface->DrawSetTextPos(ScreenHead.x - tw/2, ScreenHead.y - th);
	g_VGuiSurface->DrawPrintText(strbuffer.c_str(), wcslen(strbuffer.c_str()));

	return true;
}

void SDK::Hooks::doPaintTraverse(void(__fastcall *ofunc)(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce),
	void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
{
	ofunc(_this, edx, panel, forceRepaint, allowForce);
	if (!Menu::CConfig.m_bWallHacks)
		return;
	
	//check function validity
	if (!g_EngineClient->IsInGame())
		return;
	C_BasePlayer* localPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!localPlayer) return;

	//setup font
	static auto FONT = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->DrawSetTextFont(FONT);
	g_VGuiSurface->SetFontGlyphSet(FONT, "Consolas", 20, 0, NULL, NULL, FONTFLAG_OUTLINE);

	//screen size
	if (!screenWidth || !screenHeight)
		g_EngineClient->GetScreenSize(screenWidth, screenHeight);
	if (!screenWidth || !screenHeight) return;

	//only draw on the screen overlay
	const char* panelName = g_VGuiPanel->GetName(panel);
	if (strcmp(panelName, "FocusOverlayPanel"))
		return;

	int MyTeam = localPlayer->m_iTeamNum();
	QAngle CurrentViewAngles = localPlayer->GetRenderAngles();
	Vector MyEyePosition = localPlayer->GetEyePos();



	Vector Screen1;
	Vector Screen2;

	for (int i = 0; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity)
			continue;
		if (entity == localPlayer)
			continue;
		if (i == g_EngineClient->GetLocalPlayer())
			continue;
		if (!entity->IsAlive())
			continue;
		if (entity->IsDormant())
			continue;
		int health = entity->m_iHealth();
		if (!(0 < health && health <= 100))
			continue;

		Vector entityPosition = entity->m_vecOrigin();

		QAngle AngleTo(MyEyePosition, entityPosition);

		QAngle turnAngle = CurrentViewAngles.NormalizedTo(AngleTo);

		if (fabs(turnAngle.pitch) > 45 || turnAngle.yaw > 90)
		{//if enemy offscreen
			//dont draw arrows to offscreen teammates
			if (entity->m_iTeamNum() != MyTeam)
				continue;

			//TODO: arrows to offscreen enemies
		}
		else
		{//if enemy onscreen
			//only draw enemy hp
			if (entity->m_iTeamNum() != MyTeam)
				PaintTrav::DrawHealth(entity, health, FONT);
		}

	}
	
}