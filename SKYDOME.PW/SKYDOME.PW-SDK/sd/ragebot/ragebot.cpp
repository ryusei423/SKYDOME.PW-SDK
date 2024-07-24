#include "ragebot.h"
#include "../../CheatData.h"

QAngle CalcAngle(const Vector& src, const Vector& dst) {
	QAngle vAngle;
	Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
	vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
	vAngle.roll = 0.0f;

	if (delta.x >= 0.0)
		vAngle.yaw += 180.0f;

	return vAngle;
}


void RageBot::run(CUserCmd* cmd){

	cur_cmd = cmd;
	ang_ = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
	points.clear();
	noobs.clear();

	FindTarget();

	if (points.size()){

		QAngle shit = CalcAngle(g_CheatData->LocalPawn->GetEyePosition(),points[0].point);

		//cmd->SetSubTickAngle(shit);
		ang_ = shit;
		cmd->nButtons.nValue |= IN_ATTACK;

		/*cmd->csgoUserCmd.nAttack1StartHistoryIndex = cmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1;
		cmd->csgoUserCmd.nAttack2StartHistoryIndex = cmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1;
		cmd->csgoUserCmd.nAttack3StartHistoryIndex = cmd->csgoUserCmd.inputHistoryField.nCurrentSize - 1;*/
	}

	RemoveRecoil();

	cmd->SetSubTickAngle(ang_);

	//if ((cmd->nButtons.nValue & IN_ATTACK) && cmd->csgoUserCmd.nAttack1StartHistoryIndex != -1){
	//	std::cout << "1 - >" << cmd->csgoUserCmd.nAttack1StartHistoryIndex << "\n";
	//	std::cout << "2 - >" << cmd->csgoUserCmd.nAttack2StartHistoryIndex << "\n";
	//	std::cout << "3 - >" << cmd->csgoUserCmd.nAttack3StartHistoryIndex << "\n";
	//}



}

void RageBot::RemoveRecoil(){

	auto punch = g_CheatData->LocalPawn->m_aimPunchCache();

	if (cur_cmd->nButtons.nValue & IN_ATTACK && punch.m_Size){
		ang_.pitch -= punch.m_Data[punch.m_Size - 1].pitch * 2;
		ang_.yaw -= punch.m_Data[punch.m_Size - 1].yaw * 2;
		ang_.roll -= punch.m_Data[punch.m_Size - 1].roll * 2;

		//cur_cmd->SetSubTickAngle(cur->angValue);
	}
	





}



bool RageBot::FindTarget(){
	for (auto i = 0; i < g_EntityCache->players.size(); i++){
		auto& player = g_EntityCache->players[i];
		player.Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(player.Controller->GetPawnHandle());

		if (!player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->GetHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn) ||!player.Pawn->Visible(g_CheatData->LocalPawn))
			continue;

		
		points.emplace_back(/*player.Pawn->GetEyePosition()*/player.Pawn->GetGameSceneNode()->GetSkeletonInstance()->GetModel().GetHitboxPos(HEAD));

	}

	return noobs.size();
}
