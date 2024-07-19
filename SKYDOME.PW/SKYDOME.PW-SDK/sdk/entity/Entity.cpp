#include "Entity.h"


bool C_BaseEntity::IsBasePlayerController()
{
	SchemaClassInfoData_t* pClassInfo;
	GetSchemaClassInfo(&pClassInfo);
	if (pClassInfo == nullptr)
		return false;

	return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerController");
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
