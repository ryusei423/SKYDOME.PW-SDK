#pragma once

// used: schema field
//#include "../../utilities/memory.h"


#include "entity/CBaseHandle.h"

//OffsetManager.h没有任何包含，可能这让它能被自由地使用
//不过某天可能会出问题...
#include "../OffsetManager.h"

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64 // 0x3F
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST* MAX_ENTITY_LISTS

class C_BaseEntity;

class CGameEntitySystem
{
public:
	/// GetClientEntity
	template <typename T = C_BaseEntity>
	T* Get(int nIndex)
	{
		return reinterpret_cast<T*>(this->GetEntityByIndex(nIndex));
	}

	/// GetClientEntityFromHandle
	template <typename T = C_BaseEntity>
	T* Get(const CBaseHandle hHandle)
	{
		if (!hHandle.IsValid())
			return nullptr;

		return reinterpret_cast<T*>(this->GetEntityByIndex(hHandle.GetEntryIndex()));
	}

	__forceinline int GetHighestEntityIndex()
	{
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + cs2_dumper::offsets::client_dll::dwGameEntitySystem_highestEntityIndex);
	}

private:
	void* GetEntityByIndex(int nIndex)
	{
		//@ida: #STR: "(missing),", "(missing)", "Ent %3d: %s class %s name %s\n" | or find "cl_showents" cvar -> look for callback
		//	do { pEntity = GetBaseEntityByIndex(g_pGameEntitySystem, nCurrentIndex); ... }
		using fnGetBaseEntity = void*(__thiscall*)(void*, int);
		static auto GetBaseEntity = reinterpret_cast<fnGetBaseEntity>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_ENTITY_BY_INDEX]);
		return GetBaseEntity(this, nIndex);
	}
};


class CGameEntitySystem;

class IGameResourceService
{
public:
	MEM_PAD(0x58);
	CGameEntitySystem* pGameEntitySystem;
};