#include "ragebot.h"
#include "../../CheatData.h"
#include "penetration.h"
#include "playerlog.h"

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

float calc_lerp() noexcept {

	const float cl_interp = g_interfaces->EngineCVar->Find("cl_interp")->value.fl;
	auto lerp = cl_interp / 64.f;

	if (lerp <= cl_interp)
		lerp = cl_interp;

	return lerp;
}

void RageBot::run(CUserCmd* cmd){

	cur_cmd = cmd;
	ang_ = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
	points.clear();
	noobs.clear();

	if(!CanRunRage())
		return;

	if (points.size()){
		auto eye = g_CheatData->LocalPawn->GetEyePosition();
		//eye = g_CheatData->LocalPawn->CalcEyePosition();
		QAngle shit = CalcAngle(eye,points[0].point);

		//cmd->SetSubTickAngle(shit);
		ang_ = shit;
		cmd->nButtons.nValue |= IN_ATTACK;
		cmd->nButtons.nValueChanged |= IN_ATTACK;

		cmd->csgoUserCmd.nAttack1StartHistoryIndex = 0;

		cmd->csgoUserCmd.CheckAndSetBits(ECSGOUserCmdBits::CSGOUSERCMD_BITS_ATTACK1START);

		std::cout << "tick: " << points[0].tick << std::endl;
		SetCmdTick(points[0].tick);
		
		cmd->csgoUserCmd.pBaseCmd->flForwardMove = 0.f;
		cmd->csgoUserCmd.pBaseCmd->flSideMove = 0.f;
		cmd->csgoUserCmd.pBaseCmd->flUpMove = 0.f;

		
	}
	

	RemoveRecoil();

	cmd->SetSubTickAngle(ang_);

	cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = ang_;


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


#include "../../sdk/entity/Weapon.h"
bool RageBot::FindTarget(){

	auto weapon = g_CheatData->LocalPawn->ActiveWeapon();

	if (!weapon) {
		return false;
	}

	for (auto i = 0; i < g_EntityCache->players.size(); i++){
		auto& player = g_EntityCache->players[i];
		player.Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(player.Controller->GetPawnHandle());

		if (!player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->GetHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn) /*||!player.Pawn->Visible(g_CheatData->LocalPawn)*/)
			continue;

		if (g_PlayerLog->logs[player.handle.GetEntryIndex()].record.empty())
			continue;
		
		lag_record_t backup(player.Pawn);

		g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].recover(player.Pawn);
		//目前，用计算位置扫描。用虚函数eyepos射击
		//计算位置基于oldorigin，这确保我们真的能击中目标
		//但是它不能用于角度计算，这是个十分贫民窟的解决方法，在适当的时候我会想办法调查eyepos相关的东西
		float damage = 0;
		bool canHit = false;
		F::AUTOWALL::c_auto_wall::data_t data;
		F::AUTOWALL::g_auto_wall->pen(data, g_CheatData->LocalPawn->CalcEyePosition()/*g_CheatData->net_update_end_eyepos*//*g_CheatData->LocalPawn->GetEyePosition()*/, /*player.Pawn->GetHitBoxPos(0)*/g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].matrix[6].pos, player.Controller, g_CheatData->LocalController, g_CheatData->LocalPawn, player.Pawn, weapon->datawep(), damage, canHit);
		if (data.m_can_hit && data.m_dmg >= 100.f) {
			auto& pt = points.emplace_back(/*player.Pawn->GetEyePosition()*//*player.Pawn->GetGameSceneNode()->GetSkeletonInstance()->GetModel().GetHitboxPos(HEAD)*//*player.Pawn->GetHitBoxPos(0)*/
				g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].matrix[6].pos);

			pt.tick = TIME_TO_TICKS(/*player.Pawn->GetSimulationTime()*/g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].m_flSimulationTime);
			pt.tick = ((0.5 + (float)(g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].m_flSimulationTime)) / 0.015625);

		}

		backup.recover(player.Pawn);
		//noobs.emplace_back(player);
	}

	return points.size();
}


bool RageBot::CanRunRage(){
	if (!g_interfaces->EngineClient->IsInGame() || 
		!g_interfaces->EngineClient->IsConnected() ||
		!g_CheatData->LocalController || 
		!g_CheatData->LocalPawn ||
		!g_CheatData->LocalPawn->GetHealth() ||		
		!g_CheatData->LocalPawn->CanShoot() ||
		!FindTarget()){

		return false;

	}

	return true;
}

void RageBot::SetCmdTick(int tick, int history){

	auto History = cur_cmd->GetInputHistoryEntry(history);
	History->nRenderTickCount = tick + 1;

	if (History->cl_interp) {
		History->cl_interp->nSrcTick = tick + 1;
		History->cl_interp->nDstTick = tick;
	}

	if (History->sv_interp0) {
		History->sv_interp0->nSrcTick = tick + 1;
		History->sv_interp0->nDstTick = tick;
	}

	if (History->sv_interp1) {
		History->sv_interp1->nSrcTick = tick + 1;
		History->sv_interp1->nDstTick = tick;
	}

	if (History->player_interp) {
		History->player_interp->nSrcTick = tick + 1;
		History->player_interp->nDstTick = tick;
	}


}
