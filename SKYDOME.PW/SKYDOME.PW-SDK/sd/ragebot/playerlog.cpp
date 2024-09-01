#include "playerlog.h"
#include "../entitycache/entitycache.h"
/*
* ��¼ϵͳ����ͬ��С˵(
* 
	"��������ʲô��?"
	�����������һ�������������û��˿�����ƶ����ƺ��ǲ������ж��Լ��Ļ��䡣��������ͷ���Ѷ��������ҡ���һ���²�һ���Լ���������������������ק������
	����
	"�������Ǵ��ϷŹ������������ĸ��ط��Ļ��������š�"
	"��Ҳ���ˡ��������û�������ˡ�������������˺ö�ö���ǰ�����顣"
	"����Խ�Ǳ���Ļ���Խ���ܻ����Ҹо��ܲ�������������ָо���"
	"��֪���������С��ڼ��ѵ�ʱ�����ǵı�����ʱ�����˶�δ�������־塣���¡���Ҳ�ܸ��ܵ��������������������?"
	"�š���"
	"���������������ı䲻ȷ����δ����"

	������櫿������򾲣���ʵ�������ı��ڳ����κ�һ���˶��졣��ʹ���ھ���������ӣ��������Ҳ��Ϊ��ϲ������������ӵ����󲿷�ʱ�䶼���������������Ƕ������ò�����櫸е��ޱ�ѹ�֣����ο��ǻ��������Ҹ��Ե�������Ҳ��Ϊʲô����֪�����˶����ǲ�������ȴ����Ϊ�����������ڹ¶�Ժ���ڴ˿̣��Ѿ�������˿���һ�α�˺�ѣ����е�ʹ���䱻�������𣬵���ȴ������ľ��
	�������ҡ��ҡͷ���Լ�����һ�㣬�����������Լ��ĸ���������˼���ص���ǰ��ȴ�����ղ������������Ĺ��µ��������϶��������εı��顣���ո�������Ѫ�۵�˫����û��ѪԵ��ϵ����ȴ�ᶨѡ�����ļ����ǲ����ں�������Ը�����˫�ֽ�����ס����Ը�⽫���������¡�

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
		//��ͼ�Ѹ���
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
