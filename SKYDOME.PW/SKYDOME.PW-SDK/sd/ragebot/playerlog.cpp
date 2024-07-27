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

		if (new_data.m_flSimulationTime > records[0].m_flSimulationTime) {
			records.emplace_front(new_data);
			continue;
		}
	




	}






}
