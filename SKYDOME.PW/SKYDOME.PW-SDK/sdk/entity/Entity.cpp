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

//通过eyepos进行检查可见性
//注意它无法穿透玻璃，以及不是那么全面
bool C_CSPlayerPawn::Visible(C_CSPlayerPawn* local)
{
	trace_filter_t filter = {};
	g_interfaces->Trace->Init(filter, local, 0x1C3003, 4, 7);

	game_trace_t trace = {};
	ray_t ray = {};

	Vector start_eye = local->GetEyePosition();
	Vector end_eye = this->GetEyePosition();

	g_interfaces->Trace->TraceShape(ray, &start_eye, &end_eye, filter, trace);

	return trace.HitEntity && trace.HitEntity->GetRefEHandle().GetEntryIndex() == this->GetRefEHandle().GetEntryIndex() || trace.Fraction > 0.97f;


};