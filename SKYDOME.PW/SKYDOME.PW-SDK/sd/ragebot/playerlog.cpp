#include "playerlog.h"
#include "../entitycache/entitycache.h"

void PlayerLog::Log(){
	for (int i = 0; i < g_EntityCache->players.size(); i++) {
		auto& player = g_EntityCache->players[i];
		if (player.Controller)
			player.UpdatePawn();

		if (!player.Valid())
			continue;

		

		auto& player_log = logs[player.handle.GetEntryIndex()];
		auto& records = player_log.record;
		
		if (!player.Controller->IsPawnAlive()) {
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


	FilterRecords();



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

#include "../../CheatData.h"
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
	const float flLatency = NetworkChannel->get_latency(FLOW_OUTGOING) + NetworkChannel->get_latency(FLOW_INCOMING);
	const float flCorrectedValue = std::clamp(flLatency + g_interfaces->NetworkClientService->GetNetworkClient()->GetClientInterpAmount(), 0.0f, flMaxUnlag);
	float flMaxDelta = min(flMaxUnlag - flCorrectedValue, 0.2f);
	const float flDelta = g_interfaces->GlobalVars->flCurtime - flMaxDelta;

	return m_flSimulationTime > flDelta;
}
