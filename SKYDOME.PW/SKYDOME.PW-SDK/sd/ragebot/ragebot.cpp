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
	ѡȡ�������Լ�¼?
	ǰ �� ��
	���ȫ������ʧ��,���˳�ɨ��

	�����������¼�еõ���best_point����õ�
	���û��,���һ����Ч�ļ�¼��ʼ�򷴷����ƽ�

	������������Ҫ������ǿɼ��Ͳ��ɼ���������Ҫ���Ǳ�ס���ɼ�ʱ��fps

	���������ʹ��pred eyepos�Ĳ������ҵ�������Ҫ���best_point,Ȼ������ʹ��save eyepos��������ɨ��
	��������������ǻ����������������best point

	�������ȫ�����Գɹ������Ǵ�ͷ����ʼɨ��
	���ֻ���к�β���Գɹ������Ǵ��в����ɨ��
	���ֻ��β��ɨ��ɹ������Ǵ�β����ǰɨ��

	����Ҳ������pred����������ɨ��
	�������������Ժ������Զ���ͣ��һ���


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
			LOG(DEBUG) << "��Ч�������¼ -> " << std::dec << i;
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

//��ƶ���,���Ǵ������
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

		//���˳�������Ч��¼���ȴ���һ��ɨ��
		for (int i = 0; i < g_PlayerLog->logs[player.handle.GetEntryIndex()].record.size(); i++) {
			if (g_PlayerLog->logs[player.handle.GetEntryIndex()].record[i].IsValid()){
				target.records.emplace_back(g_PlayerLog->logs[player.handle.GetEntryIndex()].record[i]);
			}

		}

		if (target.records.size() >= 3){

			//ȡ��ͷ����β����¼
			target.records_for_test.emplace_back(target.records.front());
			target.records.erase(target.records.begin());
			target.records_for_test.emplace_back(target.records.back());
			target.records.pop_back();

			//ȡ���м��¼
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

	//����֮������
	SD_ASSERT(1);

	
}

/*
	����һֱ�ڿ����˺�������ȱ�����Ǹ���������
	��Ҫ�������������Ǳ������Ϊ��Ҫ���صģ���ʱ���������������ֻ�ǽ��м�ͣ

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

		//��������Ե�תΪ�����ʧ��
		//Ӧ�õ��������Ƿ���ж�㣬�ڿ��ܵ������Ӧ��Ϊ���Խ��ж��

		//���ͷ�����Գɹ������ǰ�ο�ʼɨ��
		if (test == TS_FIRST_OK){

			//����һ��ɨ�賢��һ��������ѵ�
			//�ɹ��ͽ���
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

	//Ԥ����дpoint��һ������Ϣ
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
		
		//���δ�����������������ڲ���
		if (!i.need_multpoint || is_test){

			AutoWallTest(center, target->player, canhit, damage, is_test ? AWTEST_PRED : AWTEST_SAVE);

			if (canhit && damage > min_damage){
				if (damage > max_damage){
					max_damage = damage;
				}

				cur_point.damage = damage;
				cur_point.point = center;

				//������ڲ���,�ѵ��ͽ�Ԥ�������
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
	//��Ȼ��֪��Ϊʲô���������������ʱ�����ĺܺã���ʱ������һЩ��������
	//�ڵ��˾�ֹ��ʱ��
	if (target->m_pGameSceneNode()->CalculateInterpInfos(&cl, &sv0, &sv1, &tf)) {
	
		auto History = cur_cmd->GetInputHistoryEntry(history);
		if (!History)
			return;
		if(cl.dstTick < History->nRenderTickCount)
		LOG(DEBUG) << std::dec << History->nRenderTickCount - tick <<" ���Ի��� " << std::dec << History->nRenderTickCount - cl.dstTick << " ��tick �� nRenderTickCount �� " << std::dec << History->nRenderTickCount << " ����Ϊ " << std::dec << cl.dstTick << " GlobalVars TickCount: " << std::dec << g_interfaces->GlobalVars->nTickCount;
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
