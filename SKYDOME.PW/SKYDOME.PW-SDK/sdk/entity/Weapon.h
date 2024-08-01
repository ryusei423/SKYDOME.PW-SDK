#include "Entity.h"


enum CSWeaponType : std::uint32_t {
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL = 1,
	WEAPONTYPE_SUBMACHINEGUN = 2,
	WEAPONTYPE_RIFLE = 3,
	WEAPONTYPE_SHOTGUN = 4,
	WEAPONTYPE_SNIPER_RIFLE = 5,
	WEAPONTYPE_MACHINEGUN = 6,
	WEAPONTYPE_C4 = 7,
	WEAPONTYPE_TASER = 8,
	WEAPONTYPE_GRENADE = 9,
	WEAPONTYPE_EQUIPMENT = 10,
	WEAPONTYPE_STACKABLEITEM = 11,
	WEAPONTYPE_FISTS = 12,
	WEAPONTYPE_BREACHCHARGE = 13,
	WEAPONTYPE_BUMPMINE = 14,
	WEAPONTYPE_TABLET = 15,
	WEAPONTYPE_MELEE = 16,
	WEAPONTYPE_SHIELD = 17,
	WEAPONTYPE_ZONE_REPULSOR = 18,
	WEAPONTYPE_UNKNOWN = 19,
};

class C_EconEntity : public C_BaseModelEntity {
public:
	//CS_CLASS_NO_INITIALIZER(C_EconEntity);
	/* offset */
	//SCHEMA_ADD_FIELD(CAttributeManager, m_AttributeManager, "C_EconEntity->m_AttributeManager");
	SCHEMA_ADD_FIELD(uint32_t, m_OriginalOwnerXuidLow, "C_EconEntity->m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(uint32_t, m_OriginalOwnerXuidHigh, "C_EconEntity->m_OriginalOwnerXuidHigh");
	SCHEMA_ADD_OFFSET(bool, m_bShouldGlow, 0x15B8);

	uint64_t GetOriginalOwnerXuid() {
		return ((uint64_t)(m_OriginalOwnerXuidHigh()) << 32) |
			m_OriginalOwnerXuidLow();
	}

};

class BasePlayerWeaponVData : public EntSubClassVDataBase {
public:
	SCHEMA_ADD_OFFSET(int32_t, m_iMaxClip1, 0x1FC);

};

class CBasePlayerWeapon : public C_EconEntity {
public:
	//CS_CLASS_NO_INITIALIZER(CBasePlayerWeapon);

	SCHEMA_ADD_FIELD(GameTick_t, m_nNextPrimaryAttackTick, "C_BasePlayerWeapon->m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(float, m_flNextPrimaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextPrimaryAttackTickRatio");
	SCHEMA_ADD_FIELD(GameTick_t, m_nNextSecondaryAttackTick, "C_BasePlayerWeapon->m_nNextSecondaryAttackTick");
	SCHEMA_ADD_FIELD(float, m_flNextSecondaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextSecondaryAttackTickRatio");
	SCHEMA_ADD_OFFSET(float, m_flRecoilIndex, 0x1734);

	SCHEMA_ADD_FIELD(int32_t, clip1, "C_BasePlayerWeapon->m_iClip1");
	BasePlayerWeaponVData* GetVData() noexcept {
		return m_pVDataBase()->as<BasePlayerWeaponVData>();
	}
};



enum CSWeaponCategory : std::uint32_t {
	WEAPONCATEGORY_OTHER = 0,
	WEAPONCATEGORY_MELEE = 1,
	WEAPONCATEGORY_SECONDARY = 2,
	WEAPONCATEGORY_SMG = 3,
	WEAPONCATEGORY_RIFLE = 4,
	WEAPONCATEGORY_HEAVY = 5,
	WEAPONCATEGORY_COUNT = 6,
};
class CFiringModeFloat
{
public:
	float flValue[2];
};

class CCSWeaponBaseVData : public BasePlayerWeaponVData {
public:
	SCHEMA_ADD_OFFSET(const char*, m_szName, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_szName);
	SCHEMA_ADD_OFFSET(uint32_t, m_WeaponType, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_WeaponType);
	SCHEMA_ADD_OFFSET(CSWeaponCategory, m_WeaponCategory, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_WeaponCategory);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flSpread, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flSpread);
	SCHEMA_ADD_OFFSET(float, m_flHeadshotMultiplier, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flHeadshotMultiplier);
	SCHEMA_ADD_OFFSET(float, m_flArmorRatio, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flArmorRatio);
	SCHEMA_ADD_OFFSET(float, m_flPenetration, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flPenetration);
	SCHEMA_ADD_OFFSET(float, m_flRange, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flRange);
	SCHEMA_ADD_OFFSET(float, m_flRangeModifier, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flRangeModifier);
	SCHEMA_ADD_OFFSET(bool, m_bIsFullAuto, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_bIsFullAuto);

	SCHEMA_ADD_OFFSET(int32_t, m_nNumBullets, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_nNumBullets);

	SCHEMA_ADD_OFFSET(int32_t, m_nDamage, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_nDamage);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flInaccuracyCrouch, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flInaccuracyCrouch);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flInaccuracyStand, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flInaccuracyStand);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flInaccuracyJump, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flInaccuracyJump);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flInaccuracyMove, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flInaccuracyMove);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flCycleTime, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flCycleTime);
	SCHEMA_ADD_OFFSET(CFiringModeFloat, m_flMaxSpeed, cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_flMaxSpeed);


};

enum CSWeaponMode : int /* it works atleast */
{
	Primary_Mode = 0,
	Secondary_Mode = 1,
	WeaponMode_MAX = 2,
};

class C_CSWeaponBase : public CBasePlayerWeapon {
public:
	void AddStattrakEntity();
	void AddNametagEntity();
	SCHEMA_ADD_OFFSET(CCSWeaponBaseVData*, datawep, /*0x368*/0x360);
	//SCHEMA_ADD_OFFSET(int32_t, m_zoomLevel, 0x1A70);
	//SCHEMA_ADD_OFFSET(bool, m_bInReload, 0x1744);
	SCHEMA_ADD_FIELD(int32_t, m_zoomLevel, "C_CSWeaponBaseGun->m_bInReload");
	SCHEMA_ADD_FIELD(bool, m_bInReload, "C_CSWeaponBase->m_bInReload");

	SCHEMA_ADD_FIELD(float, m_fAccuracyPenalty, "C_CSWeaponBase->m_fAccuracyPenalty");
	SCHEMA_ADD_FIELD(CSWeaponMode, m_weaponMode, "C_CSWeaponBase->m_weaponMode");
	SCHEMA_ADD_FIELD(int, m_iOriginalTeamNumber, "C_CSWeaponBase->m_iOriginalTeamNumber");

	SCHEMA_ADD_FIELD(bool, m_bUIWeapon, "C_CSWeaponBase->m_bUIWeapon");
	//SCHEMA_ADD_FIELD(void, m_hStattrakAttachment, CS_XOR("C_CSWeaponBase->m_iNumEmptyAttacks"));
	//SCHEMA_ADD_FIELD(void, m_hNametagAttachment, CS_XOR("C_CSWeaponBase->m_iNumEmptyAttacks"));

	__forceinline float get_spread() {
		/*using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[349](this);*/

		return MEM::CallVFunc<float, 357>(this);
	}

	__forceinline float get_inaccuracy() {
		/*using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[399](this);*/
		float x = 0.0f, y = 0.0f;
		return MEM::CallVFunc<float,403>(this,&x,&y);

	}

	// for reference.    
	// sv_accelerate_use_weapon_speed inside "CGameMovement::Accelerate"
	// 48 8B C4 F3 0F 11 50 ? 41 55 41 56 41 57 48 81 EC ? ? ? ? F2 41 0F 10 21 4D 8B E9 F2 0F 10 5A ? 0F 28 C4 44 0F 29 40 ? 0F 28 CB F3 44 0F 10 42 ? 4C 8B FA F3 45 0F 59 41 ? 
	// mov     rcx, rbx
	// call    qword ptr [ rax + 0AF8h ] <- index
	// mov     rax, [ rbx ]
	__forceinline float get_max_speed() {
		/*using original_fn = float(__fastcall*)(void*);
		return (*(original_fn**)this)[340](this);*/

		return MEM::CallVFunc<float, 348>(this);
	}

};