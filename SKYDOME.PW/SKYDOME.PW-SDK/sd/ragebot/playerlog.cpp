#include "playerlog.h"
#include "../entitycache/entitycache.h"
/*
* 记录系统附赠同人小说(
* 
	"今天听的什么歌?"
	布洛妮娅做了一个深呼吸，视线没有丝毫的移动，似乎是并不想中断自己的回忆。她歪了歪头，把耳机冲向我。我一边吐槽一边自己把蓝牙耳机从她耳朵里拽出来。
	……
	"这是我们船上放过的音乐吗。是哪个地方的环境音来着。"
	"我也忘了……许多年没有听过了。布洛妮娅想起了好多好多以前的事情。"
	"……越是宝贵的回忆越是总会让我感觉很不舒服，你有这种感觉吗。"
	"我知道，曾经有。在艰难的时候，它们的宝贵有时会让人对未来产生恐惧。别害怕。你也能感受到回忆带来的力量，对吗?"
	"嗯……"
	"这种力量会帮助你改变不确定的未来。"

	布洛妮娅看起来镇静，其实心脏跳的比在场的任何一个人都快。即使是在军队里的日子，布洛妮娅也因为不喜欢打出致命的子弹而大部分时间都在做技术工作。那段日子让布洛妮娅感到无比压抑，更何况那还是年幼且感性的她，这也是为什么她明知道有人对她们不怀好意却仍因为侥幸心理留在孤儿院。在此刻，已经结痂的伤口再一次被撕裂，心中的痛苦虽被重新忆起，但是却早已麻木。
	布洛妮娅摇了摇头让自己清醒一点，减轻回忆带给自己的负担。她的思绪回到当前，却看见刚才组团想听她的故事的三人脸上都带着愧疚的表情。往日给她溅上血污的双翼。她没有血缘关系但是却坚定选择爱她的家人们并不在乎，他们愿意伸出双手紧紧抱住她，愿意将她护在身下。

*/
void PlayerLog::Log(){
	for (int i = 0; i < g_EntityCache->players.size(); i++) {
		auto& player = g_EntityCache->players[i];
		if (player.Controller)
			player.UpdatePawn();

		auto& player_log = logs[player.handle.GetEntryIndex()];

		if (!player.Valid()) {
			player_log.valid = false;
			continue;
		}


		player_log.player = player;
		player_log.valid = player_log.alive = true;

		auto& records = player_log.record;
		
		if (!player.Controller->IsPawnAlive()) {
			player_log.valid = false;
			records.clear();
		}

		while (records.size() > 32)
			records.pop_back();
		
		lag_record_t new_data(player.Pawn);

		

		if (records.empty()){
			records.emplace_front(new_data);
			continue;
		}
		//地图已更改
		if (new_data.m_flSimulationTime < records[0].m_flSimulationTime) {
			records.clear();
			continue;
		}
		if (new_data.m_flSimulationTime > records[0].m_flSimulationTime) {
			records.emplace_front(new_data);
			continue;
		}
	




	}

	if (g_interfaces->GlobalVars)
	{
		FilterRecords();
	}
	



}

void PlayerLog::FilterRecords(){

	for (int i = 0; i < 65; i++){
		auto& player = logs[i];

		if (player.record.empty())
			continue;
		while (!player.record.empty() && (player.record.size() > 32 || !player.record.back().IsValid())) {

			player.record.pop_back();

		}
			
	}



}

struct data_info
{
	std::uint64_t tick;
	float time;
	char pad[0x24];
};






#include "../../CheatData.h"
#include "../menu/config.h"
bool lag_record_t::IsValid()
{


	//static auto sv_maxunlag = 0.2;
	//// more to do
	//float time = g_interfaces->GlobalVars->flCurtime - sv_maxunlag;
	//int tick = static_cast<int>(time / (1.f / 64.f) + 0.5f);
	//float fraction = time / (1.f / 64.f) - tick;

	//Tickfrac_t tf{ tick, 1.f };
	//InterpInfo_t cl, sv0, sv1;
	//if (!g_CheatData->LocalPawn->m_pGameSceneNode()->CalculateInterpInfos(&cl, &sv0, &sv1, &tf)) return m_flSimulationTime < time;

	//float newTime = cl.srcTick * (1.f / 64.f);

	//return m_flSimulationTime < newTime;

	const auto NetworkChannel = g_interfaces->EngineClient->GetNetChannelInfo(0);
	if (!NetworkChannel || !g_interfaces->NetworkClientService->GetNetworkClient())
		return false;

	const float flMaxUnlag = 0.2f;
	const float flLatency = NetworkChannel->get_network_latency() + NetworkChannel->get_engine_latency();
	const float flCorrectedValue = std::clamp(flLatency + g_interfaces->NetworkClientService->GetNetworkClient()->GetClientInterpAmount(), 0.0f, flMaxUnlag);
	float flMaxDelta = min(flMaxUnlag - flCorrectedValue, 0.2f);
	float flDelta = flMaxDelta - g_interfaces->GlobalVars->flCurtime;

	return g_interfaces->GlobalVars->flCurtime - m_flSimulationTime < flMaxDelta;
}

bool lag_record_t::MakeHitboxList(C_CSPlayerPawn* pawn){

	Vector min,max;

	if (g_ConfigManager->rage_hitbox[0]) {
		if (pawn->GetHitboxMinMax(HITGROUP_HEAD, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[0]);
		};
			
	}
	if (g_ConfigManager->rage_hitbox[1]) {
		if (pawn->GetHitboxMinMax(HITGROUP_NECK, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[1]);
		};
			
	}
	if (g_ConfigManager->rage_hitbox[2]) {
		if (pawn->GetHitboxMinMax(HITGROUP_CHEST, min, max)) {
			hitboxs.emplace_back(min, max,g_ConfigManager->rage_multpoint_hitbox[2]);
		};
			
	}
	if (g_ConfigManager->rage_hitbox[3]) {
		if (pawn->GetHitboxMinMax(HITGROUP_STOMACH, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[3]);
		};
			
	}
	if (g_ConfigManager->rage_hitbox[4]) {
		if (pawn->GetHitboxMinMax(HITGROUP_LEFTARM, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[4]);
		};
			
		if (pawn->GetHitboxMinMax(HITGROUP_RIGHTARM, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[4]);
		};
			
	}
	if (g_ConfigManager->rage_hitbox[5]) {
		if (pawn->GetHitboxMinMax(HITGROUP_LEFTLEG, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[5]);
		};
			
		if (pawn->GetHitboxMinMax(HITGROUP_RIGHTLEG, min, max)) {
			hitboxs.emplace_back(min, max, g_ConfigManager->rage_multpoint_hitbox[5]);
		};
			
	}

	return hitboxs.size();
}
