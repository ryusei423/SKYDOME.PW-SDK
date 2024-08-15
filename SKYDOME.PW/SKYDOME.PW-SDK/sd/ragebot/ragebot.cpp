#include "ragebot.h"
#include "../../CheatData.h"
#include "penetration.h"
#include "playerlog.h"
#include "rbhelper.h"

#include "../esp/esp.h"

#include "../../sdk/entity/Weapon.h"
#include "../menu/config.h"

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

void RageBot::Fire() {
	ang_ = CalcAngle(g_CheatData->save_eyepos, best_point.point);



	cur_cmd->nButtons.nValue |= IN_ATTACK;
	cur_cmd->nButtons.nValueChanged |= IN_ATTACK;
	cur_cmd->csgoUserCmd.nAttack1StartHistoryIndex = 0;
	cur_cmd->csgoUserCmd.CheckAndSetBits(ECSGOUserCmdBits::CSGOUSERCMD_BITS_ATTACK1START);
	SetCmdTick(best_point.tick, best_point.player.Pawn);

	RemoveRecoil();

	cur_cmd->SetSubTickAngle(ang_);
	if (!GET_CFG("ragebot_force_psilent",bl)){
		cur_cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = ang_;
	}
	

}

/*
	选取三个测试记录?
	前 中 后
	如果全部测试失败,则退出扫描

	如果在三个记录中得到了best_point是最好的
	如果没有,则从一个有效的记录开始向反方向推进

	首先两种最主要的情况是可见和不可见，而最重要的是保住不可见时的fps

	如果我们在使用pred eyepos的测试中找到了满足要求的best_point,然后马上使用save eyepos进行重新扫描
	如果不出意外我们会获得真正用于射击的best point

	如果三个全部测试成功，我们从头部开始扫描
	如果只有中和尾测试成功，我们从中部向后扫描
	如果只有尾部扫描成功，我们从尾部向前扫描

	我们也不会在pred上运行完整扫描
	这样看来，测试和早期自动急停是一体的


*/
void RageBot::run(CUserCmd* cmd){

	cur_cmd = cmd;
	ang_ = cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue;
	points.clear();
	noobs.clear(); 
	best_point_finded = false;
	hc_ok = false;
	have_test_ok = false;

	if(!CanRunRage())
		return;

	if(!MainScan())
		return;
	
	if (!points.size() && have_test_ok){
		g_RageBotHelper->AutomaticStop(g_CheatData->LocalPawn,
							g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
							cur_cmd);

	}
	else if(points.size())
	{
		if (!best_point_finded && !FindBestPoint()){
			if (!hc_ok){
				g_RageBotHelper->AutomaticStop(g_CheatData->LocalPawn,
					g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
					cur_cmd);
			}

			return;
		}


		Fire();

	}
	


	/*RemoveRecoil();

	cmd->SetSubTickAngle(ang_);

	cmd->csgoUserCmd.pBaseCmd->pViewAngles->angValue = ang_;*/


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
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp0->flFraction = " << pInputEntry->sv_interp0->flFraction;
		}

		if (pInputEntry->sv_interp1) {
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->nSrcTick = " << std::dec << pInputEntry->sv_interp1->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->nDstTick = " << std::dec << pInputEntry->sv_interp1->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->sv_interp1->flFraction = " << pInputEntry->sv_interp1->flFraction;
		}

		if (pInputEntry->player_interp) {
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->nSrcTick = " << std::dec << pInputEntry->player_interp->nSrcTick;
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->nDstTick = " << std::dec << pInputEntry->player_interp->nDstTick;
			LOG(DEBUG) << i << " -> pInputEntry->player_interp->flFraction = " << pInputEntry->player_interp->flFraction;
		}

	}


}

//很贫民窟,但是大概有用
void RageBot::RemoveRecoil(){

	auto punch = g_CheatData->LocalPawn->m_aimPunchCache();

	if (cur_cmd->nButtons.nValue & IN_ATTACK && punch.m_Size){
		ang_.pitch -= punch.m_Data[punch.m_Size - 1].pitch * 2;
		ang_.yaw -= punch.m_Data[punch.m_Size - 1].yaw * 2;
		ang_.roll -= punch.m_Data[punch.m_Size - 1].roll * 2;

	}
	
}


bool RageBot::FindTarget(){


	for (auto i = 0; i < g_EntityCache->players.size(); i++){
		auto& player = g_EntityCache->players[i];
		player.Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(player.Controller->GetPawnHandle());

		if (!player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->m_iHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn))
			continue;

		if (g_PlayerLog->logs[player.handle.GetEntryIndex()].record.empty())
			continue;

		RageBotTarget target;
		target.player = player;

		//过滤出所有有效记录，等待下一步扫描
		for (int i = 0; i < g_PlayerLog->logs[player.handle.GetEntryIndex()].record.size(); i++) {
			if (g_PlayerLog->logs[player.handle.GetEntryIndex()].record[i].IsValid()){
				target.records.emplace_back(g_PlayerLog->logs[player.handle.GetEntryIndex()].record[i]);
			}

		}

		if (target.records.size() >= 3){

			//取出头部和尾部记录
			target.records_for_test.emplace_back(target.records.front());
			target.records.erase(target.records.begin());
			target.records_for_test.emplace_back(target.records.back());
			target.records.pop_back();

			//取出中间记录
			size_t middle_index = target.records.size() / 2;
			target.records_for_test.push_back(target.records[middle_index]);
			target.records.erase(target.records.begin() + middle_index);
		}

		if (!target.records.empty())
			noobs.emplace_back(target);


	}

	return noobs.size();
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






int RageBot::RunTest(RageBotTarget* target){

	bool first = false, half = false, back = false;

	for (int i = 0; i < target->records_for_test.size(); i++){
		int dmg = RecordScan(&target->records_for_test[i],target,true);

		if (i == 0){
			first = dmg;
		}

		if (i == 1) {
			half = dmg;
		}

		if (i == 2) {
			back = dmg;
		}
	}
	
	if (!target->player_points_pred.size()){
		return TS_ALL_FAIL;
	}

	if (first){
		return TS_FIRST_OK;
	}

	if (half) {
		return TS_HALF_OK;
	}

	if (back){
		return TS_BACK_OK;
	}

	return TS_ALL_FAIL;

}

void RageBot::AutoWallTest(Vector point, CachedEntity target, bool& canhit, float& damage, int test_type){

	CPlayer_WeaponServices* WeaponServices = g_CheatData->LocalPawn->GetWeaponServices();
	auto ActiveWeapon = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(WeaponServices->m_hActiveWeapon());
	auto wpdata = ActiveWeapon->datawep();

	F::AUTOWALL::c_auto_wall::data_t data;
	if (test_type == AWTEST_PRED)
	{
		F::AUTOWALL::g_auto_wall->pen(data, g_CheatData->pred_eyepos, point, target.Controller, g_CheatData->LocalController, g_CheatData->LocalPawn, target.Pawn, wpdata, damage, canhit);
		canhit = data.m_can_hit;
		damage = data.m_dmg;
		
		return;
	}
	else if(test_type == AWTEST_SAVE){
		F::AUTOWALL::g_auto_wall->pen(data, g_CheatData->save_eyepos, point, target.Controller, g_CheatData->LocalController, g_CheatData->LocalPawn, target.Pawn, wpdata, damage, canhit);
		canhit = data.m_can_hit;
		damage = data.m_dmg;

		return;
	}

	//意料之外的情况
	SD_ASSERT(1);

	
}

/*
	我们一直在考虑伤害，但是缺乏考虑个人命中率
	主要是命中率是我们本地玩家为主要因素的，暂时不单独考虑这个，只是进行急停

*/


bool RageBot::MainScan(){

	bool finded_point = false;

	for (int i = 0; i < noobs.size();i++) {
		int test = RunTest(&noobs[i]);
		if (test == TS_ALL_FAIL){
			continue;
		}

		have_test_ok = true;
		
		if ((test == TS_FIRST_OK || test == TS_HALF_OK )&& noobs[i].pred_pt2save_pt()){
			if (noobs[i].set_best_point()) {
				points.emplace_back(noobs[i].player_best_point);
				continue;
			}
		}

		//如果将测试点转为保存点失败
		//应该单独控制是否进行多点，在可能的情况下应该为测试进行多点

		//如果头部测试成功，则从前段开始扫描
		if (test == TS_FIRST_OK){

			//运行一次扫描尝试一次设置最佳点
			//成功就结束
			for (int r = 0; r < noobs[i].records.size(); r++){
				RecordScan(&noobs[i].records[r],&noobs[i]);
				if (noobs[i].player_points.size() && noobs[i].set_best_point()) {
					points.emplace_back(noobs[i].player_best_point);
					continue;
				}
			}


			continue;
		}




		/*if (test == TS_HALF_OK)
		{
			
			continue;
		}*/



		if (test == /*TS_BACK_OK*/TS_HALF_OK)
		{

			for (int r = noobs[i].records.size() - 1; r != 0 ; r--) {
				RecordScan(&noobs[i].records[r], &noobs[i]);
				if (noobs[i].player_points.size() && noobs[i].set_best_point()) {
					points.emplace_back(noobs[i].player_best_point);
					continue;
				}

			}

			continue;
		}
		
	}



	return points.size();
}

int RageBot::RecordScan(lag_record_t* record, RageBotTarget* target, bool is_test){

	lag_record_t backup(target->player.Pawn);
	record->recover(target->player.Pawn);

	int health = target->player.Pawn->m_iHealth();
	int min_damage = GET_CFG("rage_damage",it) > health ? health : GET_CFG("rage_damage", it);
	bool canhit = false;
	float damage = 0;

	int max_damage = 0;

	RageBotPoint cur_point;

	//预先填写point的一部分信息
	/*cur_point.hcok = g_RageBotHelper->Hitchance(g_CheatData->LocalPawn,
		target->player.Pawn,
		g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
		CalcAngle(g_CheatData->pred_eyepos,record->matrix[HEAD].pos),
		GET_CFG("ragebot_hc",it));*/

	cur_point.tick = (record->m_flSimulationTime + g_interfaces->NetworkClientService->GetNetworkClient()->GetClientInterpAmount()) / 0.015625;
	cur_point.player = target->player;
	cur_point.health = health;




	for (auto& i : record->hitboxs){
		
		Vector center = (i.min + i.max) * 0.5;
		
		//如果未开启多点或者我们正在测试
		if (!i.need_multpoint || is_test){

			AutoWallTest(center, target->player, canhit, damage, is_test ? AWTEST_PRED : AWTEST_SAVE);

			if (canhit && damage > min_damage){
				if (damage > max_damage){
					max_damage = damage;
				}

				cur_point.damage = damage;
				cur_point.point = center;

				//如果正在测试,把点送进预测点容器
				is_test ? target->player_points_pred.emplace_back(cur_point): target->player_points.emplace_back(cur_point);
			}

			continue;
		}



		float radius = 0.5 * sqrt(pow(i.max.x - i.min.x, 2) + pow(i.max.y - i.min.y, 2) + pow(i.max.z - i.min.z, 2));
		
		const auto cur_angles = CalcAngle(center, g_CheatData->save_eyepos);

		Vector forward;
		MATH::AngleVectors(cur_angles, &forward);

		auto rs = radius * (GET_CFG("ragebot_multpoint",fl) / 100.f);

		if (rs < 0.2)
			continue;

		const auto right = forward.CrossProduct(Vector(0, 0, 1)) * rs;
		const auto left = Vector(-right.x, -right.y, right.z);
		const auto top = Vector(0, 0, 1) * rs;

		//Vector center
		Vector rightPoint = center + right;
		Vector leftPoint = center + left;
		Vector topPoint = center + top;

		AutoWallTest(rightPoint, target->player, canhit, damage, AWTEST_SAVE);
		cur_point.damage = damage;
		cur_point.point = rightPoint;
		if (canhit && damage > min_damage) {
			if (damage > max_damage) {
				max_damage = damage;
			}
			target->player_points.emplace_back(cur_point);
		}


		AutoWallTest(leftPoint, target->player, canhit, damage, AWTEST_SAVE);
		cur_point.damage = damage;
		cur_point.point = leftPoint;
		if (canhit && damage > min_damage) {
			if (damage > max_damage) {
				max_damage = damage;
			}
			target->player_points.emplace_back(cur_point);
		}


		AutoWallTest(topPoint, target->player, canhit, damage, AWTEST_SAVE);
		cur_point.damage = damage;
		cur_point.point = topPoint;
		if (canhit && damage > min_damage) {
			if (damage > max_damage) {
				max_damage = damage;
			}
			target->player_points.emplace_back(cur_point);
		}


	}
	

	

	backup.recover(target->player.Pawn);
	return max_damage;

}

bool RageBot::FindBestPoint(){

	int damage = 0, health = 100;
	RageBotPoint hight_damage, low_health;

	for (auto& i : points){
		if (!g_RageBotHelper->Hitchance(g_CheatData->LocalPawn,
			i.player.Pawn,
			g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(g_CheatData->LocalPawn->GetWeaponServices()->m_hActiveWeapon()),
			CalcAngle(g_CheatData->pred_eyepos, i.point),
			GET_CFG("ragebot_hc", it))){

			hc_ok = false;
			continue;
		}

		if (i.damage > damage){
			damage = i.damage;
			hight_damage = i;
		}

		if (i.health < health){
			health = i.health;
			low_health = i;
		}

		hc_ok = true;

	}

	

	if (damage != 0){
		if (health == 100){
			low_health = hight_damage;
		}
		best_point = GET_CFG("ragebot_target_selection", it) == 0 ? hight_damage : low_health;
		best_point_finded = true;

		return true;
	}

	return false;
}






void RageBot::SetCmdTick(int tick, C_CSPlayerPawn* target, int history){

	Tickfrac_t tf{ tick, 1.f };
	InterpInfo_t cl, sv0, sv1;
	//虽然不知道为什么，但是这个函数有时候工作的很好，有时候会给出一些糟糕的数字
	//在敌人静止的时候
	if (target->m_pGameSceneNode()->CalculateInterpInfos(&cl, &sv0, &sv1, &tf)) {
	
		auto History = cur_cmd->GetInputHistoryEntry(history);
		if (!History)
			return;
		if(cl.dstTick < History->nRenderTickCount)
		LOG(DEBUG) << std::dec << History->nRenderTickCount - tick <<" 尝试回溯 " << std::dec << History->nRenderTickCount - cl.dstTick << " 个tick 将 nRenderTickCount 从 " << std::dec << History->nRenderTickCount << " 调整为 " << std::dec << cl.dstTick << " GlobalVars TickCount: " << std::dec << g_interfaces->GlobalVars->nTickCount;
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

bool RageBotTarget::pred_pt2save_pt()
{
	bool canhit;
	float damage;
	int health = this->player.Pawn->m_iHealth();
	int min_damage = GET_CFG("rage_damage", it) > health ? health : GET_CFG("rage_damage", it);

	for (int i = 0; i < player_points_pred.size(); i++){
		auto& cur = player_points_pred[i];
		g_RageBot->AutoWallTest(cur.point, cur.player, canhit, damage, g_RageBot->AWTEST_SAVE);

		if (canhit && damage >= min_damage){
			player_points.emplace_back(cur);
		}

	}

	return player_points.size();
}

bool RageBotTarget::set_best_point(){
	int damage = 0;

	for (int i = 0; i < player_points.size(); i++) {
		if (player_points[i].damage > damage){
			damage = player_points[i].damage;
			player_best_point = player_points[i];
		}

	}

	return damage;
}
