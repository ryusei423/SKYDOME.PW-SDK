#include "legitbot.h"

#include "../../CheatData.h"

void LegitBot::run(CUserCmd* cmd){
	if (!FindTarget())
		return;





}

bool LegitBot::FindTarget(){

	for (auto i = 0; i < g_EntityCache->players.size(); i++) {
		auto& player = g_EntityCache->players[i];
		

		if (!player.UpdatePawn() || !player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->GetHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn) || !player.Pawn->Visible(g_CheatData->LocalPawn))
			continue;

		noobs.emplace_back(player);
	}

	return noobs.size();




}

void LegitBot::TriggerBot(){
	/*QAngle*/









}
