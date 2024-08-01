#include "ragebot.h"
#include "../../CheatData.h"
#include "penetration.h"
#include "playerlog.h"
#include "rbhelper.h"

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

//float calc_lerp() noexcept {
//
//	
//
//
//	const float cl_interp = MEM::CallFunc<float>(g_OffsetManager->offsets[g_OffsetManager->OFFSET_GET_CLIENT_INTERP]);
//	auto lerp = cl_interp / 64.f;
//
//	if (lerp <= cl_interp)
//		lerp = cl_interp;
//
//	return lerp;
//}

void RageBot::run(CUserCmd* cmd){

	cur_cmd = cmd;
	ang_ = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
	points.clear();
	noobs.clear();

	if(!CanRunRage())
		return;

	if (points.size()){

		auto eye = g_CheatData->LocalPawn->GetEyePosition();
		auto calc_eye = g_CheatData->LocalPawn->CalcEyePosition();
		QAngle shit = CalcAngle(calc_eye/*(eye + calc_eye) / 2*/, points[0].point);
		if (g_RageBotHelper->Hitchance(g_CheatData->LocalPawn,
			points[0].player.Pawn,
			g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
			shit,
			100.f)) {
		
		
			//cmd->SetSubTickAngle(shit);
			ang_ = shit;
			cmd->nButtons.nValue |= IN_ATTACK;
			cmd->nButtons.nValueChanged |= IN_ATTACK;

			cmd->csgoUserCmd.nAttack1StartHistoryIndex = 0;

			cmd->csgoUserCmd.CheckAndSetBits(ECSGOUserCmdBits::CSGOUSERCMD_BITS_ATTACK1START);

			SetCmdTick(points[0].tick,points[0].player.Pawn);

		}
		else {
			g_RageBotHelper->AutomaticStop(g_CheatData->LocalPawn,
				g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
				cur_cmd);
		}
		
	}
	

	RemoveRecoil();

	cmd->SetSubTickAngle(ang_);

	cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = ang_;


}

void RageBot::DumpCmdInfo(CUserCmd* cmd){
	LOG(INFO) << "----------------------------------";
	LOG(DEBUG) << "cmd->csgoUserCmd.pBaseCmd->nClientTick = " << std::dec << cmd->csgoUserCmd.pBaseCmd->nClientTick;
	LOG(DEBUG) << "cmd->csgoUserCmd.nAttack1StartHistoryIndex = " << std::dec << cmd->csgoUserCmd.nAttack1StartHistoryIndex;
	LOG(DEBUG) << "cmd->csgoUserCmd.nAttack2StartHistoryIndex = " << std::dec << cmd->csgoUserCmd.nAttack2StartHistoryIndex;
	LOG(DEBUG) << "cmd->csgoUserCmd.nAttack3StartHistoryIndex = " << std::dec << cmd->csgoUserCmd.nAttack3StartHistoryIndex;

	LOG(DEBUG) << "cmd->csgoUserCmd.inputHistoryField.pRep->nAllocatedSize = " << std::dec << cmd->csgoUserCmd.inputHistoryField.pRep->nAllocatedSize;
	LOG(DEBUG) << "cmd->csgoUserCmd.inputHistoryField.nCurrentSize = " << std::dec << cmd->csgoUserCmd.inputHistoryField.nCurrentSize;

	for (int i = 0; i < cmd->csgoUserCmd.inputHistoryField.pRep->nAllocatedSize; i++){
		CCSGOInputHistoryEntryPB* pInputEntry = cmd->GetInputHistoryEntry(i);
		if (!pInputEntry || !pInputEntry->pViewAngles) {
			LOG(DEBUG) << "无效的输入记录 -> " << std::dec << i;
			continue;
		}
			
		LOG(DEBUG) << i << " -> pInputEntry->nRenderTickCount = " << std::dec << pInputEntry->nRenderTickCount;
		LOG(DEBUG) << i << " -> pInputEntry->flRenderTickFraction = " << pInputEntry->flRenderTickFraction;
		LOG(DEBUG) << i << " -> pInputEntry->nPlayerTickCount = " << std::dec << pInputEntry->nPlayerTickCount;
		LOG(DEBUG) << i << " -> pInputEntry->flPlayerTickFraction = " << pInputEntry->flPlayerTickFraction;
		if (pInputEntry->cl_interp) {
			LOG(DEBUG) << i << " -> pInputEntry->cl_interp->nSrcTick = " << std::dec << pInputEntry->cl_interp->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->cl_interp->nDstTick = " << std::dec << pInputEntry->cl_interp->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->cl_interp->flFraction = " << pInputEntry->cl_interp->flFraction;
		}

		if (pInputEntry->sv_interp0) {
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp0->nSrcTick = " << std::dec << pInputEntry->sv_interp0->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp0->nDstTick = " << std::dec << pInputEntry->sv_interp0->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp0->flFraction = " << pInputEntry->cl_interp->flFraction;
		}

		if (pInputEntry->sv_interp1) {
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->nSrcTick = " << std::dec << pInputEntry->sv_interp1->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->nDstTick = " << std::dec << pInputEntry->sv_interp1->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->flFraction = " << pInputEntry->cl_interp->flFraction;
		}

		if (pInputEntry->player_interp) {
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->nSrcTick = " << std::dec << pInputEntry->player_interp->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->nDstTick = " << std::dec << pInputEntry->player_interp->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->flFraction = " << pInputEntry->player_interp->flFraction;
		}

	}


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
#include "../esp/esp.h"

#include "../../sdk/entity/Weapon.h"
bool RageBot::FindTarget(){

	auto weapon = g_CheatData->LocalPawn->ActiveWeapon();

	if (!weapon) {
		return false;
	}

	for (auto i = 0; i < g_EntityCache->players.size(); i++){
		auto& player = g_EntityCache->players[i];
		player.Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(player.Controller->GetPawnHandle());

		if (!player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->m_iHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn) /*||!player.Pawn->Visible(g_CheatData->LocalPawn)*/)
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
				);

			pt.player = player;
			pt.point = g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].matrix[6].pos;
			//g_EspDrawManager->test = pt.point;
			//pt.tick = TIME_TO_TICKS(/*player.Pawn->GetSimulationTime()*/g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].m_flSimulationTime + g_interfaces->Client->get_lerp_time());
			pt.tick = g_PlayerLog->logs[player.handle.GetEntryIndex()].record[0].m_flSimulationTime / 0.015625;
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
		!g_CheatData->LocalPawn->m_iHealth() ||		
		!g_CheatData->LocalPawn->CanShoot() ||
		!FindTarget()){

		return false;

	}

	return true;
}

void RageBot::SetCmdTick(int tick, C_CSPlayerPawn* target, int history){

	Tickfrac_t tf{ tick, 1.f };
	InterpInfo_t cl, sv0, sv1;
	if (target->m_pGameSceneNode()->CalculateInterpInfos(&cl, &sv0, &sv1, &tf)) {
	
		auto History = cur_cmd->GetInputHistoryEntry(history);
		if (!History)
			return;
		LOG(DEBUG) << "将 nRenderTickCount 从 " << std::dec << History->nRenderTickCount << " 调整为 " << std::dec << cl.dstTick << " GlobalVars TickCount: " << std::dec << g_interfaces->GlobalVars->nTickCount;
		History->nRenderTickCount = cl.dstTick;

		if (History->cl_interp) {
			History->cl_interp->nSrcTick = cl.srcTick;
			History->cl_interp->nDstTick = cl.dstTick;
			History->cl_interp->flFraction = cl.fraction;

		}

		if (History->sv_interp0) {
			History->sv_interp0->nSrcTick = sv0.srcTick;
			History->sv_interp0->nDstTick = sv0.dstTick;
			History->sv_interp0->flFraction = sv0.fraction;
		}

		if (History->sv_interp1) {
			History->sv_interp1->nSrcTick = sv1.srcTick;
			History->sv_interp1->nDstTick = sv1.dstTick;
			History->sv_interp1->flFraction = sv1.fraction;
		}
	
	
	};

	



}
