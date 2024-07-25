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


}

bool C_CSPlayerPawn::GetHitboxMinMax(int i,Vector& min, Vector& max){
	constexpr int MAX_HITBOXES = 64;

	CHitBoxSet* hitboxSet = GetHitboxSet(0);
	if (!hitboxSet) return false;

	CHitBox* hitbox = &hitboxSet->m_HitBoxes()[i];
	if (!hitbox) return false;

	CTransform hitBoxTransforms[MAX_HITBOXES];
	int hitBoxCount = HitboxToWorldTransforms(hitboxSet, hitBoxTransforms);
	if (!hitBoxCount) return false;

	const Matrix3x4_t hitBoxMatrix = hitBoxTransforms[i].ToMatrix();
	Vector worldMins, worldMaxs;

	MATH::TransformAABB(hitBoxMatrix, hitbox->m_vMinBounds(), hitbox->m_vMaxBounds(), worldMins, worldMaxs);
	
	min = worldMins;
	max = worldMaxs;
	
	return true;
}

Vector C_CSPlayerPawn::GetHitBoxPos(int hitbox)
{
	Vector min, max;
	if(GetHitboxMinMax(hitbox,min,max))
	return (min + max) * 0.5;

	return Vector(0,0,0);
}


CHitBoxSet* C_CSPlayerPawn::GetHitboxSet(int i){
	using fnGetHitboxSet = CHitBoxSet*(__fastcall*)(void* ,int);
	return reinterpret_cast<fnGetHitboxSet>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_HITBOX_SET])(this,i);
}

int C_CSPlayerPawn::HitboxToWorldTransforms(CHitBoxSet* hitBoxSet, CTransform* hitboxToWorld){
	using fnHitboxToWorldTransforms =  int(__fastcall*)(void*, CHitBoxSet*, CTransform*, int);
	return reinterpret_cast<fnHitboxToWorldTransforms>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_HITBOX_TO_WORLD_TRANSFORMS])(this,hitBoxSet,hitboxToWorld,1024);
}
