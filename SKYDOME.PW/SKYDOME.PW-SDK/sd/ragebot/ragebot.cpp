#include "ragebot.h"
#include "../../CheatData.h"

QAngle_t CalcAngle(const Vector& src, const Vector& dst) {
	QAngle_t vAngle;
	Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	vAngle.x = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
	vAngle.y = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
	vAngle.z = 0.0f;

	if (delta.x >= 0.0)
		vAngle.y += 180.0f;

	return vAngle;
}


void RageBot::run(CUserCmd* cmd){

	points.clear();
	noobs.clear();

	FindTarget();

	if (points.size()){

		QAngle_t shit = CalcAngle(g_CheatData->LocalPawn->GetEyePosition(),points[0].point);

		cmd->SetSubTickAngle(shit);
		cmd->nButtons.nValue |= IN_ATTACK;


	}

}

bool RageBot::FindTarget(){
	for (auto i = 0; i < g_EntityCache->players.size(); i++){
		auto& player = g_EntityCache->players[i];
		player.Pawn = g_interfaces->GameResourceService->pGameEntitySystem->Get<C_CSPlayerPawn>(player.Controller->GetPawnHandle());

		if (!player.Valid() || !player.Controller->IsPawnAlive() || player.Pawn->GetHealth() <= 0 || !player.Pawn->IsEnemy(g_CheatData->LocalPawn) ||!player.Pawn->Visible(g_CheatData->LocalPawn))
			continue;


		points.emplace_back(player.Pawn->GetEyePosition());

	}

	return noobs.size();
}
