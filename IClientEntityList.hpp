#pragma once


//increase it if valve added some funcs to baseentity
constexpr unsigned long long VALVE_ADDED_FUNCS = 0ull; //ull is probably overkill? a short would do probably

#include "SourceInterface.hpp"

#include "datamap.hpp"
#include "Recv.hpp"
#include "Enums.hpp"
#include "Vector.hpp"
#include "QAngle.hpp"
#include "VMatrix.hpp"
#include "Util.hpp"
#include "vfunc.hpp"
#include "Vector4D.hpp"
#include "UtlBuffer.hpp"
#include "IAppSystem.hpp"


#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

#define MAXSTUDIOBONES		128		// total bones actually used
#define BONE_USED_BY_HITBOX             0x00000100    // bone (or child) is used by a hit box

#define FRUSTUM_NUMPLANES    6
#define MAX_VIS_LEAVES    32
#define MAX_AREA_STATE_BYTES        32
#define MAX_AREA_PORTAL_STATE_BYTES 24

namespace SDK
{
	template< class T >
	class CHandle;
	class IHandleEntity;
	class CBaseHandle;
	class IClientUnknown;
	class ClientClass;
	class bf_read;
	class IClientNetworkable;
	class IClientRenderable;
	class CClientThinkHandlePtr;
	class IClientThinkable;
	class IClientEntity;
	class IClientEntityList;
	class C_BaseEntity;
	class C_BasePlayer;
	class C_EconItemView;
	class C_BaseAttributableItem;
	class C_BaseCombatWeapon;
	class IClientAlphaProperty;
	class ICollideable;
	class IWeaponSystem;
	class CCSWeaponInfo;
	class IMaterialVar;

	class CStudioHdr;
	struct virtualmodel_t;
	struct mstudiobone_t;
	struct mstudiobbox_t;
	struct mstudiohitboxset_t;
	struct studiohwdata_t;
	struct DrawModelInfo_t;
	struct MaterialLightingState_t;
	struct ColorMeshInfo_t;
	class studiohdr_t;
	class IVModelInfo;
	class IVModelInfoClient;
	class IMatRenderContext;
	class DataCacheHandle_t;
	class ITexture;

	typedef float Quaternion[4];
	typedef float RadianEuler[3];
	typedef unsigned short MDLHandle_t;


	typedef unsigned short ModelInstanceHandle_t;
	typedef void* LightCacheHandle_t;
	typedef void* StudioDecalHandle_t;
	typedef int OverrideType_t;
	typedef int ImageFormat;
	typedef int VertexFormat_t;
	typedef int MaterialPropertyTypes_t;

	typedef unsigned short LeafIndex_t;

	class IHandleEntity
	{
	public:
		virtual ~IHandleEntity() {}
		virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
		virtual const CBaseHandle& GetRefEHandle() const = 0;
	};

	class CBaseHandle
	{
	public:
		CBaseHandle();
		CBaseHandle(const CBaseHandle &other);
		CBaseHandle(unsigned long value);
		CBaseHandle(int iEntry, int iSerialNumber);

		void Init(int iEntry, int iSerialNumber);
		void Term();

		// Even if this returns true, Get() still can return return a non-null value.
		// This just tells if the handle has been initted with any values.
		bool IsValid() const;

		int GetEntryIndex() const;
		int GetSerialNumber() const;

		int ToInt() const;
		bool operator !=(const CBaseHandle &other) const;
		bool operator ==(const CBaseHandle &other) const;
		bool operator ==(const IHandleEntity* pEnt) const;
		bool operator !=(const IHandleEntity* pEnt) const;
		bool operator <(const CBaseHandle &other) const;
		bool operator <(const IHandleEntity* pEnt) const;

		// Assign a value to the handle.
		const CBaseHandle& operator=(const IHandleEntity *pEntity);
		const CBaseHandle& Set(const IHandleEntity *pEntity);

		// Use this to dereference the handle.
		// Note: this is implemented in game code (ehandle.h)
		// just use g_EntityList->GetClientEntityFromHandle(*this)
		// IHandleEntity* Get() const;

	protected:
		// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
		// The high NUM_SERIAL_NUM_BITS bits are the serial number.
		unsigned long	m_Index;
	};

	inline CBaseHandle::CBaseHandle()
	{
		m_Index = INVALID_EHANDLE_INDEX;
	}

	inline CBaseHandle::CBaseHandle(const CBaseHandle &other)
	{
		m_Index = other.m_Index;
	}

	inline CBaseHandle::CBaseHandle(unsigned long value)
	{
		m_Index = value;
	}

	inline CBaseHandle::CBaseHandle(int iEntry, int iSerialNumber)
	{
		Init(iEntry, iSerialNumber);
	}

	inline void CBaseHandle::Init(int iEntry, int iSerialNumber)
	{
		m_Index = iEntry | (iSerialNumber << NUM_ENT_ENTRY_BITS);
	}

	inline void CBaseHandle::Term()
	{
		m_Index = INVALID_EHANDLE_INDEX;
	}

	inline bool CBaseHandle::IsValid() const
	{
		return m_Index != INVALID_EHANDLE_INDEX;
	}

	inline int CBaseHandle::GetEntryIndex() const
	{
		return m_Index & ENT_ENTRY_MASK;
	}

	inline int CBaseHandle::GetSerialNumber() const
	{
		return m_Index >> NUM_ENT_ENTRY_BITS;
	}

	inline int CBaseHandle::ToInt() const
	{
		return (int)m_Index;
	}

	inline bool CBaseHandle::operator !=(const CBaseHandle &other) const
	{
		return m_Index != other.m_Index;
	}

	inline bool CBaseHandle::operator ==(const CBaseHandle &other) const
	{
		return m_Index == other.m_Index;
	}

	/* do it yourself, retard
	inline bool CBaseHandle::operator ==(const IHandleEntity* pEnt) const
	{
		return Get() == pEnt;
	}

	inline bool CBaseHandle::operator !=(const IHandleEntity* pEnt) const
	{
		return Get() != pEnt;
	}
	*/

	inline bool CBaseHandle::operator <(const CBaseHandle &other) const
	{
		return m_Index < other.m_Index;
	}

	inline bool CBaseHandle::operator <(const IHandleEntity *pEntity) const
	{
		unsigned long otherIndex = (pEntity) ? pEntity->GetRefEHandle().m_Index : INVALID_EHANDLE_INDEX;
		return m_Index < otherIndex;
	}

	inline const CBaseHandle& CBaseHandle::operator=(const IHandleEntity *pEntity)
	{
		return Set(pEntity);
	}

	inline const CBaseHandle& CBaseHandle::Set(const IHandleEntity *pEntity)
	{
		if (pEntity) {
			*this = pEntity->GetRefEHandle();
		}
		else {
			m_Index = INVALID_EHANDLE_INDEX;
		}

		return *this;
	}

	template< class T >
	class CHandle : public CBaseHandle
	{
	public:

		CHandle();
		CHandle(int iEntry, int iSerialNumber);
		CHandle(const CBaseHandle &handle);
		CHandle(T *pVal);

		// The index should have come from a call to ToInt(). If it hasn't, you're in trouble.
		static CHandle<T> FromIndex(int index);

		//T*		Get() const;
		void	Set(const T* pVal);

		operator T*();
		operator T*() const;

		bool	operator !() const;
		bool	operator==(T *val) const;
		bool	operator!=(T *val) const;
		const CBaseHandle& operator=(const T *val);

		T*		operator->() const;
	};
	template<class T>
	CHandle<T>::CHandle()
	{
	}


	template<class T>
	CHandle<T>::CHandle(int iEntry, int iSerialNumber)
	{
		Init(iEntry, iSerialNumber);
	}


	template<class T>
	CHandle<T>::CHandle(const CBaseHandle &handle)
		: CBaseHandle(handle)
	{
	}


	template<class T>
	CHandle<T>::CHandle(T *pObj)
	{
		Term();
		Set(pObj);
	}


	template<class T>
	inline CHandle<T> CHandle<T>::FromIndex(int index)
	{
		CHandle<T> ret;
		ret.m_Index = index;
		return ret;
	}

	/*
	template<class T>
	inline T* CHandle<T>::Get() const
	{
		return (T*)CBaseHandle::Get();
	}
	*/


	template<class T>
	inline CHandle<T>::operator T *()
	{
		return Get();
	}

	template<class T>
	inline CHandle<T>::operator T *() const
	{
		return Get();
	}


	template<class T>
	inline bool CHandle<T>::operator !() const
	{
		return !Get();
	}

	template<class T>
	inline bool CHandle<T>::operator==(T *val) const
	{
		return Get() == val;
	}

	template<class T>
	inline bool CHandle<T>::operator!=(T *val) const
	{
		return Get() != val;
	}

	template<class T>
	void CHandle<T>::Set(const T* pVal)
	{
		CBaseHandle::Set(reinterpret_cast<const IHandleEntity*>(pVal));
	}

	template<class T>
	inline const CBaseHandle& CHandle<T>::operator=(const T *val)
	{
		Set(val);
		return *this;
	}

	template<class T>
	T* CHandle<T>::operator -> () const
	{
		return (T*)g_EntityList->GetClientEntityFromHandle(*this);
	}


	class IClientUnknown : public IHandleEntity
	{
	public:
		virtual ICollideable*              GetCollideable() = 0;
		virtual IClientNetworkable*        GetClientNetworkable() = 0;
		virtual IClientRenderable*         GetClientRenderable() = 0;
		virtual IClientEntity*             GetIClientEntity() = 0;
		virtual C_BaseEntity*              GetBaseEntity() = 0;
		virtual IClientThinkable*          GetClientThinkable() = 0;
		//virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
		virtual IClientAlphaProperty*      GetClientAlphaProperty() = 0;
	};


	class IClientNetworkable
	{
	public:
		virtual IClientUnknown*  GetIClientUnknown() = 0;
		virtual void             Release() = 0;
		virtual ClientClass*     GetClientClass() = 0;
		virtual void             NotifyShouldTransmit(int state) = 0;
		virtual void             OnPreDataChanged(int updateType) = 0;
		virtual void             OnDataChanged(int updateType) = 0;
		virtual void             PreDataUpdate(int updateType) = 0;
		virtual void             PostDataUpdate(int updateType) = 0;
		virtual void             __unkn(void) = 0;
		virtual bool             IsDormant(void) = 0;
		virtual int              EntIndex(void) const = 0;
		virtual void             ReceiveMessage(int classID, bf_read& msg) = 0;
		virtual void*            GetDataTableBasePtr() = 0;
		virtual void             SetDestroyedOnRecreateEntities(void) = 0;
	};

	typedef SDK::IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
	typedef SDK::IClientNetworkable* (*CreateEventFn)();
	class ClientClass
	{
	public:
		CreateClientClassFn      m_pCreateFn;
		CreateEventFn            m_pCreateEventFn;
		char*                    m_pNetworkName;
		RecvTable*               m_pRecvTable;
		ClientClass*             m_pNext;
		ClassId                  m_ClassID;
	};

	typedef unsigned short ClientShadowHandle_t;
	typedef unsigned short ClientRenderHandle_t;
	typedef unsigned short ModelInstanceHandle_t;
	typedef unsigned char uint8_t;

	struct model_t
	{
		void*   fnHandle;               //0x0000 
		char    szName[260];            //0x0004 
		__int32 nLoadFlags;             //0x0108 
		__int32 nServerCount;           //0x010C 
		__int32 type;                   //0x0110 
		__int32 flags;                  //0x0114 
		Vector  vecMins;                //0x0118 
		Vector  vecMaxs;                //0x0124 
		float   radius;                 //0x0130 
		char    pad[0x1C];              //0x0134
	};//Size=0x0150

	class IClientRenderable
	{
	public:
		virtual IClientUnknown*            GetIClientUnknown() = 0;
		virtual Vector const&              GetRenderOrigin(void) = 0;
		virtual QAngle const&              GetRenderAngles(void) = 0;
		virtual bool                       ShouldDraw(void) = 0;
		virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
		virtual void                       Unused(void) const {}
		virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
		virtual ClientRenderHandle_t&      RenderHandle() = 0;
		virtual const model_t*             GetModel() const = 0;
		virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/ &instance) = 0;
		virtual int                        GetBody() = 0;
		virtual void                       GetColorModulation(float* color) = 0;
		virtual bool                       LODTest() = 0;
		virtual bool                       SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
		virtual void                       SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights) = 0;
		virtual void                       DoAnimationEvents(void) = 0;
		virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
		virtual void                       GetRenderBounds(Vector& mins, Vector& maxs) = 0;
		virtual void                       GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
		virtual void                       GetShadowRenderBounds(Vector &mins, Vector &maxs, int /*ShadowType_t*/ shadowType) = 0;
		virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
		virtual bool                       GetShadowCastDistance(float *pDist, int /*ShadowType_t*/ shadowType) const = 0;
		virtual bool                       GetShadowCastDirection(Vector *pDirection, int /*ShadowType_t*/ shadowType) const = 0;
		virtual bool                       IsShadowDirty() = 0;
		virtual void                       MarkShadowDirty(bool bDirty) = 0;
		virtual IClientRenderable*         GetShadowParent() = 0;
		virtual IClientRenderable*         FirstShadowChild() = 0;
		virtual IClientRenderable*         NextShadowPeer() = 0;
		virtual int /*ShadowType_t*/       ShadowCastType() = 0;
		virtual void                       CreateModelInstance() = 0;
		virtual ModelInstanceHandle_t      GetModelInstance() = 0;
		virtual const matrix3x4_t&         RenderableToWorldTransform() = 0;
		virtual int                        LookupAttachment(const char *pAttachmentName) = 0;
		virtual   bool                     GetAttachment(int number, Vector &origin, QAngle &angles) = 0;
		virtual bool                       GetAttachment(int number, matrix3x4_t &matrix) = 0;
		virtual float*                     GetRenderClipPlane(void) = 0;
		virtual int                        GetSkin() = 0;
		virtual void                       OnThreadedDrawSetup() = 0;
		virtual bool                       UsesFlexDelayedWeights() = 0;
		virtual void                       RecordToolMessage() = 0;
		virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
		virtual uint8_t                      OverrideAlphaModulation(uint8_t nAlpha) = 0;
		virtual uint8_t                      OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
	};

	class CClientThinkHandlePtr;
	typedef CClientThinkHandlePtr* ClientThinkHandle_t;

	class IClientThinkable
	{
	public:
		virtual IClientUnknown*     GetIClientUnknown() = 0;
		virtual void                ClientThink() = 0;
		virtual ClientThinkHandle_t GetThinkHandle() = 0;
		virtual void                SetThinkHandle(ClientThinkHandle_t hThink) = 0;
		virtual void                Release() = 0;
	};

	class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
	{
	public:
		virtual void Release(void) = 0;
	};

	class IClientEntityList
	{
	public:
		virtual IClientNetworkable*   GetClientNetworkable(int entnum) = 0;
		virtual void*                 vtablepad0x1(void) = 0;
		virtual void*                 vtablepad0x2(void) = 0;
		virtual IClientEntity*        GetClientEntity(int entNum) = 0;
		virtual IClientEntity*        GetClientEntityFromHandle(CBaseHandle hEnt) = 0;
		virtual int                   NumberOfEntities(bool bIncludeNonNetworkable) = 0;
		virtual int                   GetHighestEntityIndex(void) = 0;
		virtual void                  SetMaxEntities(int maxEnts) = 0;
		virtual int                   GetMaxEntities() = 0;
	};

	class C_BaseEntity : public IClientEntity
	{
	public:
		datamap_t * GetDataDescMap() {
			typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
			return CallVFunction<o_GetPredDescMap>(this, 15)(this);
		}

		datamap_t *GetPredDescMap() {
			typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
			return CallVFunction<o_GetPredDescMap>(this, 17)(this);
		}

		NETVAR(int32_t, m_nModelIndex, "DT_BaseEntity", "m_nModelIndex");
		NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
		NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
		NETVAR(Vector, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
		NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
		NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
		NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
		NETVAR(float_t, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");


		const matrix3x4_t& m_rgflCoordinateFrame()
		{
			static auto _m_rgflCoordinateFrame = NetvarSys::Get().GetOffset("DT_BaseEntity", "m_CollisionGroup") - 0x30;
			return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
		}

		inline bool C_BaseEntity::IsPlayer()
		{
			//index: 152
			//ref: "effects/nightvision"
			//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
			return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 157 + VALVE_ADDED_FUNCS)(this);
		}
		bool IsLoot();
		bool IsWeapon();
		bool IsPlantedC4();
		inline bool C_BaseEntity::IsC4()
		{
			return GetClientClass()->m_ClassID == ClassId_CC4;
		}
		bool IsDefuseKit();
		//bool isSpotted();
	};


	class C_BasePlayer : public C_BaseEntity
	{
	public:

		NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
		NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
		NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
		NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
		NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
		NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
		NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
		NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");;
		NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
		NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
		NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
		NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");
		NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
		NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
		NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
		NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
		//NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
		NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
		NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");
		NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
		NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
		NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
		NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
		NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
		NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
		NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
		NETVAR(float, m_flCycle, "DT_BaseAnimating", "m_flCycle");
		NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");
		NETVAR(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");

		NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
		NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
		NETVAR(float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed");
		NETVAR(float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount");

		inline QAngle * GetVAngles()
		{
			static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
			return (QAngle*)((uintptr_t)this + deadflag + 0x4);
		}


		inline Vector GetEyePos()
		{
			return m_vecOrigin() + m_vecViewOffset();
		}
		inline bool IsAlive()
		{
			return m_lifeState() == LIFE_ALIVE;
		}

		inline Vector GetHitboxPos(int hitbox_id);


		//add more to this class as it becomes necessary
	};

	class C_EconItemView
	{
	private:
		using str_32 = char[32];
	public:
		NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
		NETVAR(short, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
		NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
		NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
		NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
		NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
		NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
		NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");
	};

	class C_BaseAttributableItem : public C_BaseEntity
	{
	public:
		NETVAR(uint64_t, m_OriginalOwnerXuid, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
		NETVAR(int32_t, m_OriginalOwnerXuidLow, "DT_BaseAttributableItem", "m_OriginalOwnerXuidLow");
		NETVAR(int32_t, m_OriginalOwnerXuidHigh, "DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh");
		NETVAR(int32_t, m_nFallbackStatTrak, "DT_BaseAttributableItem", "m_nFallbackStatTrak");
		NETVAR(int32_t, m_nFallbackPaintKit, "DT_BaseAttributableItem", "m_nFallbackPaintKit");
		NETVAR(int32_t, m_nFallbackSeed, "DT_BaseAttributableItem", "m_nFallbackSeed");
		NETVAR(float_t, m_flFallbackWear, "DT_BaseAttributableItem", "m_flFallbackWear");

		//NETVAR(C_EconItemView, m_Item2, "DT_BaseAttributableItem", "m_Item");

		C_EconItemView& m_Item()
		{
			// Cheating. It should be this + m_Item netvar but then the netvars inside C_EconItemView wont work properly.
			// A real fix for this requires a rewrite of the netvar manager
			return *(C_EconItemView*)this;
		}

		void SetGloveModelIndex(int modelIndex);

	};

	class IWeaponSystem
	{
		virtual void unused0() = 0;
		virtual void unused1() = 0;
	public:
		virtual CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
	};

	class CCSWeaponInfo
	{
	public:
		int8_t pad0[20];
		int32_t iMaxClip1;
		int8_t pad1[12];
		int32_t iMaxReservedAmmo;
		int8_t pad2[96];
		char* szHudName;
		char* szWeaponName;
		int8_t pad3[56];
		int32_t iWeaponType;
		int8_t pad4[4];
		int32_t iWeaponPrice;
		int32_t iKillAward;
		int8_t pad5[20];
		uint8_t bFullAuto;
		int8_t pad6[3];
		int32_t iDamage;
		float_t flArmorRatio;
		int32_t iBullets;
		float_t flPenetration;
		int8_t pad7[8];
		float_t flRange;
		float_t flRangeModifier;
		int8_t pad8[16];
		uint8_t bHasSilencer;
		int8_t pad9[15];
		float_t flSpread;
		float_t flSpreadAlt;
		int8_t pad10[76];
		int32_t iRecoilSeed;
		int8_t pad11[32];
	};

	class C_BaseCombatWeapon : public C_BaseAttributableItem
	{
	public:
		NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
		NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
		NETVAR(int32_t, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
		NETVAR(int32_t, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
		NETVAR(float_t, m_flRecoilIndex, "DT_WeaponCSBase", "m_flRecoilIndex");
		NETVAR(int32_t, m_iViewModelIndex, "DT_BaseCombatWeapon", "m_iViewModelIndex");
		NETVAR(int32_t, m_iWorldModelIndex, "DT_BaseCombatWeapon", "m_iWorldModelIndex");
		NETVAR(int32_t, m_iWorldDroppedModelIndex, "DT_BaseCombatWeapon", "m_iWorldDroppedModelIndex");
		NETVAR(bool, m_bPinPulled, "DT_BaseCSGrenade", "m_bPinPulled");
		NETVAR(float_t, m_fThrowTime, "DT_BaseCSGrenade", "m_fThrowTime");
		NETVAR(float_t, m_flPostponeFireReadyTime, "DT_BaseCombatWeapon", "m_flPostponeFireReadyTime");
		//NETVAR(CHandle<C_BaseWeaponWorldModel>, m_hWeaponWorldModel, "DT_BaseCombatWeapon", "m_hWeaponWorldModel");


		CCSWeaponInfo* GetCSWeaponData();
		inline bool IsGrenade()
		{
			return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
		}
		inline bool IsKnife()
		{
			if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
			return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
		}
		inline bool IsReloading()
		{
			static auto inReload = *(uint32_t*)(Util::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
			return *(bool*)((uintptr_t)this + inReload);
		}
		bool IsRifle();
		bool IsPistol();
		bool IsSniper();
		inline bool IsGun()
		{
			switch (GetCSWeaponData()->iWeaponType)
			{
			case WEAPONTYPE_C4:
				return false;
			case WEAPONTYPE_GRENADE:
				return false;
			case WEAPONTYPE_KNIFE:
				return false;
			case WEAPONTYPE_UNKNOWN:
				return false;
			default:
				return true;
			}
		}
		inline float GetInaccuracy()
		{
			return CallVFunction<float(__thiscall*)(void*)>(this, 478 + VALVE_ADDED_FUNCS)(this);
		}
		float GetSpread();
		void UpdateAccuracyPenalty();
		bool* m_bCustomMaterialInitialized();

	};
}


#pragma region MASKS

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

#define   CONTENTS_EMPTY                0

#define   CONTENTS_SOLID                0x1       
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_AUX                  0x4
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_SLIME                0x10
#define   CONTENTS_WATER                0x20
#define   CONTENTS_BLOCKLOS             0x40 
#define   CONTENTS_OPAQUE               0x80 
#define   LAST_VISIBLE_CONTENTS         CONTENTS_OPAQUE

#define   ALL_VISIBLE_CONTENTS            (LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))

#define   CONTENTS_TESTFOGVOLUME        0x100
#define   CONTENTS_UNUSED               0x200     
#define   CONTENTS_BLOCKLIGHT           0x400
#define   CONTENTS_TEAM1                0x800 
#define   CONTENTS_TEAM2                0x1000 
#define   CONTENTS_IGNORE_NODRAW_OPAQUE 0x2000
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_AREAPORTAL           0x8000
#define   CONTENTS_PLAYERCLIP           0x10000
#define   CONTENTS_MONSTERCLIP          0x20000
#define   CONTENTS_CURRENT_0            0x40000
#define   CONTENTS_CURRENT_90           0x80000
#define   CONTENTS_CURRENT_180          0x100000
#define   CONTENTS_CURRENT_270          0x200000
#define   CONTENTS_CURRENT_UP           0x400000
#define   CONTENTS_CURRENT_DOWN         0x800000

#define   CONTENTS_ORIGIN               0x1000000 

#define   CONTENTS_MONSTER              0x2000000 
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_DETAIL               0x8000000 
#define   CONTENTS_TRANSLUCENT          0x10000000
#define   CONTENTS_LADDER               0x20000000
#define   CONTENTS_HITBOX               0x40000000

#define   SURF_LIGHT                    0x0001 
#define   SURF_SKY2D                    0x0002 
#define   SURF_SKY                      0x0004 
#define   SURF_WARP                     0x0008 
#define   SURF_TRANS                    0x0010
#define   SURF_NOPORTAL                 0x0020 
#define   SURF_TRIGGER                  0x0040 
#define   SURF_NODRAW                   0x0080 

#define   SURF_HINT                     0x0100 

#define   SURF_SKIP                     0x0200   
#define   SURF_NOLIGHT                  0x0400   
#define   SURF_BUMPLIGHT                0x0800   
#define   SURF_NOSHADOWS                0x1000   
#define   SURF_NODECALS                 0x2000   
#define   SURF_NOPAINT                  SURF_NODECALS
#define   SURF_NOCHOP                   0x4000   
#define   SURF_HITBOX                   0x8000   

// -----------------------------------------------------
// spatial content masks - used for spatial queries (traceline,etc.)
// -----------------------------------------------------
#define   MASK_ALL                      (0xFFFFFFFF)
#define   MASK_SOLID                    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCSOLID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
#define   MASK_NPCFLUID                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_WATER                    (CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME)
#define   MASK_OPAQUE                   (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE)
#define   MASK_OPAQUE_AND_NPCS          (MASK_OPAQUE|CONTENTS_MONSTER)
#define   MASK_BLOCKLOS                 (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_BLOCKLOS)
#define   MASK_BLOCKLOS_AND_NPCS        (MASK_BLOCKLOS|CONTENTS_MONSTER)
#define   MASK_VISIBLE                  (MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_VISIBLE_AND_NPCS         (MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE)
#define   MASK_SHOT                     (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)
#define   MASK_SHOT_BRUSHONLY           (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS)
#define   MASK_SHOT_HULL                (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE)
#define   MASK_SHOT_PORTAL              (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER)
#define   MASK_SOLID_BRUSHONLY          (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE)
#define   MASK_PLAYERSOLID_BRUSHONLY    (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE)
#define   MASK_NPCSOLID_BRUSHONLY       (CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC           (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE)
#define   MASK_NPCWORLDSTATIC_FLUID     (CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP)
#define   MASK_SPLITAREAPORTAL          (CONTENTS_WATER|CONTENTS_SLIME)
#define   MASK_CURRENT                  (CONTENTS_CURRENT_0|CONTENTS_CURRENT_90|CONTENTS_CURRENT_180|CONTENTS_CURRENT_270|CONTENTS_CURRENT_UP|CONTENTS_CURRENT_DOWN)
#define   MASK_DEADSOLID                (CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_GRATE)
#pragma endregion

namespace SDK
{
	class IHandleEntity;
	struct Ray_t;
	class CGameTrace;
	typedef CGameTrace trace_t;
	class ICollideable;
	class ITraceListData;
	class CPhysCollide;
	struct cplane_t;
	struct virtualmeshlist_t;

	enum class TraceType
	{
		TRACE_EVERYTHING = 0,
		TRACE_WORLD_ONLY,
		TRACE_ENTITIES_ONLY,
		TRACE_EVERYTHING_FILTER_PROPS,
	};

	class ITraceFilter
	{
	public:
		virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask) = 0;
		virtual TraceType GetTraceType() const = 0;
	};


	//-----------------------------------------------------------------------------
	// Classes are expected to inherit these + implement the ShouldHitEntity method
	//-----------------------------------------------------------------------------

	// This is the one most normal traces will inherit from
	class CTraceFilter : public ITraceFilter
	{
	public:
		bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
		{
			return !(pEntityHandle == pSkip);
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_EVERYTHING;
		}
		void* pSkip;
	};

	class CTraceFilterSkipEntity : public ITraceFilter
	{
	public:
		CTraceFilterSkipEntity(IHandleEntity* pEntityHandle)
		{
			pSkip = pEntityHandle;
		}

		bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
		{
			return !(pEntityHandle == pSkip);
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_EVERYTHING;
		}
		void* pSkip;
	};

	class CTraceFilterEntitiesOnly : public ITraceFilter
	{
	public:
		bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
		{
			return true;
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_ENTITIES_ONLY;
		}
	};


	//-----------------------------------------------------------------------------
	// Classes need not inherit from these
	//-----------------------------------------------------------------------------
	class CTraceFilterWorldOnly : public ITraceFilter
	{
	public:
		bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
		{
			return false;
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_WORLD_ONLY;
		}
	};

	class CTraceFilterWorldAndPropsOnly : public ITraceFilter
	{
	public:
		bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
		{
			return false;
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_EVERYTHING;
		}
	};

	class CTraceFilterPlayersOnlySkipOne : public ITraceFilter
	{
	public:
		CTraceFilterPlayersOnlySkipOne(IClientEntity* ent)
		{
			pEnt = ent;
		}
		bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
		{
			return pEntityHandle != pEnt && ((IClientEntity*)pEntityHandle)->GetClientClass()->m_ClassID == ClassId_CCSPlayer;
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_ENTITIES_ONLY;
		}

	private:
		IClientEntity* pEnt;
	};

	class CTraceFilterSkipTwoEntities : public ITraceFilter
	{
	public:
		CTraceFilterSkipTwoEntities(IClientEntity* ent1, IClientEntity* ent2)
		{
			pEnt1 = ent1;
			pEnt2 = ent2;
		}
		bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
		{
			return !(pEntityHandle == pEnt1 || pEntityHandle == pEnt2);
		}
		virtual TraceType GetTraceType() const
		{
			return TraceType::TRACE_EVERYTHING;
		}

	private:
		IClientEntity* pEnt1;
		IClientEntity* pEnt2;
	};

	class CTraceFilterHitAll : public CTraceFilter
	{
	public:
		virtual bool ShouldHitEntity(IHandleEntity* /*pServerEntity*/, int /*contentsMask*/)
		{
			return true;
		}
	};


	enum class DebugTraceCounterBehavior_t
	{
		kTRACE_COUNTER_SET = 0,
		kTRACE_COUNTER_INC,
	};

	//-----------------------------------------------------------------------------
	// Enumeration interface for EnumerateLinkEntities
	//-----------------------------------------------------------------------------
	class IEntityEnumerator
	{
	public:
		// This gets called with each handle
		virtual bool EnumEntity(IHandleEntity *pHandleEntity) = 0;
	};


	struct BrushSideInfo_t
	{
		Vector4D plane;               // The plane of the brush side
		unsigned short bevel;    // Bevel plane?
		unsigned short thin;     // Thin?
	};

	class CPhysCollide;

	struct vcollide_t
	{
		unsigned short solidCount : 15;
		unsigned short isPacked : 1;
		unsigned short descSize;
		// VPhysicsSolids
		CPhysCollide   **solids;
		char           *pKeyValues;
		void           *pUserData;
	};

	struct cmodel_t
	{
		Vector         mins, maxs;
		Vector         origin;        // for sounds or lights
		int            headnode;
		vcollide_t     vcollisionData;
	};

	struct csurface_t
	{
		const char     *name;
		short          surfaceProps;
		unsigned short flags;         // BUGBUG: These are declared per surface, not per material, but this database is per-material now
	};

	//-----------------------------------------------------------------------------
	// A ray...
	//-----------------------------------------------------------------------------
	struct Ray_t
	{
		VectorAligned  m_Start;  // starting point, centered within the extents
		VectorAligned  m_Delta;  // direction + length of the ray
		VectorAligned  m_StartOffset; // Add this to m_Start to Get the actual ray start
		VectorAligned  m_Extents;     // Describes an axis aligned box extruded along a ray
		const matrix3x4_t *m_pWorldAxisTransform;
		bool m_IsRay;  // are the extents zero?
		bool m_IsSwept;     // is delta != 0?

		Ray_t() : m_pWorldAxisTransform(NULL) {}

		void Init(Vector const& start, Vector const& end)
		{
			m_Delta = end - start;

			m_IsSwept = (m_Delta.LengthSqr() != 0);

			m_Extents.Init();

			m_pWorldAxisTransform = NULL;
			m_IsRay = true;

			// Offset m_Start to be in the center of the box...
			m_StartOffset.Init();
			m_Start = start;
		}

		void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
		{
			m_Delta = end - start;

			m_pWorldAxisTransform = NULL;
			m_IsSwept = (m_Delta.LengthSqr() != 0);

			m_Extents = maxs - mins;
			m_Extents *= 0.5f;
			m_IsRay = (m_Extents.LengthSqr() < 1e-6);

			// Offset m_Start to be in the center of the box...
			m_StartOffset = maxs + mins;
			m_StartOffset *= 0.5f;
			m_Start = start + m_StartOffset;
			m_StartOffset *= -1.0f;
		}
		Vector InvDelta() const
		{
			Vector vecInvDelta;
			for (int iAxis = 0; iAxis < 3; ++iAxis) {
				if (m_Delta[iAxis] != 0.0f) {
					vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
				}
				else {
					vecInvDelta[iAxis] = FLT_MAX;
				}
			}
			return vecInvDelta;
		}

	private:
	};

	class CBaseTrace
	{
	public:
		bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
		bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
		bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
		bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
		bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

	public:

		// these members are aligned!!
		Vector         startpos;            // start position
		Vector         endpos;              // final position
		cplane_t       plane;               // surface normal at impact

		float          fraction;            // time completed, 1.0 = didn't hit anything

		int            contents;            // contents on other side of surface hit
		unsigned short dispFlags;           // displacement flags for marking surfaces with data

		bool           allsolid;            // if true, plane is not valid
		bool           startsolid;          // if true, the initial point was in a solid area

		CBaseTrace() {}

	};

	class CGameTrace : public CBaseTrace
	{
	public:
		bool DidHitWorld() const;
		bool DidHitNonWorldEntity() const;
		int GetEntityIndex() const;
		bool DidHit() const;
		bool IsVisible() const;

	public:

		float               fractionleftsolid;  // time we left a solid, only valid if we started in solid
		csurface_t          surface;            // surface hit (impact surface)
		int                 hitgroup;           // 0 == generic, non-zero is specific body part
		short               physicsbone;        // physics bone hit by trace in studio
		unsigned short      worldSurfaceIndex;  // Index of the msurface2_t, if applicable
		IClientEntity*      hit_entity;
		int                 hitbox;                       // box hit by trace in studio

		CGameTrace() {}

		// No copy constructors allowed
		CGameTrace(const CGameTrace& other) :
			fractionleftsolid(other.fractionleftsolid),
			surface(other.surface),
			hitgroup(other.hitgroup),
			physicsbone(other.physicsbone),
			worldSurfaceIndex(other.worldSurfaceIndex),
			hit_entity(other.hit_entity),
			hitbox(other.hitbox)
		{
			startpos = other.startpos;
			endpos = other.endpos;
			plane = other.plane;
			fraction = other.fraction;
			contents = other.contents;
			dispFlags = other.dispFlags;
			allsolid = other.allsolid;
			startsolid = other.startsolid;
		}

		CGameTrace& CGameTrace::operator=(const CGameTrace& other)
		{
			startpos = other.startpos;
			endpos = other.endpos;
			plane = other.plane;
			fraction = other.fraction;
			contents = other.contents;
			dispFlags = other.dispFlags;
			allsolid = other.allsolid;
			startsolid = other.startsolid;
			fractionleftsolid = other.fractionleftsolid;
			surface = other.surface;
			hitgroup = other.hitgroup;
			physicsbone = other.physicsbone;
			worldSurfaceIndex = other.worldSurfaceIndex;
			hit_entity = other.hit_entity;
			hitbox = other.hitbox;
			return *this;
		}
	};


	inline bool CGameTrace::DidHit() const
	{
		return fraction < 1 || allsolid || startsolid;
	}

	inline bool CGameTrace::IsVisible() const
	{
		return fraction > 0.97f;
	}

	class IEngineTrace
	{
	public:
		virtual int   GetPointContents(const Vector &vecAbsPosition, int contentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
		virtual int   GetPointContents_WorldOnly(const Vector &vecAbsPosition, int contentsMask = MASK_ALL) = 0;
		virtual int   GetPointContents_Collideable(ICollideable *pCollide, const Vector &vecAbsPosition) = 0;
		virtual void  ClipRayToEntity(const Ray_t &ray, unsigned int fMask, IHandleEntity *pEnt, CGameTrace *pTrace) = 0;
		virtual void  ClipRayToCollideable(const Ray_t &ray, unsigned int fMask, ICollideable *pCollide, CGameTrace *pTrace) = 0;
		virtual void  TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, CGameTrace *pTrace) = 0;
	};

	struct mstudiobbox_t
	{
		int         bone;
		int         group;
		Vector      bbmin;
		Vector      bbmax;
		int         szhitboxnameindex;
		int32_t     m_iPad01[3];
		float       m_flRadius;
		int32_t     m_iPad02[4];

		const char* GetName()
		{
			if (!szhitboxnameindex) return nullptr;
			return (const char*)((uint8_t*)this + szhitboxnameindex);
		}
	};

	struct mstudiohitboxset_t
	{
		int    sznameindex;
		int    numhitboxes;
		int    hitboxindex;

		const char* GetName()
		{
			if (!sznameindex) return nullptr;
			return (const char*)((uint8_t*)this + sznameindex);
		}

		mstudiobbox_t* GetHitbox(int i)
		{
			if (i > numhitboxes) return nullptr;
			return (mstudiobbox_t*)((uint8_t*)this + hitboxindex) + i;
		}
	};

	struct mstudiobone_t
	{
		int                    sznameindex;
		inline char * const    pszName(void) const { return ((char *)this) + sznameindex; }
		int                    parent;
		int                    bonecontroller[6];    // bone controller index, -1 == none
		Vector                 pos;
		Quaternion             quat;
		RadianEuler            rot;
		// compression scale
		Vector                 posscale;
		Vector                 rotscale;

		matrix3x4_t            poseToBone;
		Quaternion             qAlignment;
		int                    flags;
		int                    proctype;
		int                    procindex;
		mutable int            physicsbone;
		inline void *          pProcedure() const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
		int                    surfacepropidx;
		inline char * const    pszSurfaceProp(void) const { return ((char *)this) + surfacepropidx; }
		inline int             GetSurfaceProp(void) const { return surfacepropLookup; }

		int                    contents;
		int                    surfacepropLookup;
		int                    m_iPad01[7];

		mstudiobone_t() {}
	private:
		// No copy constructors allowed
		mstudiobone_t(const mstudiobone_t& vOther);
	};

	class studiohdr_t
	{
	public:
		__int32 id;                     //0x0000 
		__int32 version;                //0x0004 
		long    checksum;               //0x0008 
		char    szName[64];             //0x000C 
		__int32 length;                 //0x004C 
		Vector  vecEyePos;              //0x0050 
		Vector  vecIllumPos;            //0x005C 
		Vector  vecHullMin;             //0x0068 
		Vector  vecHullMax;             //0x0074 
		Vector  vecBBMin;               //0x0080 
		Vector  vecBBMax;               //0x008C 
		__int32 flags;                  //0x0098 
		__int32 numbones;               //0x009C 
		__int32 boneindex;              //0x00A0 
		__int32 numbonecontrollers;     //0x00A4 
		__int32 bonecontrollerindex;    //0x00A8 
		__int32 numhitboxsets;          //0x00AC 
		__int32 hitboxsetindex;         //0x00B0 
		__int32 numlocalanim;           //0x00B4 
		__int32 localanimindex;         //0x00B8 
		__int32 numlocalseq;            //0x00BC 
		__int32 localseqindex;          //0x00C0 
		__int32 activitylistversion;    //0x00C4 
		__int32 eventsindexed;          //0x00C8 
		__int32 numtextures;            //0x00CC 
		__int32 textureindex;           //0x00D0

		mstudiohitboxset_t* GetHitboxSet(int i)
		{
			if (i > numhitboxsets) return nullptr;
			return (mstudiohitboxset_t*)((uint8_t*)this + hitboxsetindex) + i;
		}
		mstudiobone_t* GetBone(int i)
		{
			if (i > numbones) return nullptr;
			return (mstudiobone_t*)((uint8_t*)this + boneindex) + i;
		}

	};//Size=0x00D4

	class IVModelInfo
	{
	public:
		virtual                                 ~IVModelInfo(void) {}
		virtual const model_t*                  GetModel(int modelindex) const = 0;
		virtual int                             GetModelIndex(const char *name) const = 0;
		virtual const char*                     GetModelName(const model_t *model) const = 0;
		virtual vcollide_t*                     GetVCollide(const model_t *model) const = 0;
		virtual vcollide_t*                     GetVCollide(int modelindex) const = 0;
		virtual void                            GetModelBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
		virtual void                            GetModelRenderBounds(const model_t *model, Vector& mins, Vector& maxs) const = 0;
		virtual int                             GetModelFrameCount(const model_t *model) const = 0;
		virtual int                             GetModelType(const model_t *model) const = 0;
		virtual void*                           GetModelExtraData(const model_t *model) = 0;
		virtual bool                            ModelHasMaterialProxy(const model_t *model) const = 0;
		virtual bool                            IsTranslucent(model_t const* model) const = 0;
		virtual bool                            IsTranslucentTwoPass(const model_t *model) const = 0;
		virtual void                            Unused0() {};
		virtual void UNUSED() = 0;
		virtual void UNUSE11D() = 0;
		virtual RenderableTranslucencyType_t    ComputeTranslucencyType(const model_t *model, int nSkin, int nBody) = 0;
		virtual int                             GetModelMaterialCount(const model_t* model) const = 0;
		virtual void                            GetModelMaterials(const model_t *model, int count, IMaterial** ppMaterial) = 0;
		virtual bool                            IsModelVertexLit(const model_t *model) const = 0;
		virtual const char*                     GetModelKeyValueText(const model_t *model) = 0;
		virtual bool                            GetModelKeyValue(const model_t *model, CUtlBuffer &buf) = 0;
		virtual float                           GetModelRadius(const model_t *model) = 0;
		virtual CStudioHdr*                     GetStudioHdr(MDLHandle_t handle) = 0;
		virtual const studiohdr_t*              FindModel(const studiohdr_t *pStudioHdr, void **cache, const char *modelname) const = 0;
		virtual const studiohdr_t*              FindModel(void *cache) const = 0;
		virtual virtualmodel_t*                 GetVirtualModel(const studiohdr_t *pStudioHdr) const = 0;
		virtual uint8_t*                        GetAnimBlock(const studiohdr_t *pStudioHdr, int iBlock) const = 0;
		virtual void                            GetModelMaterialColorAndLighting(const model_t *model, Vector const& origin, QAngle const& angles, trace_t* pTrace, Vector& lighting, Vector& matColor) = 0;
		virtual void                            GetIlluminationPoint(const model_t *model, IClientRenderable *pRenderable, Vector const& origin, QAngle const& angles, Vector* pLightingCenter) = 0;
		virtual int                             GetModelContents(int modelIndex) const = 0;
		virtual studiohdr_t*                    GetStudiomodel(const model_t *mod) = 0;
		virtual int                             GetModelSpriteWidth(const model_t *model) const = 0;
		virtual int                             GetModelSpriteHeight(const model_t *model) const = 0;
		virtual void                            SetLevelScreenFadeRange(float flMinSize, float flMaxSize) = 0;
		virtual void                            GetLevelScreenFadeRange(float *pMinArea, float *pMaxArea) const = 0;
		virtual void                            SetViewScreenFadeRange(float flMinSize, float flMaxSize) = 0;
		virtual unsigned char                   ComputeLevelScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
		virtual unsigned char                   ComputeViewScreenFade(const Vector &vecAbsOrigin, float flRadius, float flFadeScale) const = 0;
		virtual int                             GetAutoplayList(const studiohdr_t *pStudioHdr, unsigned short **pAutoplayList) const = 0;
		virtual CPhysCollide*                   GetCollideForVirtualTerrain(int index) = 0;
		virtual bool                            IsUsingFBTexture(const model_t *model, int nSkin, int nBody, IClientRenderable* *pClientRenderable) const = 0;
		virtual const model_t*                  FindOrLoadModel(const char *name) const = 0;
		virtual MDLHandle_t                     GetCacheHandle(const model_t *model) const = 0;
		virtual int                             GetBrushModelPlaneCount(const model_t *model) const = 0;
		virtual void                            GetBrushModelPlane(const model_t *model, int nIndex, cplane_t &plane, Vector *pOrigin) const = 0;
		virtual int                             GetSurfacepropsForVirtualTerrain(int index) = 0;
		virtual bool                            UsesEnvCubemap(const model_t *model) const = 0;
		virtual bool                            UsesStaticLighting(const model_t *model) const = 0;
	};

	class IVModelInfoClient : public IVModelInfo
	{
	public:
	};


	struct DrawModelState_t
	{
		studiohdr_t*            m_pStudioHdr;
		studiohwdata_t*         m_pStudioHWData;
		IClientRenderable*      m_pRenderable;
		const matrix3x4_t*      m_pModelToWorld;
		StudioDecalHandle_t     m_decals;
		int                     m_drawFlags;
		int                     m_lod;
	};

	struct StaticPropRenderInfo_t
	{
		const matrix3x4_t*      pModelToWorld;
		const model_t*          pModel;
		IClientRenderable*      pRenderable;
		Vector*                 pLightingOrigin;
		short                   skin;
		ModelInstanceHandle_t   instance;
	};

	struct ModelRenderInfo_t
	{
		Vector                  origin;
		QAngle                  angles;
		char                    pad[4];
		IClientRenderable*      pRenderable;
		const model_t*          pModel;
		const matrix3x4_t*      pModelToWorld;
		const matrix3x4_t*      pLightingOffset;
		const Vector*           pLightingOrigin;
		int                     flags;
		int                     entity_index;
		int                     skin;
		int                     body;
		int                     hitboxset;
		ModelInstanceHandle_t   instance;

		ModelRenderInfo_t()
		{
			pModelToWorld = NULL;
			pLightingOffset = NULL;
			pLightingOrigin = NULL;
		}
	};

	struct LightingQuery_t
	{
		Vector                  m_LightingOrigin;
		ModelInstanceHandle_t   m_InstanceHandle;
		bool                    m_bAmbientBoost;
	};

	struct StaticLightingQuery_t : public LightingQuery_t
	{
		IClientRenderable*        m_pRenderable;
	};
	/*
	class IMaterial
	{
	public:
		virtual const char*             GetName() const = 0;
		virtual const char*             GetTextureGroupName() const = 0;
		virtual PreviewImageRetVal_t    GetPreviewImageProperties(int *width, int *height, ImageFormat *imageFormat, bool* isTranslucent) const = 0;
		virtual PreviewImageRetVal_t    GetPreviewImage(unsigned char *data, int width, int height, ImageFormat imageFormat) const = 0;
		virtual int                     GetMappingWidth() = 0;
		virtual int                     GetMappingHeight() = 0;
		virtual int                     GetNumAnimationFrames() = 0;
		virtual bool                    InMaterialPage(void) = 0;
		virtual    void                 GetMaterialOffset(float *pOffset) = 0;
		virtual void                    GetMaterialScale(float *pScale) = 0;
		virtual IMaterial*              GetMaterialPage(void) = 0;
		virtual IMaterialVar*           FindVar(const char *varName, bool *found, bool complain = true) = 0;
		virtual void                    IncrementReferenceCount(void) = 0;
		virtual void                    DecrementReferenceCount(void) = 0;
		inline void                     AddRef() { IncrementReferenceCount(); }
		inline void                     Release() { DecrementReferenceCount(); }
		virtual int                     GetEnumerationID(void) const = 0;
		virtual void                    GetLowResColorSample(float s, float t, float *color) const = 0;
		virtual void                    RecomputeStateSnapshots() = 0;
		virtual bool                    IsTranslucent() = 0;
		virtual bool                    IsAlphaTested() = 0;
		virtual bool                    IsVertexLit() = 0;
		virtual VertexFormat_t          GetVertexFormat() const = 0;
		virtual bool                    HasProxy(void) const = 0;
		virtual bool                    UsesEnvCubemap(void) = 0;
		virtual bool                    NeedsTangentSpace(void) = 0;
		virtual bool                    NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
		virtual bool                    NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
		virtual bool                    NeedsSoftwareSkinning(void) = 0;
		virtual void                    AlphaModulate(float alpha) = 0;
		virtual void                    ColorModulate(float r, float g, float b) = 0;
		virtual void                    SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
		virtual bool                    GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;
		virtual void                    GetReflectivity(Vector& reflect) = 0;
		virtual bool                    GetPropertyFlag(MaterialPropertyTypes_t type) = 0;
		virtual bool                    IsTwoSided() = 0;
		virtual void                    SetShader(const char *pShaderName) = 0;
		virtual int                     GetNumPasses(void) = 0;
		virtual int                     GetTextureMemoryBytes(void) = 0;
		virtual void                    Refresh() = 0;
		virtual bool                    NeedsLightmapBlendAlpha(void) = 0;
		virtual bool                    NeedsSoftwareLighting(void) = 0;
		virtual int                     ShaderParamCount() const = 0;
		virtual IMaterialVar**          GetShaderParams(void) = 0;
		virtual bool                    IsErrorMaterial() const = 0;
		virtual void                    Unused() = 0;
		virtual float                   GetAlphaModulation() = 0;
		virtual void                    GetColorModulation(float *r, float *g, float *b) = 0;
		virtual bool                    IsTranslucentUnderModulation(float fAlphaModulation = 1.0f) const = 0;
		virtual IMaterialVar*           FindVarFast(char const *pVarName, unsigned int *pToken) = 0;
		virtual void                    SetShaderAndParams(KeyValues *pKeyValues) = 0;
		virtual const char*             GetShaderName() const = 0;
		virtual void                    DeleteIfUnreferenced() = 0;
		virtual bool                    IsSpriteCard() = 0;
		virtual void                    CallBindProxy(void *proxyData) = 0;
		virtual void                    RefreshPreservingMaterialVars() = 0;
		virtual bool                    WasReloadedFromWhitelist() = 0;
		virtual bool                    SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
		virtual int                     GetReferenceCount() const = 0;
	};
	*/
	struct MorphFormat_t;
	class IMaterial
	{
	public:
		// Get the name of the material.  This is a full path to 
		// the vmt file starting from "hl2/materials" (or equivalent) without
		// a file extension.
		virtual const char *	GetName() const = 0;
		virtual const char *	GetTextureGroupName() const = 0;

		// Get the preferred size/bitDepth of a preview image of a material.
		// This is the sort of image that you would use for a thumbnail view
		// of a material, or in WorldCraft until it uses materials to render.
		// separate this for the tools maybe
		virtual PreviewImageRetVal_t GetPreviewImageProperties(int *width, int *height,
			ImageFormat *imageFormat, bool* isTranslucent) const = 0;

		// Get a preview image at the specified width/height and bitDepth.
		// Will do resampling if necessary.(not yet!!! :) )
		// Will do color format conversion. (works now.)
		virtual PreviewImageRetVal_t GetPreviewImage(unsigned char *data,
			int width, int height,
			ImageFormat imageFormat) const = 0;
		// 
		virtual int				GetMappingWidth() = 0;
		virtual int				GetMappingHeight() = 0;

		virtual int				GetNumAnimationFrames() = 0;

		// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
		virtual bool			InMaterialPage(void) = 0;
		virtual	void			GetMaterialOffset(float *pOffset) = 0;
		virtual void			GetMaterialScale(float *pScale) = 0;
		virtual IMaterial		*GetMaterialPage(void) = 0;

		// find a vmt variable.
		// This is how game code affects how a material is rendered.
		// The game code must know about the params that are used by
		// the shader for the material that it is trying to affect.
		virtual IMaterialVar *	FindVar(const char *varName, bool *found, bool complain = true) = 0;

		// The user never allocates or deallocates materials.  Reference counting is
		// used instead.  Garbage collection is done upon a call to 
		// IMaterialSystem::UncacheUnusedMaterials.
		virtual void			IncrementReferenceCount(void) = 0;
		virtual void			DecrementReferenceCount(void) = 0;

		inline void AddRef() { IncrementReferenceCount(); }
		inline void Release() { DecrementReferenceCount(); }

		// Each material is assigned a number that groups it with like materials
		// for sorting in the application.
		virtual int 			GetEnumerationID(void) const = 0;

		virtual void			GetLowResColorSample(float s, float t, float *color) const = 0;

		// This computes the state snapshots for this material
		virtual void			RecomputeStateSnapshots() = 0;

		// Are we translucent?
		virtual bool			IsTranslucent() = 0;

		// Are we alphatested?
		virtual bool			IsAlphaTested() = 0;

		// Are we vertex lit?
		virtual bool			IsVertexLit() = 0;

		// Gets the vertex format
		virtual VertexFormat_t	GetVertexFormat() const = 0;

		// returns true if this material uses a material proxy
		virtual bool			HasProxy(void) const = 0;

		virtual bool			UsesEnvCubemap(void) = 0;

		virtual bool			NeedsTangentSpace(void) = 0;

		virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
		virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;

		// returns true if the shader doesn't do skinning itself and requires
		// the data that is sent to it to be preskinned.
		virtual bool			NeedsSoftwareSkinning(void) = 0;

		// Apply constant color or alpha modulation
		virtual void			AlphaModulate(float alpha) = 0;
		virtual void			ColorModulate(float r, float g, float b) = 0;

		// Material Var flags...
		virtual void			SetMaterialVarFlag(MaterialVarFlags_t flag, bool on) = 0;
		virtual bool			GetMaterialVarFlag(MaterialVarFlags_t flag) const = 0;

		// Gets material reflectivity
		virtual void			GetReflectivity(Vector& reflect) = 0;

		// Gets material property flags
		virtual bool			GetPropertyFlag(MaterialPropertyTypes_t type) = 0;

		// Is the material visible from both sides?
		virtual bool			IsTwoSided() = 0;

		// Sets the shader associated with the material
		virtual void			SetShader(const char *pShaderName) = 0;

		// Can't be const because the material might have to precache itself.
		virtual int				GetNumPasses(void) = 0;

		// Can't be const because the material might have to precache itself.
		virtual int				GetTextureMemoryBytes(void) = 0;

		// Meant to be used with materials created using CreateMaterial
		// It updates the materials to reflect the current values stored in the material vars
		virtual void			Refresh() = 0;

		// GR - returns true is material uses lightmap alpha for blending
		virtual bool			NeedsLightmapBlendAlpha(void) = 0;

		// returns true if the shader doesn't do lighting itself and requires
		// the data that is sent to it to be prelighted
		virtual bool			NeedsSoftwareLighting(void) = 0;

		// Gets at the shader parameters
		virtual int				ShaderParamCount() const = 0;
		virtual IMaterialVar	**GetShaderParams(void) = 0;

		// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
		// the material can't be found.
		virtual bool			IsErrorMaterial() const = 0;

		virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;

		// Gets the current alpha modulation
		virtual float			GetAlphaModulation() = 0;
		virtual void			GetColorModulation(float *r, float *g, float *b) = 0;

		// Gets the morph format
		virtual MorphFormat_t	GetMorphFormat() const = 0;

		// fast find that stores the index of the found var in the string table in local cache
		virtual IMaterialVar *	FindVarFast(char const *pVarName, unsigned int *pToken) = 0;

		// Sets new VMT shader parameters for the material
		virtual void			SetShaderAndParams(void *pKeyValues) = 0;
		virtual const char *	GetShaderName() const = 0;

		virtual void			DeleteIfUnreferenced() = 0;

		virtual bool			IsSpriteCard() = 0;

		virtual void			CallBindProxy(void *proxyData) = 0;

		virtual IMaterial		*CheckProxyReplacement(void *proxyData) = 0;

		virtual void			RefreshPreservingMaterialVars() = 0;

		virtual bool			WasReloadedFromWhitelist() = 0;

		virtual bool			IsPrecached() const = 0;
	};


	class IVModelRender
	{
	public:
		virtual int                     DrawModel(int flags, IClientRenderable *pRenderable, ModelInstanceHandle_t instance, int entity_index, const model_t *model, Vector const& origin, QAngle const& angles, int skin, int body, int hitboxset, const matrix3x4_t *modelToWorld = NULL, const matrix3x4_t *pLightingOffset = NULL) = 0;
		virtual void                    ForcedMaterialOverride(IMaterial *newMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0) = 0;
		virtual bool                    IsForcedMaterialOverride(void) = 0;
		virtual void                    SetViewTarget(const CStudioHdr *pStudioHdr, int nBodyIndex, const Vector& target) = 0;
		virtual ModelInstanceHandle_t   CreateInstance(IClientRenderable *pRenderable, LightCacheHandle_t *pCache = NULL) = 0;
		virtual void                    DestroyInstance(ModelInstanceHandle_t handle) = 0;
		virtual void                    SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
		virtual LightCacheHandle_t      GetStaticLighting(ModelInstanceHandle_t handle) = 0;
		virtual bool                    ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable *pRenderable) = 0;
		virtual void                    AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, Vector const& decalUp, int decalIndex, int body, bool noPokeThru, int maxLODToDecal) = 0;
		virtual void                    RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
		virtual bool                    ModelHasDecals(ModelInstanceHandle_t handle) = 0;
		virtual void                    RemoveAllDecalsFromAllModels() = 0;
		virtual matrix3x4_t*            DrawModelShadowSetup(IClientRenderable *pRenderable, int body, int skin, DrawModelInfo_t *pInfo, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
		virtual void                    DrawModelShadow(IClientRenderable *pRenderable, const DrawModelInfo_t &info, matrix3x4_t *pCustomBoneToWorld = NULL) = 0;
		virtual bool                    RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
		virtual void                    ReleaseAllStaticPropColorData(void) = 0;
		virtual void                    RestoreAllStaticPropColorData(void) = 0;
		virtual int                     DrawModelEx(ModelRenderInfo_t &pInfo) = 0;
		virtual int                     DrawModelExStaticProp(ModelRenderInfo_t &pInfo) = 0;
		virtual bool                    DrawModelSetup(ModelRenderInfo_t &pInfo, DrawModelState_t *pState, matrix3x4_t **ppBoneToWorldOut) = 0;
		virtual void                    DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
		virtual void                    SetupLighting(const Vector &vecCenter) = 0;
		virtual int                     DrawStaticPropArrayFast(StaticPropRenderInfo_t *pProps, int count, bool bShadowDepth) = 0;
		virtual void                    SuppressEngineLighting(bool bSuppress) = 0;
		virtual void                    SetupColorMeshes(int nTotalVerts) = 0;
		virtual void                    SetupLightingEx(const Vector &vecCenter, ModelInstanceHandle_t handle) = 0;
		virtual bool                    GetBrightestShadowingLightSource(const Vector &vecCenter, Vector& lightPos, Vector& lightBrightness, bool bAllowNonTaggedLights) = 0;
		virtual void                    ComputeLightingState(int nCount, const LightingQuery_t *pQuery, MaterialLightingState_t *pState, ITexture **ppEnvCubemapTexture) = 0;
		virtual void                    GetModelDecalHandles(StudioDecalHandle_t *pDecals, int nDecalStride, int nCount, const ModelInstanceHandle_t *pHandles) = 0;
		virtual void                    ComputeStaticLightingState(int nCount, const StaticLightingQuery_t *pQuery, MaterialLightingState_t *pState, MaterialLightingState_t *pDecalState, ColorMeshInfo_t **ppStaticLighting, ITexture **ppEnvCubemapTexture, DataCacheHandle_t *pColorMeshHandles) = 0;
		virtual void                    CleanupStaticLightingState(int nCount, DataCacheHandle_t *pColorMeshHandles) = 0;
	};

	struct BrushVertex_t
	{
		Vector        m_Pos;
		Vector        m_Normal;
		Vector        m_TangentS;
		Vector        m_TangentT;
		Vector2D    m_TexCoord;
		Vector2D    m_LightmapCoord;

	private:
		BrushVertex_t(const BrushVertex_t& src);
	};

	class IBrushSurface
	{
	public:
		// Computes texture coordinates + lightmap coordinates given a world position
		virtual void ComputeTextureCoordinate(Vector const& worldPos, Vector2D& texCoord) = 0;
		virtual void ComputeLightmapCoordinate(Vector const& worldPos, Vector2D& lightmapCoord) = 0;

		// Gets the vertex data for this surface
		virtual int  GetVertexCount() const = 0;
		virtual void GetVertexData(BrushVertex_t* pVerts) = 0;

		// Gets at the material properties for this surface
		virtual IMaterial* GetMaterial() = 0;
	};

	class IWorldRenderList /*: public IRefCounted*/
	{
	};
	class IBrushRenderer
	{
	public:
		// Draws the surface; returns true if decals should be rendered on this surface
		virtual bool RenderBrushModelSurface(IClientEntity* pBaseEntity, IBrushSurface* pBrushSurface) = 0;
	};
	struct VisibleFogVolumeInfo_t
	{
		int            m_nVisibleFogVolume;
		int            m_nVisibleFogVolumeLeaf;
		bool        m_bEyeInFogVolume;
		float       m_flDistanceToWater;
		float       m_flWaterHeight;
		IMaterial*  m_pFogVolumeMaterial;
	};

	struct WorldListLeafData_t
	{
		LeafIndex_t     leafIndex;    // 16 bits
		int16_t         waterData;
		uint16_t        firstTranslucentSurface;    // engine-internal list index
		uint16_t        translucentSurfaceCount;    // count of translucent surfaces+disps
	};

	struct WorldListInfo_t
	{
		int                     m_ViewFogVolume;
		int                     m_LeafCount;
		bool                    m_bHasWater;
		WorldListLeafData_t*    m_pLeafDataList;
	};

	struct VPlane
	{
		Vector        m_Normal;
		vec_t        m_Dist;
	};
	typedef VPlane Frustum[FRUSTUM_NUMPLANES];

	struct VisOverrideData_t
	{
		Vector        m_vecVisOrigin;                    // The point to to use as the viewpoint for area portal backface cull checks.
		float        m_fDistToAreaPortalTolerance;    // The distance from an area portal before using the full screen as the viewable portion.
	};

	class IVRenderView
	{
		enum
		{
			VIEW_SETUP_VIS_EX_RETURN_FLAGS_USES_RADIAL_VIS = 0x00000001
		};
	public:
		virtual void                DrawBrushModel(IClientEntity *baseentity, model_t *model, const Vector& origin, const QAngle& angles, bool sort) = 0;
		virtual void                DrawIdentityBrushModel(IWorldRenderList *pList, model_t *model) = 0;
		virtual void                TouchLight(struct dlight_t *light) = 0;
		virtual void                Draw3DDebugOverlays(void) = 0;
		virtual void                SetBlend(float blend) = 0;
		virtual float               GetBlend(void) = 0;
		virtual void                SetColorModulation(float const* blend) = 0;
		inline void                 SetColorModulation(float r, float g, float b)
		{
			float clr[3] = { r, g, b };
			SetColorModulation(clr);
		}
		virtual void                GetColorModulation(float* blend) = 0;
		virtual void                SceneBegin(void) = 0;
		virtual void                SceneEnd(void) = 0;
		virtual void                GetVisibleFogVolume(const Vector& eyePoint, VisibleFogVolumeInfo_t *pInfo) = 0;
		virtual IWorldRenderList*   CreateWorldList() = 0;
		virtual void                BuildWorldLists(IWorldRenderList *pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = NULL, bool bShadowDepth = false, float *pReflectionWaterHeight = NULL) = 0;
		virtual void                DrawWorldLists(IWorldRenderList *pList, unsigned long flags, float waterZAdjust) = 0;
		virtual int                 GetNumIndicesForWorldLists(IWorldRenderList *pList, unsigned long nFlags) = 0;
		virtual void                DrawTopView(bool enable) = 0;
		virtual void                TopViewBounds(Vector2D const& mins, Vector2D const& maxs) = 0;
		virtual void                DrawLights(void) = 0;
		virtual void                DrawMaskEntities(void) = 0;
		virtual void                DrawTranslucentSurfaces(IWorldRenderList *pList, int *pSortList, int sortCount, unsigned long flags) = 0;
		virtual void                DrawLineFile(void) = 0;
		virtual void                DrawLightmaps(IWorldRenderList *pList, int pageId) = 0;
		virtual void                ViewSetupVis(bool novis, int numorigins, const Vector origin[]) = 0;
		virtual bool                AreAnyLeavesVisible(int *leafList, int nLeaves) = 0;
		virtual    void             VguiPaint(void) = 0;
		virtual void                ViewDrawFade(uint8_t *color, IMaterial *pMaterial) = 0;
		virtual void                OLD_SetProjectionMatrix(float fov, float zNear, float zFar) = 0;
		virtual unsigned long       GetLightAtPoint(Vector& pos) = 0;
		virtual int                 GetViewEntity(void) = 0;
		virtual bool                IsViewEntity(int entindex) = 0;
		virtual float               GetFieldOfView(void) = 0;
		virtual unsigned char**     GetAreaBits(void) = 0;
		virtual void                SetFogVolumeState(int nVisibleFogVolume, bool bUseHeightFog) = 0;
		virtual void                InstallBrushSurfaceRenderer(IBrushRenderer* pBrushRenderer) = 0;
		virtual void                DrawBrushModelShadow(IClientRenderable *pRenderable) = 0;
		virtual    bool             LeafContainsTranslucentSurfaces(IWorldRenderList *pList, int sortIndex, unsigned long flags) = 0;
		virtual bool                DoesBoxIntersectWaterVolume(const Vector &mins, const Vector &maxs, int leafWaterDataID) = 0;
		virtual void                SetAreaState(unsigned char chAreaBits[MAX_AREA_STATE_BYTES], unsigned char chAreaPortalBits[MAX_AREA_PORTAL_STATE_BYTES]) = 0;
		virtual void                VGui_Paint(int mode) = 0;
		virtual void                Push3DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
		virtual void                Push2DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes) = 0;
		virtual void                PopView(Frustum frustumPlanes) = 0;
		virtual void                SetMainView(const Vector &vecOrigin, const QAngle &angles) = 0;
		virtual void                ViewSetupVisEx(bool novis, int numorigins, const Vector origin[], unsigned int &returnFlags) = 0;
		virtual void                OverrideViewFrustum(Frustum custom) = 0;
		virtual void                DrawBrushModelShadowDepth(IClientEntity *baseentity, model_t *model, const Vector& origin, const QAngle& angles, bool bSort) = 0;
		virtual void                UpdateBrushModelLightmap(model_t *model, IClientRenderable *pRenderable) = 0;
		virtual void                BeginUpdateLightmaps(void) = 0;
		virtual void                EndUpdateLightmaps(void) = 0;
		virtual void                OLD_SetOffCenterProjectionMatrix(float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight) = 0;
		virtual void                OLD_SetProjectionMatrixOrtho(float left, float top, float right, float bottom, float zNear, float zFar) = 0;
		virtual void                Push3DView(const CViewSetup &view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture) = 0;
		virtual void                GetMatricesForView(const CViewSetup &view, VMatrix *pWorldToView, VMatrix *pViewToProjection, VMatrix *pWorldToProjection, VMatrix *pWorldToPixels) = 0;
	};

	struct MaterialSystem_Config_t;
	class IMaterialProxyFactory;
	class ITexture;
	class IMaterialSystemHardwareConfig;
	class CShadowMgr;

	struct MaterialAdapterInfo_t
	{
		char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
		unsigned int m_VendorID;
		unsigned int m_DeviceID;
		unsigned int m_SubSysID;
		unsigned int m_Revision;
		int m_nDXSupportLevel;			// This is the *preferred* dx support level
		int m_nMinDXSupportLevel;
		int m_nMaxDXSupportLevel;
		unsigned int m_nDriverVersionHigh;
		unsigned int m_nDriverVersionLow;
	};

	struct MaterialVideoMode_t
	{
		int m_Width;			// if width and height are 0 and you select 
		int m_Height;			// windowed mode, it'll use the window size
		ImageFormat m_Format;	// use ImageFormats (ignored for windowed mode)
		int m_RefreshRate;		// 0 == default (ignored for windowed mode)
	};
	typedef void(*MaterialBufferReleaseFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
	typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
	typedef void(*ModeChangeCallbackFunc_t)(void);
	typedef void(*EndFrameCleanupFunc_t)(void);
	typedef bool(*EndFramePriorToNextContextFunc_t)(void);
	typedef void(*OnLevelShutdownFunc_t)(void *pUserData);
	struct MaterialSystemHWID_t;
	struct AspectRatioInfo_t;
	class IShader;
	typedef unsigned short MaterialHandle_t;

	class IMaterialSystem : public IAppSystem
	{
	public:

		virtual CreateInterfaceFn               Init(char const* pShaderAPIDLL, IMaterialProxyFactory *pMaterialProxyFactory, CreateInterfaceFn fileSystemFactory, CreateInterfaceFn cvarFactory = NULL) = 0;
		virtual void                            SetShaderAPI(char const *pShaderAPIDLL) = 0;
		virtual void                            SetAdapter(int nAdapter, int nFlags) = 0;
		virtual void                            ModInit() = 0;
		virtual void                            ModShutdown() = 0;
		virtual void                            SetThreadMode(MaterialThreadMode_t mode, int nServiceThread = -1) = 0;
		virtual MaterialThreadMode_t            GetThreadMode() = 0;
		virtual void                            ExecuteQueued() = 0;
		virtual void                            OnDebugEvent(const char *pEvent) = 0;
		virtual IMaterialSystemHardwareConfig*  GetHardwareConfig(const char *pVersion, int *returnCode) = 0;
		virtual void                            __unknown() = 0;
		virtual bool                            UpdateConfig(bool bForceUpdate) = 0; //20
		virtual bool                            OverrideConfig(const MaterialSystem_Config_t &config, bool bForceUpdate) = 0;
		virtual const MaterialSystem_Config_t&  GetCurrentConfigForVideoCard() const = 0;
		virtual bool                            GetRecommendedConfigurationInfo(int nDXLevel, KeyValues * pKeyValues) = 0;
		virtual int                             GetDisplayAdapterCount() const = 0;
		virtual int                             GetCurrentAdapter() const = 0;
		virtual void                            GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const = 0;
		virtual int                             GetModeCount(int adapter) const = 0;
		virtual void                            GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const = 0;
		virtual void                            AddModeChangeCallBack(ModeChangeCallbackFunc_t func) = 0;
		virtual void                            GetDisplayMode(MaterialVideoMode_t& mode) const = 0; //30
		virtual bool                            SetMode(void* hwnd, const MaterialSystem_Config_t &config) = 0;
		virtual bool                            SupportsMSAAMode(int nMSAAMode) = 0;
		virtual const MaterialSystemHWID_t&     GetVideoCardIdentifier(void) const = 0;
		virtual void                            SpewDriverInfo() const = 0;
		virtual void                            GetBackBufferDimensions(int &width, int &height) const = 0;
		virtual ImageFormat                     GetBackBufferFormat() const = 0;
		virtual const AspectRatioInfo_t&        GetAspectRatioInfo() const = 0;
		virtual bool                            SupportsHDRMode(HDRType_t nHDRModede) = 0;
		virtual bool                            AddView(void* hwnd) = 0;
		virtual void                            RemoveView(void* hwnd) = 0; //40
		virtual void                            SetView(void* hwnd) = 0;
		virtual void                            BeginFrame(float frameTime) = 0;
		virtual void                            EndFrame() = 0;
		virtual void                            Flush(bool flushHardware = false) = 0;
		virtual unsigned int                    GetCurrentFrameCount() = 0;
		virtual void                            SwapBuffers() = 0;
		virtual void                            EvictManagedResources() = 0;
		virtual void                            ReleaseResources(void) = 0;
		virtual void                            ReacquireResources(void) = 0;
		virtual void                            AddReleaseFunc(MaterialBufferReleaseFunc_t func) = 0; //50
		virtual void                            RemoveReleaseFunc(MaterialBufferReleaseFunc_t func) = 0;
		virtual void                            AddRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
		virtual void                            RemoveRestoreFunc(MaterialBufferRestoreFunc_t func) = 0;
		virtual void                            AddEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
		virtual void                            RemoveEndFrameCleanupFunc(EndFrameCleanupFunc_t func) = 0;
		virtual void                            OnLevelShutdown() = 0;
		virtual bool                            AddOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
		virtual bool                            RemoveOnLevelShutdownFunc(OnLevelShutdownFunc_t func, void *pUserData) = 0;
		virtual void                            OnLevelLoadingComplete() = 0;
		virtual void                            ResetTempHWMemory(bool bExitingLevel = false) = 0; //60
		virtual void                            HandleDeviceLost() = 0;
		virtual int                             ShaderCount() const = 0;
		virtual int                             GetShaders(int nFirstShader, int nMaxCount, IShader **ppShaderList) const = 0;
		virtual int                             ShaderFlagCount() const = 0;
		virtual const char*                     ShaderFlagName(int nIndex) const = 0;
		virtual void                            GetShaderFallback(const char *pShaderName, char *pFallbackShader, int nFallbackLength) = 0;
		virtual IMaterialProxyFactory*          GetMaterialProxyFactory() = 0;
		virtual void                            SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) = 0;
		virtual void                            EnableEditorMaterials() = 0;
		virtual void                            EnableGBuffers() = 0; //70
		virtual void                            SetInStubMode(bool bInStubMode) = 0;
		virtual void                            DebugPrintUsedMaterials(const char *pSearchSubString, bool bVerbose) = 0;
		virtual void                            DebugPrintUsedTextures(void) = 0;
		virtual void                            ToggleSuppressMaterial(char const* pMaterialName) = 0;
		virtual void                            ToggleDebugMaterial(char const* pMaterialName) = 0;
		virtual bool                            UsingFastClipping(void) = 0;
		virtual int                             StencilBufferBits(void) = 0; //number of bits per pixel in the stencil buffer
		virtual void                            UncacheAllMaterials() = 0;
		virtual void                            UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) = 0;
		virtual void                            CacheUsedMaterials() = 0; //80
		virtual void                            ReloadTextures() = 0;
		virtual void                            ReloadMaterials(const char *pSubString = NULL) = 0;
		virtual IMaterial*                      CreateMaterial(const char *pMaterialName, KeyValues *pVMTKeyValues) = 0;
		virtual IMaterial*                      FindMaterial(char const* pMaterialName, const char *pTextureGroupName = nullptr, bool complain = true, const char *pComplainPrefix = NULL) = 0;
		virtual MaterialHandle_t                FirstMaterial() const = 0;
		virtual MaterialHandle_t                NextMaterial(MaterialHandle_t h) const = 0;
		virtual MaterialHandle_t                InvalidMaterial() const = 0;
		virtual IMaterial*                      GetMaterial(MaterialHandle_t h) const = 0;
		virtual int                             GetNumMaterials() const = 0;
		virtual ITexture*                       FindTexture(char const* pTextureName, const char *pTextureGroupName, bool complain = true) = 0;
		virtual bool                            IsTextureLoaded(char const* pTextureName) const = 0;
		virtual ITexture*                       CreateProceduralTexture(const char	*pTextureName, const char *pTextureGroupName, int w, int h, ImageFormat fmt, int nFlags) = 0;
		virtual void                            BeginRenderTargetAllocation() = 0;
		virtual void                            EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources
		virtual ITexture*                       CreateRenderTargetTexture(int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat	format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED) = 0;
		virtual ITexture*                       CreateNamedRenderTargetTextureEx(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;
		virtual ITexture*                       CreateNamedRenderTargetTexture(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false) = 0;
		virtual ITexture*                       CreateNamedRenderTargetTextureEx2(const char *pRTName, int w, int h, RenderTargetSizeMode_t sizeMode, ImageFormat format, MaterialRenderTargetDepth_t depth = MATERIAL_RT_DEPTH_SHARED, unsigned int textureFlags = TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, unsigned int renderTargetFlags = 0) = 0;

	};
}