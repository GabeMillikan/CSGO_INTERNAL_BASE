#include "doDrawModelExecute.hpp"
#include "HookManager.hpp"

#define MAXSTUDIOSKINS		32

#define TEXTURE_GROUP_LIGHTMAP						        "Lightmaps"
#define TEXTURE_GROUP_WORLD							          "World textures"
#define TEXTURE_GROUP_MODEL							          "Model textures"
#define TEXTURE_GROUP_VGUI							          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						        "Particle textures"
#define TEXTURE_GROUP_DECAL							          "Decal textures"
#define TEXTURE_GROUP_SKYBOX						          "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				      "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							          "Other textures"
#define TEXTURE_GROUP_PRECACHED						        "Precached"
#define TEXTURE_GROUP_CUBE_MAP						        "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					      "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					        "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			    "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			  "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			  "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					      "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					        "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					      "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				      "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			  "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					      "Morph Targets"



void SDK::Hooks::Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool transparent, bool glass, const Color& rgba)
{
	static IMaterial* materialRegular = g_MatSystem->FindMaterial("debug/debugambientcube");
	static IMaterial* materialFlat = g_MatSystem->FindMaterial("debug/debugdrawflat");
	static IMaterial* materialGlass = g_MatSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass");

	IMaterial* material = nullptr;

	if (flat) {
		material = materialFlat;
	}
	else if (glass)
	{
		material = materialGlass;
	}
	else {
		material = materialRegular;
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);


	if (transparent) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else {
		material->AlphaModulate(
			rgba.a() / 255.0f);
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(
		rgba.r() / 255.0f,
		rgba.g() / 255.0f,
		rgba.b() / 255.0f);

	g_MdlRender->ForcedMaterialOverride(material);
}

void SDK::Hooks::doDrawModelExecute(

	void(__fastcall *ofunc)(void* _this, int edx, IMatRenderContext * ctx, const DrawModelState_t &state, const ModelRenderInfo_t &pInfo, matrix3x4_t *pCustomBoneToWorld),

	void* _this,
	int edx,
	IMatRenderContext * ctx,
	const DrawModelState_t &state,
	const ModelRenderInfo_t &pInfo,
	matrix3x4_t *pCustomBoneToWorld
)
{
	const auto mdl = pInfo.pModel;

	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	bool is_flash = strstr(mdl->szName, "flash") != nullptr;

	if (is_player) {
		C_BasePlayer* local = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
		C_BasePlayer* ent = (C_BasePlayer*)g_EntityList->GetClientEntity(pInfo.entity_index);


		if (ent && local && ent->IsAlive()) {
			bool isEnemy = ent->m_iTeamNum() != local->m_iTeamNum();

			if (isEnemy)
			{
				Color color_blocked(200, 100, 100);
				Color color_visible(100, 200, 100);

				Chams::OverrideMaterial(
					true,	//viz thru wall?
					false,	//shade flat?
					false,	//wireframe?
					false,	//transparent?
					false,	//glass?
					color_blocked);

				ofunc(g_MdlRender, 0, ctx, state, pInfo, pCustomBoneToWorld);

				Chams::OverrideMaterial(
					false,	//viz thru wall?
					false,	//shade flat?
					false,	//wireframe?
					false,	//transparent?
					false,	//glass?
					color_visible);
			}
			else
			{
				Chams::OverrideMaterial(
					false,	//viz thru wall?
					true,	//shade flat?
					false,	//wireframe?
					false,	//transparent?
					false,	//glass?
					Color(50,50,200));
			}
		}
	}
	else if (is_arm)
	{
		auto material = g_MatSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);

		if (!material)
			return;
		if (/*have no hands*/ false)
		{
			// 
			// No hands.
			// 
			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			g_MdlRender->ForcedMaterialOverride(material);
		}
		else if (/* arm chams */true)
		{
			Chams::OverrideMaterial(
				false,
				false,
				false,
				false,
				true,
				Color(180,180,180)
			);
		}
	}
	else if (is_flash)
	{
		IMaterial* Flash = g_MatSystem->FindMaterial("effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
		IMaterial* FlashWhite = g_MatSystem->FindMaterial("effects\\flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);
		Flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		FlashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		g_MdlRender->ForcedMaterialOverride(Flash);
		g_MdlRender->ForcedMaterialOverride(FlashWhite);
	}
	else
	{
		//Con::Out(mdl->szName);
	}

}