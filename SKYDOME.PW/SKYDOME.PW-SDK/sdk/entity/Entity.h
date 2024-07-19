#pragma once



#include <type_traits>
#include "CBaseHandle.h"
#include "../datatypes/quaternion.h"

#include "../ischemasystem.h"
#include "../../OffsetManager.h"
#include "../../sd/schema/schema.h"
#include "../../sd/interfaces/interfaces.h"
#include "../../sd/utilities/fnv1a.h"


#define _OFFSET(TYPE, NAME, OFFSET)                                                                 \
	[[nodiscard]] __forceinline std::add_lvalue_reference_t<TYPE> NAME()                                          \
	{                                                                                                         \
		static const std::uint32_t uOffset = OFFSET;                                                          \
		return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

class CEntityInstance;
using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;



class CEntityIdentity
{
public:

	// @note: handle index is not entity index
	//_OFFSET(std::uint32_t, GetIndex, 0x10);
	//_OFFSET(const char*, GetDesignerName, cs2_dumper::schemas::client_dll::CEntityIdentity::m_designerName);
	//_OFFSET(std::uint32_t, GetFlags, cs2_dumper::schemas::client_dll::CEntityIdentity::m_flags);
	SCHEMA_ADD_OFFSET(std::uint32_t, GetIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, GetDesignerName, "CEntityIdentity->m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "CEntityIdentity->m_flags");
	

	[[nodiscard]] bool IsValid()
	{
		return GetIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;

		return GetIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		return GetIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance; // 0x00
};

class CEntityInstance
{
public:
	//CS_CLASS_NO_INITIALIZER(CEntityInstance);

	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return MEM::CallVFunc<void, 38U>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = GetIdentity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->GetFlags() & 1));
	}

	//_OFFSET(CEntityIdentity*, GetIdentity, cs2_dumper::schemas::client_dll::CEntityInstance::m_pEntity);
	SCHEMA_ADD_FIELD(CEntityIdentity*, GetIdentity, "CEntityInstance->m_pEntity");
};

class CCollisionProperty
{
public:
	//CS_CLASS_NO_INITIALIZER(CCollisionProperty);

	SCHEMA_ADD_FIELD(Vector, GetMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector, GetMaxs, "CCollisionProperty->m_vecMaxs");
};

class CTransform
{
public:
	VectorAligned_t vecPosition;
	QuaternionAligned_t quatOrientation;
};

class CSkeletonInstance;
class CGameSceneNode
{
public:
	//CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, GetNodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, GetOwner, "CGameSceneNode->m_pOwner");

	SCHEMA_ADD_FIELD(Vector, GetAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector, GetRenderOrigin, "CGameSceneNode->m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle_t, GetAngleRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle_t, GetAbsAngleRotation, "CGameSceneNode->m_angAbsRotation");

	SCHEMA_ADD_FIELD(bool, IsDormant, "CGameSceneNode->m_bDormant");

	/*CSkeletonInstance* GetSkeletonInstance()
	{
		return MEM::CallVFunc<CSkeletonInstance*, 8U>(this);
	}*/
};



class C_BaseEntity : public CEntityInstance
{
public:
	//CS_CLASS_NO_INITIALIZER(C_BaseEntity);

	[[nodiscard]] bool IsBasePlayerController();

	[[nodiscard]] bool IsWeapon();
	

	//static C_BaseEntity* GetLocalPlayer();

	// get entity origin on scene
	[[nodiscard]] const Vector& GetSceneOrigin();

	SCHEMA_ADD_FIELD(CGameSceneNode*, GetGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, GetCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(std::uint8_t, GetTeam, "C_BaseEntity->m_iTeamNum");
	SCHEMA_ADD_FIELD(CBaseHandle, GetOwnerHandle, "C_BaseEntity->m_hOwnerEntity");
	SCHEMA_ADD_FIELD(Vector, GetBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector, GetAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, "C_BaseEntity->m_bTakesDamage");
	SCHEMA_ADD_FIELD(std::uint32_t, GetFlags, "C_BaseEntity->m_fFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetEflags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(std::int32_t, GetMoveType, "C_BaseEntity->m_MoveType");
	SCHEMA_ADD_FIELD(std::uint8_t, GetLifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::int32_t, GetHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetMaxHealth, "C_BaseEntity->m_iMaxHealth");
	SCHEMA_ADD_FIELD(float, GetWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD_OFFSET(void*, GetVData, "C_BaseEntity->m_nSubclassID", 0x8);
};

class CGlowProperty;

class C_BaseModelEntity : public C_BaseEntity
{
public:
	//CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, GetCollisionInstance, "C_BaseModelEntity->m_Collision");
	SCHEMA_ADD_FIELD(CGlowProperty, GetGlowProperty, "C_BaseModelEntity->m_Glow");
	SCHEMA_ADD_FIELD(Vector, GetViewOffset, "C_BaseModelEntity->m_vecViewOffset");
	SCHEMA_ADD_FIELD(GameTime_t, GetCreationTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, GetCreationTick, "C_BaseModelEntity->m_nCreationTick");
	SCHEMA_ADD_FIELD(CBaseHandle, GetMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
	SCHEMA_ADD_FIELD(std::float_t, GetAnimTime, "C_BaseModelEntity->m_flAnimTime");
	SCHEMA_ADD_FIELD(std::float_t, GetSimulationTime, "C_BaseModelEntity->m_flSimulationTime");
};

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	//CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);

	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, "C_BasePlayerPawn->m_hController");
	//SCHEMA_ADD_FIELD(CCSPlayer_WeaponServices*, GetWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	//SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, "C_BasePlayerPawn->m_pItemServices");
	//SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, "C_BasePlayerPawn->m_pCameraServices");
};

class CCSPlayer_ViewModelServices;

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	//CS_CLASS_NO_INITIALIZER(C_CSPlayerPawnBase);

	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, GetViewModelServices, "C_CSPlayerPawnBase->m_pViewModelServices");
	SCHEMA_ADD_FIELD(float, GetLowerBodyYawTarget, "C_CSPlayerPawnBase->m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD(float, GetFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(float, GetFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");
	SCHEMA_ADD_FIELD(Vector, GetLastSmokeOverlayColor, "C_CSPlayerPawnBase->m_vLastSmokeOverlayColor");
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam"); // danger zone
};



class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	//CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);

	[[nodiscard]] bool IsOtherEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] int GetAssociatedTeam();
	[[nodiscard]] bool CanAttack(const float flServerTime);

	SCHEMA_ADD_FIELD(bool, IsScoped, "C_CSPlayerPawn->m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, IsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, IsGrabbingHostage, "C_CSPlayerPawn->m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, IsWaitForNoAttack, "C_CSPlayerPawn->m_bWaitForNoAttack");
	SCHEMA_ADD_FIELD(int, GetShotsFired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA_ADD_FIELD(std::int32_t, GetArmorValue, "C_CSPlayerPawn->m_ArmorValue");
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
	//CS_CLASS_NO_INITIALIZER(CBasePlayerController);

	SCHEMA_ADD_FIELD(std::uint64_t, GetSteamId, "CBasePlayerController->m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, GetTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPawnHandle, "CBasePlayerController->m_hPawn");
	SCHEMA_ADD_FIELD(bool, IsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");
};


// forward decleration
class C_CSWeaponBaseGun;
class C_BasePlayerWeapon;
class CCSPlayerController : public CBasePlayerController
{
public:
	//CS_CLASS_NO_INITIALIZER(CCSPlayerController);

	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();

	// @note: always get origin from pawn not controller
	[[nodiscard]] const Vector& GetPawnOrigin();

	SCHEMA_ADD_FIELD(std::uint32_t, GetPing, "CCSPlayerController->m_iPing");
	SCHEMA_ADD_FIELD(const char*, GetPlayerName, "CCSPlayerController->m_sSanitizedPlayerName");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnHealth, "CCSPlayerController->m_iPawnHealth");
	SCHEMA_ADD_FIELD(std::int32_t, GetPawnArmor, "CCSPlayerController->m_iPawnArmor");
	SCHEMA_ADD_FIELD(bool, IsPawnHasDefuser, "CCSPlayerController->m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, IsPawnHasHelmet, "CCSPlayerController->m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, IsPawnAlive, "CCSPlayerController->m_bPawnIsAlive");
	SCHEMA_ADD_FIELD(CBaseHandle, GetPlayerPawnHandle, "CCSPlayerController->m_hPlayerPawn");
};