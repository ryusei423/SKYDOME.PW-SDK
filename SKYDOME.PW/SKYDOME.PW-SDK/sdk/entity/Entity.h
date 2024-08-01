#pragma once



//#include <type_traits>
#include "CBaseHandle.h"
#include "../datatypes/quaternion.h"

#include "../ischemasystem.h"
#include "../../OffsetManager.h"
#include "../../sd/schema/schema.h"
#include "../../sd/interfaces/interfaces.h"
#include "../../sd/utilities/fnv1a.h"

//#include "Weapon.h"
#include "../hitbox/hitbox.h"

#define _OFFSET(TYPE, NAME, OFFSET)                                                                 \
	[[nodiscard]] __forceinline std::add_lvalue_reference_t<TYPE> NAME()                                          \
	{                                                                                                         \
		static const std::uint32_t uOffset = OFFSET;                                                          \
		return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset)); \
	}

class CEntityInstance;
using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

//听说可以用作可见性检查，不过会比较慢
//我想会有用，不过暂时说不上来也
//EntitySpottedState_t m_entitySpottedState;
//if (target->m_bSpottedByMask & (1 << localPlayer)
struct EntitySpottedState_t
{
private:
	[[maybe_unused]] uint8_t __pad0009[0x3];
public:
	bool m_bSpotted;
	uint32_t m_bSpottedByMask[2];
};


enum HitGroup_t : std::uint32_t {
	HITGROUP_INVALID = -1,
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD = 1,
	HITGROUP_CHEST = 2,
	HITGROUP_STOMACH = 3,
	HITGROUP_LEFTARM = 4,
	HITGROUP_RIGHTARM = 5,
	HITGROUP_LEFTLEG = 6,
	HITGROUP_RIGHTLEG = 7,
	HITGROUP_NECK = 8,
	HITGROUP_UNUSED = 9,
	HITGROUP_GEAR = 10,
	HITGROUP_SPECIAL = 11,
	HITGROUP_COUNT = 12,
};


class CPlayer_ItemServices {
public:
	SCHEMA_ADD_FIELD(bool, m_bHasDefuser, "CCSPlayer_ItemServices->m_bHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bHasHelmet, "CCSPlayer_ItemServices->m_bHasHelmet");
	SCHEMA_ADD_FIELD(bool, m_bHasHeavyArmor, "CCSPlayer_ItemServices->m_bHasHeavyArmor");
};

class CPlayer_MovementServices
{
public:

	SCHEMA_ADD_FIELD(float, m_flMaxspeed, "CPlayer_MovementServices->m_flMaxspeed");
	SCHEMA_ADD_FIELD(float, m_flForwardMove, "CPlayer_MovementServices->m_flForwardMove");
	SCHEMA_ADD_FIELD(float, m_flLeftMove, "CPlayer_MovementServices->m_flLeftMove");
	SCHEMA_ADD_OFFSET(float, m_flSurfaceFriction, 0x1FC);
};


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

	SCHEMA_ADD_FIELD(Vector, m_vecMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector, m_vecMaxs, "CCollisionProperty->m_vecMaxs");
};

class CTransform
{
public:
	VectorAligned_t vecPosition;
	QuaternionAligned_t quatOrientation;

	Matrix3x4_t ToMatrix() const {
	
		Matrix3x4_t matrix{};

		const auto& rot = quatOrientation;
		const auto& pos = vecPosition;

		matrix[0][0] = 1.0f - 2.0f * rot.y * rot.y - 2.0f * rot.z * rot.z;
		matrix[1][0] = 2.0f * rot.x * rot.y + 2.0f * rot.w * rot.z;
		matrix[2][0] = 2.0f * rot.x * rot.z - 2.0f * rot.w * rot.y;

		matrix[0][1] = 2.0f * rot.x * rot.y - 2.0f * rot.w * rot.z;
		matrix[1][1] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.z * rot.z;
		matrix[2][1] = 2.0f * rot.y * rot.z + 2.0f * rot.w * rot.x;

		matrix[0][2] = 2.0f * rot.x * rot.z + 2.0f * rot.w * rot.y;
		matrix[1][2] = 2.0f * rot.y * rot.z - 2.0f * rot.w * rot.x;
		matrix[2][2] = 1.0f - 2.0f * rot.x * rot.x - 2.0f * rot.y * rot.y;

		matrix[0][3] = pos.x;
		matrix[1][3] = pos.y;
		matrix[2][3] = pos.z;

		return matrix;
	
	};
};

struct Tickfrac_t
{
	int tick;
	float fraction;
};

struct InterpInfo_t
{
	int srcTick;
	int dstTick;
	float fraction;
};

class CSkeletonInstance;
class CGameSceneNode
{
public:
	//CS_CLASS_NO_INITIALIZER(CGameSceneNode);

	SCHEMA_ADD_FIELD(CTransform, m_nodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, m_pOwner, "CGameSceneNode->m_pOwner");

	SCHEMA_ADD_FIELD(Vector, m_vecOrigin, "CGameSceneNode->m_vecOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vRenderOrigin, "CGameSceneNode->m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle, m_angRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle, m_angAbsRotation, "CGameSceneNode->m_angAbsRotation");

	SCHEMA_ADD_FIELD(bool, m_bDormant, "CGameSceneNode->m_bDormant");

	bool CalculateInterpInfos(InterpInfo_t* cl, InterpInfo_t* sv0, InterpInfo_t* sv1, Tickfrac_t* pl);

	CSkeletonInstance* GetSkeletonInstance()
	{
		return MEM::CallVFunc<CSkeletonInstance*, 8U>(this);
	}
};

class EntSubClassVDataBase {
public:
	template <typename T> requires std::derived_from<T, EntSubClassVDataBase>
	inline T* as() {
		return (T*)this;
	}
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

	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, m_pCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");
	SCHEMA_ADD_FIELD(CBaseHandle, m_hOwnerEntity, "C_BaseEntity->m_hOwnerEntity");
	SCHEMA_ADD_FIELD(Vector, m_vecVelocity, "C_BaseEntity->m_vecVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(bool, IsTakingDamage, "C_BaseEntity->m_bTakesDamage");
	SCHEMA_ADD_FIELD(std::uint32_t, m_fFlags, "C_BaseEntity->m_fFlags");
	SCHEMA_ADD_FIELD(std::int32_t, m_iEFlags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(std::int32_t, m_MoveType, "C_BaseEntity->m_MoveType");
	SCHEMA_ADD_FIELD(std::uint8_t, m_lifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::int32_t, m_iHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(std::int32_t, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");
	SCHEMA_ADD_FIELD(float, m_flWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD(std::float_t, m_flSimulationTime, "C_BaseEntity->m_flSimulationTime");
	SCHEMA_ADD_FIELD_OFFSET(EntSubClassVDataBase*, m_pVDataBase, "C_BaseEntity->m_nSubclassID", 0x8);
};



class CGlowProperty;

class C_BaseModelEntity : public C_BaseEntity
{
public:
	//CS_CLASS_NO_INITIALIZER(C_BaseModelEntity);

	SCHEMA_ADD_FIELD(CCollisionProperty, m_Collision, "C_BaseModelEntity->m_Collision");
	SCHEMA_ADD_FIELD(CGlowProperty, m_Glow, "C_BaseModelEntity->m_Glow");
	SCHEMA_ADD_FIELD(Vector, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");
	SCHEMA_ADD_FIELD(GameTime_t, m_flCreateTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, m_nCreationTick, "C_BaseModelEntity->m_nCreationTick");
	SCHEMA_ADD_FIELD(CBaseHandle, m_hOldMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
	SCHEMA_ADD_FIELD(std::float_t, m_flAnimTime, "C_BaseModelEntity->m_flAnimTime");
	

};

class CPlayer_WeaponServices : public C_BaseModelEntity
{
public:

	SCHEMA_ADD_OFFSET(GameTime_t, m_flNextAttack, 0xB8);
	SCHEMA_ADD_FIELD(CBaseHandle, m_hActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
	SCHEMA_ADD_FIELD(uint16_t[32], m_iAmmo, "CPlayer_WeaponServices->m_iAmmo");

};


class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
	//CS_CLASS_NO_INITIALIZER(C_BasePlayerPawn);
	SCHEMA_ADD_FIELD(Vector, m_vOldOrigin, "C_BasePlayerPawn->m_vOldOrigin");

	SCHEMA_ADD_FIELD(CBaseHandle, GetControllerHandle, "C_BasePlayerPawn->m_hController");
	SCHEMA_ADD_FIELD(CPlayer_WeaponServices*, GetWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA_ADD_FIELD(CPlayer_ItemServices*, GetItemServices, "C_BasePlayerPawn->m_pItemServices");
	//SCHEMA_ADD_FIELD(CPlayer_CameraServices*, GetCameraServices, "C_BasePlayerPawn->m_pCameraServices");
	SCHEMA_ADD_FIELD(CPlayer_MovementServices*, m_pMovementServices, "C_BasePlayerPawn->m_pMovementServices");
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
	SCHEMA_ADD_FIELD(QAngle, m_angEyeAngles, "C_CSPlayerPawnBase->m_angEyeAngles");
	SCHEMA_ADD_FIELD(int, GetSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam"); // danger zone

	
};


class C_CSWeaponBase;
class CHitBoxSet;
class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	//CS_CLASS_NO_INITIALIZER(C_CSPlayerPawn);

	[[nodiscard]] bool IsEnemy(C_CSPlayerPawn* pOther);

	[[nodiscard]] int GetAssociatedTeam();
	[[nodiscard]] bool CanShoot(int tick = 0,bool forrage = true);
	[[nodiscard]] bool Visible(C_CSPlayerPawn* local);
	[[nodiscard]] bool hasArmour(const int hitgroup);
	[[nodiscard]] bool GetHitboxMinMax(int hitbox,Vector& min,Vector& max);
	[[nodiscard]] Vector GetHitBoxPos(int hitbox);
	[[nodiscard]] C_CSWeaponBase* ActiveWeapon();
	[[nodiscard]] CHitBoxSet* GetHitboxSet(int i);
	[[nodiscard]] int HitboxToWorldTransforms(CHitBoxSet* hitBoxSet, CTransform* hitboxToWorld);
	

	[[nodiscard]] Vector GetEyePosition() {

		Vector EyePosition;
		MEM::CallVFunc<void, 166>(this, &EyePosition);
		return EyePosition;
	};

	[[nodiscard]] Vector CalcEyePosition() {
		
		return m_pGameSceneNode()->m_vecOrigin() + m_vecViewOffset();
	};

	SCHEMA_ADD_OFFSET(CUtlVectorSimple< QAngle >, m_aimPunchCache, 0x14F0);
	SCHEMA_ADD_OFFSET(QAngle, m_aimPunchAngle, 0x177C);

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