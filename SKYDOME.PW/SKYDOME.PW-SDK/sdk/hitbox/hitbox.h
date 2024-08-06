#pragma once
#include "../entity/Entity.h"
#include "../datatypes/cstronghandle.hpp"

class CTransform;

//hitbox index : 0 hitgroup : 1 bone index in list : 6 name : head_0
//hitbox index : 1 hitgroup : 8 bone index in list : 5 name : neck_0
//hitbox index : 2 hitgroup : 3 bone index in list : 0 name : pelvis
//hitbox index : 3 hitgroup : 3 bone index in list : 1 name : spine_0
//hitbox index : 4 hitgroup : 2 bone index in list : 2 name : spine_1
//hitbox index : 5 hitgroup : 2 bone index in list : 3 name : spine_2
//hitbox index : 6 hitgroup : 2 bone index in list : 4 name : spine_3
//hitbox index : 7 hitgroup : 6 bone index in list : 22 name : leg_upper_l
//hitbox index : 8 hitgroup : 7 bone index in list : 25 name : leg_upper_r
//hitbox index : 9 hitgroup : 6 bone index in list : 23 name : leg_lower_l
//hitbox index : 10 hitgroup : 7 bone index in list : 26 name : leg_lower_r
//hitbox index : 11 hitgroup : 6 bone index in list : 24 name : ankle_l
//hitbox index : 12 hitgroup : 7 bone index in list : 27 name : ankle_r
//hitbox index : 13 hitgroup : 4 bone index in list : 10 name : hand_l
//hitbox index : 14 hitgroup : 5 bone index in list : 15 name : hand_r
//hitbox index : 15 hitgroup : 4 bone index in list : 8 name : arm_upper_l
//hitbox index : 16 hitgroup : 4 bone index in list : 9 name : arm_lower_l
//hitbox index : 17 hitgroup : 5 bone index in list : 13 name : arm_upper_r
//hitbox index : 18 hitgroup : 5 bone index in list : 14 name : arm_lower_r

enum HITBOXES : uint32_t {
	HEAD = 6,
	NECK = 5,
	CHEST = 4,
	RIGHT_CHEST = 8,
	LEFT_CHEST = 13,
	STOMACH = 3,
	PELVIS = 2,
	CENTER = 1,
	L_LEG = 23,
	L_FEET = 24,
	R_LEG = 26,
	R_FEET = 27
};

enum bone_flags : uint32_t {
	FLAG_NO_BONE_FLAGS = 0x0,
	FLAG_BONEFLEXDRIVER = 0x4,
	FLAG_CLOTH = 0x8,
	FLAG_PHYSICS = 0x10,
	FLAG_ATTACHMENT = 0x20,
	FLAG_ANIMATION = 0x40,
	FLAG_MESH = 0x80,
	FLAG_HITBOX = 0x100,
	FLAG_BONE_USED_BY_VERTEX_LOD0 = 0x400,
	FLAG_BONE_USED_BY_VERTEX_LOD1 = 0x800,
	FLAG_BONE_USED_BY_VERTEX_LOD2 = 0x1000,
	FLAG_BONE_USED_BY_VERTEX_LOD3 = 0x2000,
	FLAG_BONE_USED_BY_VERTEX_LOD4 = 0x4000,
	FLAG_BONE_USED_BY_VERTEX_LOD5 = 0x8000,
	FLAG_BONE_USED_BY_VERTEX_LOD6 = 0x10000,
	FLAG_BONE_USED_BY_VERTEX_LOD7 = 0x20000,
	FLAG_BONE_MERGE_READ = 0x40000,
	FLAG_BONE_MERGE_WRITE = 0x80000,
	FLAG_ALL_BONE_FLAGS = 0xfffff,
	BLEND_PREALIGNED = 0x100000,
	FLAG_RIGIDLENGTH = 0x200000,
	FLAG_PROCEDURAL = 0x400000,
};

struct alignas(16) bone_data {
	Vector pos;
	float scale;
	Vector4D_t rot;
};


class c_drawcalls
{
public:
	__int32 m_nPrimitiveType; // 0x0000
	__int32 m_nBaseVertex;	  // 0x0004
	__int32 m_nVertexCount;	  // 0x0008
	__int32 m_nStartIndex;	  // 0x000C
	__int32 m_nIndexCount;	  // 0x0010
	float	m_flUvDensity;	  // 0x0014
	float	m_vTintColor[3];  // 0x0018
	float	m_flAlpha;		  // 0x0024
	char	pad_0x0028[0xC0]; // 0x0028

}; // Size=0x00E8

class c_bones
{
public:
	const char* m_boneName;		   // 0x0000
	const char* m_parentName;	   // 0x0008
	float		m_invBindPose[12]; // 0x0010
	Vector		m_vecCenter;	   // 0x0040
	Vector		m_vecSize;		   // 0x004C
	float		m_flSphereradius;  // 0x0058
	char		pad_0x005C[0x4];   // 0x005C

}; // Size=0x0060


class CRenderMesh
{

public:

	char		   pad_0x0000[40];	// 0x0000
	Vector		   m_vMinBounds;		// 0x0028
	Vector		   m_vMaxBounds;		// 0x0034
	char		   pad_0x0044[8];		// 0x0044
	c_drawcalls* m_drawCalls;			// 0x0048
	char		   pad_0x0050[104];	// 0x0050
	__int32		   m_skeletoncount;		// 0x00B8
	char		   pad_0x00BC[4];		// 0x00BC
	c_bones* m_skeleton;			// 0x00C0
	char		   pad_0x00C8[112];		// 0x00C8


};
class CRenderM {
public:
	CRenderMesh* mesh;
};


//Center = (min + max) / 2
class CHitBox {
public:
	//SCHEMA_ADD_FIELD(Vector, m_vMinBounds, "CHitBox->m_vMinBounds");
	//SCHEMA_ADD_FIELD(Vector, m_vMaxBounds, "CHitBox->m_vMaxBounds");
	SCHEMA_ADD_OFFSET(Vector, m_vMinBounds, cs2_dumper::schemas::animationsystem_dll::CHitBox::m_vMinBounds);
	SCHEMA_ADD_OFFSET(Vector, m_vMaxBounds, cs2_dumper::schemas::animationsystem_dll::CHitBox::m_vMaxBounds);
	

private:
	// Size of 'CHitBox' class. Can be obtainted through the SchemaSystem.
	// Must have this here or we can't iterate the 'm_HitBoxes' vector that stores
	// CHitBox directly and not by a pointer.
	char pad[0x70];
};

class CHitBoxSet {
public:
	//SCHEMA_ADD_FIELD(CUtlVector<CHitBox>, m_HitBoxes, "CHitBoxSet->m_HitBoxes");
	SCHEMA_ADD_OFFSET(CUtlVector<CHitBox>, m_HitBoxes, cs2_dumper::schemas::animationsystem_dll::CHitBoxSet::m_HitBoxes);
};


class CModelSkeleton
{
public:
	CModelSkeleton() = delete;
	CModelSkeleton(CModelSkeleton&&) = delete;
	CModelSkeleton(const CModelSkeleton&) = delete;

public:
	/*SCHEMA_ADD_OFFSET(CUtlVector<const char*>, vecBoneNames, 0x4);
	SCHEMA_ADD_OFFSET(CUtlVector<std::uint16_t>, vecBoneParent, 0x18);
	SCHEMA_ADD_OFFSET(CUtlVector<float>, m_boneSphere, 0x30);
	SCHEMA_ADD_OFFSET(CUtlVector<std::uint32_t>, m_nFlag, 0x48);
	SCHEMA_ADD_OFFSET(CUtlVector<Vector>, m_bonePosParent, 0x60);
	SCHEMA_ADD_OFFSET(CUtlVector<QuaternionAligned_t>, m_boneRotParent, 0x78);
	SCHEMA_ADD_OFFSET(CUtlVector<float>, m_boneScaleParent, 0x90);*/

	SCHEMA_ADD_OFFSET(CUtlVector<const char*>, vecBoneNames, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_boneName);
	SCHEMA_ADD_OFFSET(CUtlVector<std::uint16_t>, vecBoneParent, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_nParent);
	SCHEMA_ADD_OFFSET(CUtlVector<float>, m_boneSphere, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_boneSphere);
	SCHEMA_ADD_OFFSET(CUtlVector<std::uint32_t>, m_nFlag, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_nFlag);
	SCHEMA_ADD_OFFSET(CUtlVector<Vector>, m_bonePosParent, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_bonePosParent);
	SCHEMA_ADD_OFFSET(CUtlVector<QuaternionAligned_t>, m_boneRotParent, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_boneRotParent);
	SCHEMA_ADD_OFFSET(CUtlVector<float>, m_boneScaleParent, cs2_dumper::schemas::animationsystem_dll::ModelSkeletonData_t::m_boneScaleParent);

};


class CModel {
public:
	CModel() = delete;
	CModel(CModel&&) = delete;
	CModel(const CModel&) = delete;
public:

	/*SCHEMA_ADD_OFFSET(const char*, szName, 0x8);
	SCHEMA_ADD_OFFSET(CModelSkeleton, m_modelSkeleton, 0x188);
	SCHEMA_ADD_OFFSET(CRenderM*, m_meshes, 0x78);*/

	SCHEMA_ADD_OFFSET(const char*, szName, cs2_dumper::schemas::animationsystem_dll::PermModelData_t::m_name);
	SCHEMA_ADD_OFFSET(CModelSkeleton, m_modelSkeleton, cs2_dumper::schemas::animationsystem_dll::PermModelData_t::m_modelSkeleton);
	SCHEMA_ADD_OFFSET(CRenderM*, m_meshes, cs2_dumper::schemas::animationsystem_dll::PermModelData_t::m_refMeshes);


	uint32_t GetHitboxFlags(uint32_t index);
	const char* GetHitboxName(uint32_t index);
	uint32_t GetHitboxParent(uint32_t index);

	uint32_t GetHitboxesNum();

};



class CModelState
{
public:
	CModelState() = delete;
	CModelState(CModelState&&) = delete;
	CModelState(const CModelState&) = delete;

	SCHEMA_ADD_FIELD(CStrongHandle< CModel >, m_hModel, "CModelState->m_hModel");
	SCHEMA_ADD_OFFSET(CTransform*, BoneTransform, 0x80);

	bone_data* GetBoneData() noexcept {
		bone_data* boneDataPtr = *reinterpret_cast<bone_data**>(this + 0x80);

		if (boneDataPtr == nullptr)
			boneDataPtr = *reinterpret_cast<bone_data**>(reinterpret_cast<uintptr_t>(this) + 0x80);

		//SD_ASSERT(boneDataPtr != nullptr);

		return boneDataPtr;
	}


	Vector GetBonePos(uint32_t index) {
		auto hitbox = this->GetBoneData();
		if (!hitbox)
			return nullptr;

		if (!(this->m_hModel()->GetHitboxFlags(index) & bone_flags::FLAG_HITBOX))
			return nullptr;

		auto parent_index = this->m_hModel()->GetHitboxParent(index);
		if (parent_index == -1)
			return nullptr;

		return hitbox[index].pos;
	}

	Vector4D_t GetBoneRotation(uint32_t index) {
		auto hitbox = this->GetBoneData();
		if (!hitbox)
			return 0;

		if (!(this->m_hModel()->GetHitboxFlags(index) & bone_flags::FLAG_HITBOX))
			return 0;

		auto parent_index = this->m_hModel()->GetHitboxParent(index);
		if (parent_index == -1)
			return 0;

		return hitbox[index].rot;
	}

	const char* GetBoneName(uint32_t index) {
		auto hitbox = this->GetBoneData();
		if (!hitbox)
			return nullptr;

		if (!(this->m_hModel()->GetHitboxFlags(index) & bone_flags::FLAG_HITBOX))
			return nullptr;

		auto parent_index = this->m_hModel()->GetHitboxParent(index);
		if (parent_index == -1)
			return nullptr;

		return this->m_hModel()->GetHitboxName(index);
	}
};



class CSkeletonInstance
{
public:
	CSkeletonInstance() = delete;
	CSkeletonInstance(CSkeletonInstance&&) = delete;
	CSkeletonInstance(const CSkeletonInstance&) = delete;

	SCHEMA_ADD_FIELD(CModelState, GetModel, "CSkeletonInstance->m_modelState");
	SCHEMA_ADD_FIELD(bool, m_bIsAnimationEnabled, "CSkeletonInstance->m_bIsAnimationEnabled");
	SCHEMA_ADD_FIELD(bool, m_bUseParentRenderBounds, "CSkeletonInstance->m_bUseParentRenderBounds");
	SCHEMA_ADD_FIELD(bool, m_bDisableSolidCollisionsForHierarchy, "CSkeletonInstance->m_bDisableSolidCollisionsForHierarchy");
	SCHEMA_ADD_FIELD(bool, m_bDirtyMotionType, "CSkeletonInstance->m_bDirtyMotionType");
	SCHEMA_ADD_FIELD(bool, m_bIsGeneratingLatchedParentSpaceState, "CSkeletonInstance->m_bIsGeneratingLatchedParentSpaceState");
	SCHEMA_ADD_FIELD(uint8_t, m_nHitboxSet, "CSkeletonInstance->m_nHitboxSet");
	
	std::uint32_t get_bone_count(){
		return *reinterpret_cast<std::uint32_t*>(std::uintptr_t(this) + 0x1CC);
	}

	MEM_PAD(0x1CC); //0x0000
	int m_nBoneCount; //0x01CC
	MEM_PAD(0x18); //0x01D0
	int m_nMask; //0x01E8
	MEM_PAD(0x4); //0x01EC 
	Matrix4x2_t* m_pBoneCache; //0x01F0

	void CalculateWorldSpaceBones(uint32_t flags);
	/*void get_bone_data(bone_data& data, int index);
	void CS_FASTCALL calc_world_space_bones(uint32_t parent, uint32_t mask);
	void CS_FASTCALL spoofed_calc_world_space_bones(uint32_t mask);*/
};