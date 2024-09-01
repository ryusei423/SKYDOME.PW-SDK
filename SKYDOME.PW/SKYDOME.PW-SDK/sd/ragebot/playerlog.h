#pragma once
#include<deque>
#include "../../sdk/hitbox/hitbox.h"
#include "../entitycache/entitycache.h"
struct CACHE_HITBOX{
	Vector min, max;
	bool need_multpoint;
};


class lag_record_t {
public:
	lag_record_t(C_CSPlayerPawn* pawn) {
		has_hitbox = false;

		m_flSimulationTime = pawn->m_flSimulationTime();
		m_nSimulationTick = pawn->m_nSimulationTick();
		m_angEyeAngles = pawn->m_angEyeAngles();

		auto scenenode = pawn->m_pGameSceneNode();

		m_vRenderOrigin = scenenode->m_vRenderOrigin();
		m_vecAbsOrigin = scenenode->m_vecAbsOrigin();
		m_vecOrigin = scenenode->m_vecOrigin();

		m_angRotation = scenenode->m_angRotation();
		m_angAbsRotation = scenenode->m_angAbsRotation();

		CSkeletonInstance* skeleton = scenenode->GetSkeletonInstance();
		if (!skeleton)
			return;

		skeleton->CalculateWorldSpaceBones(FLAG_HITBOX);

		bone_count = skeleton->get_bone_count();
		if (skeleton->GetModel().GetBoneData() && bone_count && skeleton->m_pBoneCache){
			memcpy(&matrix, skeleton->GetModel().GetBoneData(), bone_count * sizeof(bone_data));
			memcpy(&bone_cache, skeleton->m_pBoneCache, bone_count * sizeof(Matrix4x2_t));
			has_hitbox = MakeHitboxList(pawn);
		}

		
	}
	void recover(C_CSPlayerPawn* pawn) {
		pawn->m_angEyeAngles() = m_angEyeAngles;

		auto scenenode = pawn->m_pGameSceneNode();

		scenenode->m_vRenderOrigin() = m_vRenderOrigin;
		scenenode->m_vecAbsOrigin() = m_vecAbsOrigin;
		scenenode->m_vecOrigin() = m_vecOrigin;

		scenenode->m_angRotation() = m_angRotation;
		scenenode->m_angAbsRotation() = m_angAbsRotation;

		CSkeletonInstance* skeleton = scenenode->GetSkeletonInstance();
		if (!skeleton)
			return;

		if (skeleton->GetModel().GetBoneData() && bone_count && skeleton->m_pBoneCache){
			memcpy(skeleton->GetModel().GetBoneData(), &matrix, bone_count * sizeof(bone_data));
			memcpy(skeleton->m_pBoneCache, &bone_cache, bone_count * sizeof(Matrix4x2_t));
		}
		

	};


	bool IsValid();
	bool MakeHitboxList(C_CSPlayerPawn* pawn);

	Vector m_vecOrigin, m_vecAbsOrigin, m_vRenderOrigin;
	QAngle m_angRotation, m_angAbsRotation, m_angEyeAngles;

	int bone_count;
	bone_data matrix[128];
	Matrix4x2_t bone_cache[128];
	float m_flSimulationTime;
	int m_nSimulationTick;

	bool has_hitbox;
	std::vector<CACHE_HITBOX>hitboxs;
};

class PlayerLog_t {
public:
	bool valid;
	bool alive;
	CachedEntity player;
	std::deque<lag_record_t> record;
};

class PlayerLog {
public:
	void Log();
	void FilterRecords();





	PlayerLog_t logs[65];
};

inline PlayerLog* g_PlayerLog = new PlayerLog;