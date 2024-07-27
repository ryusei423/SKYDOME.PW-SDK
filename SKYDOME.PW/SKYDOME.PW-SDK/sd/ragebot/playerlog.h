#pragma once
#include<deque>
#include "../../sdk/hitbox/hitbox.h"

class lag_record_t {
public:
	lag_record_t(C_CSPlayerPawn* pawn) {
		m_flSimulationTime = pawn->GetSimulationTime();
		bone_count = pawn->GetGameSceneNode()->GetSkeletonInstance()->get_bone_count();

		if (pawn->GetGameSceneNode()->GetSkeletonInstance()->GetModel().GetBoneData() && bone_count && pawn->GetGameSceneNode()->GetSkeletonInstance()->m_pBoneCache){
			memcpy(&matrix, pawn->GetGameSceneNode()->GetSkeletonInstance()->GetModel().GetBoneData(), bone_count * sizeof(bone_data));
			memcpy(&bone_cache, pawn->GetGameSceneNode()->GetSkeletonInstance()->m_pBoneCache, bone_count * sizeof(Matrix4x2_t));
		}
		
	}
	void recover(C_CSPlayerPawn* pawn) {
		if (bone_count){
			memcpy(pawn->GetGameSceneNode()->GetSkeletonInstance()->GetModel().GetBoneData(), &matrix, bone_count * sizeof(bone_data));
			memcpy(pawn->GetGameSceneNode()->GetSkeletonInstance()->m_pBoneCache, &bone_cache, bone_count * sizeof(Matrix4x2_t));
		}
		
	};

	int bone_count;
	bone_data matrix[128];
	Matrix4x2_t bone_cache[128];
	float m_flSimulationTime;

};

class PlayerLog_t {
public:
	std::deque<lag_record_t> record;
};

class PlayerLog {
public:
	void Log();






	PlayerLog_t logs[65];
};

inline PlayerLog* g_PlayerLog = new PlayerLog;