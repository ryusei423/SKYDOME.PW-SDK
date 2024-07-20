#include "Entity.h"


bool C_BaseEntity::IsBasePlayerController()
{
	SchemaClassInfoData_t* pClassInfo;
	GetSchemaClassInfo(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("CCSPlayerController");
}

bool C_BaseEntity::IsWeapon()
{
	static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
	if (pWeaponBaseClass == nullptr)
		g_interfaces->SchemaSystem->FindTypeScopeForModule(XorStr("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, XorStr("C_CSWeaponBase"));


	SchemaClassInfoData_t* pClassInfo;
	GetSchemaClassInfo(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return (pClassInfo->InheritsFrom(pWeaponBaseClass));
}

static Vector vecEmpty = Vector(0, 0, 0);

const Vector& C_BaseEntity::GetSceneOrigin()
{
	if (this->GetGameSceneNode())
		return GetGameSceneNode()->GetAbsOrigin();

	return vecEmpty;
}


const Vector& CCSPlayerController::GetPawnOrigin()
{
	CBaseHandle hPawn = this->GetPawnHandle();
	if (!hPawn.IsValid())
		return vecEmpty;

	C_CSPlayerPawn* pPawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(hPawn);
	if (pPawn == nullptr)
		return vecEmpty;

	return pPawn->GetSceneOrigin();
}
